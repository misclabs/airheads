#include "video_proc.h"
#include "cluster_map_proc.h"
#include "cluster_proc.h"

#include "resources.h"
#include "imgui.h"

#include <limits>

namespace Airheads {

int DistanceBetween(cv::Point a, cv::Point b) {
  auto dx = b.x - a.x;
  auto dy = b.y - a.y;
  return (int) sqrt(dx * dx + dy * dy);
}

void VideoProcessorPipeline::AddProcessor(VideoProcessorUniquePtr processor) {
  assert(processor);

  processors_.push_back(std::move(processor));
}

void VideoProcessorPipeline::StartCapture(int width, int height, unsigned char *data) {
  context_.SetFrameBGR(width, height, data);
  context_.ResetOutput();

  for (auto &processor : processors_) {
    if (processor->is_enabled_)
      processor->StartCapture(width, height);
  }
}

void VideoProcessorPipeline::StopCapture() {
  context_.ClearFrame();
}

void VideoProcessorPipeline::UpdateConfigGui() {
  ImGui::SliderInt("Saturation Threshold", &context_.saturation_threshold_, 0, 255);

  ImGui::SliderInt("Value Threshold", &context_.value_threshold_, 0, 255);

  if (ImGui::Button("Reset Cluster Guess")) {
    context_.ResetOutput();
  }
  if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
    ImGui::SetTooltip("Reset the cluster centers to default locations.");

  ForEach([](VideoProcessor &processor) {
    ImGui::Separator();

    ImGui::Checkbox(processor.Name().c_str(), &processor.is_enabled_);
    processor.UpdateConfigControls();
  });
}

void VideoProcessorPipeline::UpdateStatsGui() {
  ImGui::Separator();
  ImGui::Text("Intercluster Distance: %dpx", context_.DotsDistPx());

  ImGui::Text("Min distance: %dpx", context_.MinDotDistPx());
  ImGui::Text("Max distance: %dpx", context_.MaxDotDistPx());
  double dmdm = context_.MaxDotDistPx() / (double) context_.MinDotDistPx();
  //double dmdm_thresh = 1.08;
  //if dmdm > dmdm_thresh:
  //	dm_color = overlay_color
  //else:
  //	dm_color = nope_color
  ImGui::Text("Dmax/Dmin: %.3f", dmdm);

  ForEach([](VideoProcessor &processor) {
    ImGui::Separator();
    ImGui::Text(processor.Name().c_str());
    if (processor.is_enabled_) {
      processor.UpdateStatsControls();
    }
  });
}

void VideoProcessorPipeline::ProcessFrame() {
  for (auto &processor : processors_) {
    if (processor->is_enabled_)
      processor->ProcessFrame(context_);
  }
}

void VideoProcessorPipeline::ForEach(const std::function<void(VideoProcessor &)>& operation) {
  for (auto &processor : processors_) {
    operation(*processor);
  }
}

void LoadProcessors(VideoProcessorPipeline &registry) {
  registry.AddProcessor(std::move(ClusterMapProc::Create()));
  registry.AddProcessor(std::move(ClusterProc::Create()));
}

void ProcessingContext::ResetOutput() {
  top_dot_loc_ = {frame.cols / 2, (int) (frame.rows * 0.3)};
  bot_dot_loc_ = {frame.cols / 2, (int) (frame.rows * 0.6)};
  dots_dist_px_ = DistanceBetween(top_dot_loc_, bot_dot_loc_);
  min_dots_dist_px_ = std::numeric_limits<int>::max();
  max_dots_dist_px_ = 0;
  top_cluster_ = {};
  bot_cluster_ = {};
}

[[nodiscard]] cv::Point ProcessingContext::ClampLocToFrame(cv::Point pt) const {
  return {
      std::clamp(pt.x, 0, frame.cols),
      std::clamp(pt.y, 0, frame.rows)
  };
}

void ProcessingContext::UpdateClusterResults(ClusterResult top, ClusterResult bot) {
  top_cluster_ = top;
  bot_cluster_ = bot;

  if (IsClusterValid(top)) {
    top_dot_loc_ = ClampLocToFrame(top.center);
  }
  if (IsClusterValid(bot)) {
    bot_dot_loc_ = ClampLocToFrame(bot.center);
  }

  // Only update measurements when both clusters are valid
  if (IsClusterValid(top) && IsClusterValid(bot)) {
    dots_dist_px_ = DistanceBetween(top_dot_loc_, bot_dot_loc_);

    min_dots_dist_px_ = std::min(min_dots_dist_px_, dots_dist_px_);
    max_dots_dist_px_ = std::max(max_dots_dist_px_, dots_dist_px_);
  }
}

}