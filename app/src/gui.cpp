#include "gui.h"

#include "app.h"
#include "app_window.h"
#include "log.h"

#include "imgui.h"

#include <cassert>

namespace Airheads {

void DrawRuler(ImDrawList *draw, ImVec2 left_side, float window_pixels_per_cm, ImU32 color, float line_width) {
  ImVec2 right_side = {left_side.x + window_pixels_per_cm, left_side.y};

  draw->AddLine({left_side.x, left_side.y + 6}, {left_side.x, left_side.y - 6}, color, line_width);
  draw->AddLine({right_side.x, right_side.y + 6}, {right_side.x, right_side.y - 6}, color, line_width);
  draw->AddLine(left_side, right_side, color, line_width);
  draw->AddText({right_side.x + 3, right_side.y}, color, "1cm");
}

template<typename ContentCallback>
void ImGuiWindow(const char *name, ContentCallback content, bool *open = nullptr, ImGuiWindowFlags flag = 0) {
  if (open != nullptr && !*open)
    return;

  if (ImGui::Begin(name, open))
    content();

  ImGui::End();
}

Gui::Gui(App *app, AppWindow *app_window)
    : saturation_map_renderer_(app_window->NativeRenderer(), &processor_pipeline_.Context().saturation_map_),
      value_map_renderer_(app_window->NativeRenderer(), &processor_pipeline_.Context().value_map_),
      cluster_map_renderer_(app_window->NativeRenderer(), &processor_pipeline_.Context().cluster_map_) {
  assert(app);
  assert(app_window);

  app_ = app;
  app_window_ = app_window;

  LoadProcessors(processor_pipeline_);
}

void Gui::Update() {
  if (should_update_available_cameras_) {
    should_update_available_cameras_ = false;
    camera_names_ = videoInput::getDeviceList();
  }

  ImGui::DockSpaceOverViewport();

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
      ImGui::MenuItem("Ruler", nullptr, &is_ruler_visible_);
      ImGui::Separator();
      ImGui::MenuItem("Saturation Map", nullptr, &is_saturation_map_visible_);
      ImGui::MenuItem("Value Map", nullptr, &is_value_map_visible_);
      ImGui::MenuItem("Cluster Map", nullptr, &is_cluster_map_visible_);
      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }

  ImGuiWindow("Pipeline Config", [this]() -> void { UpdateConfigContent(); }, &is_pipeline_config_visible_);

  ImGui::Begin("Actually the Real App", nullptr);
  UpdateMainGuiContent();
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

void Gui::UpdateConfigContent() {
  ImGuiWindow("Pipeline Config", [this]() -> void {
    ImGui::Checkbox("Mirror Camera", &is_camera_mirrored_);
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
      ImGui::SetTooltip("Display the camera feed as a mirror image?");

    ImGui::Checkbox("Show Ruler", &is_ruler_visible_);
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
      ImGui::SetTooltip("Display the ruler overlay on the camera feed?");

    ImGui::SliderFloat("pixels/cm", &processor_pipeline_.Context().frame_pixels_per_cm_, 3, 100);
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
      ImGui::SetTooltip("Approximate pixels per cm at the distances dots are from the camera.");

    ImGui::Separator();

    processor_pipeline_.UpdateConfigGui();
  }, &is_pipeline_config_visible_);
}

void Gui::UpdateStatsContent() {
  ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);

  if (active_camera_ != -1) {
    ImGui::Separator();
    ImGui::Text("Camera Input");
    ImVec2 cameraSize{(float) video_input_.getWidth(active_camera_), (float) video_input_.getHeight(active_camera_)};
    ImGui::Text("Width:%d Height:%d", (int) cameraSize.x, (int) cameraSize.y);
    ImGui::Text("Buffer size (bytes):%d", video_input_.getSize(active_camera_));

    processor_pipeline_.UpdateStatsGui();
  } else {
    ImGui::Text("No camera active");
  }
}

