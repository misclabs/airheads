#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <cassert>

namespace Airheads {

	struct FrameRef {
		// assume 3 8-bit color channels in BGR order
		// pitch is width * 3
		int width;
		int height;
		unsigned char* data;
	};
	
	class VideoProcessor {
	public:

		virtual ~VideoProcessor() {};

		virtual const std::string& Name() const = 0;

		virtual void ProcessFrame(const FrameRef& frame) = 0;

		virtual void UpdateGuiControls() {};

		bool isEnabled = false;
	};

	using VideoProcessorUniquePtr = std::unique_ptr<VideoProcessor>;

	class VideoProcessorRegistry {
	public:
		void AddProcessor(VideoProcessorUniquePtr processor) {
			assert(processor);

			m_processors.push_back(std::move(processor));
		}
	
		void ProcessFrame(const FrameRef& frame) {
			for (auto& processor : m_processors) {
				if (processor->isEnabled)
					processor->ProcessFrame(frame);
			}
		}

		void ForEach(std::function<void(VideoProcessor&)> operation) {
			for (auto& processor : m_processors) {
				operation(*processor);
			}
		}

	private:
		std::vector<VideoProcessorUniquePtr> m_processors;
	};

	void LoadProcessors(VideoProcessorRegistry& registry);

}