#include "results_overlay.h"

#include "opencv2/imgproc.hpp"

namespace Airheads {

	static const std::string g_resultsOverlayName {"Results Overlay"};

	ResultsOverlayUniquePtr ResultsOverlay::Create() {
		return std::make_unique<ResultsOverlay>();
	}

	const std::string& ResultsOverlay::Name() const {
		return g_resultsOverlayName;
	}

	void ResultsOverlay::ProcessFrame(ProcessingContext& context) {
		const int overlayLineWidthPx = 2;
		auto topCluster = context.TopCluster();
		if (context.IsClusterValid(topCluster)) {
			int dotRadius = (int)(2 + 16.0 * topCluster.size / context.maxClusterSizePx);
			cv::circle(context.frame, topCluster.center, dotRadius, cv::Scalar(0, 255, 0), overlayLineWidthPx, cv::LINE_8);
		}

		auto botCluster = context.BotCluster();
		if (context.IsClusterValid(botCluster)) {
			int dotRadius = (int)(2 + 16.0 * botCluster.size / context.maxClusterSizePx);
			cv::circle(context.frame, botCluster.center, dotRadius, cv::Scalar(0, 255, 0), overlayLineWidthPx, cv::LINE_8);
		}

		cv::circle(context.frame, context.TopDotLoc(), 6, cv::Scalar(255, 0, 255), cv::FILLED, cv::FILLED);
		cv::circle(context.frame, context.BotDotLoc(), 6, cv::Scalar(255, 0, 255), cv::FILLED, cv::FILLED);

		if (context.IsClusterValid(topCluster) && context.IsClusterValid(botCluster)) {
			cv::line(context.frame, context.TopDotLoc(), context.BotDotLoc(), cv::Scalar(0, 255, 0), overlayLineWidthPx);
		}
	}
}