#include "resources.h"

#include <SDL.h>

namespace Airheads {

	static const std::string BASE_PATH{SDL_GetBasePath()};

	static std::filesystem::path GetResourcePath(const std::filesystem::path& file_path) {
		std::filesystem::path font_path{BASE_PATH};
		font_path /= "../share";
		font_path /= "fonts" / file_path;
		return font_path;
	}

	std::filesystem::path Resources::GetFontPath(const std::string_view& font_file) {
		return GetResourcePath(font_file);
	}

}
