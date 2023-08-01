#pragma once

#include "app_window.h"

#include "imgui.h"
#include "SDL.h"

namespace Airheads {

	struct WindowSize {
		int width;
		int height;
	};

	class DPIHandler {
	public:
		[[nodiscard]] static float GetScale();

		[[nodiscard]] static WindowSize GetDpiAwareWindowSize(const AppWindow::Settings& settings);

		static void SetRenderScale(SDL_Renderer* renderer);
		static void SetGlobalFontScaling(ImGuiIO* io);
	};

}
