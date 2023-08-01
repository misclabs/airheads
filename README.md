# Airheads
A simple project demo of capturing frames from a webcam, manipulating the frame, and displaying it in a window.

Most of the interesting stuff happens in [gui.cpp](app/src/gui.cpp). Specificlly `Gui::UpdateCameraTexture()` grabs a frame from the webcam, does some stuff with it, and then copies it to a texture for display.

## Dependencies and Credits

The project structure is based roughly on [cpp-gui-template-sdl2 template](https://github.com/MartinHelmut/cpp-gui-template-sdl2).

The build system is cmake. 

Basic rendering, window, and input are done with [SDL2](https://www.libsdl.org).
The GUI is made with [Dear ImGUI](https://github.com/ocornut/imgui).

Webcam capture uses [videoInput](https://github.com/ofTheo/videoInput)

The [Manrope](https://manropefont.com) font is include.

The video capturing is Windows dependent, but everything else should be reasonable portable.

## Building and Debugging w/ Visual Studio
The is a cmake project. Make sure you have [C++ CMake tools for Windows](https://learn.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio?view=msvc-170) installed and then open this folder using "Open a local folder". The first time you open the project it might take a bit for the dependencies to download (check the output panel for message like "Fetching fmt ..."). Once that's done select `airheads.exe (app\aiheads.exe)` as the target and run.