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
				m_pixels.clear();
				Cluster::ClusterFill(context.clusterMap, *seed, (uchar)context.InvertedValueThreshold(), 
					m_clusterColor, context.maxClusterSizePx, m_pixels);

				Cluster::ClusterMetrics metrics;
				if (m_centerStrat == CenterStrategy::WeightedAverage)
					metrics = Cluster::ClusterMetrics::FromWeightedPixels(m_pixels);
				else
					metrics = Cluster::ClusterMetrics::FromPixels(m_pixels);

				return ClusterResult{ metrics.center, metrics.sizePx };
			} 

			return ClusterResult{ seedGuess };
		};

		ClusterResult upperClusterResult = getCluster(context.TopDotLoc());
		ClusterResult lowerClusterResult = getCluster(context.BotDotLoc());

		context.UpdateClusterResults(upperClusterResult, lowerClusterResult);
	}

	void ClusterProc::UpdateConfigControls() {
		const char* stratNames[(int)CenterStrategy::StrategyCount] = {"Average", "Weighted Average"};
		const char* previewName = stratNames[(int)m_centerStrat];

		if (ImGui::BeginCombo("Find Center Strategy", previewName, 0)) {
			for (int i = 0; i < (int)CenterStrategy::StrategyCount; ++i) {
				const bool is_selected = ((int)m_centerStrat == i);
				if (ImGui::Selectable(stratNames[i], is_selected))
					m_centerStrat = (CenterStrategy)i;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

	}

}