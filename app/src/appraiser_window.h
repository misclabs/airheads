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
};

class AppraiserWindow {
 public:

  AppraiserWindow(AppWindow *app_window, VideoCapture &video_capture)
      : app_window_(app_window), video_capture_{video_capture} {}

  void Update(VideoProcessorPipeline &pipeline);

  bool is_ruler_visible_ = false;

 private:

  void UpdateToolbar(VideoProcessorPipeline &pipeline);
  void UpdateCalibrationView(VideoProcessorPipeline &pipeline);
  void UpdateTestingView(VideoProcessorPipeline &pipeline);
  CameraViewMetrics UpdateCameraView();
  void DrawOverlayTargets(ImDrawList *draw, const ProcessingContext &context, CameraViewMetrics view_metrics);

  void SetActiveCamera(int device_id, VideoProcessorPipeline &pipeline);
  void CaptureAndProcessCameraFrame(VideoProcessorPipeline &pipeline, bool pull_frame = true);

  AppWindow *app_window_;

  VideoCapture &video_capture_;

  VideoCapture::DeviceId selected_device_ = 0;

  SDL_Texture *camera_render_tex_ = nullptr;

  const ImU32 kClusterColor = IM_COL32(0, 255, 255, 255 / 3 * 2);
  const ImU32 kTargetValidColor = IM_COL32(0, 255, 0, 255 / 3 * 2);
  const ImU32 kTargetInvalidColor = IM_COL32(255, 0, 0, 255 / 3 * 2);

};

}