void Gui::UpdateMainGuiContent() {
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
        SetActiveCamera(-1);
      }
    } else {
      if (ImGui::Button(kStartVideoCaptureText)) {
        SetActiveCamera(selected_camera_);
      }
    }
  } else {
    ImGui::BeginDisabled();
    ImGui::Button(kStartVideoCaptureText);
    ImGui::EndDisabled();
  }

  if (active_camera_ != -1) {
    UpdateCameraTexture();

    ImVec2 camera_size{(float) video_input_.getWidth(active_camera_), (float) video_input_.getHeight(active_camera_)};
    ImVec2 avail_size = ImGui::GetContentRegionAvail();
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
    ImVec2 cursor = ImGui::GetCursorScreenPos();

    ImGui::Image(camera_render_tex_, render_size);

    ImDrawList *draw = ImGui::GetWindowDrawList();

    const ImU32 cluster_color = IM_COL32(0, 255, 255, 255 / 3 * 2);
    const ImU32 target_valid_color = IM_COL32(0, 255, 0, 255 / 3 * 2);
    const ImU32 target_invalid_color = IM_COL32(255, 0, 0, 255 / 3 * 2);

    const auto &context = processor_pipeline_.Context();
    const float target_radius_px = context.CmToPx(context.target_diameter_cm_ / 2.0f);

    auto draw_cluster_indicator = [&](const ClusterResult &cluster) {
      ImVec2 center = {cursor.x + (float) cluster.center.x * scale, cursor.y + (float) cluster.center.y * scale};
      float radius = target_radius_px + target_radius_px * 2 * (float) cluster.size / (float) context.max_cluster_size_px_;
      draw->AddCircle(center, radius, cluster_color);
    };
    if (context.IsClusterValid(context.TopCluster())) {
      draw_cluster_indicator(context.TopCluster());
    }
    if (context.IsClusterValid(context.BotCluster())) {
      draw_cluster_indicator(context.BotCluster());
    }

    float line_width = processor_pipeline_.Context().target_diameter_cm_ / 3.0f;
    const auto is_top_cluster_valid = context.IsClusterValid(context.TopCluster());
    const auto is_bot_cluster_valid = context.IsClusterValid(context.BotCluster());
    ImVec2 top_target = {
        cursor.x + (float) context.TopTargetLoc().x * scale,
        cursor.y + (float) context.TopTargetLoc().y * scale};
    draw->AddCircleFilled(top_target, target_radius_px, is_top_cluster_valid ? target_valid_color : target_invalid_color);

    ImVec2 bot_target = {
        cursor.x + (float) context.BotTargetLoc().x * scale,
        cursor.y + (float) context.BotTargetLoc().y * scale};
    draw->AddCircleFilled(bot_target, target_radius_px, is_bot_cluster_valid ? target_valid_color : target_invalid_color);

    if (is_top_cluster_valid && is_bot_cluster_valid) {
      draw->AddLine(top_target, bot_target, target_valid_color, line_width);
    }

    if (is_ruler_visible_) {
      ImU32 ruler_color = IM_COL32(255, 255, 0, 255 / 3 * 2);
      ImVec2 center_pt = {
          cursor.x + render_size.x / 2,
          cursor.y + render_size.y / 2
      };
      float cm_in_window_pixels = context.frame_pixels_per_cm_ * scale;
      DrawRuler(draw, center_pt, cm_in_window_pixels, ruler_color, line_width);
    }
  }
}

void Gui::SetActiveCamera(int index) {
  if (index == active_camera_)
    return;

  if (active_camera_ >= 0) {
    processor_pipeline_.StopCapture();
    video_input_.stopDevice(active_camera_);
    SDL_DestroyTexture(camera_render_tex_);
    camera_render_tex_ = nullptr;
  }

  active_camera_ = index;
  if (active_camera_ == -1)
    return;

  video_input_.setupDevice(active_camera_);
  camera_render_tex_ = SDL_CreateTexture(app_window_->NativeRenderer(),
                                         SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STREAMING,
                                         video_input_.getWidth(active_camera_), video_input_.getHeight(active_camera_)
  );
  if (!camera_render_tex_) {
    APP_ERROR("Error creating camera texture: {}", SDL_GetError());
  }

  auto pixels = GetNextFramePixels();
  processor_pipeline_.StartCapture(
      video_input_.getWidth(active_camera_),
      video_input_.getHeight(active_camera_),
      pixels);

  UpdateCameraTexture(pixels);
}

unsigned char *Gui::GetNextFramePixels() {
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

void Gui::UpdateCameraTexture(unsigned char *pixels) {
  const SDL_Rect rect{0, 0, video_input_.getWidth(active_camera_), video_input_.getHeight(active_camera_)};
  const int pitch = rect.w * 3;
  if (pixels == nullptr)
    pixels = GetNextFramePixels();

  processor_pipeline_.ProcessFrame();

  int result = SDL_UpdateTexture(camera_render_tex_,
                                 &rect, pixels, pitch);
  if (result) {
    APP_ERROR("Error updating camera texture: {}", SDL_GetError());
  }
}

}