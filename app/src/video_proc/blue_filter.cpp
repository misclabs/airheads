#include "blue_filter.h"

#include "imgui.h"

#include <algorithm>

namespace Airheads {

	static const std::string g_blueFilterName {"Blue Filter"};

	BlueFilterUniquePtr BlueFilter::Create() {
		return std::make_unique<BlueFilter>();
	}

	const std::string& BlueFilter::Name() const {
		return g_blueFilterName;
	}

	void BlueFilter::ProcessFrame(ProcessingContext& context) {
		for (int i = 0; i < context.frame.cols * context.frame.rows * 3; i += 3) {
			context.frame.data[i] = (unsigned char)std::clamp(context.frame.data[i] + m_blueValue, 0, 255);
		}
	}

	void BlueFilter::UpdateGuiControls() {
		//ImGui::SameLine();
		ImGui::SliderInt("###blue_channel_value", &m_blueValue, -UCHAR_MAX, UCHAR_MAX);
	}
}