#pragma once

#include "opencv2/core.hpp"

#include <vector>

namespace Airheads::Cluster {

	bool FindSeed(cv::Mat& img, cv::Point seed_guess, int minval, int max_radius, cv::Point& out_point);

	struct ClusterPixel {
		cv::Point loc;
		uchar value;
	};

	struct ClusterMetrics {

		static ClusterMetrics FromPixels(std::vector<ClusterPixel>& accepted);
		static ClusterMetrics FromWeightedPixels(std::vector<ClusterPixel>& accepted);

		//ClusterMetrics() {};

		//def get_center(self,use_weighted_averages = False):
		//	#returns (y,x) coordinates. y,x because that can be used to index the image and get pixel values.
		//	if use_weighted_averages:
		//		return (round(self.YWavg), round(self.XWavg))
		//	else:
		//		return (round(self.Yavg), round(self.Xavg))
		//cv::Point get_center() {
		//	return cv::Point((int)round(Xavg), (int)round(Yavg));
		//}

		int sizePx = 0;
		cv::Point center;
		//float Ymin = 0;
		//float Ymax = 0;
		//float Xmin = 0;
		//float Xmax = 0;
		////float Wmin = -1;
		////float Wmax = -1;
		//float Yavg = 0;
		//float Xavg = 0;
		//float Wavg = -1;
		//float Ystdev = -1;
		//float Xstdev = -1;
		//float Wstdev = -1;
		//float YWavg = -1;
		//float XWavg = -1;
		//float YWstdev = -1;
		//float XWstdev = -1;
	};

	ClusterMetrics ClusterFill(cv::Mat& img, cv::Point seed, uchar minval, int m_clusterColor, int maxIterations);

}