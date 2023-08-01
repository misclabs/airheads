#include "dpi_handler.h"

namespace Airheads {

	float DPIHandler::GetScale() {
		constexpr int displayIndex{ 0 };
		const float defaultDpi{ 96.0F };
		float dpi{ defaultDpi };

		SDL_GetDisplayDPI(displayIndex, nullptr, &dpi, nullptr);

		return dpi / defaultDpi;
	}

	WindowSize DPIHandler::GetDpiAwareWindowSize(const AppWindow::Settings& settings) {
		const float scale{ DPIHandler::GetScale() };
		const int width{ static_cast<int>(static_cast<float>(settings.width) * scale) };
		const int height{ static_cast<int>(static_cast<float>(settings.height) * scale) };
		return { width, height };
	}

	void DPIHandler::SetRenderScale([[maybe_unused]] SDL_Renderer* renderer) {
		// do nothing
	}

	void DPIHandler::SetGlobalFontScaling([[maybe_unused]] ImGuiIO* io) {
		// do nothing
	}

}
