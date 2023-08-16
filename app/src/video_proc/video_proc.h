#pragma once

#include "processing_context.h"
#include "opencv2/core.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <cassert>

namespace Airheads {

class VideoProcessor {
 public:

  virtual ~VideoProcessor() = default;

  [[nodiscard]] virtual const std::string &Name() const = 0;

  virtual void StartCapture(int frame_width, int frame_height) {}

  virtual void ProcessFrame(ProcessingContext &context) = 0;

  virtual void UpdateConfigControls() {};
  virtual void UpdateStatsControls() {};

  bool is_enabled_ = true;
};

using VideoProcessorUniquePtr = std::unique_ptr<VideoProcessor>;

class VideoProcessorPipeline {
 public:

  void StartCapture(int width, int height, unsigned char *data);
  void StopCapture();

  void AddProcessor(VideoProcessorUniquePtr processor);
  void ProcessFrame();
  void UpdateConfigGui();
  void UpdateStatsGui();

  void ForEach(const std::function<void(VideoProcessor &)>& operation);

  [[nodiscard]] ProcessingContext &Context() { return context_; }

 private:
  std::vector<VideoProcessorUniquePtr> processors_;
  ProcessingContext context_;
};

void LoadProcessors(VideoProcessorPipeline &registry);

}