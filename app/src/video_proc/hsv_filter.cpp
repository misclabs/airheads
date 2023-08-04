#include "hsv_filter.h"

#include "opencv2/core.hpp"
#include "opencv2/core/mat.hpp"
#include "opencv2/imgproc.hpp"

namespace Airheads {

	static const std::string g_hsvFilterName {"HSV Filter"};

	HsvFilterUniquePtr HsvFilter::Create() {
		return std::make_unique<HsvFilter>();
	}

	const std::string& HsvFilter::Name() const {
		return g_hsvFilterName;
	}

	void HsvFilter::ProcessFrame(const FrameRef& frame) {
		cv::Mat frameMat {frame.height, frame.width, CV_8UC3, frame.data};
		cv::cvtColor(frameMat, frameMat, cv::COLOR_BGR2HSV);

		for (int i = 0; i < frame.width * frame.height * 3; i += 3) {
			frame.data[i] = frame.data[i + 2];
			frame.data[i + 1] = frame.data[i + 2];
		}
	}

}

