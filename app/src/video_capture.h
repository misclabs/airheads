#pragma once

#include "vec.h"
#include "videoInput.h"
#include <vector>
#include <cstddef>

namespace Airheads {

enum class CaptureError {
  kNone,
  kUnknownDevice,
  kDeviceSetupFailure,
  kNoActiveDevice,
  kGrabFrameFailure
};

const std::string &CaptureErrorMsg(CaptureError error);

class VideoCapture {
 public:

  using DeviceId = int;
  static constexpr DeviceId kNoDevice = -1;
  static constexpr int kNumMissedFramesBeforeReconnect = 60;

  VideoCapture();

  [[nodiscard]] const std::vector<std::string> &DeviceNames() const noexcept;

  [[nodiscard]] bool IsDevice(DeviceId id) const noexcept;

  [[nodiscard]] const std::string &Name(DeviceId id) const noexcept;
  [[nodiscard]] Vec2i FrameSize(DeviceId id) noexcept;
  [[nodiscard]] size_t FrameBufferSizeBytes(DeviceId id) noexcept;

  [[nodiscard]] bool IsCameraMirrored() const noexcept { return is_camera_mirrored_; }
  void IsCameraMirrored(const bool is_camera_mirrored) { is_camera_mirrored_ = is_camera_mirrored; }

  [[nodiscard]] bool Capturing() const noexcept;

  CaptureError BeginCapture(DeviceId id);
  void EndCapture();

  [[nodiscard]] DeviceId CapturingDeviceId() const noexcept;
  CaptureError PullFrame();
  uint8_t *FrameBuffer();
  bool NewFrameAvailable();

 private:
  videoInput video_input_;
  std::vector<std::string> device_names_;
  bool is_camera_mirrored_ = true;
  DeviceId active_device_ = kNoDevice;
  std::vector<uint8_t> frame_buffer_;
};

inline const std::vector<std::string> &VideoCapture::DeviceNames() const noexcept {
  return device_names_;
}

inline uint8_t *VideoCapture::FrameBuffer() {
  return frame_buffer_.data();
}

inline bool VideoCapture::NewFrameAvailable() {
  return Capturing() && video_input_.isFrameNew(active_device_);
}

}