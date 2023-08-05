#include "dot_diff.h"

#include "opencv2/core.hpp"
//#include "opencv2/core/mat.hpp"
//#include "opencv2/core/mat.inl.hpp"
//#include "opencv2/core/matx.hpp"
#include "opencv2/imgproc.hpp"

namespace Airheads {

	namespace Cluster {
		bool seek(cv::Mat& img, cv::Point seed_guess, int minval, int imgW, int imgH, int max_radius, cv::Point& out_point) {
			
			//if (img.at(seed_guess) >= minval) {
			//	out_point = seed_guess;
			//	return true;
			//}
			//
			//const int step = 5;
			////for R in range(step, max_radius + 1, step) :
			//for (int R = step; R < max_radius + 1; R += step) {
			//	//#limit values of the squares for this Radius R
			//	//y0 = max(0, seed_guess[0] - R)
			//	//x0 = max(0, seed_guess[1] - R)
			//	//y1 = min(imgH, seed_guess[0] + R + 1)
			//	//x1 = min(imgW, seed_guess[1] + R + 1)
			//	int y0 = std::max(0, seed_guess.y - R);
			//	int x0 = std::max(0, seed_guess.x - R);
			//	int y1 = std::min(imgH, seed_guess.y + R + 1);
			//	int x1 = std::min(imgW, seed_guess.x + R + 1);

			//	//#Look North
			//	//y = y0
			//	//for x in range(x0, x1, step) :
			//	int y = y0;
			//	for (int x = x0; x < x1; step) {
			//		//#if is_cluster(img[y][x], minval):
			//		//if img[y][x] >= minval:
			//		//    return True, (y,x)
			//		if (img.at(y, x) >= minval) {
			//			out_point = { x, y };
			//			return true;
			//		}
			//	}

			//	//#Look South    
			//	//y = y1-1
			//	//for x in range(x0, x1, step):
			//	cv::Point pt;
			//	pt.y = y1 - 1;
			//	for (pt.x = x0; pt.x < x1; pt.x += step) {
			//		//#if is_cluster(img[y][x], minval):
			//		//if img[y][x] >= minval:
			//		//    return True, (y,x)
			//		if (img.at(pt) >= minval) {
			//			out_point = pt;
			//			return true;
			//		}
			//	}

			//	//#Look East
			//	//x = x0
			//	//for y in range(y0, y1, step) :
			//	pt.x = x0;
			//	for (pt.y = y0; pt.y < y1; pt.y += step) {
			//		//#if is_cluster(img[y][x], minval):
			//		//if img[y][x] >= minval:
			//		//	return True, (y,x)
			//		if (img.at(pt) >= minval) {
			//			out_point = pt;
			//			return true;
			//		}
			//	}

			//	//#Look West
			//	//x = x1-1
			//	//for y in range(y0, y1, step):
			//	pt.x = x1 - 1;
			//	for (pt.y = y0; pt.y < y1; pt.y += step) {
			//		//#if is_cluster(img[y][x], minval):
			//		//if img[y][x] >= minval:
			//		//	return True, (y,x)
			//		if (img.at(pt) >= minval) {
			//			out_point = pt;
			//			return true;
			//		}
			//	}
			//}

			out_point = { -1, -1 };
			return false;
		}
	}

	static const std::string g_dotDiffName {"Dot Diff"};

	DotDiffUniquePtr DotDiff::Create() {
		return std::make_unique<DotDiff>();
	}

	const std::string& DotDiff::Name() const {
		return g_dotDiffName;
	}

	void DotDiff::ProcessFrame(ProcessingContext& context) {
		int value_threshold = 200; // #93
		int saturation_threshold = 65;

		//upper_cluster_frame0_coords = (219, 893) #Y, X
		//lower_cluster_frame0_coords = (945, 934) #Y, X

		//cluster_color = 50 #ought to be significantly less than value_threshold

		//dmdm_thresh = 1.08
		//text_overlay3 = "Threshold: {:.2f}".format(dmdm_thresh)

		//overlay_color = (0, 255, 0) #BGR color.a 4th and final param is possible here.If it's alpha, it doesn't work.
		//nope_color = (255, 255, 255)

		//cap = cv2.VideoCapture(input_video_file_name)
		//fps = cap.get(cv2.CAP_PROP_FPS)
			
		//frame_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
		//frame_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
		//total_frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
		//frame_dims = (frame_width, frame_height)

		//upper_cluster_last_coords = upper_cluster_frame0_coords #y, x
		//lower_cluster_last_coords = lower_cluster_frame0_coords

		//dmin = frame_width
		//dmax = 0
		//intercluster_distance_pixels = frame_width
		int intercluster_distance_pixels = context.frame.cols;
		//nframe = -1

		
		// EACH FRAME

		// in_frame is context.frame now
		//cv::Mat in_frame {height, width, CV_8UC3, data};

		double inv_thresh = (double)(255 - value_threshold);
		cv::Mat hsv;

		//_, saturation_map, value_map = cv2.split(cv2.cvtColor(in_frame, cv2.COLOR_BGR2HSV))
		cv::cvtColor(hsv, context.frame, cv::COLOR_BGR2HSV);
		cv::Mat saturation_map {hsv.rows, hsv.cols, CV_8UC1};
		cv::Mat value_map {hsv.rows, hsv.cols, CV_8UC1};
		{
			cv::Mat sv_out[] = { saturation_map, value_map };
			int from_to[] = { 1,0, 2,1 };
			cv::mixChannels(&hsv, 1, sv_out, 2, from_to, 2);
		}
		
		//_, saturation_map = cv2.threshold(saturation_map, saturation_threshold, 255, cv2.THRESH_BINARY_INV) #now saturation_map is really a mask
		cv::threshold(saturation_map, saturation_map, saturation_threshold, 255.0, cv::THRESH_BINARY_INV);

		//_, value_map = cv2.threshold(~value_map, inv_thresh, 255, cv2.THRESH_TOZERO)
		// TODO(jw): ~ operator above??? what's it do???
		cv::threshold(value_map, value_map, inv_thresh, 255.0, cv::THRESH_TOZERO);

		//#first return value of cv2.threshold is the threshold
		//#manual: https://docs.opencv.org/3.4/d7/d4d/tutorial_py_thresholding.html
		//value_map = cv2.bitwise_and(value_map, saturation_map)
		cv::bitwise_and(value_map, saturation_map, value_map);

		//max_seek_radius = int(max(100, 0.65 * intercluster_distance_pixels))
		int max_seek_radius = (int)std::max(100.0, 0.65 * intercluster_distance_pixels);


	}

}