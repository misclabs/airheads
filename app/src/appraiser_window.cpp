#include "appraiser_window.h"
#include "log.h"
#include "imgui.h"

namespace Airheads {

static inline float OverlayLineWidthPx(const ProcessingContext &context) {
  float line_width = context.target_diameter_cm_ / 3.0f;
  return context.CmToPx(line_width);
}

static ImVec2 CameraFrameToWindowLoc(CameraViewMetrics view_metrics, cv::Point frame_loc) {
  return {
      view_metrics.window_pos.x + (float) frame_loc.x * view_metrics.frame_to_window_scale,
      view_metrics.window_pos.y + (float) frame_loc.y * view_metrics.frame_to_window_scale};
}

static void DrawRuler(ImDrawList *draw, ImVec2 left_side, float window_pixels_per_cm, ImU32 color, float line_width) {
  ImVec2 right_side = {left_side.x + window_pixels_per_cm, left_side.y};

  draw->AddLine({left_side.x, left_side.y + 6}, {left_side.x, left_side.y - 6}, color, line_width);
  draw->AddLine({right_side.x, right_side.y + 6}, {right_side.x, right_side.y - 6}, color, line_width);
  draw->AddLine(left_side, right_side, color, line_width);
  draw->AddText({right_side.x + 3, right_side.y}, color, "1cm");
}

void AppraiserWindow::Update(VideoProcessorPipeline &pipeline) {
  if (should_update_available_cameras_) {
    should_update_available_cameras_ = false;
    camera_names_ = videoInput::getDeviceList();
  }

  ImGui::Begin("Appraiser View", nullptr);
  UpdateToolbar(pipeline);

  if (IsCameraActive()) {
    if (ImGui::BeginTabBar("##ModeSelect", ImGuiTabBarFlags_None)) {
      if (ImGui::BeginTabItem("Calibration")) {
        UpdateCalibrationView(pipeline);
        ImGui::EndTabItem();
      }
      if (ImGui::BeginTabItem("Test")) {
        UpdateTestingView(pipeline);
        ImGui::EndTabItem();
      }
      ImGui::EndTabBar();
    }
  }
  ImGui::End();

}

void AppraiserWindow::UpdateToolbar(VideoProcessorPipeline &pipeline) {
  // Camera Selection Combobox
  {
    const char *combo_preview = selected_camera_ >= 0 && selected_camera_ < camera_names_.size() ?
                                camera_names_[selected_camera_].c_str() :
                                "No camera detected";
    if (ImGui::BeginCombo("###Camera", combo_preview, 0)) {
      for (int i = 0; i < camera_names_.size(); ++i) {
        const bool is_selected = (selected_camera_ == i);
        if (ImGui::Selectable(camera_names_[i].c_str(), is_selected))
          selected_camera_ = i;

        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
        if (is_selected)
          ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }
  }
  ImGui::SameLine();
  constexpr const char *kStartVideoCaptureText = "Start Video Capture";
  if (selected_camera_ >= 0 && selected_camera_ < camera_names_.size()) {
    if (selected_camera_ == active_camera_) {
      if (ImGui::Button("Stop Video Capture")) {
        SetActiveCamera(-1, pipeline);
      }
    } else {
      if (ImGui::Button(kStartVideoCaptureText)) {
        SetActiveCamera(selected_camera_, pipeline);
      }
    }
  } else {
    ImGui::BeginDisabled();
    ImGui::Button(kStartVideoCaptureText);
    ImGui::EndDisabled();
  }
}

void AppraiserWindow::UpdateCalibrationView(VideoProcessorPipeline &pipeline) {
  pipeline.Context().SetMode(ProcessingMode::kCalibration);
  pipeline.Context().ResetOutput();
  CaptureAndProcessCameraFrame(pipeline);

  const auto view_metrics = UpdateCameraView();

  ImDrawList *draw = ImGui::GetWindowDrawList();
  const auto &context = pipeline.Context();
  const float line_width = OverlayLineWidthPx(pipeline.Context());

  DrawOverlayTargets(draw, context, view_metrics);

  if (is_ruler_visible_) {
    ImU32 ruler_color = IM_COL32(255, 255, 0, 255 / 3 * 2);
    ImVec2 center_pt = {
        view_metrics.window_pos.x + view_metrics.render_size.x / 2,
        view_metrics.window_pos.y + view_metrics.render_size.y / 2
    };
    float cm_in_window_pixels = context.frame_pixels_per_cm_ * view_metrics.frame_to_window_scale;
    DrawRuler(draw, center_pt, cm_in_window_pixels, ruler_color, line_width);
  }
}

void AppraiserWindow::UpdateTestingView(VideoProcessorPipeline &pipeline) {
  pipeline.Context().SetMode(ProcessingMode::kTesting);
  CaptureAndProcessCameraFrame(pipeline);

  const auto view_metrics = UpdateCameraView();

  ImDrawList *draw = ImGui::GetWindowDrawList();
  const auto &context = pipeline.Context();
  const float line_width = OverlayLineWidthPx(pipeline.Context());
  const float target_radius_px = context.CmToPx(context.target_diameter_cm_ / 2.0f);

  auto draw_cluster_indicator = [&](const ClusterResult &cluster) {
    ImVec2 center = {
        view_metrics.window_pos.x + (float) cluster.center.x * view_metrics.frame_to_window_scale,
        view_metrics.window_pos.y + (float) cluster.center.y * view_metrics.frame_to_window_scale};
    float
        radius = target_radius_px + target_radius_px * 2 * (float) cluster.size / (float) context.max_cluster_size_px_;
    draw->AddCircle(center, radius, kClusterColor);
  };
  if (context.IsClusterValid(context.TopCluster())) {
    draw_cluster_indicator(context.TopCluster());
  }
  if (context.IsClusterValid(context.BotCluster())) {
    draw_cluster_indicator(context.BotCluster());
  }

  DrawOverlayTargets(draw, context, view_metrics);

  const auto is_top_cluster_valid = context.IsClusterValid(context.TopCluster());
  const auto is_bot_cluster_valid = context.IsClusterValid(context.BotCluster());
  if (is_top_cluster_valid && is_bot_cluster_valid) {
    ImVec2 top_target = CameraFrameToWindowLoc(view_metrics, context.TopTargetLoc());
    ImVec2 bot_target = CameraFrameToWindowLoc(view_metrics, context.BotTargetLoc());
    draw->AddLine(top_target, bot_target, kTargetValidColor, line_width);
  }
}

void AppraiserWindow::DrawOverlayTargets(ImDrawList *draw,
                                         const ProcessingContext &context,
                                         CameraViewMetrics view_metrics) {
  const auto is_top_cluster_valid = context.IsClusterValid(context.TopCluster());
  const auto is_bot_cluster_valid = context.IsClusterValid(context.BotCluster());
  const float target_radius_px = context.CmToPx(context.target_diameter_cm_ / 2.0f);
  ImVec2 top_target = CameraFrameToWindowLoc(view_metrics, context.TopTargetLoc());
  draw->AddCircleFilled(top_target, target_radius_px, is_top_cluster_valid ? kTargetValidColor : kTargetInvalidColor);

  ImVec2 bot_target = CameraFrameToWindowLoc(view_metrics, context.BotTargetLoc());
  draw->AddCircleFilled(bot_target, target_radius_px, is_bot_cluster_valid ? kTargetValidColor : kTargetInvalidColor);
}

CameraViewMetrics AppraiserWindow::UpdateCameraView() {
  const ImVec2 cursor = ImGui::GetCursorScreenPos();
  const ImVec2
      camera_size{(float) video_input_.getWidth(active_camera_), (float) video_input_.getHeight(active_camera_)};
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

  return CameraViewMetrics{cursor, render_size, scale};
}

void AppraiserWindow::SetActiveCamera(int index, VideoProcessorPipeline &pipeline) {
  if (index == active_camera_)
    return;

  if (active_camera_ >= 0) {
    APP_INFO("Stopping capture on device {}", video_input_.getDeviceName(active_camera_));
    pipeline.StopCapture();
    video_input_.stopDevice(active_camera_);
    SDL_DestroyTexture(camera_render_tex_);
    camera_render_tex_ = nullptr;
  }

  active_camera_ = index;
  if (active_camera_ == -1)
    return;

  APP_INFO("Starting capture on device {}", video_input_.getDeviceName(active_camera_));
  if (!video_input_.setupDevice(active_camera_)) {
    APP_ERROR("Failed to setup device {}", video_input_.getDeviceName(active_camera_));
    active_camera_ = -1;
    return;
  }
  video_input_.setAutoReconnectOnFreeze(active_camera_, true, 60);

  camera_render_tex_ = SDL_CreateTexture(app_window_->NativeRenderer(),
                                         SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STREAMING,
                                         video_input_.getWidth(active_camera_), video_input_.getHeight(active_camera_)
  );
  if (!camera_render_tex_) {
    APP_ERROR("Error creating camera texture: {}", SDL_GetError());
  }

  auto pixels = GetNextFramePixels();
  pipeline.StartCapture(
      video_input_.getWidth(active_camera_),
      video_input_.getHeight(active_camera_),
      pixels);

  CaptureAndProcessCameraFrame(pipeline, pixels);
}

void AppraiserWindow::CaptureAndProcessCameraFrame(VideoProcessorPipeline &pipeline, unsigned char *pixels) {
  const SDL_Rect rect{0, 0, video_input_.getWidth(active_camera_), video_input_.getHeight(active_camera_)};
  const int pitch = rect.w * 3;
  if (pixels == nullptr) {
    if (!video_input_.isDeviceSetup(active_camera_)) {
      APP_ERROR("CaptureAndProcessCameraFrame device {}({}) is not setup!",
                video_input_.getDeviceName(active_camera_), active_camera_);
      return;
    }

    if (!video_input_.isFrameNew(active_camera_))
      return;

    pixels = GetNextFramePixels();
  }

  pipeline.ProcessFrame();

  int result = SDL_UpdateTexture(camera_render_tex_,
                                 &rect, pixels, pitch);
  if (result) {
    APP_ERROR("Error updating camera {}({}) {}x{} texture: {}",
              video_input_.getDeviceName(active_camera_), active_camera_,
              rect.w, rect.h,
              SDL_GetError());
  }
}

unsigned char *AppraiserWindow::GetNextFramePixels() {
  unsigned char *pixels = video_input_.getPixels(active_camera_, false, true);
  if (is_camera_mirrored_) {
    const int frame_width = video_input_.getWidth(active_camera_);
    const int frame_height = video_input_.getHeight(active_camera_);
    for (int y = 0; y < frame_height; ++y) {
      for (int x = 0; x < frame_width - x - 1; ++x) {
        int row_index = y * frame_width * 3;
        int left_index = x * 3;
        int right_index = (frame_width - x - 1) * 3;
        std::swap<unsigned char>(pixels[row_index + left_index], pixels[row_index + right_index]);
        std::swap<unsigned char>(pixels[row_index + left_index + 1], pixels[row_index + right_index + 1]);
        std::swap<unsigned char>(pixels[row_index + left_index + 2], pixels[row_index + right_index + 2]);
      }
    }
  }

  return pixels;
}

}