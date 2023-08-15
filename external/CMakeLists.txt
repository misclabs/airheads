include(FetchContent)

set(fmt_VERSION "10.1.0" CACHE INTERNAL "")
FetchContent_Declare(
        fmt
        GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
        GIT_TAG ${fmt_VERSION}
)
message(STATUS "Fetching fmt ...")
FetchContent_MakeAvailable(fmt)

set(spdlog_VERSION "v1.12.0" CACHE INTERNAL "")
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
        GIT_TAG ${spdlog_VERSION}
)
add_subdirectory(spdlog)

set(imgui_VERSION "v1.89.8-docking" CACHE INTERNAL "")
FetchContent_Declare(
        imgui
        GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
        GIT_TAG ${imgui_VERSION}
)
add_subdirectory(imgui)

set(SDL2_VERSION "release-2.26.5" CACHE INTERNAL "")
FetchContent_Declare(
        SDL2
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
        GIT_TAG ${SDL2_VERSION}
)
message(STATUS "Fetching SDL ...")
FetchContent_MakeAvailable(SDL2)

add_subdirectory(opencv)

add_subdirectory(video_input)