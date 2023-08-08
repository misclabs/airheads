#include "video_proc.h"
#include "dot_diff.h"

#include "resources.h"

namespace Airheads {

	void VideoProcessorPipeline::AddProcessor(VideoProcessorUniquePtr processor) {
		assert(processor);

		m_processors.push_back(std::move(processor));
	}

	void VideoProcessorPipeline::StartCapture(int frameWidth, int frameHeight) {
		for (auto& processor : m_processors) {
			if (processor->isEnabled)
				processor->StartCapture(frameWidth, frameHeight);
		}
	}

	void VideoProcessorPipeline::ProcessFrame(ProcessingContext& context) {
		for (auto& processor : m_processors) {
			if (processor->isEnabled)
				processor->ProcessFrame(context);
		}
	}

	void VideoProcessorPipeline::ForEach(std::function<void(VideoProcessor&)> operation) {
		for (auto& processor : m_processors) {
			operation(*processor);
		}
	}

	void LoadProcessors(VideoProcessorPipeline& registry) {
		registry.AddProcessor(std::move(DotDiff::Create()));
	}

}