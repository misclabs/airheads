#pragma once

#include "opencv2/core.hpp"

#include <vector>
#include <optional>

namespace Airheads::Cluster {

	[[nodiscard]] std::optional<cv::Point> FindSeed(cv::Mat& img, cv::Point seed_guess, int min_val, int max_radius_px, int step_px);

	struct ClusterPixel {
		cv::Point loc;
		uchar value;
	};

	struct ClusterMetrics {

		static ClusterMetrics FromPixels(std::vector<ClusterPixel>& accepted);
		static ClusterMetrics FromWeightedPixels(std::vector<ClusterPixel>& accepted);

		int size_px = 0;
		cv::Point center;
	};

	void ClusterFill(cv::Mat& img, cv::Point seed, uchar min_val, int cluster_color, int max_size_px, std::vector<ClusterPixel>& accepted);

}