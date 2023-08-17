#include "app_window.h"

#include "dpi_handler.h"
#include "log.h"

namespace Airheads {

AppWindow::AppWindow(const Settings &settings) {
  const auto windowFlags{
      static_cast<SDL_WindowFlags>(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI)};
  const WindowSize size{DPIHandler::GetDpiAwareWindowSize(settings)};

  sdl_window_ = SDL_CreateWindow(settings.title.c_str(),
                                 SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED,
                                 size.width,
                                 size.height,
                                 windowFlags);

  auto renderer_flags{
      static_cast<SDL_RendererFlags>(SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED)};
  sdl_renderer_ = SDL_CreateRenderer(sdl_window_, -1, renderer_flags);

  if (sdl_renderer_ == nullptr) {
    APP_ERROR("Error creating SDL_Renderer!");
    return;
  }

  SDL_RendererInfo info;
  SDL_GetRendererInfo(sdl_renderer_, &info);
  DPIHandler::SetRenderScale(sdl_renderer_);

  APP_INFO("Current SDL_Renderer: {}", info.name);
}

AppWindow::~AppWindow() {
  SDL_DestroyRenderer(sdl_renderer_);
  SDL_DestroyWindow(sdl_window_);
}

SDL_Window *AppWindow::NativeWindow() const {
  return sdl_window_;
}

SDL_Renderer *AppWindow::NativeRenderer() const {
  return sdl_renderer_;
}

}
