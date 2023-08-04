#pragma once

#include "video_proc.h"
#include <memory>
#include <filesystem>

namespace Airheads {

	class FaceDetector;
	using FaceDetectorUniquePtr = std::unique_ptr<FaceDetector>;
	class FaceDetector : public VideoProcessor {
	public:

		static FaceDetectorUniquePtr Create();

		virtual bool LoadFaceClassifier(const std::filesystem::path& path) = 0;
		virtual bool LoadEyesClassifier(const std::filesystem::path& path) = 0;
	};

}