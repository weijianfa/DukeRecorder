// Standalone test for src/encode/x264_encoder, run against a STUB x264.h.
// Validates wrapper logic only (param mapping, AVCC gathering, error path);
// real x264 encode is verified on CI. Build (stub in /tmp/x264_stub):
//   g++ -std=c++17 -Isrc -I/tmp/x264_stub tests/test_x264_encoder.cpp
//       src/encode/x264_encoder.cpp /tmp/x264_stub/x264_stub.cpp

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "core/config.h"
#include "core/frame.h"
#include "encode/encoder.h"
#include "encode/x264_encoder.h"

// Inspection hooks (g_stub_last_param, g_stub_open_fail) are declared by the
// stub <x264.h>.

namespace {

int g_failures = 0;

#define CHECK(cond)                                                        \
  do {                                                                     \
    if (!(cond)) {                                                         \
      std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
      ++g_failures;                                                        \
    }                                                                      \
  } while (0)

duke::RecorderConfig MakeCfg() {
  duke::RecorderConfig cfg;
  cfg.width = 320;
  cfg.height = 240;
  cfg.fps = 30;
  cfg.bitrate_kbps = 4000;
  cfg.gop_seconds = 2;
  return cfg;
}

void TestParamMapping() {
  g_stub_open_fail = 0;
  duke::X264Encoder enc;
  CHECK(enc.Initialize(MakeCfg()));
  // keyint = gop_seconds * fps
  CHECK(g_stub_last_param.i_keyint_max == 2 * 30);
  CHECK(g_stub_last_param.i_width == 320);
  CHECK(g_stub_last_param.i_height == 240);
  CHECK(g_stub_last_param.i_fps_num == 30);
  CHECK(g_stub_last_param.i_fps_den == 1);
  CHECK(g_stub_last_param.rc.i_bitrate == 4000);
  CHECK(g_stub_last_param.b_annexb == 0);        // we AVCC-frame ourselves
  CHECK(g_stub_last_param.b_repeat_headers == 1); // SPS/PPS per IDR
}

void TestInitFailure() {
  g_stub_open_fail = 1;
  duke::X264Encoder enc;
  CHECK(!enc.Initialize(MakeCfg()));  // graceful failure, no crash
  g_stub_open_fail = 0;
}

void TestEncodeForwardsAvcc() {
  g_stub_open_fail = 0;
  duke::X264Encoder enc;
  CHECK(enc.Initialize(MakeCfg()));

  std::vector<uint8_t> captured;
  enc.SetNalSink([&](const uint8_t* d, size_t n) {
    captured.insert(captured.end(), d, d + n);
  });

  // NV12 frame: width*height*3/2 bytes.
  std::vector<uint8_t> nv12(static_cast<size_t>(320) * 240 * 3 / 2, 0x10);
  duke::FrameDesc desc{};
  desc.width = 320;
  desc.height = 240;
  desc.timestamp_us = 123456;
  desc.is_keyframe = true;
  CHECK(enc.Encode(nv12.data(), desc));

  // Stub emits one NAL of payload {0x65,0x88,0x80}; wrapper must AVCC-frame it
  // as [00 00 00 03][65 88 80].
  const uint8_t expected[] = {0, 0, 0, 3, 0x65, 0x88, 0x80};
  CHECK(captured.size() == sizeof(expected));
  CHECK(std::memcmp(captured.data(), expected, sizeof(expected)) == 0);
}

}  // namespace

int main() {
  TestParamMapping();
  TestInitFailure();
  TestEncodeForwardsAvcc();

  if (g_failures > 0) {
    std::fprintf(stderr, "%d check(s) failed\n", g_failures);
    return EXIT_FAILURE;
  }
  std::printf("x264_encoder (stub) tests passed\n");
  return EXIT_SUCCESS;
}
