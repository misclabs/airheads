# A simple test of capturing frames from a webcam using OpenCV in C++

## Dependencies
This test depends on OpenCV 4.8.0 which can be downloaded at [https://github.com/opencv/opencv/releases/download/4.8.0/opencv-4.8.0-windows.exe](https://github.com/opencv/opencv/releases/download/4.8.0/opencv-4.8.0-windows.exe). This self-extracting archive must be extract to the `./` (which will create a `./opencv/` directory) unless you change all the references in the project settings.

The following screenshots show the project settings that refer to OpenCV:

![Screenshot of VS project properties where OpenCV header location is set.](https://github.com/misclabs/airheads/blob/main/misc/project_props_includes_screenshot.png?raw=true)

![Screenshot of VS project properties where OpenCV link libraries location is set.](https://github.com/misclabs/airheads/blob/main/misc/project_props_lib_dirs_screenshot.png?raw=true)

![Screenshot of VS project properties where OpenCV libraries are listed.](https://github.com/misclabs/airheads/blob/main/misc/project_props_libs_screenshot.png?raw=true)

## Running & Debugging

OpenCV is linked dynamiclly so the program must be able to find the OpenCV DLLs at runtime which means they must either be copied to the same directory as the executable or added to the path. For debugging you can add them to the path by adding `Path=$(SolutionDir)opencv\build\x64\vc16\bin;$(Path)` to the debugging environment as shown below.

![Screenshot of VS project properties where OpenCV DLL location is added to path.](https://github.com/misclabs/airheads/blob/main/misc/project_props_path_screenshot.png?raw=true)