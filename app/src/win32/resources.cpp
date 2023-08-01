#include "resources.h"

#include <SDL.h>
#include <fmt/format.h>

namespace Airheads {

	static const std::string BASE_PATH{SDL_GetBasePath()};

	static std::filesystem::path GetResourcePath(const std::filesystem::path& file_path) {
		std::filesystem::path fontPath{BASE_PATH};
		fontPath /= "../share" / file_path;
		return fontPath;
	}

	std::filesystem::path Resources::GetFontPath(const std::string_view& font_file) {
		return GetResourcePath("fonts") / font_file;
	}

}
