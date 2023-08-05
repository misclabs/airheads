#pragma once

#include "video_proc.h"

namespace Airheads {

	class DotDiff;
	using DotDiffUniquePtr = std::unique_ptr<DotDiff>;

	class DotDiff : public VideoProcessor {
	public:

		static DotDiffUniquePtr Create();

		const std::string& Name() const override;
		void ProcessFrame(ProcessingContext& context) override;

	};

}