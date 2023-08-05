#pragma once

#include "opencv2/core.hpp"

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <cassert>

namespace Airheads {

	struct ProcessingContext {
		// frame is input from the camera and displayed as the end result.
		//   - It may be written to by a VideoProcessor in the pipeline.
		//   - It is the only Mat that doesn't own it's data
		//   - Data format is BGR, 8-bits per channel (0-255)
		cv::Mat frame;
		void SetFrameBGR(int width, int height, unsigned char* data) {
			frame = {height, width, CV_8UC3, data};
		}
	};
	
	class VideoProcessor {
	public:

		virtual ~VideoProcessor() {};

		virtual const std::string& Name() const = 0;

		virtual void ProcessFrame(ProcessingContext& context) = 0;

		virtual void UpdateGuiControls() {};

		bool isEnabled = false;
	};

	using VideoProcessorUniquePtr = std::unique_ptr<VideoProcessor>;

	class VideoProcessorPipeline {
	public:

		void AddProcessor(VideoProcessorUniquePtr processor);
		void ProcessFrame(ProcessingContext& context);
		void ForEach(std::function<void(VideoProcessor&)> operation);

	private:
		std::vector<VideoProcessorUniquePtr> m_processors;
	};

	void LoadProcessors(VideoProcessorPipeline& registry);

}