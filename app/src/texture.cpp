#include "texture.h"
#include "imgui.h"
#include <stdexcept>

namespace Airheads {

void GuiImage(Texture& texture, Vec2f render_size) {
  ImGui::Image(texture.texture_ptr_.get(), ImVec2{render_size.x, render_size.y});
}

Texture Texture::Make(SDL_Renderer *renderer, Vec2i size, TextureFormat format, TextureAccess access) {
  assert(renderer);
  SDL_Texture* sdl_texture = SDL_CreateTexture(renderer, (Uint32)format, (int)access, size.x, size.y);
  if (!sdl_texture) {
    throw std::runtime_error(SDL_GetError());
  }

  return Texture(sdl_texture, size, format);
}

void Texture::Update(PixelBufferView pixels) {
  // TODO(jw): Use SDL_LockTexture and SDL_UnlockTexture instead
  int result = SDL_UpdateTexture(texture_ptr_.get(), nullptr, pixels.Bytes(), pixels.Pitch());
  if (result != 0) {
    throw std::runtime_error(SDL_GetError());
  }
}

} // Airheads