#include "video_proc.h"
#include "cluster_map_proc.h"
#include "cluster_proc.h"
#include "resources.h"
#include "imgui.h"

namespace Airheads {

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
  ImGui::Text("Intercluster Distance: %.1fmm", context_.PxToCm((float)context_.TargetsDistPx())*10.0f);

  ImGui::Text("Min distance: %.1fmm", context_.PxToCm((float)context_.MinTargetsDistPx())*10.0f);
  ImGui::Text("Max distance: %.1fmm", context_.PxToCm((float)context_.MaxTargetsDistPx())*10.0f);
  float dmdm = (float)context_.MaxTargetsDistPx() / (float) context_.MinTargetsDistPx();
  //double dmdm_thresh = 1.08;
  //if dmdm > dmdm_thresh:
  //	dm_color = overlay_color
  //else:
  //	dm_color = nope_color
  ImGui::Text("Max distance/Min distance: %.1f", context_.PxToCm(dmdm)*10.0f);

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

}