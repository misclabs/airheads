#include "developer_gui.h"

#include "app.h"
#include "app_window.h"
#include "log.h"

#include "imgui.h"

#include <cassert>

namespace Airheads {

template<typename ContentCallback>
void ImGuiWindow(const char* name,
	ContentCallback content,
	bool* open = nullptr,
	[[maybe_unused]] ImGuiWindowFlags flag = 0) {
	if (open != nullptr && !*open)
		return;

	if (ImGui::Begin(name, open))
		content();

	ImGui::End();
}

DeveloperGui::DeveloperGui(App* app, AppWindow* app_window)
	: appraiser_window_(app_window, app->GetVideoCapture(), processor_pipeline_),
	saturation_map_renderer_(app_window->NativeRenderer(), &processor_pipeline_.Context().saturation_map_),
	value_map_renderer_(app_window->NativeRenderer(), &processor_pipeline_.Context().value_map_),
	cluster_map_renderer_(app_window->NativeRenderer(), &processor_pipeline_.Context().cluster_map_) {
	assert(app);
	assert(app_window);

	app_ = app;
	app_window_ = app_window;

	LoadProcessors(processor_pipeline_);
}

void DeveloperGui::OnKeyDown(const SDL_KeyboardEvent& event) {
	if (event.keysym.scancode == SDL_SCANCODE_F8 && event.repeat == 0) {
		is_dev_ui_enabled_ = !is_dev_ui_enabled_;
	}
}
void DeveloperGui::OnKeyUp(const SDL_KeyboardEvent& event) {

}

void DeveloperGui::OnMouseButtonDown(const SDL_MouseButtonEvent& event) {
	appraiser_window_.OnMouseButtonDown(event);
}

void DeveloperGui::OnMouseButtonUp(const SDL_MouseButtonEvent& event) {
	appraiser_window_.OnMouseButtonUp(event);
}

void DeveloperGui::Update() {
	if (!is_dev_ui_enabled_) {
		constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;
		ImGui::Begin("Appraiser View", nullptr, windowFlags);
		int w, h;
		SDL_GetWindowSize(app_window_->NativeWindow(), &w, &h);
		ImGui::SetWindowSize({(float) w, (float) h});
		ImGui::SetWindowPos({0, 0});

		appraiser_window_.Update();

		ImGui::End();
	} else {
		auto dockspace_id = ImGui::DockSpaceOverViewport();

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Exit", "Cmd+Q")) {
					app_->StopMainLoop();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View")) {
				ImGui::MenuItem("Pipeline Config", nullptr, &is_pipeline_config_visible_);
				ImGui::MenuItem("Stats", nullptr, &is_stats_visible_);
				//ImGui::MenuItem("Ruler", nullptr, &appraiser_window_.is_ruler_visible_);
				ImGui::Separator();
				ImGui::MenuItem("Saturation Map", nullptr, &is_saturation_map_visible_);
				ImGui::MenuItem("Value Map", nullptr, &is_value_map_visible_);
				ImGui::MenuItem("Cluster Map", nullptr, &is_cluster_map_visible_);
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		//ImGui::ShowDemoWindow();

		ImGuiWindow("Pipeline Config",
			[this]() -> void { UpdateConfigContent(); },
			&is_pipeline_config_visible_);

		ImGui::SetNextWindowDockID(dockspace_id);
		ImGui::Begin("Appraiser View", nullptr);
		appraiser_window_.Update();
		ImGui::End();

		ImGuiWindow("Stats",
			[this]() -> void { UpdateStatsContent(); }, &is_stats_visible_);

		ImGuiWindow("Saturation Map", [this]() -> void {
			ImGui::Text("Saturation Map (threshold applied)");
			saturation_map_renderer_.UpdateTexture();
			saturation_map_renderer_.RenderImage();
		}, &is_saturation_map_visible_);
		ImGuiWindow("Value Map", [this]() -> void {
			ImGui::Text("Value Map (inverted, threshold applied)");
			value_map_renderer_.UpdateTexture();
			value_map_renderer_.RenderImage();
		}, &is_value_map_visible_);
		ImGuiWindow("Cluster Map", [this]() -> void {
			ImGui::Text("Cluster Map (saturation anded w/ value map)");
			cluster_map_renderer_.UpdateTexture();
			cluster_map_renderer_.RenderImage();
		}, &is_cluster_map_visible_);
	}
}

void DeveloperGui::UpdateConfigContent() {
	ImGuiWindow("Pipeline Config", [this]() -> void {
		bool mirror_camera = app_->GetVideoCapture().IsCameraMirrored();
		ImGui::Checkbox("Mirror Camera", &mirror_camera);
		app_->GetVideoCapture().IsCameraMirrored(mirror_camera);
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
			ImGui::SetTooltip("Display the camera feed as a mirror image?");

//		ImGui::Checkbox("Show Ruler", &appraiser_window_.is_ruler_visible_);
//		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
//			ImGui::SetTooltip("Display the ruler overlay on the camera feed?");

		ImGui::SliderFloat("pixels/cm", &processor_pipeline_.Context().frame_pixels_per_cm_, 3, 100);
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
			ImGui::SetTooltip("Approximate pixels per cm at the distances dots are from the camera.");

		ImGui::Separator();

		processor_pipeline_.UpdateConfigGui();
	}, &is_pipeline_config_visible_);
}

void DeveloperGui::UpdateStatsContent() {
	ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);

	VideoCapture& video_capture = app_->GetVideoCapture();
	if (video_capture.Capturing()) {
		ImGui::Separator();
		ImGui::Text("Video Input");
		Vec2i frame_size = video_capture.FrameSize(video_capture.CapturingDeviceId());
		ImGui::Text("Frame size:%dx%d", frame_size.x, frame_size.y);
		ImGui::Text("Buffer size (bytes):%zu",
			video_capture.FrameBufferSizeBytes(video_capture.CapturingDeviceId()));

		processor_pipeline_.UpdateStatsGui();
	} else {
		ImGui::Text("No device active");
	}
}

}