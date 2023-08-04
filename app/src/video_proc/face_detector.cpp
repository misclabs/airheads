#include "face_detector.h"

#include "opencv2/core.hpp"
#include "opencv2/core/fast_math.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect.hpp"

#include <vector>

namespace Airheads {

	static const std::string g_faceDetectorName {"Face Detector"};

	class FaceDetectorImpl : public FaceDetector {
	public:
		FaceDetectorImpl() {};

		const std::string& Name() const override {
			return g_faceDetectorName;
		}
		void ProcessFrame(const FrameRef& frame) override;
		bool LoadFaceClassifier(const std::filesystem::path& path) override;
		bool LoadEyesClassifier(const std::filesystem::path& path) override;

	private:
		cv::CascadeClassifier cascade;
		cv::CascadeClassifier nestedCascade;

		double scale = 1.3;

		std::vector<cv::Rect> faces;
		std::vector<cv::Rect> faces2;

		cv::Mat smallImg;
		cv::Mat gray;

		std::vector<cv::Rect> nestedObjects;
		cv::Mat smallImgRoi;
	};

	FaceDetectorUniquePtr FaceDetector::Create() {
		return std::make_unique<FaceDetectorImpl>();
	}

	bool FaceDetectorImpl::LoadFaceClassifier(const std::filesystem::path& path) {
		return cascade.load(path.string());
	}

	bool FaceDetectorImpl::LoadEyesClassifier(const std::filesystem::path& path) {
		return nestedCascade.load(path.string());
	}

	void FaceDetectorImpl::ProcessFrame(const FrameRef& frame) {
		const static cv::Scalar colors[] = {
			cv::Scalar(255,0,0),
			cv::Scalar(255,128,0),
			cv::Scalar(255,255,0),
			cv::Scalar(0,255,0),
			cv::Scalar(0,128,255),
			cv::Scalar(0,255,255),
			cv::Scalar(0,0,255),
			cv::Scalar(255,0,255)
		};

		cv::Mat img {frame.height, frame.width, CV_8UC3, frame.data};
		bool tryFlip = false;

		faces.clear();
		faces2.clear();
		cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
		double fx = 1.0 / scale;
		cv::resize(gray, smallImg, cv::Size(), fx, fx, cv::INTER_LINEAR_EXACT);
		cv::equalizeHist(smallImg, smallImg);

		double t = (double)cv::getTickCount();
		cascade.detectMultiScale(smallImg, faces,
			1.1, 2, 
			0 | cv::CASCADE_SCALE_IMAGE,
			cv::Size(30, 30)
		);
		if (tryFlip) {
			// TODO(jw): flip
		}
		t = (double)cv::getTickCount() - t;
		// TODO(jw): time in ms: t*1000/cv::getTickFrequency()

		for (size_t i = 0; i < faces.size(); ++i) {
			cv::Rect r = faces[i];
			//cv::Mat smallImgRoi;
			nestedObjects.clear();
			cv::Point center;
			cv::Scalar color = colors[i % 8];
			int radius;

			double aspect_ratio = (double)r.width / r.height;
			if (0.75 < aspect_ratio && aspect_ratio < 1.3) {
				center.x = cvRound((r.x + r.width * 0.5) * scale);
				center.y = cvRound((r.y + r.height * 0.5) * scale);
				radius = cvRound((r.width + r.height) * 0.25 * scale);
				cv::circle(img, center, radius, color, 3, 8, 0);
			} else {
				cv::rectangle(img, cv::Point(cvRound(r.x*scale), cvRound(r.y*scale)),
					cv::Point(cvRound((r.x + r.width - 1) * scale), cvRound((r.y + r.height-1)*scale)),
					color, 3, 8, 0);
			}

			if (nestedCascade.empty())
				continue;

			smallImgRoi = smallImg(r);
			nestedCascade.detectMultiScale(smallImgRoi, nestedObjects,
				1.1, 2, 0
				//|CASCADE_FIND_BIGGEST_OBJECT
				//|CASCADE_DO_ROUGH_SEARCH
				//|CASCADE_DO_CANNY_PRUNING
				| cv::CASCADE_SCALE_IMAGE,
				cv::Size(30, 30)
			);
			for (size_t j = 0; j < nestedObjects.size(); ++j) {
				cv::Rect nr = nestedObjects[j];
				center.x = cvRound((r.x + nr.x + nr.width * 0.5) * scale);
				center.y = cvRound((r.y + nr.y + nr.height * 0.5) * scale);
				radius = cvRound((nr.width + nr.height) * 0.25 * scale);
				cv::circle(img, center, radius, color, 3, 8, 0);
			}
		}
	}

}