#include "dpi_handler.hpp"

#include <cmath>

namespace Airheads {

	float DPIHandler::GetScale() {
		constexpr int displayIndex{ 0 };
		// @todo: This should be 72.0F on Mac, but it seems like it is not. I'm not
		//  sure why, but this works ¯\_(ツ)_/¯
		const float defaultDpi{ 96.0F };
		float dpi{ defaultDpi };

		SDL_GetDisplayDPI(displayIndex, nullptr, &dpi, nullptr);

		return std::floor(dpi / defaultDpi);
	}

	WindowSize DPIHandler::GetDpiAwareWindowSize(const Window::Settings& settings) {
		return { settings.width, settings.height };
	}

	void DPIHandler::SetRenderScale(SDL_Renderer* renderer) {
		auto scale{ GetScale() };
		SDL_RenderSetScale(renderer, scale, scale);
	}

	void DPIHandler::SetGlobalFontScaling(ImGuiIO* io) {
		io->FontGlobalScale = 1.0F / GetScale();
	}

}
