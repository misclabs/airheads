#pragma once

#include "SDL.h"
#include "opencv2/core.hpp"
#include <memory>

namespace Airheads {

	class GuiMatRenderer {
	public:
		GuiMatRenderer(SDL_Renderer* sdlRenderer, cv::Mat* mat);
		~GuiMatRenderer();

		GuiMatRenderer(const GuiMatRenderer&) = delete;
		GuiMatRenderer& operator=(const GuiMatRenderer&) = delete;
		GuiMatRenderer(GuiMatRenderer&&) = delete;
		GuiMatRenderer& operator=(GuiMatRenderer&&) = delete;

		[[nodiscard]] bool IsMatRenderable() noexcept;
		void Clear();
		void UpdateTexture();
		void RenderImage();

	private:
		cv::Mat* m_mat;
		SDL_Renderer* m_sdlRenderer;
		SDL_Texture* m_sdlTexture = nullptr;
		SDL_Rect m_texRect = { 0, 0, 0, 0 };
		int m_texChannels = 0;
		std::unique_ptr<unsigned char[]> m_displayBuffer;

		[[nodiscard]] bool DoesTextureMatchMat();
		[[nodiscard]] bool CreateTexture();
	};
}