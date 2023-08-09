#include "gui.h"

#include "app.h"
#include "app_window.h"
#include "log.h"

#include "imgui.h"
//#include "opencv2/core.hpp"

//#include <algorithm>
#include <cassert>

namespace Airheads {

	template<typename ContentCallback>
	void ImGuiWindow(const char* name, ContentCallback content, bool* open = 0, ImGuiWindowFlags flag = 0) {
		if (open != nullptr && !*open)
			return;

		if (ImGui::Begin(name, open))
			content();

		ImGui::End();
	}

	Gui::Gui(App* app, AppWindow* appWindow) 
		: m_saturationMapRenderer(appWindow->NativeRenderer(), &m_processingContext.saturation_map),
		m_valueMapRenderer(appWindow->NativeRenderer(), &m_processingContext.value_map),
		m_clusterMapRenderer(appWindow->NativeRenderer(), &m_processingContext.cluster_map)
	{
		assert(app);
		assert(appWindow);

		m_app = app;
		m_appWindow = appWindow;

		LoadProcessors(m_processorPipeline);
	}

	void Gui::Update() {
		if (m_shouldUpdateAvailableCameras) {
			m_shouldUpdateAvailableCameras = false;
			m_cameraNames = m_videoInput.getDeviceList();
		}

		ImGui::DockSpaceOverViewport();

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Exit", "Cmd+Q")) {
					m_app->StopMainLoop();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View")) {
				ImGui::MenuItem("Pipeline Config", nullptr, &m_showPipelineConfig);
				ImGui::MenuItem("Stats", nullptr, &m_showStats);
				ImGui::MenuItem("Saturation Map", nullptr, &m_showSaturationMap);
				ImGui::MenuItem("Value Map", nullptr, &m_showValueMap);
				ImGui::MenuItem("Cluster Map", nullptr, &m_showClusterMap);
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		ImGuiWindow("Pipeline Config", 
			[this]() -> void { UpdatePipelineConfigContent(); }, &m_showPipelineConfig);
		
		ImGui::Begin("Actually the Real App", nullptr);
		UpdateMainGuiContent();
		ImGui::End();

		ImGuiWindow("Stats",
			[this]() -> void { UpdateStatsContent(); }, &m_showStats);

		ImGuiWindow("Saturation Map",
			[this]() -> void { UpdateSaturationMapContent(); }, &m_showSaturationMap);
		ImGuiWindow("Value Map",
			[this]() -> void { UpdateValueMapContent(); }, &m_showValueMap);
		ImGuiWindow("Cluster Map",
			[this]() -> void { UpdateClusterMapContent(); }, &m_showClusterMap);
	}

	void Gui::UpdateStatsContent() {
		ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
		ImGui::Separator();

		if (m_activeCamera != -1) {
			ImVec2 cameraSize{ (float)m_videoInput.getWidth(m_activeCamera), (float)m_videoInput.getHeight(m_activeCamera) };
			ImGui::Text("Width:%d Height:%d", (int)cameraSize.x, (int)cameraSize.y);
			ImGui::Text("Buffer size (bytes):%d", m_videoInput.getSize(m_activeCamera));

			m_processorPipeline.ForEach([](VideoProcessor& processor) {
				ImGui::Separator();
				ImGui::Text(processor.Name().c_str());
				if (processor.isEnabled) {
					processor.UpdateStatsControls();
				}
			});
		} else {
			ImGui::Text("No camera active");
		}
	}

	void Gui::UpdatePipelineConfigContent() {
		bool first = true;
		m_processorPipeline.ForEach([&first](VideoProcessor& processor) {
			if (!first) {
				ImGui::Separator();
			} else {
				first = false;
			}

			//ImGui::Checkbox(processor.Name().c_str(), &processor.isEnabled);
			ImGui::Text(processor.Name().c_str());
			if (processor.isEnabled) {
				processor.UpdateConfigControls();
			}
		});
	}

	void Gui::UpdateMainGuiContent() {
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
		constexpr const char* START_VIDEO_CAPTURE_TEXT = "Start Video Capture";
		if (m_selectedCamera >= 0 && m_selectedCamera < m_cameraNames.size()) {
			if (m_selectedCamera == m_activeCamera) {
				if (ImGui::Button("Stop Video Capture")) {
					SetActiveCamera(-1);
				}
			}
			else {
				if (ImGui::Button(START_VIDEO_CAPTURE_TEXT)) {
					SetActiveCamera(m_selectedCamera);
				}
			}
		}
		else {
			ImGui::BeginDisabled();
			ImGui::Button(START_VIDEO_CAPTURE_TEXT);
			ImGui::EndDisabled();
		}

		if (m_activeCamera != -1) {
			UpdateCameraTexture();

			ImVec2 cameraSize{ (float)m_videoInput.getWidth(m_activeCamera), (float)m_videoInput.getHeight(m_activeCamera) };
			ImVec2 availSize = ImGui::GetContentRegionAvail();
			ImVec2 renderSize = [&] {
				if (cameraSize.x / cameraSize.y > availSize.x / availSize.y) {
					// fit width
					float scale = availSize.x / cameraSize.x;
					return ImVec2{ cameraSize.x * scale, cameraSize.y * scale };
				}
				// fit height
				float scale = availSize.y / cameraSize.y;
				return ImVec2{ cameraSize.x * scale, cameraSize.y * scale };
			}();
			
			ImGui::Image(m_cameraRenderTex, renderSize);
		}
	}

	void Gui::UpdateSaturationMapContent() {
		ImGui::Text("Saturation Map (threshold applied)");
		m_saturationMapRenderer.UpdateTexture();
		m_saturationMapRenderer.RenderImage();
	}

	void Gui::UpdateValueMapContent() {
		ImGui::Text("Value Map (inverted, threshold applied)");
		m_valueMapRenderer.UpdateTexture();
		m_valueMapRenderer.RenderImage();
	}

	void Gui::UpdateClusterMapContent() {
		ImGui::Text("Cluster Map (saturation anded w/ value map)");
		m_clusterMapRenderer.UpdateTexture();
		m_clusterMapRenderer.RenderImage();
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

		m_processorPipeline.StartCapture(
			m_videoInput.getWidth(m_activeCamera),
			m_videoInput.getHeight(m_activeCamera));

		UpdateCameraTexture();
	}

	void Gui::UpdateCameraTexture() {
		const SDL_Rect rect{ 0, 0, m_videoInput.getWidth(m_activeCamera), m_videoInput.getHeight(m_activeCamera) };
		const int pitch = rect.w * 3;
		unsigned char* const pixels = m_videoInput.getPixels(m_activeCamera, false, true);

		m_processingContext.SetFrameBGR(rect.w, rect.h, pixels);
		m_processorPipeline.ProcessFrame(m_processingContext);

		int result = SDL_UpdateTexture(m_cameraRenderTex,
			&rect, pixels, pitch);
		if (result) {
			APP_ERROR("Error updating camera texture: {}", SDL_GetError());
		}
	}
}