#pragma once

#include "opencv2/core.hpp"

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <cassert>

namespace Airheads {

	struct ClusterResult {
		cv::Point center = { 0, 0 };
		int size = 0;
	};

	struct ProcessingContext {
		void SetFrameBGR(int width, int height, unsigned char* data);
		void ClearFrame();

		void ResetOutput();
		void UpdateClusterResults(ClusterResult top, ClusterResult bot);

		cv::Point TopDotLoc() const { return m_topDotLoc; }
		cv::Point BotDotLoc() const { return m_botDotLoc; }
		int DotsDistPx() const { return m_dotsDistPx; }
		int MinDotDistPx() const { return m_minDotsDistPx; }
		int MaxDotDistPx() const { return m_maxDotsDistPx; }
		ClusterResult TopCluster() const { return m_topCluster; }
		ClusterResult BotCluster() const { return m_botCluster; }

		int InvertedValueThreshold();
		bool IsClusterValid(ClusterResult cluster);

		// frame is input from the camera and displayed as the end result.
		//   - It may be written to by a VideoProcessor in the pipeline.
		//   - It is the only Mat that doesn't own it's data
		//   - Data format is BGR, 8-bits per channel (0-255)
		cv::Mat frame;

		cv::Mat saturationMap;
		cv::Mat valueMap;
		cv::Mat clusterMap;

		int minClusterSizePx = 2;
		int maxClusterSizePx = 4096;

		int valueThreshold = 93; // 200;
		int saturationThreshold = 190;  // 65;


	private:
		cv::Point ClampLoc(cv::Point pt);

		cv::Point m_topDotLoc;
		cv::Point m_botDotLoc;

		ClusterResult m_topCluster;
		ClusterResult m_botCluster;

		int m_dotsDistPx;
		int m_minDotsDistPx;
		int m_maxDotsDistPx;		
	};
	
	inline void ProcessingContext::SetFrameBGR(int width, int height, unsigned char* data) {
		frame = { height, width, CV_8UC3, data };
	}
	
	inline void ProcessingContext::ClearFrame() {
		frame = cv::Mat();
	}

	inline int ProcessingContext::InvertedValueThreshold() {
		auto invertedThreshold = 255 - valueThreshold;
		if (invertedThreshold < 1)
			return 1;

		return invertedThreshold;
	}

	inline bool ProcessingContext::IsClusterValid(ClusterResult cluster) {
		return cluster.size > minClusterSizePx && cluster.size < maxClusterSizePx - 1;
	}


	class VideoProcessor {
	public:

		virtual ~VideoProcessor() {};

		virtual const std::string& Name() const = 0;

		virtual void StartCapture(int frameWidth, int frameHeight) {}
		
		virtual void ProcessFrame(ProcessingContext& context) = 0;

		virtual void UpdateConfigControls() {};
		virtual void UpdateStatsControls() {};

		bool isEnabled = true;
	};

	using VideoProcessorUniquePtr = std::unique_ptr<VideoProcessor>;

	class VideoProcessorPipeline {
	public:

		void StartCapture(int width, int height, unsigned char* data);
		void StopCapture();

		void AddProcessor(VideoProcessorUniquePtr processor);
		void ProcessFrame();
		void UpdateConfigGui();
		void UpdateStatsGui();
		
		void ForEach(std::function<void(VideoProcessor&)> operation);

		ProcessingContext& Context() { return m_context; }

	private:
		std::vector<VideoProcessorUniquePtr> m_processors;
		bool m_isCapturing = false;
		ProcessingContext m_context;
	};

	void LoadProcessors(VideoProcessorPipeline& registry);

}