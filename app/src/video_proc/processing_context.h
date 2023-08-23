#pragma once

#pragma once

#include "vec.h"
#include "cluster.h"
#include "opencv2/core.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <cassert>

namespace Airheads {

struct ClusterResult {

	[[nodiscard]] explicit ClusterResult(const Cluster::ClusterMetrics& metrics) noexcept
		: center(metrics.center), size(metrics.size_px), bounds(metrics.bounds) {}

	[[nodiscard]] explicit ClusterResult(const Vec2i& c) noexcept
		: center(c) {
	}

	[[nodiscard]] ClusterResult() noexcept = default;
	[[nodiscard]] ClusterResult(const ClusterResult&) noexcept = default;
	ClusterResult& operator=(const ClusterResult&) noexcept = default;
	[[nodiscard]] ClusterResult(ClusterResult&&) noexcept = default;
	ClusterResult& operator=(ClusterResult&&) noexcept = default;

	Vec2i center = {0, 0};
	int size = 0;
	Bounds2i bounds = {};
};

enum class ProcessingMode {
	kCalibration,
	kTesting
};

class ProcessingContext {
public:
	void SetFrameBGR(int width, int height, unsigned char* data);
	void ClearFrame();

	void ResetOutput();
	void UpdateClusterResults(ClusterResult top, ClusterResult bot);

	[[nodiscard]] Vec2i TopTargetLoc() const { return top_target_.center; }
	[[nodiscard]] Vec2i BotTargetLoc() const { return bot_target_.center; }
	[[nodiscard]] const ClusterResult& TopTarget() const { return top_target_; }
	[[nodiscard]] const ClusterResult& BotTarget() const { return bot_target_; }
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
	[[nodiscard]] float PxToCm(float px) const { return px * 1.0f / frame_pixels_per_cm_; }

	void SetMode(ProcessingMode mode) { mode_ = mode; }

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
	[[nodiscard]] Vec2i ClampLocToFrame(Vec2i pt) const;
	[[nodiscard]] ClusterResult ClampLocToFrame(const ClusterResult& cluster) const;

	// frame is input from the camera and displayed as the end result.
	//   - It may be written to by a VideoProcessor in the pipeline.
	//   - It is the only Mat that doesn't own it's data
	//   - Data format is BGR, 8-bits per channel (0-255)
	cv::Mat frame_;

	ClusterResult top_target_;
	ClusterResult bot_target_;

	ClusterResult top_cluster_;
	ClusterResult bot_cluster_;

	int targets_dist_px_;
	int min_targets_dist_px_;
	int max_targets_dist_px_;

	ProcessingMode mode_;

};

inline void ProcessingContext::SetFrameBGR(int width, int height, unsigned char* data) {
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