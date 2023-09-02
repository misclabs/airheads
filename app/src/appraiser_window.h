#pragma once

#include "vec.h"
#include "app_window.h"
#include "video_proc/video_proc.h"
#include "video_capture.h"
#include "imgui.h"
#include "SDL.h"

namespace Airheads {

struct CameraViewMetrics {
	ImVec2 window_pos;
	ImVec2 render_size;
	float frame_to_window_scale = 1.0f;
	bool is_hovered = false;

	[[nodiscard]] ImVec2 CameraFrameToWindowLoc(Vec2i frame_loc) const;
	[[nodiscard]] Vec2i WindowLocToCameraFrame(Vec2i window_loc) const;
	[[nodiscard]] bool IsWindowLocInside(Vec2i window_loc) const;
};

enum class AppraiserMode {
	Idle,
	SelectTopTarget,
	SelectBottomTarget,
	Testing,
};

class AppraiserWindow {
public:

	AppraiserWindow(AppWindow* app_window, VideoCapture& video_capture, VideoProcessorPipeline& pipeline)
		: app_window_(app_window), video_capture_{video_capture}, pipeline_{pipeline} {}

	void OnMouseButtonDown(const SDL_MouseButtonEvent& event);
	void OnMouseButtonUp(const SDL_MouseButtonEvent& event);

	void Update();

	bool is_ruler_visible_ = false;

private:

	void UpdateToolbar(VideoProcessorPipeline& pipeline);
	CameraViewMetrics UpdateCameraView();
	void DrawOverlayTargets(ImDrawList* draw,
		const ProcessingContext& context,
		CameraViewMetrics view_metrics);
	void DrawTargetOverlay(ImDrawList* draw,
		const ProcessingContext& context,
		bool is_valid,
		const ClusterResult& target,
		const ClusterResult& cluster,
		CameraViewMetrics view_metrics);

	void SetActiveCamera(int device_id, VideoProcessorPipeline& pipeline);
	void CaptureAndProcessCameraFrame(VideoProcessorPipeline& pipeline, bool pull_frame = true);

	AppWindow* app_window_;

	VideoCapture& video_capture_;
	VideoProcessorPipeline& pipeline_;

	VideoCapture::DeviceId selected_device_ = 0;

	SDL_Texture* camera_render_tex_ = nullptr;

	CameraViewMetrics camera_view_metrics_;

	AppraiserMode mode_ = AppraiserMode::Idle;

	const ImU32 kClusterColor = IM_COL32(0, 255, 255, 255 / 3 * 2);
	const ImU32 kTargetValidColor = IM_COL32(0, 255, 0, 255 / 3 * 2);
	const ImU32 kTargetInvalidColor = IM_COL32(255, 0, 0, 255 / 3 * 2);

};

}