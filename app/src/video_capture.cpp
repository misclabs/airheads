#include "video_capture.h"
#include "log.h"

namespace Airheads {

static const std::string kNoneMsg = "None";
static const std::string kUnknownDeviceMsg = "Unknown device";
static const std::string kDeviceSetupFailureMsg = "Device setup failure";
static const std::string kNoActiveDeviceMsg = "No active device";
static const std::string kGrabFrameFailureMsg = "Failed to grab a video frame";
static const std::string kUnknownErrorMsg = "Unknown error";

const std::string &CaptureErrorMsg(CaptureError error) {
  switch (error) {
    case CaptureError::kNone: return kNoneMsg;
    case CaptureError::kUnknownDevice: return kUnknownDeviceMsg;
    case CaptureError::kDeviceSetupFailure: return kDeviceSetupFailureMsg;
    case CaptureError::kNoActiveDevice: return kNoActiveDeviceMsg;
    case CaptureError::kGrabFrameFailure: return kGrabFrameFailureMsg;
    default: return kUnknownErrorMsg;
  }
}

VideoCapture::VideoCapture() {
  device_names_ = videoInput::getDeviceList();
}

bool VideoCapture::IsDevice(DeviceId id) const noexcept {
  return id >= 0 && id < device_names_.size();
}

const std::string &VideoCapture::Name(DeviceId id) const noexcept {
  if (!IsDevice(id))
    return kUnknownDeviceMsg;

  return device_names_[id];
}

Vec2i VideoCapture::FrameSize(DeviceId id) noexcept {
  if (!IsDevice(id))
    return {0, 0};

  return {video_input_.getWidth(id), video_input_.getHeight(id)};
}

size_t VideoCapture::FrameBufferSizeBytes(DeviceId id) noexcept {
  if (!IsDevice(id))
    return 0;

  return video_input_.getSize(id);
}

bool VideoCapture::Capturing() const noexcept {
  return IsDevice(active_device_);
}

CaptureError VideoCapture::BeginCapture(DeviceId id) {
  if (!IsDevice(id))
    return CaptureError::kUnknownDevice;

  EndCapture();

  if (!video_input_.setupDevice(id)) {
    APP_ERROR("VideoCapture: failed to setup device {}", video_input_.getDeviceName(id));
    return CaptureError::kDeviceSetupFailure;
  }

  active_device_ = id;
  video_input_.setAutoReconnectOnFreeze(id, true, kNumMissedFramesBeforeReconnect);
  frame_buffer_.resize(video_input_.getSize(id));

  return CaptureError::kNone;
}

void VideoCapture::EndCapture() {
  if (!Capturing())
    return;

  frame_buffer_.clear();
  video_input_.stopDevice(active_device_);
  active_device_ = kNoDevice;
}

VideoCapture::DeviceId VideoCapture::CapturingDeviceId() const noexcept {
  return active_device_;
}

CaptureError VideoCapture::PullFrame() {
  if (!Capturing())
    return CaptureError::kNoActiveDevice;

  bool result = video_input_.getPixels(active_device_, frame_buffer_.data(), false, true);
  if (!result) {
    APP_ERROR("VideoCapture: getPixels failed for {}", video_input_.getDeviceName(active_device_));
    return CaptureError::kGrabFrameFailure;
  }

  if (is_camera_mirrored_) {
    const auto frame_size = FrameSize(active_device_);
    uint8_t *pixels = frame_buffer_.data();
    for (int y = 0; y < frame_size.y; ++y) {
      for (int x = 0; x < frame_size.x - x - 1; ++x) {
        int row_index = y * frame_size.x * 3;
        int left_index = x * 3;
        int right_index = (frame_size.x - x - 1) * 3;
        std::swap<uint8_t>(pixels[row_index + left_index], pixels[row_index + right_index]);
        std::swap<uint8_t>(pixels[row_index + left_index + 1], pixels[row_index + right_index + 1]);
        std::swap<uint8_t>(pixels[row_index + left_index + 2], pixels[row_index + right_index + 2]);
      }
    }
  }

  return CaptureError::kNone;
}

}