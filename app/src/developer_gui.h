#pragma once

#include "video_proc/video_proc.h"
#include "gui_mat_renderer.h"
#include "appraiser_window.h"
#include "imgui.h"

namespace Airheads {

class App;
class AppWindow;

class DeveloperGui {
public:

	DeveloperGui(App* app, AppWindow* app_window);

	void OnKeyDown(const SDL_KeyboardEvent& event);
	void OnKeyUp(const SDL_KeyboardEvent& event);
	void OnMouseButtonDown(const SDL_MouseButtonEvent& event);
	void OnMouseButtonUp(const SDL_MouseButtonEvent& event);

	void Update();

private:

	void UpdateStatsContent();
	void UpdateConfigContent();

	App* app_;
	AppWindow* app_window_;

	VideoProcessorPipeline processor_pipeline_;

	bool is_dev_ui_enabled_ = false;
	bool is_stats_visible_ = true;
	bool is_pipeline_config_visible_ = true;
	bool is_saturation_map_visible_ = true;
	bool is_value_map_visible_ = true;
	bool is_cluster_map_visible_ = true;

	AppraiserWindow appraiser_window_;

	GuiMatRenderer saturation_map_renderer_;
	GuiMatRenderer value_map_renderer_;
	GuiMatRenderer cluster_map_renderer_;

};

}