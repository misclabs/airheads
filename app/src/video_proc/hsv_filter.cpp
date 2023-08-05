#include "hsv_filter.h"

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"

namespace Airheads {

	static const std::string g_hsvFilterName {"HSV Filter"};

	HsvFilterUniquePtr HsvFilter::Create() {
		return std::make_unique<HsvFilter>();
	}

	const std::string& HsvFilter::Name() const {
		return g_hsvFilterName;
	}

	void HsvFilter::ProcessFrame(ProcessingContext & context) {
		cv::cvtColor(context.frame, context.frame, cv::COLOR_BGR2HSV);
		for (int i = 0; i < context.frame.cols * context.frame.rows * 3; i += 3) {
			context.frame.data[i] = context.frame.data[i + 2];
			context.frame.data[i + 1] = context.frame.data[i + 2];
		}
	}

}

