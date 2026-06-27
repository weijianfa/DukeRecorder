// Standalone test for src/encode/color_convert, run against a STUB libyuv.
// Validates wrapper logic (buffer sizing, stride/pointer wiring, even-dim
// guard); real libyuv conversion is verified on CI.
// Build: g++ -std=c++17 -Isrc -I/tmp/libyuv_stub tests/test_color_convert.cpp
//        src/encode/color_convert.cpp /tmp/libyuv_stub/libyuv_stub.cpp

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "encode/color_convert.h"

// Inspection hooks declared by the stub <libyuv.h>.
extern int g_libyuv_last_src_stride;
extern int g_libyuv_last_dst_y_stride;
extern int g_libyuv_last_dst_uv_stride;
extern int g_libyuv_last_width;
extern int g_libyuv_last_height;

namespace {

int g_failures = 0;

#define CHECK(cond)                                                        \
  do {                                                                     \
    if (!(cond)) {                                                         \
      std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
      ++g_failures;                                                        \
    }                                                                      \
  } while (0)

void TestInitAndSize() {
  duke::BgraToNv12Converter c;
  CHECK(c.Initialize(320, 240));
  CHECK(c.nv12_size() == static_cast<size_t>(320) * 240 * 3 / 2);
}

void TestOddDimsRejected() {
  duke::BgraToNv12Converter c;
  CHECK(!c.Initialize(321, 240));  // odd width
  CHECK(!c.Initialize(320, 241));  // odd height
}

void TestConvertWiring() {
  duke::BgraToNv12Converter c;
  CHECK(c.Initialize(320, 240));
  std::vector<uint8_t> bgra(static_cast<size_t>(320) * 4 * 240, 0);
  const uint8_t* nv12 = nullptr;
  size_t len = 0;
  CHECK(c.Convert(bgra.data(), 320 * 4, &nv12, &len));
  CHECK(nv12 != nullptr);
  CHECK(len == static_cast<size_t>(320) * 240 * 3 / 2);
  // Stub fills Y plane with 0x11 and UV plane with 0x22.
  CHECK(nv12[0] == 0x11);
  CHECK(nv12[static_cast<size_t>(320) * 240] == 0x22);
  // libyuv received correct args.
  CHECK(g_libyuv_last_src_stride == 320 * 4);
  CHECK(g_libyuv_last_dst_y_stride == 320);
  CHECK(g_libyuv_last_dst_uv_stride == 320);
  CHECK(g_libyuv_last_width == 320);
  CHECK(g_libyuv_last_height == 240);
}

void TestConvertBeforeInitFails() {
  duke::BgraToNv12Converter c;
  const uint8_t* nv12 = nullptr;
  size_t len = 0;
  std::vector<uint8_t> bgra(16, 0);
  CHECK(!c.Convert(bgra.data(), 16, &nv12, &len));
}

}  // namespace

int main() {
  TestInitAndSize();
  TestOddDimsRejected();
  TestConvertWiring();
  TestConvertBeforeInitFails();

  if (g_failures > 0) {
    std::fprintf(stderr, "%d check(s) failed\n", g_failures);
    return EXIT_FAILURE;
  }
  std::printf("color_convert (stub) tests passed\n");
  return EXIT_SUCCESS;
}
