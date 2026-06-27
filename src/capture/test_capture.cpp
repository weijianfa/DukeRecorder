#include "capture/test_capture.h"

#include <algorithm>

namespace duke {

TestCaptureEngine::~TestCaptureEngine() { Stop(); }

bool TestCaptureEngine::Initialize(const RecorderConfig& config) {
  if (config.width == 0 || config.height == 0 || config.fps == 0) {
    return false;
  }
  config_ = config;
  frame_.assign(static_cast<size_t>(config.width) * config.height * 4, 0);
  return true;
}

bool TestCaptureEngine::Start() {
  if (running_.load() || frame_.empty()) {
    return false;
  }
  stop_.store(false);
  running_.store(true);
  thread_ = std::thread(&TestCaptureEngine::Run, this);
  return true;
}

bool TestCaptureEngine::Stop() {
  if (!running_.load() && !thread_.joinable()) {
    return false;
  }
  stop_.store(true);
  if (thread_.joinable()) {
    thread_.join();
  }
  running_.store(false);
  return true;
}

void TestCaptureEngine::SetCallback(RawFrameCallback callback) {
  callback_ = std::move(callback);
}

void TestCaptureEngine::GetEncoderParams(uint32_t& width, uint32_t& height,
                                         uint32_t& fps) {
  width = config_.width;
  height = config_.height;
  fps = config_.fps;
}

void TestCaptureEngine::SetFrameCount(uint32_t n) { max_frames_ = n; }

void TestCaptureEngine::Run() {
  const uint32_t stride = config_.width * 4;
  const TimestampUs duration_us =
      static_cast<TimestampUs>(1000000) / config_.fps;
  uint32_t frame_index = 0;
  while (!stop_.load()) {
    if (max_frames_ != 0 && frame_index >= max_frames_) {
      break;
    }
    // Moving pattern: every byte = (frame_index + pixel_offset) & 0xFF, so
    // consecutive frames differ and the content is cheap to generate.
    const uint8_t v = static_cast<uint8_t>(frame_index & 0xFF);
    for (size_t i = 0; i < frame_.size(); ++i) {
      frame_[i] = static_cast<uint8_t>(v + (i & 0x3F));
    }

    FrameDesc desc;
    desc.width = config_.width;
    desc.height = config_.height;
    desc.stride = stride;
    desc.format = kFourCCBGRA;
    desc.timestamp_us = static_cast<TimestampUs>(frame_index) * duration_us;
    desc.duration_us = duration_us;
    desc.is_keyframe = (frame_index == 0);

    if (callback_) {
      callback_(frame_.data(), desc);
    }
    ++frame_index;
  }
  running_.store(false);
}

}  // namespace duke
