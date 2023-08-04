#include "app.h"

#include "dpi_handler.h"
#include "gui.h"
#include "log.h"
#include "project_conf.h"
#include "resources.h"

#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer.h"
#include "imgui.h"

namespace Airheads {
	AppUniquePtr App::CreateApplication() {
		const std::string appTitle{"Airheads"};

		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
			APP_ERROR("Error: {}", SDL_GetError());
			return nullptr;
		}
		AppWindowUniquePtr windowPtr = std::make_unique<Airheads::AppWindow>(Airheads::AppWindow::Settings{appTitle});

		const char* userConfigPath = SDL_GetPrefPath(Conf::COMPANY_NAMESPACE.c_str(), Conf::APP_NAME.c_str());
		APP_INFO("User config path: {}", userConfigPath);

		return std::make_unique<App>(std::move(windowPtr), userConfigPath);
	}

	App::~App() {
		ImGui_ImplSDLRenderer_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();

		SDL_Quit();
	}

	void App::StopMainLoop() {
		m_shouldKeepLooping = false;
	}

	ExitStatus App::RunMainLoop() {
		if (m_exitStatus == ExitStatus::FAILURE) {
			return m_exitStatus;
		}

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io{ ImGui::GetIO() };

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable |
			ImGuiConfigFlags_ViewportsEnable;

		const std::string user_config_path{
			SDL_GetPrefPath(Conf::COMPANY_NAMESPACE.c_str(), Conf::APP_NAME.c_str())};
		APP_DEBUG("User config path: {}", user_config_path);

		// Absolute imgui.ini path to preserve settings independent of app location.
		static const std::string imgui_ini_filename{m_userConfigPath + "imgui.ini"};
		io.IniFilename = imgui_ini_filename.c_str();

		// ImGUI font
		const float font_scaling_factor{ DPIHandler::GetScale() };
		const float font_size{ 18.0F * font_scaling_factor };
		const std::string fontPath{Resources::GetFontPath("Manrope.ttf").generic_string()};

		io.Fonts->AddFontFromFileTTF(fontPath.c_str(), font_size);
		io.FontDefault = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), font_size);
		DPIHandler::SetGlobalFontScaling(&io);

		// Setup Platform/Renderer backends
		ImGui_ImplSDL2_InitForSDLRenderer(
			m_windowPtr->NativeWindow(), m_windowPtr->NativeRenderer());
		ImGui_ImplSDLRenderer_Init(m_windowPtr->NativeRenderer());
		Gui gui(m_windowPtr.get());

		m_shouldKeepLooping = true;
		while (m_shouldKeepLooping) {
			SDL_Event event{};
			while (SDL_PollEvent(&event) == 1) {
				ImGui_ImplSDL2_ProcessEvent(&event);

				if (event.type == SDL_QUIT) {
					StopMainLoop();
				}

				if (event.type == SDL_WINDOWEVENT &&
					event.window.windowID == SDL_GetWindowID(m_windowPtr->NativeWindow())) {
					on_event(event.window);
				}
			}

			// Start the Dear ImGui frame
			ImGui_ImplSDLRenderer_NewFrame();
			ImGui_ImplSDL2_NewFrame();
			ImGui::NewFrame();

			if (!m_minimized) {
				gui.Update();
			}

			// Rendering
			ImGui::Render();

			SDL_SetRenderDrawColor(m_windowPtr->NativeRenderer(), 100, 100, 100, 255);
			SDL_RenderClear(m_windowPtr->NativeRenderer());
			ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
			SDL_RenderPresent(m_windowPtr->NativeRenderer());
		}

		return m_exitStatus;
	}

	App::App(AppWindowUniquePtr platformWindowPtr, const char* userConfigPath) :
		m_windowPtr{ std::move(platformWindowPtr) },
		m_userConfigPath{ userConfigPath }
	{
		assert(m_windowPtr);
	}

	void App::on_event(const SDL_WindowEvent& event) {
		switch (event.event) {
		case SDL_WINDOWEVENT_CLOSE:
			StopMainLoop();
			return;

		case SDL_WINDOWEVENT_MINIMIZED:
			m_minimized = true;
			return;

		case SDL_WINDOWEVENT_SHOWN:
			m_minimized = false;
			return;
		}
	}

}