#include "dot_diff.h"
#include "cluster.h"

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "imgui.h"

#include <vector>
#include <queue>
#include <limits>
#include <cmath>

namespace Airheads {

	//int DistanceBetween(cv::Point a, cv::Point b) {
	//	auto dx = b.x - a.x;
	//	auto dy = b.y - a.y;
	//	return (int)sqrt(dx * dx + dy * dy);
	//}

	//struct Bounds {
	//	cv::Point topLeft;
	//	cv::Point bottomRight;

	//	void Clamp(cv::Point& pt) {
	//		if (pt.x < topLeft.x)
	//			pt.x = topLeft.x;
	//		else if (pt.x > bottomRight.x)
	//			pt.x = bottomRight.x;

	//		if (pt.y < topLeft.y)
	//			pt.y = topLeft.y;
	//		else if (pt.y > bottomRight.y)
	//			pt.y = bottomRight.y;
	//	}
	//};

	static const std::string g_dotDiffName {"Dot Diff"};

	DotDiffUniquePtr DotDiff::Create() {
		return std::make_unique<DotDiff>();
	}

	const std::string& DotDiff::Name() const {
		return g_dotDiffName;
	}

	void DotDiff::ProcessFrame(ProcessingContext& context) {

		double inv_thresh = (double)(255 - m_valueThreshold);
		if (inv_thresh < 1.0)
			inv_thresh = 1.0;

		cv::cvtColor(context.frame, hsv, cv::COLOR_BGR2HSV);

		context.saturationMap.create(hsv.rows, hsv.cols, CV_8UC1);
		context.valueMap.create(hsv.rows, hsv.cols, CV_8UC1);
		{
			cv::Mat sv_out[] = { context.saturationMap, context.valueMap };
			int from_to[] = { 1,0, 2,1 };
			cv::mixChannels(&hsv, 1, sv_out, 2, from_to, 2);
		}

		//_, saturation_map = cv2.threshold(saturation_map, saturation_threshold, 255, cv2.THRESH_BINARY_INV) #now saturation_map is really a mask
		cv::threshold(context.saturationMap, context.saturationMap, m_saturationThreshold, 255, cv::THRESH_BINARY_INV);

		//_, value_map = cv2.threshold(~value_map, inv_thresh, 255, cv2.THRESH_TOZERO)
		context.valueMap.forEach<uchar>([](uchar& pixel, const int* position) -> void {
			pixel = ~pixel;
		});
		cv::threshold(context.valueMap, context.valueMap, inv_thresh, 255, cv::THRESH_TOZERO);

		//#first return value of cv2.threshold is the threshold
		//#manual: https://docs.opencv.org/3.4/d7/d4d/tutorial_py_thresholding.html
		//value_map = cv2.bitwise_and(value_map, saturation_map)
		//cv::bitwise_and(context.value_map, context.saturation_map, context.value_map);
		context.clusterMap.create(context.valueMap.rows, context.valueMap.cols, CV_8UC1);
		cv::bitwise_and(context.valueMap, context.saturationMap, context.clusterMap);

		int max_seek_radius = (int)std::max(100.0, 0.65 * context.DotsDistPx()); // m_interclusterDistPx);

		auto getCluster = [&](cv::Point seedGuess) {
			ClusterResult result;
			cv::Point seed;
			bool foundU = Cluster::FindSeed(context.clusterMap,
				seedGuess, //m_upperClusterLastCoords, 
				(int)inv_thresh, max_seek_radius,
				seed);
			//Cluster::Cluster cluster_upper;
			if (foundU) {
				Cluster::Cluster cluster_upper = Cluster::GrowCluster(context.clusterMap, seed, (uchar)inv_thresh, m_clusterColor, context.m_maxClusterSizePx);
				//m_upperClusterLastCoords = cluster_upper.get_center();
				//frameBounds.Clamp(m_upperClusterLastCoords);
				result.center = cluster_upper.get_center();
				result.size = cluster_upper.N;
			} else {
				result.center = seed;
			}

			return result;
		};

		ClusterResult upperClusterResult = getCluster(context.TopDotLoc());
		ClusterResult lowerClusterResult = getCluster(context.BotDotLoc());

		context.UpdateClusterResults(upperClusterResult, lowerClusterResult);
	}

	void DotDiff::UpdateConfigControls() {
		ImGui::SliderInt("Saturation Threshold", &m_saturationThreshold, 0, 255);
		ImGui::SliderInt("Value Threshold", &m_valueThreshold, 0, 255);
	}

}