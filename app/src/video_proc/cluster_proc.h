#pragma once

#include "video_proc.h"

namespace Airheads {

	class ClusterProc;
	using ClusterProcUniquePtr = std::unique_ptr<ClusterProc>;

	class ClusterProc : public VideoProcessor {
	public:

		static ClusterProcUniquePtr Create();

		const std::string& Name() const override;
		
		void ProcessFrame(ProcessingContext& context) override;
	
	private:
		int m_clusterColor = 50; // #ought to be significantly less than value_threshold
	};

}