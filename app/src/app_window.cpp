#include "app_window.h"

#include "dpi_handler.h"
#include "log.h"

namespace Airheads {

	AppWindow::AppWindow(const Settings& settings) {
		const auto windowFlags{
			static_cast<SDL_WindowFlags>(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI) };
		const WindowSize size{ DPIHandler::GetDpiAwareWindowSize(settings) };

		m_window = SDL_CreateWindow(settings.title.c_str(),
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			size.width,
			size.height,
			windowFlags);

		auto renderer_flags{
			static_cast<SDL_RendererFlags>(SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED) };
		m_renderer = SDL_CreateRenderer(m_window, -1, renderer_flags);

		if (m_renderer == nullptr) {
			APP_ERROR("Error creating SDL_Renderer!");
			return;
		}

		SDL_RendererInfo info;
		SDL_GetRendererInfo(m_renderer, &info);
		DPIHandler::SetRenderScale(m_renderer);

		APP_DEBUG("Current SDL_Renderer: {}", info.name);
	}

	AppWindow::~AppWindow() {
		SDL_DestroyRenderer(m_renderer);
		SDL_DestroyWindow(m_window);
	}

	SDL_Window* AppWindow::NativeWindow() const {
		return m_window;
	}

	SDL_Renderer* AppWindow::NativeRenderer() const {
		return m_renderer;
	}

}
