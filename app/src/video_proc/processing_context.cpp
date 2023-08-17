#include "processing_context.h"

namespace Airheads {

int DistanceBetween(cv::Point a, cv::Point b) {
  auto dx = b.x - a.x;
  auto dy = b.y - a.y;
  return (int) sqrt(dx * dx + dy * dy);
}

void ProcessingContext::ResetOutput() {
  top_target_loc_ = {Frame().cols / 2, (int) (Frame().rows * 0.3)};
  bot_target_loc_ = {Frame().cols / 2, (int) (Frame().rows * 0.6)};
  targets_dist_px_ = DistanceBetween(top_target_loc_, bot_target_loc_);
  min_targets_dist_px_ = std::numeric_limits<int>::max();
  max_targets_dist_px_ = 0;
  top_cluster_ = {};
  bot_cluster_ = {};
}

[[nodiscard]] cv::Point ProcessingContext::ClampLocToFrame(cv::Point pt) const {
  return {
      std::clamp(pt.x, 0, Frame().cols),
      std::clamp(pt.y, 0, Frame().rows)
  };
}

void ProcessingContext::UpdateClusterResults(ClusterResult top, ClusterResult bot) {
  top_cluster_ = top;
  bot_cluster_ = bot;

  if (mode_ == ProcessingMode::kTesting && IsClusterValid(top)) {
    top_target_loc_ = ClampLocToFrame(top.center);
  }
  if (mode_ == ProcessingMode::kTesting && IsClusterValid(bot)) {
    bot_target_loc_ = ClampLocToFrame(bot.center);
  }

  // Only update measurements when both clusters are valid
  if (mode_ == ProcessingMode::kTesting && IsClusterValid(top) && IsClusterValid(bot)) {
    targets_dist_px_ = DistanceBetween(top_target_loc_, bot_target_loc_);

    min_targets_dist_px_ = std::min(min_targets_dist_px_, targets_dist_px_);
    max_targets_dist_px_ = std::max(max_targets_dist_px_, targets_dist_px_);
  }
}

}