#include "gui_mat_renderer.h"
#include "vec.h"
#include "imgui.h"
#include "log.h"
#include <cassert>

namespace Airheads {
GuiMatRenderer::GuiMatRenderer(SDL_Renderer *sdl_renderer, cv::Mat *mat)
    : mat_{mat}, sdl_renderer_{sdl_renderer} {
  assert(sdl_renderer_);
  assert(mat_);
}

GuiMatRenderer::~GuiMatRenderer() {
  Clear();
}

bool GuiMatRenderer::CanRenderMat() noexcept {
  return !mat_->empty()
      && mat_->dims == 2
      && mat_->depth() == CV_8U
      && mat_->cols > 0 && mat_->rows > 0
      && (mat_->channels() == 3 || mat_->channels() == 1);
}

bool GuiMatRenderer::IsTextureCompatibleWithMat() {
  return texture_
      && mat_->depth() == CV_8U
      && mat_->cols == texture_.Size().x
      && mat_->rows == texture_.Size().y
      && mat_->channels() == NumChannels(texture_.Format());
}

void GuiMatRenderer::Clear() {
  texture_.Clear();
}

void GuiMatRenderer::UpdateTexture() {
  if (!CanRenderMat()) {
    texture_.Clear();
    return;
  }

  if (!texture_ || !IsTextureCompatibleWithMat()) {
    CreateTextureForMat();
  }

  if (texture_) {
    unsigned char *data;
    if (mat_->channels() == 3) {
      data = mat_->data;
    } else { // m_mat->channels() == 1
      data = display_buffer_.get();

      for (int col = 0; col < mat_->cols; ++col) {
        for (int row = 0; row < mat_->rows; ++row) {
          const size_t mat_idx = (col + row * mat_->cols);
          uchar value = mat_->data[mat_idx];

          const size_t dp_idx = (col + row * mat_->cols) * 3;
          data[dp_idx] = value;
          data[dp_idx + 1] = value;
          data[dp_idx + 2] = value;
        }
      }
    }

    texture_.Update({data, {mat_->cols, mat_->rows}, PixelFormat::kBGR24});
  }

}

void GuiMatRenderer::RenderImage() {
  if (texture_) {
    Vec2f tex_size_f{(float) texture_.Size().x, (float) texture_.Size().y};
    Vec2f render_size = [&] {
      ImVec2 avail_size = ImGui::GetContentRegionAvail();
      if (tex_size_f.x / tex_size_f.y > avail_size.x / avail_size.y) {
        // fit width
        float scale = avail_size.x / tex_size_f.x;
        return Vec2f{tex_size_f.x * scale, tex_size_f.y * scale};
      }
      // fit height
      float scale = avail_size.y / tex_size_f.y;
      return Vec2f{tex_size_f.x * scale, tex_size_f.y * scale};
    }();
    GuiImage(texture_, render_size);
  }
}

void GuiMatRenderer::CreateTextureForMat() {
  if (!CanRenderMat())
    throw std::runtime_error("Cannot create texture for un-renderable Mat.");

  texture_ = Texture::Make(sdl_renderer_, {mat_->cols, mat_->rows}, TextureFormat::kBGR24, TextureAccess::kStreaming);

  if (mat_->channels() == 1) {
    display_buffer_ = std::make_unique<unsigned char[]>(texture_.Size().x * texture_.Size().y * 3);
  }
}

}