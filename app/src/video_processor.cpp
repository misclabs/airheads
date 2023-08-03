#include "video_processor.h"

#include "opencv2/core.hpp"
#include "opencv2/core/mat.hpp"
#include "opencv2/imgproc.hpp"

namespace Airheads {

	void VideoProcessor::ProcessFrame(int width, int height, unsigned char* inOut_data) {
		cv::Mat frame {width, height, CV_8UC3, inOut_data};
		cv::cvtColor(frame, frame, cv::COLOR_BGR2HSV);

		for (int i = 0; i < width * height * 3; i += 3) {
			inOut_data[i] = inOut_data[i + 2];
			inOut_data[i + 1] = inOut_data[i + 2];
		}
	}

}

