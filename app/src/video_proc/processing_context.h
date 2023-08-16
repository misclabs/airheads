#pragma once

#pragma once

#include "opencv2/core.hpp"

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <cassert>

namespace Airheads {

struct ClusterResult {
  cv::Point center = {0, 0};
  int size = 0;
};

class ProcessingContext {
 public:
  void SetFrameBGR(int width, int height, unsigned char *data);
  void ClearFrame();

  void ResetOutput();
  void UpdateClusterResults(ClusterResult top, ClusterResult bot);

  [[nodiscard]] cv::Point TopTargetLoc() const { return top_target_loc_; }
  [[nodiscard]] cv::Point BotTargetLoc() const { return bot_target_loc_; }
  [[nodiscard]] int TargetsDistPx() const { return targets_dist_px_; }
  [[nodiscard]] int MinTargetsDistPx() const { return min_targets_dist_px_; }
  [[nodiscard]] int MaxTargetsDistPx() const { return max_targets_dist_px_; }
  [[nodiscard]] ClusterResult TopCluster() const { return top_cluster_; }
  [[nodiscard]] ClusterResult BotCluster() const { return bot_cluster_; }

  [[nodiscard]] int InvertedValueThreshold() const;
  [[nodiscard]] bool IsClusterValid(ClusterResult cluster) const;

  [[nodiscard]] cv::Mat& Frame() { return frame_; }
  [[nodiscard]] const cv::Mat& Frame() const { return frame_; }

  [[nodiscard]] float CmToPx(float cm) const { return cm * frame_pixels_per_cm_; }
  [[nodiscard]] float PxToCm(float px) const { return px * 1.0f/frame_pixels_per_cm_; }

  cv::Mat saturation_map_;
  cv::Mat value_map_;
  cv::Mat cluster_map_;

  int min_cluster_size_px_ = 2;
  int max_cluster_size_px_ = 4096;

  int value_threshold_ = 93; // 200;
  int saturation_threshold_ = 190;  // 65;

  float target_diameter_cm_ = 0.75f;

  float frame_pixels_per_cm_ = 10.0f;

 private:
  [[nodiscard]] cv::Point ClampLocToFrame(cv::Point pt) const;

  // frame is input from the camera and displayed as the end result.
  //   - It may be written to by a VideoProcessor in the pipeline.
  //   - It is the only Mat that doesn't own it's data
  //   - Data format is BGR, 8-bits per channel (0-255)
  cv::Mat frame_;

  cv::Point top_target_loc_;
  cv::Point bot_target_loc_;

  ClusterResult top_cluster_;
  ClusterResult bot_cluster_;

  int targets_dist_px_;
  int min_targets_dist_px_;
  int max_targets_dist_px_;
};

inline void ProcessingContext::SetFrameBGR(int width, int height, unsigned char *data) {
  frame_ = {height, width, CV_8UC3, data};
}

inline void ProcessingContext::ClearFrame() {
  frame_ = cv::Mat();
}

inline int ProcessingContext::InvertedValueThreshold() const {
  auto inverted_threshold = 255 - value_threshold_;
  if (inverted_threshold < 1)
    return 1;

  return inverted_threshold;
}

inline bool ProcessingContext::IsClusterValid(ClusterResult cluster) const {
  return cluster.size > min_cluster_size_px_ && cluster.size < max_cluster_size_px_ - 1;
}

}