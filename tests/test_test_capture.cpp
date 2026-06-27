// Standalone test for src/capture/capture_engine.h + test_capture.h.

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <vector>

#include "capture/capture_engine.h"
#include "capture/test_capture.h"
#include "core/config.h"

namespace {

int g_failures = 0;

#define CHECK(cond)                                                        \
  do {                                                                     \
    if (!(cond)) {                                                         \
      std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
      ++g_failures;                                                        \
    }                                                                      \
  } while (0)

bool WaitForDone(duke::TestCaptureEngine& cap, int timeout_ms) {
  const auto deadline = std::chrono::steady_clock::now() +
                        std::chrono::milliseconds(timeout_ms);
  while (cap.IsRunning()) {
    if (std::chrono::steady_clock::now() >= deadline) return false;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  return true;
}

void TestParams() {
  duke::TestCaptureEngine cap;
  duke::RecorderConfig cfg;
  cfg.width = 640;
  cfg.height = 480;
  cfg.fps = 30;
  CHECK(cap.Initialize(cfg));
  uint32_t w = 0, h = 0, fps = 0;
  cap.GetEncoderParams(w, h, fps);
  CHECK(w == 640);
  CHECK(h == 480);
  CHECK(fps == 30);
}

void TestEmitsExactFrameCount() {
  duke::TestCaptureEngine cap;
  duke::RecorderConfig cfg;
  cfg.width = 32;
  cfg.height = 32;
  cfg.fps = 1000;  // fast cadence; timestamps only, no real-time sleep
  CHECK(cap.Initialize(cfg));
  cap.SetFrameCount(30);

  std::atomic<int> count{0};
  duke::FrameDesc first_desc{};
  bool got_first = false;
  cap.SetCallback([&](const uint8_t* /*data*/, const duke::FrameDesc& desc) {
    int i = count.fetch_add(1);
    if (i == 0) {
      first_desc = desc;
      got_first = true;
    }
  });

  CHECK(cap.Start());
  CHECK(WaitForDone(cap, 2000));
  cap.Stop();
  CHECK(count.load() == 30);
  CHECK(got_first);
  CHECK(first_desc.width == 32);
  CHECK(first_desc.height == 32);
  CHECK(first_desc.stride == 32 * 4);  // BGRA = 4 bytes/px
  CHECK(first_desc.format == duke::kFourCCBGRA);
  CHECK(first_desc.duration_us == 1000000 / 1000);
  CHECK(first_desc.is_keyframe);  // first frame is a keyframe
}

void TestFramesDiffer() {
  duke::TestCaptureEngine cap;
  duke::RecorderConfig cfg;
  cfg.width = 4;
  cfg.height = 4;
  cfg.fps = 1000;
  CHECK(cap.Initialize(cfg));
  cap.SetFrameCount(3);

  std::vector<std::vector<uint8_t>> frames;
  cap.SetCallback([&](const uint8_t* data, const duke::FrameDesc& desc) {
    std::vector<uint8_t> copy(data, data + desc.stride * desc.height);
    frames.push_back(std::move(copy));
  });

  cap.Start();
  WaitForDone(cap, 2000);
  cap.Stop();
  CHECK(frames.size() == 3);
  // Consecutive frames must differ (moving pattern).
  CHECK(std::memcmp(frames[0].data(), frames[1].data(), frames[0].size()) != 0);
  CHECK(std::memcmp(frames[1].data(), frames[2].data(), frames[1].size()) != 0);
}

void TestStopHalts() {
  duke::TestCaptureEngine cap;
  duke::RecorderConfig cfg;
  cfg.width = 8;
  cfg.height = 8;
  cfg.fps = 1000;
  CHECK(cap.Initialize(cfg));
  // No SetFrameCount -> runs until Stop().
  std::atomic<int> count{0};
  cap.SetCallback([&](const uint8_t*, const duke::FrameDesc&) {
    count.fetch_add(1);
  });
  CHECK(cap.Start());
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  cap.Stop();
  CHECK(!cap.IsRunning());
  int after = count.load();
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  // No more frames after Stop.
  CHECK(count.load() == after);
}

}  // namespace

int main() {
  TestParams();
  TestEmitsExactFrameCount();
  TestFramesDiffer();
  TestStopHalts();

  if (g_failures > 0) {
    std::fprintf(stderr, "%d check(s) failed\n", g_failures);
    return EXIT_FAILURE;
  }
  std::printf("test_capture tests passed\n");
  return EXIT_SUCCESS;
}
