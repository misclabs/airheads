#include "video_proc.h"
#include "cluster_map_proc.h"
#include "cluster_proc.h"

#include "resources.h"
#include "imgui.h"

#include <limits>

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
		m_context.SetFrameBGR(width, height, data);
		m_context.ResetOutput();

		for (auto& processor : m_processors) {
			if (processor->isEnabled)
				processor->StartCapture(width, height);
		}
	}

	void VideoProcessorPipeline::StopCapture() {
		m_context.ClearFrame();
	}

	void VideoProcessorPipeline::UpdateConfigGui(){
		ImGui::SliderInt("Saturation Threshold", &m_context.saturationThreshold, 0, 255);
		ImGui::SliderInt("Value Threshold", &m_context.valueThreshold, 0, 255);

		if (ImGui::Button("Reset Cluster Guess")) {
			m_context.ResetOutput();
		}

		ForEach([](VideoProcessor& processor) {
			ImGui::Separator();

			ImGui::Checkbox(processor.Name().c_str(), &processor.isEnabled);
			processor.UpdateConfigControls();
		});
	}

	void VideoProcessorPipeline::UpdateStatsGui() {
		ImGui::Separator();
		ImGui::Text("Intercluster Distance: %dpx", m_context.DotsDistPx());

		ImGui::Text("Min distance: %dpx", m_context.MinDotDistPx());
		ImGui::Text("Max distance: %dpx", m_context.MaxDotDistPx());
		double dmdm = m_context.MaxDotDistPx() / (double)m_context.MinDotDistPx();
		//double dmdm_thresh = 1.08;
		//if dmdm > dmdm_thresh:
		//	dm_color = overlay_color
		//else:
		//	dm_color = nope_color
		ImGui::Text("Dmax/Dmin: %.3f", dmdm);

		ForEach([](VideoProcessor& processor) {
			ImGui::Separator();
			ImGui::Text(processor.Name().c_str());
			if (processor.isEnabled) {
				processor.UpdateStatsControls();
			}
		});
	}

	void VideoProcessorPipeline::ProcessFrame() {
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
		registry.AddProcessor(std::move(ClusterMapProc::Create()));
		registry.AddProcessor(std::move(ClusterProc::Create()));
	}

	void ProcessingContext::ResetOutput() {
		m_topDotLoc = { frame.cols / 2, (int)(frame.rows * 0.3) };
		m_botDotLoc = { frame.cols / 2, (int)(frame.rows * 0.6) };
		m_dotsDistPx = DistanceBetween(m_topDotLoc, m_botDotLoc);
		m_minDotsDistPx = std::numeric_limits<int>::max();
		m_maxDotsDistPx = 0;
		m_topCluster = {};
		m_botCluster = {};
	}

	cv::Point ProcessingContext::ClampLoc(cv::Point pt) {
		return {
			std::clamp(pt.x, 0, frame.cols),
			std::clamp(pt.y, 0, frame.rows)
		};
	}
	void ProcessingContext::UpdateClusterResults(ClusterResult top, ClusterResult bot) {
		m_topCluster = top;
		m_botCluster = bot;

		if (IsClusterValid(top)) {
			m_topDotLoc = ClampLoc(top.center);
		}
		if (IsClusterValid(bot)) {
			m_botDotLoc = ClampLoc(bot.center);
		}

		// Only update measurements when both clusters are valid
		if (IsClusterValid(top) && IsClusterValid(bot)) {
			m_dotsDistPx = DistanceBetween(m_topDotLoc, m_botDotLoc);

			m_minDotsDistPx = std::min(m_minDotsDistPx, m_dotsDistPx);
			m_maxDotsDistPx = std::max(m_maxDotsDistPx, m_dotsDistPx);
		}
	}

}