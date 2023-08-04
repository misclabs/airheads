#pragma once

#include "video_proc.h"

namespace Airheads {

	class BlueFilter;
	using BlueFilterUniquePtr = std::unique_ptr<BlueFilter>;

	class BlueFilter : public VideoProcessor {
	public:

		static BlueFilterUniquePtr Create();

		const std::string& Name() const override;
		void ProcessFrame(const FrameRef& frame) override;
		void UpdateGuiControls() override;

	private:
		int m_blueValue = 128;
	};

}