#pragma once

#include "video_proc.h"

namespace Airheads {

	class ResultsOverlay;
	using ResultsOverlayUniquePtr = std::unique_ptr<ResultsOverlay>;

	class ResultsOverlay : public VideoProcessor {
	public:

		static ResultsOverlayUniquePtr Create();

		const std::string& Name() const override;

		void ProcessFrame(ProcessingContext& context) override;
	};
}