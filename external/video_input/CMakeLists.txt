set(NAME "video_input_lib")

add_library(${NAME} STATIC videoInput.cpp videoInput.h)
target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
#target_compile_features(${NAME} PRIVATE cxx_std_20)
#target_link_libraries(${NAME}
  #PRIVATE project_warnings
#  PUBLIC fmt spdlog SDL2::SDL2 imgui settings_lib)
