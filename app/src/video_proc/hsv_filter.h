#pragma once

#include "video_proc.h"

namespace Airheads {

	class HsvFilter;
	using HsvFilterUniquePtr = std::unique_ptr<HsvFilter>;

	class HsvFilter : public VideoProcessor {
	public:

		static HsvFilterUniquePtr Create();

		const std::string& Name() const override;
		void ProcessFrame(const FrameRef& frame) override;

	};

}
