#ifndef DUKE_CAPTURE_TEST_CAPTURE_H_
#define DUKE_CAPTURE_TEST_CAPTURE_H_

#include <atomic>
#include <cstdint>
#include <thread>
#include <vector>

#include "capture/capture_engine.h"
#include "core/config.h"

namespace duke {

// Synthetic CaptureEngine for testing and diagnostics. Emits BGRA frames with
// a moving test pattern (so consecutive frames differ) at the configured fps
// cadence — timestamps and duration_us reflect fps, but frames are emitted as
// fast as possible (no real-time sleep) so tests are deterministic and fast.
//
// Frame count: SetFrameCount(n) stops after exactly n frames; default 0 runs
// until Stop(). Frame 0 is marked is_keyframe.
class TestCaptureEngine : public CaptureEngine {
 public:
  TestCaptureEngine() = default;
  ~TestCaptureEngine() override;

  bool Initialize(const RecorderConfig& config) override;
  bool Start() override;
  bool Stop() override;
  void SetCallback(RawFrameCallback callback) override;
  void GetEncoderParams(uint32_t& width, uint32_t& height,
                        uint32_t& fps) override;

  // Stop after emitting n frames (0 = run until Stop()).
  void SetFrameCount(uint32_t n);

  // True while the capture thread is producing frames.
  bool IsRunning() const { return running_.load(); }

 private:
  void Run();

  RecorderConfig config_;
  RawFrameCallback callback_;
  std::vector<uint8_t> frame_;  // reused BGRA buffer
  uint32_t max_frames_ = 0;
  std::atomic<bool> running_{false};
  std::atomic<bool> stop_{false};
  std::thread thread_;
};

}  // namespace duke

#endif  // DUKE_CAPTURE_TEST_CAPTURE_H_
