#include "video_proc.h"
#include "dot_diff.h"
#include "face_detector.h"
#include "hsv_filter.h"
#include "blue_filter.h"

#include "log.h"
#include "resources.h"

namespace Airheads {

	void VideoProcessorPipeline::AddProcessor(VideoProcessorUniquePtr processor) {
		assert(processor);

		m_processors.push_back(std::move(processor));
	}

	void VideoProcessorPipeline::ProcessFrame(ProcessingContext& context) {
		for (auto& processor : m_processors) {
			if (processor->isEnabled)
				processor->ProcessFrame(context);
		}
	}

	void VideoProcessorPipeline::ForEach(std::function<void(VideoProcessor&)> operation) {
		for (auto& processor : m_processors) {
			operation(*processor);
		}
	}

	void LoadProcessors(VideoProcessorPipeline& registry) {
		registry.AddProcessor(std::move(HsvFilter::Create()));
		registry.AddProcessor(std::move(BlueFilter::Create()));

		registry.AddProcessor(std::move(DotDiff::Create()));

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