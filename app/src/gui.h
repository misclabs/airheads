#pragma once

#include "video_proc/video_proc.h"

#include "videoInput.h"
#include "SDL.h"

namespace Airheads {

	class AppWindow;

	class Gui {
	public:

		Gui(AppWindow* appWindow);

		void Update();

	private:

		void SetActiveCamera(int index);
		void UpdateCameraTexture();

		AppWindow* m_appWindow;
		videoInput m_videoInput;

		bool m_shouldUpdateAvailableCameras = true;
		std::vector<std::string> m_cameraNames;
		
		int m_selectedCamera = 0;
		int m_activeCamera = -1;
		
		SDL_Texture* m_cameraRenderTex = nullptr;

		VideoProcessorRegistry m_processorRegistry;
	};

}