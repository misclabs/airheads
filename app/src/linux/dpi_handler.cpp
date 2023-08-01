#include "dpi_handler.h"

namespace Airheads {

	float DPIHandler::GetScale() {
		constexpr int displayIndex{ 0 };
		const float defaultDpi{ 96.0F };
		float dpi{ defaultDpi };

		SDL_GetDisplayDPI(displayIndex, nullptr, &dpi, nullptr);

		return dpi / defaultDpi;
	}

	WindowSize DPIHandler::GetDpiAwareWindowSize(const Window::Settings& settings) {
		return { settings.width, settings.height };
	}

	void DPIHandler::SetRenderScale([[maybe_unused]] SDL_Renderer* renderer) {
		// do nothing
	}

	void DPIHandler::SetGlobalFontScaling([[maybe_unused]] ImGuiIO* io) {
		// do nothing
	}

}
