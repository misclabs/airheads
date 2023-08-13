#pragma once

#include "opencv2/core.hpp"

#include <vector>
#include <optional>

namespace Airheads::Cluster {

	std::optional<cv::Point> FindSeed(cv::Mat& img, cv::Point seed_guess, int minval, int max_radius);

	struct ClusterPixel {
		cv::Point loc;
		uchar value;
	};

	struct ClusterMetrics {

		static ClusterMetrics FromPixels(std::vector<ClusterPixel>& accepted);
		static ClusterMetrics FromWeightedPixels(std::vector<ClusterPixel>& accepted);

		int sizePx = 0;
		cv::Point center;
	};

	void ClusterFill(cv::Mat& img, cv::Point seed, uchar minval, int clusterColor, int maxSizePx, std::vector<ClusterPixel>& accepted);

}