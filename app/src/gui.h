#pragma once

#include "video_proc/video_proc.h"
#include "gui_mat_renderer.h"

#include "videoInput.h"
#include "SDL.h"

namespace Airheads {

	class App;
	class AppWindow;

	class Gui {
	public:

		Gui(App* app, AppWindow* appWindow);

		void Update();

	private:

		void SetActiveCamera(int index);
		void UpdateCameraTexture();
		void UpdateStatsContent();
		void UpdatePipelineConfigContent();
		void UpdateMainGuiContent();
		void UpdateSaturationMapContent();
		void UpdateValueMapContent();
		void UpdateClusterMapContent();

		App* m_app;
		AppWindow* m_appWindow;
		videoInput m_videoInput;

		bool m_shouldUpdateAvailableCameras = true;
		std::vector<std::string> m_cameraNames;
		
		int m_selectedCamera = 0;
		int m_activeCamera = -1;
		
		SDL_Texture* m_cameraRenderTex = nullptr;

		VideoProcessorPipeline m_processorPipeline;
		ProcessingContext m_processingContext;

		bool m_showStats = true;
		bool m_showPipelineConfig = true;
		bool m_showSaturationMap = false;
		bool m_showValueMap = false;
		bool m_showClusterMap = true;

		GuiMatRenderer m_saturationMapRenderer;
		GuiMatRenderer m_valueMapRenderer;
		GuiMatRenderer m_clusterMapRenderer;
	};

}