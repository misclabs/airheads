#include "resources.h"

#include <SDL.h>
#include <fmt/format.h>

namespace Airheads {

	static const std::filesystem::path BASE_PATH{SDL_GetBasePath()};
	static const std::filesystem::path SHARE_PATH{BASE_PATH / ".." / "share" };

	std::filesystem::path Resources::GetFontPath(const std::string_view& filename) {
		return SHARE_PATH / "fonts" / filename;
	}

}
