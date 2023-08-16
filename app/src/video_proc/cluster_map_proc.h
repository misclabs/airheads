#pragma once

#include "video_proc.h"

namespace Airheads {

	class ClusterMapProc;
	using ClusterMapProcUniquePtr = std::unique_ptr<ClusterMapProc>;

	class ClusterMapProc : public VideoProcessor {
	public:

		static ClusterMapProcUniquePtr Create();

		const std::string& Name() const override;

		void ProcessFrame(ProcessingContext& context) override;

	private:

		cv::Mat hsv;
	};

}