#pragma once

#include "texture.h"
#include "SDL.h"
#include "opencv2/core.hpp"
#include "imgui.h"
#include <memory>

namespace Airheads {

class GuiMatRenderer {
 public:
  GuiMatRenderer(SDL_Renderer *sdl_renderer, cv::Mat *mat);
  ~GuiMatRenderer();

  GuiMatRenderer(const GuiMatRenderer &) = delete;
  GuiMatRenderer &operator=(const GuiMatRenderer &) = delete;
  GuiMatRenderer(GuiMatRenderer &&) = delete;
  GuiMatRenderer &operator=(GuiMatRenderer &&) = delete;

  [[nodiscard]] bool CanRenderMat() noexcept;
  void Clear();
  void UpdateTexture();
  void RenderImage();

 private:
  [[nodiscard]] bool IsTextureCompatibleWithMat();
  void CreateTextureForMat();

  cv::Mat *mat_;
  SDL_Renderer *sdl_renderer_;
  Texture texture_;

  // display_buffer_ is used to convert 1 channel to a 3 channel buffer that can be uploaded to a texture.
  // Ideally this should be replaced by a shader that can render one channel and no conversion would be necessary.
  std::unique_ptr<unsigned char[]> display_buffer_;

};

}