#pragma once

#include "video_proc.h"
#include "cluster.h"

namespace Airheads {

	enum class CenterStrategy {
		Average,
		WeightedAverage,
		StrategyCount
	};

	class ClusterProc;
	using ClusterProcUniquePtr = std::unique_ptr<ClusterProc>;

	class ClusterProc : public VideoProcessor {
	public:

		static ClusterProcUniquePtr Create();

		const std::string& Name() const override;
		
		void ProcessFrame(ProcessingContext& context) override;
	
		void UpdateConfigControls() override;

	private:
		
		CenterStrategy m_centerStrat = CenterStrategy::WeightedAverage;

		int m_clusterColor = 50; // #ought to be significantly less than value_threshold
		std::vector<Cluster::ClusterPixel> m_pixels;
	};

}