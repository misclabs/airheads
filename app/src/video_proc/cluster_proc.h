#pragma once

#include "video_proc.h"
#include "cluster.h"

namespace Airheads {

enum class CenterStrategy {
  kAverage,
  kWeightedAverage,
  kStrategyCount
};

class ClusterProc;
using ClusterProcUniquePtr = std::unique_ptr<ClusterProc>;

class ClusterProc : public VideoProcessor {
 public:

  [[nodiscard]] static ClusterProcUniquePtr Create();

  [[nodiscard]] const std::string &Name() const override;

  void ProcessFrame(ProcessingContext &seed_guess) override;

  void UpdateConfigControls() override;

 private:

  CenterStrategy center_strat_ = CenterStrategy::kWeightedAverage;

  int cluster_color_ = 50; // #ought to be significantly less than value_threshold
  std::vector<Cluster::ClusterPixel> pixels_;
};

}