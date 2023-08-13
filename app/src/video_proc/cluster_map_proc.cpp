#include "cluster_map_proc.h"

#include "opencv2/imgproc.hpp"

namespace Airheads {
	static const std::string g_clusterMapProcName {"Cluster Map Processor"};

	ClusterMapProcUniquePtr ClusterMapProc::Create() {
		return std::make_unique<ClusterMapProc>();
	}

	const std::string& ClusterMapProc::Name() const {
		return g_clusterMapProcName;
	}

	void ClusterMapProc::ProcessFrame(ProcessingContext& context) {
		cv::cvtColor(context.frame, hsv, cv::COLOR_BGR2HSV);

		context.saturationMap.create(hsv.rows, hsv.cols, CV_8UC1);
		context.valueMap.create(hsv.rows, hsv.cols, CV_8UC1);
		{
			cv::Mat sv_out[] = { context.saturationMap, context.valueMap };
			int from_to[] = { 1,0, 2,1 };
			cv::mixChannels(&hsv, 1, sv_out, 2, from_to, 2);
		}

		//_, saturation_map = cv2.threshold(saturation_map, saturation_threshold, 255, cv2.THRESH_BINARY_INV) #now saturation_map is really a mask
		cv::threshold(context.saturationMap, context.saturationMap, context.saturationThreshold, 255, cv::THRESH_BINARY_INV);

		//_, value_map = cv2.threshold(~value_map, inv_thresh, 255, cv2.THRESH_TOZERO)
		context.valueMap.forEach<uchar>([](uchar& pixel, const int* position) -> void {
			pixel = ~pixel;
		});
		cv::threshold(context.valueMap, context.valueMap, context.InvertedValueThreshold(), 255, cv::THRESH_TOZERO);

		//#first return value of cv2.threshold is the threshold
		//#manual: https://docs.opencv.org/3.4/d7/d4d/tutorial_py_thresholding.html
		//value_map = cv2.bitwise_and(value_map, saturation_map)
		//cv::bitwise_and(context.value_map, context.saturation_map, context.value_map);
		context.clusterMap.create(context.valueMap.rows, context.valueMap.cols, CV_8UC1);
		cv::bitwise_and(context.valueMap, context.saturationMap, context.clusterMap);
	}

}