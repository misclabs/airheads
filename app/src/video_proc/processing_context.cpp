#include "processing_context.h"

namespace Airheads {

void ProcessingContext::ResetOutput() {
	top_target_.center = {Frame().cols / 2, (int) (Frame().rows * 0.3)};
	top_target_.bounds = {top_target_.center, top_target_.center};
	bot_target_.center = {Frame().cols / 2, (int) (Frame().rows * 0.6)};
	bot_target_.bounds = {bot_target_.center, bot_target_.center};
	targets_dist_px_ = Distance(top_target_.center, bot_target_.center);
	min_targets_dist_px_ = std::numeric_limits<int>::max();
	max_targets_dist_px_ = 0;
	top_cluster_ = ClusterResult();
	bot_cluster_ = ClusterResult();
}

[[nodiscard]] Vec2i ProcessingContext::ClampLocToFrame(Vec2i pt) const {
	return {
		std::clamp(pt.x, 0, Frame().cols),
		std::clamp(pt.y, 0, Frame().rows)
	};
}

[[nodiscard]] ClusterResult ProcessingContext::ClampLocToFrame(const ClusterResult& cluster) const {
	ClusterResult outCluster{};
	outCluster.center = ClampLocToFrame(cluster.center);
	outCluster.bounds.min = ClampLocToFrame(cluster.bounds.min);
	outCluster.bounds.max = ClampLocToFrame(cluster.bounds.max);
	return outCluster;
}

void ProcessingContext::InitTopTarget(Vec2i center) {
	top_target_.center = center;
	top_target_.bounds = {top_target_.center, top_target_.center};;
}

void ProcessingContext::InitBottomTarget(Vec2i center) {
	bot_target_.center = center;
	bot_target_.bounds = {bot_target_.center, bot_target_.center};;
}

void ProcessingContext::UpdateClusterResults(ClusterResult top, ClusterResult bot) {
	top_cluster_ = top;
	bot_cluster_ = bot;

	if (IsClusterValid(top)) {
		top_target_ = ClampLocToFrame(top);
	}
	if (IsClusterValid(bot)) {
		bot_target_ = ClampLocToFrame(bot);
	}

	// Only update measurements when both clusters are valid
	if (mode_ == ProcessingMode::kMeasuring && IsClusterValid(top) && IsClusterValid(bot)) {
		targets_dist_px_ = Distance(top_target_.center, bot_target_.center);

		min_targets_dist_px_ = std::min(min_targets_dist_px_, targets_dist_px_);
		max_targets_dist_px_ = std::max(max_targets_dist_px_, targets_dist_px_);
	}
}

}