#pragma once

#include "vec.h"
#include <stdexcept>

namespace Airheads {

enum class PixelFormat {
  kBGR24
};

class PixelBufferView {
 public:

  PixelBufferView(void* bytes, Vec2i size, PixelFormat format) noexcept;

  [[nodiscard]] Vec2i Size() const noexcept { return size_; }
  [[nodiscard]] PixelFormat Format() const noexcept { return format_; }
  [[nodiscard]] int Pitch() const;
  [[nodiscard]] const void* Bytes() const noexcept { return bytes_; }

 private:
  void* bytes_;
  Vec2i size_;
  PixelFormat format_;
};

inline PixelBufferView::PixelBufferView(void* bytes, Vec2i size, PixelFormat format) noexcept :
    bytes_(bytes), size_(size), format_(format) {}

inline int PixelBufferView::Pitch() const {
  switch(format_) {
    case PixelFormat::kBGR24:
      return size_.x * 3;
  }

  throw std::runtime_error("Unknown pixel format.");
}

}