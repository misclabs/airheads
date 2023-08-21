#include "app.h"

#include "dpi_handler.h"
#include "developer_gui.h"
#include "log.h"
#include "project_conf.h"
#include "project_version.h"
#include "git_version.h"
#include "resources.h"

#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include "imgui.h"

namespace Airheads {

AppUniquePtr App::CreateApplication() {
  const std::string app_title{"Airheads"};

  APP_INFO("Airheads: {}", kGitHash);
  APP_INFO("Libraries:");
  APP_INFO("  OpenCV {}", kOpenCVVersion);
  APP_INFO("  SDL {}", kSdl2Version);
  APP_INFO("  Dear ImGui {}", kImguiVersion);
  APP_INFO("  spdlog {}", kSpdlogVersion);
  APP_INFO("  fmt {}", kFmtVersion);

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
    APP_ERROR("Error: {}", SDL_GetError());
    return nullptr;
  }
  AppWindowUniquePtr window_ptr = std::make_unique<Airheads::AppWindow>(Airheads::AppWindow::Settings{app_title});

  const char *user_config_path = SDL_GetPrefPath(Conf::COMPANY_NAMESPACE.c_str(), Conf::APP_NAME.c_str());
  APP_INFO("User config path: {}", user_config_path);

  return std::make_unique<App>(std::move(window_ptr), user_config_path);
}

App::~App() {
  ImGui_ImplSDLRenderer2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_Quit();
}

void App::StopMainLoop() {
  should_keep_looping_ = false;
}

void App::RunMainLoop() {
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io{ImGui::GetIO()};

  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable |
      ImGuiConfigFlags_ViewportsEnable;

  // Absolute imgui.ini path to preserve settings independent of app location.
  static const std::string imgui_ini_filename{user_config_path_ + "imgui.ini"};
  io.IniFilename = imgui_ini_filename.c_str();

  // ImGUI font
  const float font_scaling_factor{DPIHandler::GetScale()};
  const float font_size{18.0F * font_scaling_factor};
  const std::string font_path{Resources::GetFontPath("Manrope.ttf").generic_string()};

  io.Fonts->AddFontFromFileTTF(font_path.c_str(), font_size);
  io.FontDefault = io.Fonts->AddFontFromFileTTF(font_path.c_str(), font_size);
  DPIHandler::SetGlobalFontScaling(&io);

  // Setup Platform/Renderer backends
  ImGui_ImplSDL2_InitForSDLRenderer(
      window_ptr_->NativeWindow(), window_ptr_->NativeRenderer());
  ImGui_ImplSDLRenderer2_Init(window_ptr_->NativeRenderer());
  DeveloperGui developer_gui(this, window_ptr_.get());

  should_keep_looping_ = true;
  int force_update_frames = 3;
  while (should_keep_looping_) {

    const auto kProcessEvent = [this](SDL_Event &event) {
      ImGui_ImplSDL2_ProcessEvent(&event);

      if (event.type == SDL_QUIT) {
        StopMainLoop();
      }

      if (event.type == SDL_WINDOWEVENT &&
          event.window.windowID == SDL_GetWindowID(window_ptr_->NativeWindow())) {
        OnEvent(event.window);
      }
    };

    // Process events, possibly waiting if event queue is empty
    {
      SDL_Event event{};
      if (force_update_frames == 0) {
        if (!video_capture_.Capturing()) {
          if (SDL_WaitEvent(&event) == 1) {
            kProcessEvent(event);
          }
        } else {
          constexpr int kTimeoutMs = 1000/144;
          int result = 0;
          while (!video_capture_.NewFrameAvailable() && result == 0) {
            result = SDL_WaitEventTimeout(&event, kTimeoutMs);
          };
          if (result == 1)
            kProcessEvent(event);
        }
        force_update_frames = 1;
      } else {
        --force_update_frames;
      }

      // Process all remaining queued events
      {
        while (SDL_PollEvent(&event) == 1) {
          kProcessEvent(event);
        }
      }
    }

    // Start the Dear ImGui frame
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    if (!minimized_) {
      developer_gui.Update();
    }

    // Rendering
    ImGui::Render();

    SDL_SetRenderDrawColor(window_ptr_->NativeRenderer(), 100, 100, 100, 255);
    SDL_RenderClear(window_ptr_->NativeRenderer());
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(window_ptr_->NativeRenderer());
  }
}

App::App(AppWindowUniquePtr window_ptr, const char *user_config_path) :
    window_ptr_{std::move(window_ptr)},
    user_config_path_{user_config_path} {
  assert(window_ptr_);
}

void App::OnEvent(const SDL_WindowEvent &event) {
  switch (event.event) {
    case SDL_WINDOWEVENT_CLOSE: StopMainLoop();
      return;

    case SDL_WINDOWEVENT_MINIMIZED: minimized_ = true;
      return;

    case SDL_WINDOWEVENT_SHOWN: minimized_ = false;
      return;
  }
}

}