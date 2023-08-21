#pragma once

#include "app_window.h"
#include "video_capture.h"
#include "SDL.h"
#include <string>
#include <memory>

namespace Airheads {

class App;
using AppUniquePtr = std::unique_ptr<App>;

class App {
 public:

  static AppUniquePtr CreateApplication();

  App(AppWindowUniquePtr platformWindowPtr, const char *userConfigPath);
  ~App();

  void StopMainLoop();
  [[nodiscard]] const std::string &UserConfigPath() const {
    return user_config_path_;
  }

  void RunMainLoop();

  VideoCapture& GetVideoCapture() { return video_capture_; };

 private:
  void OnEvent(const SDL_WindowEvent &event);

  bool should_keep_looping_{true};
  bool minimized_{false};
  AppWindowUniquePtr window_ptr_{nullptr};
  std::string user_config_path_;
  VideoCapture video_capture_;

};

}
