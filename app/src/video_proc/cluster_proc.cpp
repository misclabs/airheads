#include "cluster_proc.h"
#include "cluster.h"
#include "opencv2/core.hpp"
#include "imgui.h"
#include <cmath>

namespace Airheads {

static const std::string kDotDiffName{"Cluster Processor"};

ClusterProcUniquePtr ClusterProc::Create() {
  return std::make_unique<ClusterProc>();
}

const std::string &ClusterProc::Name() const {
  return kDotDiffName;
}

void ClusterProc::ProcessFrame(ProcessingContext &context) {
  if (context.cluster_map_.empty())
    return;

  int max_seek_radius = (int) std::max(context.CmToPx(3.0f), 0.65f * (float) context.TargetsDistPx());
  int seek_step = (int) std::min(1.0f, context.CmToPx(context.target_diameter_cm_/2.0f));
  auto get_cluster = [&](cv::Point seed_guess) {

    auto seed = Cluster::FindSeed(context.cluster_map_,
                                  seed_guess,
                                  context.InvertedValueThreshold(),
                                  max_seek_radius, seek_step);
    if (seed) {
      pixels_.clear();
      Cluster::ClusterFill(context.cluster_map_, *seed, (uchar) context.InvertedValueThreshold(),
                           cluster_color_, context.max_cluster_size_px_, pixels_);

      Cluster::ClusterMetrics metrics;
      if (center_strat_ == CenterStrategy::kWeightedAverage)
        metrics = Cluster::ClusterMetrics::FromWeightedPixels(pixels_);
      else
        metrics = Cluster::ClusterMetrics::FromPixels(pixels_);

      return ClusterResult{metrics.center, metrics.size_px};
    }

    return ClusterResult{seed_guess};
  };

  ClusterResult upper_cluster_result = get_cluster(context.TopTargetLoc());
  ClusterResult lower_cluster_result = get_cluster(context.BotTargetLoc());

  context.UpdateClusterResults(upper_cluster_result, lower_cluster_result);
}

void ClusterProc::UpdateConfigControls() {
  const char *strat_names[(int) CenterStrategy::kStrategyCount] = {"Average", "Weighted Average"};
  const char *preview_name = strat_names[(int) center_strat_];

  if (ImGui::BeginCombo("Find Center Strategy", preview_name, 0)) {
    for (int i = 0; i < (int) CenterStrategy::kStrategyCount; ++i) {
      const bool is_selected = ((int) center_strat_ == i);
      if (ImGui::Selectable(strat_names[i], is_selected))
        center_strat_ = (CenterStrategy) i;

      // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
      if (is_selected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }

}

}