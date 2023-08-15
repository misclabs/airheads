# Airheads
A simple project demo of capturing frames from a webcam, manipulating the frame, and displaying it in a window.

Most of the interesting stuff happens in [gui.cpp](app/src/gui.cpp). Specificlly `Gui::UpdateCameraTexture()` grabs a frame from the webcam, does some stuff with it, and then copies it to a texture for display.

## Dependencies and Credits

Dotdiff algorithm and logic is based on [Anthony Barker's](https://github.com/fiveisgreen) [python scripts](https://github.com/fiveisgreen/Dotdiff).

The project structure is based roughly on [cpp-gui-template-sdl2 template](https://github.com/MartinHelmut/cpp-gui-template-sdl2).

The build system is cmake. 

Basic rendering, window, and input are done with [SDL2](https://www.libsdl.org).
The GUI is made with [Dear ImGUI](https://github.com/ocornut/imgui).

Webcam capture uses [videoInput](https://github.com/ofTheo/videoInput)

The [Manrope](https://manropefont.com) font is include.

The video capturing is Windows dependent, but everything else should be reasonable portable.

### OpenCV
OpenCV is the one dependency that needs to be manually installed. On Windows grab [opencv-4.8.0-windows.exe](https://github.com/opencv/opencv/releases/download/4.8.0/opencv-4.8.0-windows.exe) from the [4.8.0 release page](https://github.com/opencv/opencv/releases/tag/4.8.0) (4.8.0 is confirmed to work, but later versions should work as well).
 
The environment variable `OpenCV_DIR` needs to be set to the `.\build` directory in the location you unpacked opencv. So if you unpacked it to `c:\opencv` you would set `OpenCV_DIR=c:\opencv\build`.

You also need to add `%OpenCV_DIR%\x64\vc16\bin` to your path for the program to find the OpenCV DLLs at runtime.

Note that the build does not automaticly include the required OpenCV DLLs in a distribution (like it does for the SDL DLL). Currently they need to be manually copied over.

## Building and Debugging w/ Visual Studio
The is a cmake project. Make sure you have [C++ CMake tools for Windows](https://learn.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio?view=msvc-170) installed and then open this folder using "Open a local folder". The first time you open the project it might take a bit for the dependencies to download (check the output panel for message like "Fetching fmt ..."). Once that's done select `airheads.exe (app\aiheads.exe)` as the target and run.

## Planning/Development
Some rough plans are documented in Trello here: https://trello.com/b/mAXCaU50/airheads