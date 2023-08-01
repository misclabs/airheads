#include "gui.h"

#include "app.h"
#include "app_window.h"
#include "log.h"

#include "imgui.h"

#include <algorithm>
#include <cassert>
#include <climits>

namespace Airheads {

	Gui::Gui(AppWindow* appWindow) {
		assert(appWindow);

		m_appWindow = appWindow;
	}

	void Gui::Update() {
		if (m_shouldUpdateAvailableCameras) {
			m_shouldUpdateAvailableCameras = false;
			m_cameraNames = m_videoInput.getDeviceList();
		}

		constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;
		ImGui::Begin("Content", nullptr, windowFlags);
		int w, h;
		SDL_GetWindowSize(m_appWindow->NativeWindow(), &w, &h);
		ImGui::SetWindowSize({ (float)w, (float)h });
		ImGui::SetWindowPos({ 0, 0 });

		ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);

		// Camera Selection Combobox
		{
			const char* comboPreview = m_selectedCamera >= 0 && m_selectedCamera < m_cameraNames.size() ?
				m_cameraNames[m_selectedCamera].c_str() :
				"No camera detected";
			if (ImGui::BeginCombo("###Camera", comboPreview, 0)) {
				for (int i = 0; i < m_cameraNames.size(); ++i) {
					const bool is_selected = (m_selectedCamera == i);
					if (ImGui::Selectable(m_cameraNames[i].c_str(), is_selected))
						m_selectedCamera = i;

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}
		ImGui::SameLine();
		if (m_selectedCamera >= 0 && m_selectedCamera < m_cameraNames.size()) {
			if (m_selectedCamera == m_activeCamera) {
				if (ImGui::Button("Stop")) {
					SetActiveCamera(-1);
				}
			} else {
				if (ImGui::Button("Start")) {
					SetActiveCamera(m_selectedCamera);
				}
			}
		} else {
			ImGui::BeginDisabled();
			ImGui::Button("Start");
			ImGui::EndDisabled();
		}

		if (m_activeCamera != -1) {
			ImGui::Checkbox("Add Blue channel", &m_blueFilter);
			ImGui::SameLine();
			ImGui::SliderInt("###blue_channel_value", &m_blueValue, -UCHAR_MAX, UCHAR_MAX);

			UpdateCameraTexture();

			ImVec2 cameraSize{ (float)m_videoInput.getWidth(m_activeCamera), (float)m_videoInput.getHeight(m_activeCamera) };
			ImGui::Text("Width:%d Height:%d", (int)cameraSize.x, (int)cameraSize.y);
			ImGui::Text("Buffer format: BGR24");
			ImGui::Text("Buffer size (bytes):%d", m_videoInput.getSize(m_activeCamera));
			ImGui::Image(m_cameraRenderTex, cameraSize);
		}
		ImGui::End();
	}

	void Gui::SetActiveCamera(int index) {
		if (index == m_activeCamera)
			return;

		if (m_activeCamera >= 0) {
			m_videoInput.stopDevice(m_activeCamera);
			SDL_DestroyTexture(m_cameraRenderTex);
			m_cameraRenderTex = nullptr;
		}

		m_activeCamera = index;
		if (m_activeCamera == -1)
			return;

		m_videoInput.setupDevice(m_activeCamera);
		m_cameraRenderTex = SDL_CreateTexture(m_appWindow->NativeRenderer(),
			SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STREAMING,
			m_videoInput.getWidth(m_activeCamera), m_videoInput.getHeight(m_activeCamera)
		);
		if (!m_cameraRenderTex) {
			APP_ERROR("Error creating camera texture: {}", SDL_GetError());
		}

		UpdateCameraTexture();
	}

	void Gui::UpdateCameraTexture() {
		const SDL_Rect rect{ 0, 0, m_videoInput.getWidth(m_activeCamera), m_videoInput.getHeight(m_activeCamera) };
		const int pitch = rect.w * 3;
		const int bufferSizeInBytes = m_videoInput.getSize(m_activeCamera);
		unsigned char* const pixels = m_videoInput.getPixels(m_activeCamera, false, true);

		if (m_blueFilter) {
			for (int i = 0; i < bufferSizeInBytes; i += 3) {
				pixels[i] = (unsigned char)std::clamp(pixels[i] + m_blueValue, 0, 255);
			}
		}

		int result = SDL_UpdateTexture(m_cameraRenderTex,
			&rect, pixels, pitch);
		if (result) {
			APP_ERROR("Error updating camera texture: {}", SDL_GetError());
		}
	}
}