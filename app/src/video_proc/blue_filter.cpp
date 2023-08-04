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

	void BlueFilter::ProcessFrame(const FrameRef& frame) {
		for (int i = 0; i < frame.width * frame.height * 3; i += 3) {
			frame.data[i] = (unsigned char)std::clamp(frame.data[i] + m_blueValue, 0, 255);
		}
	}

	void BlueFilter::UpdateGuiControls() {
		//ImGui::SameLine();
		ImGui::SliderInt("###blue_channel_value", &m_blueValue, -UCHAR_MAX, UCHAR_MAX);
	}
}