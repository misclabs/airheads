#pragma once

#include "app_window.h"

#include "SDL.h"

#include <string>
#include <memory>

namespace Airheads {

	enum class ExitStatus : int { SUCCESS = 0, FAILURE = 1 };

	class App;
	using AppUniquePtr = std::unique_ptr<App>;

	class App {
	public:

		static AppUniquePtr CreateApplication();

		App(AppWindowUniquePtr platformWindowPtr, const char* userConfigPath);
		~App();

		void StopMainLoop();
		const std::string& UserConfigPath() const {
			return m_userConfigPath;
		}

		ExitStatus RunMainLoop();

		AppWindow* AppWindow() {
			return m_windowPtr.get();
		}

	private:
		void on_event(const SDL_WindowEvent& event);

		bool m_shouldKeepLooping{ true };
		bool m_minimized{ false };
		ExitStatus m_exitStatus{ ExitStatus::SUCCESS };
		AppWindowUniquePtr m_windowPtr{ nullptr };
		std::string m_userConfigPath;
	};

}
