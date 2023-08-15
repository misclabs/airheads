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

  [[nodiscard]] cv::Point TopDotLoc() const { return top_dot_loc_; }
  [[nodiscard]] cv::Point BotDotLoc() const { return bot_dot_loc_; }
  [[nodiscard]] int DotsDistPx() const { return dots_dist_px_; }
  [[nodiscard]] int MinDotDistPx() const { return min_dots_dist_px_; }
  [[nodiscard]] int MaxDotDistPx() const { return max_dots_dist_px_; }
  [[nodiscard]] ClusterResult TopCluster() const { return top_cluster_; }
  [[nodiscard]] ClusterResult BotCluster() const { return bot_cluster_; }

  [[nodiscard]] int InvertedValueThreshold() const;
  [[nodiscard]] bool IsClusterValid(ClusterResult cluster) const;

  // frame is input from the camera and displayed as the end result.
  //   - It may be written to by a VideoProcessor in the pipeline.
  //   - It is the only Mat that doesn't own it's data
  //   - Data format is BGR, 8-bits per channel (0-255)
  cv::Mat frame;

  cv::Mat saturation_map_;
  cv::Mat value_map_;
  cv::Mat cluster_map_;

  int min_cluster_size_px_ = 2;
  int max_cluster_size_px_ = 4096;

  int value_threshold_ = 93; // 200;
  int saturation_threshold_ = 190;  // 65;

  float frame_pixels_per_cm_ = 10.0f;

 private:
  [[nodiscard]] cv::Point ClampLocToFrame(cv::Point pt) const;

  cv::Point top_dot_loc_;
  cv::Point bot_dot_loc_;

  ClusterResult top_cluster_;
  ClusterResult bot_cluster_;

  int dots_dist_px_;
  int min_dots_dist_px_;
  int max_dots_dist_px_;
};

inline void ProcessingContext::SetFrameBGR(int width, int height, unsigned char *data) {
  frame = {height, width, CV_8UC3, data};
}

inline void ProcessingContext::ClearFrame() {
  frame = cv::Mat();
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

class VideoProcessor {
 public:

  virtual ~VideoProcessor() = default;

  [[nodiscard]] virtual const std::string &Name() const = 0;

  virtual void StartCapture(int frame_width, int frame_height) {}

  virtual void ProcessFrame(ProcessingContext &context) = 0;

  virtual void UpdateConfigControls() {};
  virtual void UpdateStatsControls() {};

  bool is_enabled_ = true;
};

using VideoProcessorUniquePtr = std::unique_ptr<VideoProcessor>;

class VideoProcessorPipeline {
 public:

  void StartCapture(int width, int height, unsigned char *data);
  void StopCapture();

  void AddProcessor(VideoProcessorUniquePtr processor);
  void ProcessFrame();
  void UpdateConfigGui();
  void UpdateStatsGui();

  void ForEach(const std::function<void(VideoProcessor &)>& operation);

  ProcessingContext &Context() { return context_; }

 private:
  std::vector<VideoProcessorUniquePtr> processors_;
  ProcessingContext context_;
};

void LoadProcessors(VideoProcessorPipeline &registry);

}