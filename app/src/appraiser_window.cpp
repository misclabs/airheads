#include "appraiser_window.h"
#include "log.h"
#include "imgui.h"

namespace Airheads {

static inline float OverlayLineWidthPx(const ProcessingContext& context) {
	float line_width = context.target_diameter_cm_ / 3.0f;
	return context.CmToPx(line_width);
}

ImVec2 CameraViewMetrics::CameraFrameToWindowLoc(Vec2i frame_loc) const {
	return {
		window_pos.x + (float) frame_loc.x * frame_to_window_scale,
		window_pos.y + (float) frame_loc.y * frame_to_window_scale};
}

Vec2i CameraViewMetrics::WindowLocToCameraFrame(Vec2i window_loc) const {
	return {
		(int) (((float) window_loc.x - window_pos.x) / frame_to_window_scale),
		(int) (((float) window_loc.y - window_pos.y) / frame_to_window_scale)
	};
}

bool CameraViewMetrics::IsWindowLocInside(Vec2i window_loc) const {
	return window_loc.x >= window_pos.x
		&& window_loc.x <= window_pos.x + render_size.x
		&& window_loc.y >= window_pos.y
		&& window_loc.y <= window_pos.y + render_size.y;
}

static void DrawRuler(ImDrawList* draw,
	ImVec2 left_side,
	float window_pixels_per_cm,
	ImU32 color,
	float line_width) {
	ImVec2 right_side = {left_side.x + window_pixels_per_cm, left_side.y};

	draw->AddLine({left_side.x, left_side.y + 6}, {left_side.x, left_side.y - 6}, color, line_width);
	draw->AddLine({right_side.x, right_side.y + 6}, {right_side.x, right_side.y - 6}, color, line_width);
	draw->AddLine(left_side, right_side, color, line_width);
	draw->AddText({right_side.x + 3, right_side.y}, color, "1cm");
}

void AppraiserWindow::Update() {

	UpdateToolbar(pipeline_);

	if (video_capture_.Capturing()) {
		ImGui::BeginChild("Tool Options", ImVec2(250, 0), true);
		{
			if (mode_ == AppraiserMode::SelectTopTarget) {
				ImGui::Text("Select Top Target");
			} else if (mode_ == AppraiserMode::SelectBottomTarget) {
				ImGui::Text("Select Bottom Target");
				if (ImGui::Button("Back to selecting top target")) {
					mode_ = AppraiserMode::SelectTopTarget;
				}
			} else if (mode_ == AppraiserMode::Testing) {
				if (ImGui::Button("Back to selecting top target")) {
					mode_ = AppraiserMode::SelectTopTarget;
				}
				if (ImGui::Button("Back to selecting bottom target")) {
					mode_ = AppraiserMode::SelectBottomTarget;
				}

			}
		}
		ImGui::EndChild();

		ImGui::SameLine();
		ImGui::BeginGroup();
		{
			// TODO(jw): probably a better place to do this
			if (mode_ == AppraiserMode::Testing) {
				pipeline_.Context().SetMode(ProcessingMode::kMeasuring);
			} else {
				pipeline_.Context().SetMode(ProcessingMode::kSetup);
			}
			CaptureAndProcessCameraFrame(pipeline_);

			camera_view_metrics_ = UpdateCameraView();
			if (camera_view_metrics_.is_hovered
				&& (mode_ == AppraiserMode::SelectTopTarget || mode_ == AppraiserMode::SelectBottomTarget)) {
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			}

			if (mode_ == AppraiserMode::SelectBottomTarget) {
				ImDrawList* draw = ImGui::GetWindowDrawList();
				const auto& context = pipeline_.Context();
				const float target_radius_px = context.CmToPx(context.target_diameter_cm_ / 2.0f);

				auto draw_cluster_indicator = [&](const ClusterResult& cluster) {
					ImVec2 center = {
						camera_view_metrics_.window_pos.x
							+ (float) cluster.center.x * camera_view_metrics_.frame_to_window_scale,
						camera_view_metrics_.window_pos.y
							+ (float) cluster.center.y * camera_view_metrics_.frame_to_window_scale};
					float
						radius =
						target_radius_px + target_radius_px * 2 * (float) cluster.size
							/ (float) context.max_cluster_size_px_;
					draw->AddCircle(center, radius, kClusterColor);
				};
				if (context.IsClusterValid(context.TopCluster())) {
					draw_cluster_indicator(context.TopCluster());
				}

				const auto is_top_cluster_valid = context.IsClusterValid(context.TopCluster());
				DrawTargetOverlay(draw,
					context,
					is_top_cluster_valid,
					context.TopTarget(),
					context.TopCluster(),
					camera_view_metrics_);

			}
			if (mode_ == AppraiserMode::Testing) {
				ImDrawList* draw = ImGui::GetWindowDrawList();
				const auto& context = pipeline_.Context();
				const float line_width = OverlayLineWidthPx(pipeline_.Context());
				const float target_radius_px = context.CmToPx(context.target_diameter_cm_ / 2.0f);

				auto draw_cluster_indicator = [&](const ClusterResult& cluster) {
					ImVec2 center = {
						camera_view_metrics_.window_pos.x
							+ (float) cluster.center.x * camera_view_metrics_.frame_to_window_scale,
						camera_view_metrics_.window_pos.y
							+ (float) cluster.center.y * camera_view_metrics_.frame_to_window_scale};
					float
						radius =
						target_radius_px + target_radius_px * 2 * (float) cluster.size
							/ (float) context.max_cluster_size_px_;
					draw->AddCircle(center, radius, kClusterColor);
				};
				if (context.IsClusterValid(context.TopCluster())) {
					draw_cluster_indicator(context.TopCluster());
				}
				if (context.IsClusterValid(context.BotCluster())) {
					draw_cluster_indicator(context.BotCluster());
				}

				DrawOverlayTargets(draw, context, camera_view_metrics_);

				const auto is_top_cluster_valid = context.IsClusterValid(context.TopCluster());
				const auto is_bot_cluster_valid = context.IsClusterValid(context.BotCluster());
				if (is_top_cluster_valid && is_bot_cluster_valid) {
					ImVec2 top_target = camera_view_metrics_.CameraFrameToWindowLoc(context.TopTargetLoc());
					ImVec2 bot_target = camera_view_metrics_.CameraFrameToWindowLoc(context.BotTargetLoc());
					draw->AddLine(top_target, bot_target, kTargetValidColor, line_width);
				}
			}
		}
		ImGui::EndGroup();
	}
}

void AppraiserWindow::UpdateToolbar(VideoProcessorPipeline& pipeline) {
	const auto& device_names = video_capture_.DeviceNames();

	// Camera Selection Combobox
	{
		const char* combo_preview = selected_device_ >= 0 && selected_device_ < device_names.size() ?
			device_names[selected_device_].c_str() :
			"No capture device detected";
		if (ImGui::BeginCombo("###Camera", combo_preview, 0)) {
			for (int i = 0; i < device_names.size(); ++i) {
				const bool is_selected = (selected_device_ == i);
				if (ImGui::Selectable(device_names[i].c_str(), is_selected))
					selected_device_ = i;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}
	ImGui::SameLine();
	constexpr const char* kStartVideoCaptureText = "Start Video Capture";
	if (selected_device_ >= 0 && selected_device_ < device_names.size()) {
		if (selected_device_ == video_capture_.CapturingDeviceId()) {
			if (ImGui::Button("Stop Video Capture")) {
				SetActiveCamera(VideoCapture::kNoDevice, pipeline);
			}
		} else {
			if (ImGui::Button(kStartVideoCaptureText)) {
				SetActiveCamera(selected_device_, pipeline);
			}
		}
	} else {
		ImGui::BeginDisabled();
		ImGui::Button(kStartVideoCaptureText);
		ImGui::EndDisabled();
	}
}

void AppraiserWindow::DrawTargetOverlay(ImDrawList* draw,
	const ProcessingContext& context,
	bool is_valid,
	const ClusterResult& target,
	const ClusterResult& cluster,
	CameraViewMetrics view_metrics) {

	const float kLineThickness = 2.0f;
	const float target_radius_px = context.CmToPx(context.target_diameter_cm_ / 2.0f);
	ImVec2 target_window_loc = view_metrics.CameraFrameToWindowLoc(target.center);
	draw->AddCircleFilled(target_window_loc,
		target_radius_px,
		is_valid ? kTargetValidColor : kTargetInvalidColor);
	draw->AddRect(
		view_metrics.CameraFrameToWindowLoc(target.bounds.min),
		view_metrics.CameraFrameToWindowLoc(target.bounds.max),
		kTargetValidColor, 0, 0, kLineThickness);
	if (!is_valid) {
		draw->AddRect(
			view_metrics.CameraFrameToWindowLoc(cluster.bounds.min),
			view_metrics.CameraFrameToWindowLoc(cluster.bounds.max),
			kTargetInvalidColor, 0, 0, kLineThickness);
	}
}

void AppraiserWindow::DrawOverlayTargets(ImDrawList* draw,
	const ProcessingContext& context,
	CameraViewMetrics view_metrics) {

	const auto is_top_cluster_valid = context.IsClusterValid(context.TopCluster());
	DrawTargetOverlay(draw,
		context,
		is_top_cluster_valid,
		context.TopTarget(),
		context.TopCluster(),
		view_metrics);

	const auto is_bot_cluster_valid = context.IsClusterValid(context.BotCluster());
	DrawTargetOverlay(draw,
		context,
		is_bot_cluster_valid,
		context.BotTarget(),
		context.BotCluster(),
		view_metrics);
}

CameraViewMetrics AppraiserWindow::UpdateCameraView() {
	const ImVec2 cursor = ImGui::GetCursorScreenPos();
	const Vec2i frame_size = video_capture_.FrameSize(video_capture_.CapturingDeviceId());
	const ImVec2 camera_size{(float) frame_size.x, (float) frame_size.y};
	const ImVec2 avail_size = ImGui::GetContentRegionAvail();

	float scale;
	ImVec2 render_size;
	if (camera_size.x / camera_size.y > avail_size.x / avail_size.y) {
		// fit width
		scale = avail_size.x / camera_size.x;
		render_size = {camera_size.x * scale, camera_size.y * scale};
	} else {
		// fit height
		scale = avail_size.y / camera_size.y;
		render_size = {camera_size.x * scale, camera_size.y * scale};
	}

	ImGui::Image(camera_render_tex_, render_size);

	return CameraViewMetrics{
		cursor,
		render_size,
		scale,
		ImGui::IsItemHovered()
	};
}

void AppraiserWindow::SetActiveCamera(VideoCapture::DeviceId device_id, VideoProcessorPipeline& pipeline) {
	if (video_capture_.Capturing()) {
		if (device_id == video_capture_.CapturingDeviceId())
			return;

		APP_INFO("Stopping capture on device {}", video_capture_.Name(video_capture_.CapturingDeviceId()));
		pipeline.StopCapture();
		video_capture_.EndCapture();
		SDL_DestroyTexture(camera_render_tex_);
		camera_render_tex_ = nullptr;
	}

	if (!video_capture_.IsDevice(device_id))
		return;

	APP_INFO("Starting capture on device {}", video_capture_.Name(video_capture_.CapturingDeviceId()));
	const CaptureError capture_result = video_capture_.BeginCapture(device_id);
	if (capture_result != CaptureError::kNone) {
		APP_ERROR("Could not begin capture for {}: {}",
			video_capture_.Name(device_id),
			CaptureErrorMsg(capture_result));
		return;
	}

	const auto frame_size = video_capture_.FrameSize(device_id);
	camera_render_tex_ = SDL_CreateTexture(app_window_->NativeRenderer(),
		SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STREAMING,
		frame_size.x, frame_size.y);
	if (!camera_render_tex_) {
		APP_ERROR("Error creating camera texture: {}", SDL_GetError());
	}

	mode_ = AppraiserMode::SelectTopTarget;

	video_capture_.PullFrame();
	pipeline.StartCapture(
		frame_size.x, frame_size.y,
		video_capture_.FrameBuffer());

	CaptureAndProcessCameraFrame(pipeline, false);
}

void AppraiserWindow::CaptureAndProcessCameraFrame(VideoProcessorPipeline& pipeline, bool pull_frame) {
	const Vec2i frame_size = video_capture_.FrameSize(video_capture_.CapturingDeviceId());
	const SDL_Rect rect{0, 0, frame_size.x, frame_size.y};
	const int pitch = frame_size.x * 3;
	if (pull_frame) {
		if (!video_capture_.NewFrameAvailable())
			return;

		video_capture_.PullFrame();
	}

	pipeline.ProcessFrame();

	int result = SDL_UpdateTexture(camera_render_tex_,
		&rect, video_capture_.FrameBuffer(), pitch);
	if (result) {
		APP_ERROR("Error updating camera {}({}) {}x{} texture: {}",
			video_capture_.Name(video_capture_.CapturingDeviceId()),
			video_capture_.CapturingDeviceId(),
			rect.w, rect.h,
			SDL_GetError());
	}
}

void AppraiserWindow::OnMouseButtonDown(const SDL_MouseButtonEvent& event) {
	switch (mode_) {
	case AppraiserMode::SelectTopTarget: {
		const Vec2i event_loc = {event.x, event.y};
		if (camera_view_metrics_.IsWindowLocInside(event_loc)) {
			auto frame_loc = camera_view_metrics_.WindowLocToCameraFrame(event_loc);
			pipeline_.Context().InitTopTarget(frame_loc);

			mode_ = AppraiserMode::SelectBottomTarget;
		}
		break;
	}
	case AppraiserMode::SelectBottomTarget: {
		const Vec2i event_loc = {event.x, event.y};
		if (camera_view_metrics_.IsWindowLocInside(event_loc)) {
			auto frame_loc = camera_view_metrics_.WindowLocToCameraFrame(event_loc);
			pipeline_.Context().InitBottomTarget(frame_loc);

			mode_ = AppraiserMode::Testing;
		}
		break;
	}
	}
}

void AppraiserWindow::OnMouseButtonUp(const SDL_MouseButtonEvent& event) {

}

}