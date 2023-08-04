#pragma once

#include "SDL.h"

#include <string>
#include <memory>

namespace Airheads {

	class AppWindow;
	using AppWindowUniquePtr = std::unique_ptr<AppWindow>;

	class AppWindow {
	public:

		struct Settings {
			std::string title;
			const int width{ 1280 };
			const int height{ 720 };
		};

		explicit AppWindow(const Settings& settings);
		~AppWindow();

		AppWindow(const AppWindow&) = delete;
		AppWindow(AppWindow&&) = delete;
		AppWindow& operator=(AppWindow other) = delete;
		AppWindow& operator=(AppWindow&& other) = delete;

		[[nodiscard]] SDL_Window* NativeWindow() const;
		[[nodiscard]] SDL_Renderer* NativeRenderer() const;

	private:
		SDL_Window* m_window{ nullptr };
		SDL_Renderer* m_renderer{ nullptr };
	};

}
