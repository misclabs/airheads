#pragma once

#include "vec.h"
#include "app_window.h"
#include "video_proc/video_proc.h"
#include "videoInput.h"
#include "imgui.h"
#include "SDL.h"

namespace Airheads {

struct CameraViewMetrics {
  ImVec2 window_pos;
  ImVec2 render_size;
  float frame_to_window_scale;
};

class AppraiserWindow {
 public:

  explicit AppraiserWindow(AppWindow* app_window) : app_window_(app_window) {}

  void Update(VideoProcessorPipeline& pipeline);

  [[nodiscard]] bool IsCameraActive() const noexcept { return active_camera_ != -1; }
  [[nodiscard]] Vec2i ActiveCameraFrameSize() noexcept;
  [[nodiscard]] size_t ActiveCameraFrameBufferSizeBytes() noexcept;

  bool is_ruler_visible_ = false;
  bool is_camera_mirrored_ = true;

 private:

  void UpdateToolbar(VideoProcessorPipeline& pipeline);
  void UpdateCalibrationView(VideoProcessorPipeline& pipeline);
  void UpdateTestingView(VideoProcessorPipeline& pipeline);
  CameraViewMetrics UpdateCameraView();
  void DrawOverlayTargets(ImDrawList *draw, const ProcessingContext& context, CameraViewMetrics view_metrics);

  void SetActiveCamera(int index, VideoProcessorPipeline& pipeline);
  void CaptureAndProcessCameraFrame(VideoProcessorPipeline& pipeline, unsigned char *pixels = nullptr);
  unsigned char *GetNextFramePixels();

  AppWindow *app_window_;

  videoInput video_input_;

  bool should_update_available_cameras_ = true;
  std::vector<std::string> camera_names_;

  int selected_camera_ = 0;
  int active_camera_ = -1;

  SDL_Texture *camera_render_tex_ = nullptr;

  const ImU32 kClusterColor = IM_COL32(0, 255, 255, 255 / 3 * 2);
  const ImU32 kTargetValidColor = IM_COL32(0, 255, 0, 255 / 3 * 2);
  const ImU32 kTargetInvalidColor = IM_COL32(255, 0, 0, 255 / 3 * 2);

};

inline Vec2i AppraiserWindow::ActiveCameraFrameSize() noexcept {
  if (active_camera_ == -1)
    return {0, 0};

  return {video_input_.getWidth(active_camera_), video_input_.getHeight(active_camera_)};
}

inline size_t AppraiserWindow::ActiveCameraFrameBufferSizeBytes() noexcept {
  if (active_camera_ == -1)
    return 0;

  return video_input_.getSize(active_camera_);
}

}