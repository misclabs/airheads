#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <string>
#include <iostream>

constexpr int EscKeyCode{ 27 };
const std::string WindowTitle{ "Airheads" };

int main() {
	cv::Mat frame {};
	cv::namedWindow(WindowTitle);

	std::cout << "OK LET'S DO THIS: Opening camera..." << std::endl;
	cv::VideoCapture capture{ 0 };
	if (!capture.isOpened()) {
		std::cerr << "LOSE: Could not open camera" << std::endl;
		return 1;
	} else {
		std::cout << "WIN: Opened camera!";
	}
	
	for (;;) {
		// Capture a single frame
		capture >> frame;
		if (!frame.empty()) {
			cv::imshow(WindowTitle, frame);
		}

		int keyPressed = cv::waitKey(16);
		if ((keyPressed & 0xff) == EscKeyCode)
			break;
	}

	return 0;
}
