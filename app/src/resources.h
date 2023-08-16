#pragma once

#include <filesystem>
#include <string_view>

namespace Airheads {

	class Resources {
	public:
		[[nodiscard]] static std::filesystem::path GetFontPath(const std::string_view& filename);
	};

}
