#include "cluster_proc.h"
#include "cluster.h"

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "imgui.h"

#include <vector>
#include <queue>
#include <limits>
#include <cmath>

namespace Airheads {

	static const std::string g_dotDiffName {"Cluster Processor"};

	ClusterProcUniquePtr ClusterProc::Create() {
		return std::make_unique<ClusterProc>();
	}

	const std::string& ClusterProc::Name() const {
		return g_dotDiffName;
	}

	void ClusterProc::ProcessFrame(ProcessingContext& context) {
		if (context.clusterMap.empty())
			return;

		int maxSeekRadius = (int)std::max(100.0, 0.65 * context.DotsDistPx());

		auto getCluster = [&](cv::Point seedGuess) {
			auto seed = Cluster::FindSeed(context.clusterMap,
				seedGuess,
				context.InvertedValueThreshold(), 
				maxSeekRadius);
			if (seed) {
				Cluster::ClusterMetrics cluster = Cluster::ClusterFill(context.clusterMap, *seed, (uchar)context.InvertedValueThreshold(), m_clusterColor, context.maxClusterSizePx);
				return ClusterResult{ cluster.center, cluster.sizePx };
			} 

			return ClusterResult{ seedGuess };
		};

		ClusterResult upperClusterResult = getCluster(context.TopDotLoc());
		ClusterResult lowerClusterResult = getCluster(context.BotDotLoc());

		context.UpdateClusterResults(upperClusterResult, lowerClusterResult);
	}

}