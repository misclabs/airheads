#pragma once

#include "video_proc.h"

namespace Airheads {

	class DotDiff;
	using DotDiffUniquePtr = std::unique_ptr<DotDiff>;

	class DotDiff : public VideoProcessor {
	public:

		static DotDiffUniquePtr Create();

		const std::string& Name() const override;
		
		//void StartCapture(int frameWidth, int frameHeight) override;

		void ProcessFrame(ProcessingContext& context) override;
		
		void UpdateConfigControls() override;
		void UpdateStatsControls() override;

	private:
		
		//void Reset();

		// Pixels under value_threshold are rejected.
		int m_valueThreshold = 93; // 200;
		int m_saturationThreshold = 190;  // 65;
		int m_clusterColor = 50; // #ought to be significantly less than value_threshold
		//cv::Point m_upperClusterLastCoords;
		//cv::Point m_lowerClusterLastCoords;
		//int m_interclusterDistPx;
		//int m_minDist;
		//int m_maxDist;
		//int m_frameWidth;
		//int m_frameHeight;
		double dmdm_thresh = 1.08;
		cv::Mat hsv;
		int m_maxClusterSizePx = 4096;
	};

}