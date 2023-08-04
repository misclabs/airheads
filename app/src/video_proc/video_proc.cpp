#include "video_proc.h"
#include "face_detector.h"
#include "hsv_filter.h"
#include "blue_filter.h"

#include "log.h"
#include "resources.h"

namespace Airheads {

	void LoadProcessors(VideoProcessorRegistry& registry) {
		registry.AddProcessor(std::move(HsvFilter::Create()));
		registry.AddProcessor(std::move(BlueFilter::Create()));

		FaceDetectorUniquePtr faceDetector = FaceDetector::Create();

		const auto faceClassifierPath = Resources::GetHaarCascadesPath("haarcascade_frontalface_alt.xml");
		if (!faceDetector->LoadFaceClassifier(faceClassifierPath)) {
			APP_ERROR("Couldn't load face classifier data at {}", faceClassifierPath.string());
		}

		const auto eyesClassifierPath = Resources::GetHaarCascadesPath("haarcascade_eye_tree_eyeglasses.xml");
		if (!faceDetector->LoadEyesClassifier(eyesClassifierPath)) {
			APP_ERROR("Couldn't load eyes classifier data at {}", eyesClassifierPath.string());
		}

		registry.AddProcessor(std::move(faceDetector));
	}

}