#pragma once

#include "app_window.h"

#include "SDL.h"

#include <string>
#include <memory>

namespace Airheads {

	enum class ExitStatus : int { SUCCESS = 0, FAILURE = 1 };

	class App {
	public:

		using UniquePtr = std::unique_ptr<App>;

		static UniquePtr CreateApplication();

		App(AppWindow::UniquePtr platformWindowPtr, const char* userConfigPath);
		~App();

		void StopMainLoop();
		const std::string& UserConfigPath() const {
			return g_userConfigPath;
		}

		ExitStatus RunMainLoop();

		AppWindow* AppWindow() {
			return m_windowPtr.get();
		}

	private:
		void on_event(const SDL_WindowEvent& event);

		bool g_shouldKeepLooping{ true };
		bool m_minimized{ false };
		ExitStatus m_exitStatus{ ExitStatus::SUCCESS };
		AppWindow::UniquePtr m_windowPtr{ nullptr };
		std::string g_userConfigPath;
	};

}
