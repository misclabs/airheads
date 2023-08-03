include(FetchContent)

FetchContent_Declare(
	fmt
	GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
	GIT_TAG 9.1.0
)
message(STATUS "Fetching fmt ...")
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
	spdlog
	GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
	GIT_TAG v1.11.0
)
add_subdirectory(spdlog)

FetchContent_Declare(
	imgui
	GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
	GIT_TAG 031148dc56d70158b3ad84d9be95b04bb3f5baaf # Branch: docking, date: 25.04.2023
)
add_subdirectory(imgui)

FetchContent_Declare(
	SDL2
	GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
	GIT_TAG release-2.26.5
)
message(STATUS "Fetching SDL ...")
FetchContent_MakeAvailable(SDL2)

add_subdirectory(opencv)

add_subdirectory(video_input)