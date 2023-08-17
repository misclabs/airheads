#pragma once

#include "vec.h"
#include "pixels.h"
#include "SDL.h"
#include <memory>
#include <stdexcept>

namespace Airheads {

enum class TextureFormat : uint32_t {
  kBGR24 = SDL_PIXELFORMAT_BGR24
};

[[nodiscard]] inline int NumChannels(TextureFormat format) {
  switch (format) {
    case TextureFormat::kBGR24:
      return 3;
  }

  throw std::runtime_error("Unknown texture format.");
}

enum class TextureAccess : int {
  kStatic = SDL_TEXTUREACCESS_STATIC,
  kStreaming = SDL_TEXTUREACCESS_STREAMING
};

class Texture {
 public:

  [[nodiscard]] static Texture Make(SDL_Renderer *renderer, Vec2i size, TextureFormat format, TextureAccess access);

  Texture() noexcept = default;

  [[nodiscard]] TextureFormat Format() const noexcept { return format_; }
  [[nodiscard]] Vec2i Size() const noexcept { return size_; }

  void Update(PixelBufferView pixels);
  void Clear() noexcept { texture_ptr_.reset(); }

  [[nodiscard]] explicit operator bool() noexcept { return (bool)texture_ptr_; }

  friend void GuiImage(Texture& texture, Vec2f render_size);

 private:
  using SdlTexturePtr = std::unique_ptr<SDL_Texture, decltype([](SDL_Texture* tp) noexcept { SDL_DestroyTexture(tp); })>;

  explicit Texture(SDL_Texture* ptr, Vec2i size, TextureFormat format) noexcept : texture_ptr_{ptr}, size_{size}, format_{format} {}

  SdlTexturePtr texture_ptr_;
  Vec2i size_{};
  TextureFormat format_{};
};

[[nodiscard]] inline bool AreByteCompatible(const PixelBufferView& pixels, const Texture& texture) noexcept {
  if (pixels.Format() == PixelFormat::kBGR24 && texture.Format() == TextureFormat::kBGR24)
    return true;

  return false;
}

void GuiImage(Texture& texture, Vec2f render_size);

} // Airheads
