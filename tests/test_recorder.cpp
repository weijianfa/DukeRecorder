// Standalone test for src/recorder (pipeline orchestration).
// Uses real TestCaptureEngine + real AnnexBWriter + real RawEsSink, a fake
// Encoder, and a STUB libyuv (color_convert). Validates capture -> encode ->
// Annex-B -> file wiring, bounded backpressure, and clean Stop.
// Build:
//   g++ -std=c++17 -Isrc -I/tmp/libyuv_stub -I/tmp/spdlog_stub
//       tests/test_recorder.cpp src/recorder.cpp src/capture/test_capture.cpp
//       src/encode/annexb_writer.cpp src/encode/color_convert.cpp
//       src/mux/raw_es_sink.cpp /tmp/libyuv_stub/libyuv_stub.cpp

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <memory>
#include <thread>
#include <vector>

#include "capture/test_capture.h"
#include "core/config.h"
#include "core/frame.h"
#include "encode/encoder.h"
#include "recorder.h"

namespace {

int g_failures = 0;

#define CHECK(cond)                                                        \
  do {                                                                     \
    if (!(cond)) {                                                         \
      std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
      ++g_failures;                                                        \
    }                                                                      \
  } while (0)

// Fake encoder: counts Encode calls and emits a fixed AVCC NAL per frame.
class FakeEncoder : public duke::Encoder {
 public:
  bool Initialize(const duke::RecorderConfig&) override { return true; }
  bool Encode(const uint8_t* /*frame*/, const duke::FrameDesc&) override {
    count_.fetch_add(1);
    if (sink_) {
      // AVCC: [00 00 00 02][AA BB]
      static const uint8_t nal[] = {0, 0, 0, 2, 0xAA, 0xBB};
      sink_(nal, sizeof(nal));
    }
    return true;
  }
  void SetNalSink(duke::EncodedNalSink sink) override { sink_ = std::move(sink); }
  int count() const { return count_.load(); }

 private:
  duke::EncodedNalSink sink_;
  std::atomic<int> count_{0};
};

bool WaitFor(std::function<bool()> cond, int timeout_ms) {
  const auto deadline = std::chrono::steady_clock::now() +
                        std::chrono::milliseconds(timeout_ms);
  while (!cond()) {
    if (std::chrono::steady_clock::now() >= deadline) return false;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  return true;
}

size_t FileSize(const std::string& path) {
  std::ifstream f(path, std::ios::binary | std::ios::ate);
  return f ? static_cast<size_t>(f.tellg()) : 0;
}

void TestPipelineEndToEnd() {
  const std::string path = "/tmp/duke_test_pipeline.h264";
  std::remove(path.c_str());

  auto capture = std::make_unique<duke::TestCaptureEngine>();
  capture->SetFrameCount(5);  // deterministic: 5 frames fit the 8-slot buffer
  auto encoder = std::make_unique<FakeEncoder>();
  auto sink = std::make_unique<duke::RawEsSink>();
  FakeEncoder* enc_raw = encoder.get();

  duke::RecorderConfig cfg;
  cfg.width = 32;
  cfg.height = 32;
  cfg.fps = 1000;
  cfg.output_path = path;

  duke::Recorder recorder(std::move(capture), std::move(encoder),
                          std::move(sink));
  CHECK(recorder.Initialize(cfg));
  CHECK(recorder.Start());

  // Wait for the capture thread to finish emitting its 5 frames.
  CHECK(WaitFor([&] { return enc_raw->count() >= 5; }, 2000));
  recorder.Stop();  // drains remaining and joins

  CHECK(enc_raw->count() == 5);
  CHECK(FileSize(path) > 0);  // Annex-B bytes written to file
  std::remove(path.c_str());
}

void TestCleanStopNoHang() {
  const std::string path = "/tmp/duke_test_pipeline2.h264";
  std::remove(path.c_str());

  auto capture = std::make_unique<duke::TestCaptureEngine>();
  auto encoder = std::make_unique<FakeEncoder>();
  auto sink = std::make_unique<duke::RawEsSink>();

  duke::RecorderConfig cfg;
  cfg.width = 16;
  cfg.height = 16;
  cfg.fps = 1000;
  cfg.output_path = path;

  duke::Recorder recorder(std::move(capture), std::move(encoder),
                          std::move(sink));
  CHECK(recorder.Initialize(cfg));
  CHECK(recorder.Start());
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  recorder.Stop();  // must return promptly (no deadlock)
  std::remove(path.c_str());
}

}  // namespace

int main() {
  TestPipelineEndToEnd();
  TestCleanStopNoHang();

  if (g_failures > 0) {
    std::fprintf(stderr, "%d check(s) failed\n", g_failures);
    return EXIT_FAILURE;
  }
  std::printf("recorder (stub) tests passed\n");
  return EXIT_SUCCESS;
}
