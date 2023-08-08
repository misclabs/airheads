#pragma once

#include "opencv2/core.hpp"

namespace Airheads::Cluster {

	bool FindSeed(cv::Mat& img, cv::Point seed_guess, int minval, int max_radius, cv::Point& out_point);

	struct Cluster {

		Cluster() {};
		Cluster(std::vector<int>& X, std::vector<int>& Y, std::vector<uchar>& W, int _N);

		//def get_center(self,use_weighted_averages = False):
		//	#returns (y,x) coordinates. y,x because that can be used to index the image and get pixel values.
		//	if use_weighted_averages:
		//		return (round(self.YWavg), round(self.XWavg))
		//	else:
		//		return (round(self.Yavg), round(self.Xavg))
		cv::Point get_center() {
			return cv::Point((int)round(Xavg), (int)round(Yavg));
		}

		int N = -1;
		float Ymin = -1;
		float Ymax = -1;
		float Xmin = -1;
		float Xmax = -1;
		//float Wmin = -1;
		//float Wmax = -1;
		float Yavg = -1;
		float Xavg = -1;
		//float Wavg = -1;
		//float Ystdev = -1;
		//float Xstdev = -1;
		//float Wstdev = -1;
		//float YWavg = -1;
		//float XWavg = -1;
		//float YWstdev = -1;
		//float XWstdev = -1;
	};

	Cluster GrowCluster(cv::Mat& img, cv::Point seed, uchar minval, int m_clusterColor, int maxIterations);

}