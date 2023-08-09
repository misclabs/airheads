#include "video_proc.h"
#include "dot_diff.h"

#include "resources.h"
#include "imgui.h"

namespace Airheads {

	int DistanceBetween(cv::Point a, cv::Point b) {
		auto dx = b.x - a.x;
		auto dy = b.y - a.y;
		return (int)sqrt(dx * dx + dy * dy);
	}

	void VideoProcessorPipeline::AddProcessor(VideoProcessorUniquePtr processor) {
		assert(processor);

		m_processors.push_back(std::move(processor));
	}

	void VideoProcessorPipeline::StartCapture(int width, int height, unsigned char* data) {
		for (auto& processor : m_processors) {
			if (processor->isEnabled)
				processor->StartCapture(frameWidth, frameHeight);
		}
	}

	void VideoProcessorPipeline::StopCapture() {
		// TODO(jw):
		//for (auto& processor : m_processors) {
		//	if (processor->isEnabled)
		//		processor->StartCapture(frameWidth, frameHeight);
		//}
	}

	//void DotDiff::StartCapture(int frameWidth, int frameHeight) {
	//	m_frameWidth = frameWidth;
	//	m_frameHeight = frameHeight;
	//	Reset();
	//}

	//void DotDiff::Reset() {
	//	m_upperClusterLastCoords = { m_frameWidth / 2, (int)(m_frameHeight * 0.3) };
	//	m_lowerClusterLastCoords = { m_frameWidth / 2, (int)(m_frameHeight * 0.6) };
	//	m_interclusterDistPx = m_frameWidth;
	//	m_minDist = m_frameWidth;
	//	m_maxDist = 0;
	//}
	void VideoProcessorPipeline::UpdateConfigGui(){
		if (ImGui::Button("Reset Cluster Guess")) {
			// TODO(jw):
			// Reset();
		}

		ForEach([](VideoProcessor& processor) {
			ImGui::Separator();

			//ImGui::Checkbox(processor.Name().c_str(), &processor.isEnabled);
			ImGui::Text(processor.Name().c_str());
				if (processor.isEnabled) {
					processor.UpdateConfigControls();
				}
			});
	}

	void VideoProcessorPipeline::UpdateStatsGui() {
		ForEach([](VideoProcessor& processor) {
			ImGui::Separator();
			ImGui::Text(processor.Name().c_str());
			if (processor.isEnabled) {
				processor.UpdateStatsControls();
			}
		});
	}

	void VideoProcessorPipeline::OnFrameDataUpdated() {
		for (auto& processor : m_processors) {
			if (processor->isEnabled)
				processor->ProcessFrame(m_context);
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

	void ProcessingContext::SetDotLocs(cv::Point top, cv::Point bot) {
		// TODO(jw): confirm it's in frame, etc.
		m_topDotLoc = top;
		m_botDotLoc = bot;


		//m_interclusterDistPx = DistanceBetween(m_upperClusterLastCoords, m_lowerClusterLastCoords);
		m_dotsDistPx = DistanceBetween(m_topDotLoc, m_botDotLoc);

		//m_minDist = std::min(m_minDist, m_interclusterDistPx);
		//m_maxDist = std::max(m_maxDist, m_interclusterDistPx);
		m_minDotsDistPx = std::min(m_minDotsDistPx, m_dotsDistPx);
		m_maxDotsDistPx = std::max(m_maxDotsDistPx, m_dotsDistPx);
	}

}