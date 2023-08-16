#include "cluster_map_proc.h"

#include "opencv2/imgproc.hpp"

namespace Airheads {

static const std::string kClusterMapProcName{"Cluster Map Processor"};

ClusterMapProcUniquePtr ClusterMapProc::Create() {
  return std::make_unique<ClusterMapProc>();
}

const std::string &ClusterMapProc::Name() const {
  return kClusterMapProcName;
}

void ClusterMapProc::ProcessFrame(ProcessingContext &context) {
  cv::cvtColor(context.Frame(), hsv, cv::COLOR_BGR2HSV);

  context.saturation_map_.create(hsv.rows, hsv.cols, CV_8UC1);
  context.value_map_.create(hsv.rows, hsv.cols, CV_8UC1);
  {
    cv::Mat sv_out[] = {context.saturation_map_, context.value_map_};
    int from_to[] = {1, 0, 2, 1};
    cv::mixChannels(&hsv, 1, sv_out, 2, from_to, 2);
  }

  //_, saturation_map = cv2.threshold(saturation_map, saturation_threshold, 255, cv2.THRESH_BINARY_INV) #now saturation_map is really a mask
  cv::threshold(context.saturation_map_,
                context.saturation_map_,
                context.saturation_threshold_,
                255,
                cv::THRESH_BINARY_INV);

  //_, value_map = cv2.threshold(~value_map, inv_thresh, 255, cv2.THRESH_TOZERO)
  context.value_map_.forEach<uchar>([](uchar &pixel, const int *position) -> void {
    pixel = ~pixel;
  });
  cv::threshold(context.value_map_, context.value_map_, context.InvertedValueThreshold(), 255, cv::THRESH_TOZERO);

  //#first return value of cv2.threshold is the threshold
  //#manual: https://docs.opencv.org/3.4/d7/d4d/tutorial_py_thresholding.html
  //value_map = cv2.bitwise_and(value_map, saturation_map)
  //cv::bitwise_and(context.value_map, context.saturation_map, context.value_map);
  context.cluster_map_.create(context.value_map_.rows, context.value_map_.cols, CV_8UC1);
  cv::bitwise_and(context.value_map_, context.saturation_map_, context.cluster_map_);
}

}