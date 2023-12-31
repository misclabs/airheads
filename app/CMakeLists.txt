set(NAME "airheads")

message(STATUS "fmt version: ${fmt_VERSION}")
message(STATUS "spdlog version: ${spdlog_VERSION}")
message(STATUS "SDL2 version: ${SDL2_VERSION}")
message(STATUS "imgui version: ${imgui_VERSION}")
message(STATUS "OpenVC version: ${OpenCV_VERSION}")

configure_file(src/project_conf.cpp.in src/project_conf.cpp @ONLY)
configure_file(src/project_version.cpp.in src/project_version.cpp @ONLY)

add_executable(${NAME} WIN32 MACOSX_BUNDLE src/main.cpp)

target_include_directories(${NAME}
        PUBLIC pub
        PRIVATE src
)

target_sources(${NAME} PRIVATE
        src/project_conf.cpp src/project_conf.h
        src/project_version.cpp src/project_version.h
        src/log.cpp src/log.h
        src/app.cpp src/app.h
        src/app_window.cpp src/app_window.h
        src/developer_gui.cpp src/developer_gui.h
        src/appraiser_window.cpp src/appraiser_window.h
        src/gui_mat_renderer.cpp src/gui_mat_renderer.h
        src/video_input/videoInput.cpp src/video_input/videoInput.h
        src/video_proc/video_proc.cpp src/video_proc/video_proc.h
        src/video_proc/processing_context.cpp src/video_proc/processing_context.h
        src/video_proc/cluster_proc.cpp src/video_proc/cluster_proc.h
        src/video_proc/cluster.cpp src/video_proc/cluster.h
        src/video_proc/cluster_map_proc.cpp src/video_proc/cluster_map_proc.h
        src/video_capture.cpp src/video_capture.h
        src/texture.cpp src/texture.h
        src/vec.h
        src/pixels.h
)

# Define set of OS specific files to include
target_sources(${NAME} PRIVATE
        src/resources.h src/dpi_handler.h)
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_sources(${NAME} PRIVATE
            src/win32/resources.cpp src/win32/dpi_handler.cpp)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_sources(${NAME} PRIVATE
            src/mac/resources.cpp src/mac/dpi_handler.cpp)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_sources(${NAME} PRIVATE
            src/linux/resources.cpp src/linux/dpi_handler.cpp)
endif ()

include(${PROJECT_SOURCE_DIR}/app/cmake/app_assets.cmake)
include(${PROJECT_SOURCE_DIR}/app/cmake/packaging.cmake)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(${NAME} PRIVATE cxx_std_20)
target_link_libraries(${NAME} PRIVATE
        project_warnings
        git_version
        fmt
        spdlog
        SDL2::SDL2
        imgui
        OpenCV
)
