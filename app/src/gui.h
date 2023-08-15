#pragma once

#include "video_proc/video_proc.h"
#include "gui_mat_renderer.h"

#include "videoInput.h"
#include "SDL.h"
#include "imgui.h"

namespace Airheads {

class App;
class AppWindow;

class Gui {
 public:

  Gui(App *app, AppWindow *app_window);

  void Update();

 private:

  void SetActiveCamera(int index);
  unsigned char *GetNextFramePixels();
  void UpdateCameraTexture(unsigned char *pixels = nullptr);
  void UpdateStatsContent();
  void UpdateMainGuiContent();

  App *app_;
  AppWindow *app_window_;
  videoInput video_input_;

  bool should_update_available_cameras_ = true;
  std::vector<std::string> camera_names_;

  int selected_camera_ = 0;
  int active_camera_ = -1;
  bool is_camera_mirrored_ = true;

  SDL_Texture *camera_render_tex_ = nullptr;

  VideoProcessorPipeline processor_pipeline_;

  bool is_stats_visible_ = true;
  bool is_pipeline_config_visible_ = true;
  bool is_saturation_map_visible_ = true;
  bool is_value_map_visible_ = true;
  bool is_cluster_map_visible_ = true;
  bool is_ruler_visible_ = false;

  GuiMatRenderer saturation_map_renderer_;
  GuiMatRenderer value_map_renderer_;
  GuiMatRenderer cluster_map_renderer_;
  void UpdateConfigContent();
};

}