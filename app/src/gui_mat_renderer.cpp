#include "gui_mat_renderer.h"

#include "imgui.h"

#include <cassert>

namespace Airheads {
	GuiMatRenderer::GuiMatRenderer(SDL_Renderer* sdlRenderer, cv::Mat* mat)
		: m_mat{ mat }, m_sdlRenderer{ sdlRenderer }
	{
		assert(m_sdlRenderer);
		assert(m_mat);
	}

	GuiMatRenderer::~GuiMatRenderer() {
		Clear();
	}

	bool GuiMatRenderer::IsMatRenderable() {
		return !m_mat->empty()
			&& m_mat->dims == 2
			&& m_mat->depth() == CV_8U
			&& m_mat->cols > 0 && m_mat->rows > 0
			&& (m_mat->channels() == 3 || m_mat->channels() == 1);
	}

	bool GuiMatRenderer::DoesTextureMatchMat() {
		return m_mat->depth() == CV_8U
			&& m_mat->cols == m_texRect.w
			&& m_mat->rows == m_texRect.h
			&& m_mat->channels() == m_texChannels;
	}

	void GuiMatRenderer::Clear() {
		if (m_sdlTexture) {
			SDL_DestroyTexture(m_sdlTexture);
			m_sdlTexture = nullptr;
		}
	}

	void GuiMatRenderer::UpdateTexture() {
		if (!IsMatRenderable()) {
			if (m_sdlTexture)
				Clear();

			return;
		}

		if (!m_sdlTexture) {
			CreateTexture();
		}
		else if (!DoesTextureMatchMat()) {
			Clear();
			CreateTexture();
		}

		if (m_sdlTexture) {
			unsigned char* data;
			if (m_texChannels == 3) {
				data = m_mat->data;
			} else { // m_texChannels == 1
				data = m_displayBuffer.get();

				for (int col = 0; col < m_texRect.w; ++col) {
					for (int row = 0; row < m_texRect.h; ++row) {
						const size_t mIdx = (col + row * m_texRect.w);
						uchar value = m_mat->data[mIdx];
						
						const size_t dpIdx = (col + row * m_texRect.w) * 3;
						data[dpIdx] = value;
						data[dpIdx + 1] = value;
						data[dpIdx + 2] = value;
					}
				}
			}

			int result = SDL_UpdateTexture(m_sdlTexture,
				&m_texRect, data, m_texRect.w * 3);
		}

	}

	void GuiMatRenderer::RenderImage() {
		if (m_sdlTexture) {
			ImGui::Image(m_sdlTexture, { (float)m_texRect.w, (float)m_texRect.h });
		}
	}

	bool GuiMatRenderer::CreateTexture() {
		if (!IsMatRenderable())
			return false;

		m_texRect.w = m_mat->cols;
		m_texRect.h = m_mat->rows;
		m_texChannels = m_mat->channels();

		m_sdlTexture = SDL_CreateTexture(m_sdlRenderer,
			SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STREAMING,
			m_texRect.w, m_texRect.h
		);

		if (m_texChannels == 1) {
			m_displayBuffer = std::make_unique<unsigned char[]>(m_texRect.w * m_texRect.h * 3);
		}

		return true;
	}
}