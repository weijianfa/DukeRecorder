// Standalone test for src/encode/annexb_writer.
// No external framework; CHECK macro + main() returning nonzero on failure.

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "encode/annexb_writer.h"

namespace {

int g_failures = 0;

#define CHECK(cond)                                                        \
  do {                                                                     \
    if (!(cond)) {                                                         \
      std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
      ++g_failures;                                                        \
    }                                                                      \
  } while (0)

using duke::AvccToAnnexB;
using duke::AppendAud;
using duke::AppendNal;
using duke::AnnexBWriter;

bool Eq(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b) {
  return a.size() == b.size() && std::memcmp(a.data(), b.data(), a.size()) == 0;
}

std::vector<uint8_t> Bytes(std::initializer_list<uint8_t> il) {
  return std::vector<uint8_t>(il);
}

void TestAppendNal() {
  std::vector<uint8_t> dst;
  const uint8_t nal[] = {0x26, 0x01, 0x02, 0x03};
  AppendNal(dst, nal, sizeof(nal));
  CHECK(Eq(dst, Bytes({0, 0, 0, 1, 0x26, 0x01, 0x02, 0x03})));
}

void TestAppendNalAppends() {
  std::vector<uint8_t> dst = Bytes({0xFF});
  const uint8_t nal[] = {0x65};
  AppendNal(dst, nal, sizeof(nal));
  CHECK(Eq(dst, Bytes({0xFF, 0, 0, 0, 1, 0x65})));
}

void TestAppendAud() {
  std::vector<uint8_t> dst;
  AppendAud(dst);
  // start code + AUD NAL (type 9) + pic_type byte.
  CHECK(Eq(dst, Bytes({0, 0, 0, 1, 0x09, 0xF0})));
}

void TestAvccSingleNal() {
  // length-prefixed (BE u32): one 4-byte NAL.
  const uint8_t avcc[] = {0, 0, 0, 4, 0x26, 0x01, 0x02, 0x03};
  auto out = AvccToAnnexB(avcc, sizeof(avcc));
  CHECK(Eq(out, Bytes({0, 0, 0, 1, 0x26, 0x01, 0x02, 0x03})));
}

void TestAvccMultipleNals() {
  // two NALs: [2]AA BB, [1]CC
  const uint8_t avcc[] = {0, 0, 0, 2, 0xAA, 0xBB, 0, 0, 0, 1, 0xCC};
  auto out = AvccToAnnexB(avcc, sizeof(avcc));
  CHECK(Eq(out, Bytes({0, 0, 0, 1, 0xAA, 0xBB, 0, 0, 0, 1, 0xCC})));
}

void TestAvccEmpty() {
  auto out = AvccToAnnexB(nullptr, 0);
  CHECK(out.empty());
}

void TestAvccTruncatedStops() {
  // length claims 10 bytes but only 3 follow — must not read out of bounds.
  const uint8_t avcc[] = {0, 0, 0, 10, 0xAA, 0xBB, 0xCC};
  auto out = AvccToAnnexB(avcc, sizeof(avcc));
  // Emits the 3 available bytes only (no start code? See impl: we emit start
  // code + whatever bytes remain). At minimum: must not crash, must not
  // over-read, output must be <= input + framing overhead.
  CHECK(out.size() <= sizeof(avcc) + 4);
  CHECK(!out.empty());
}

void TestWriterAccessUnit() {
  std::vector<uint8_t> captured;
  AnnexBWriter writer([&](const uint8_t* d, size_t n) {
    captured.insert(captured.end(), d, d + n);
  });
  // one access unit: single 2-byte NAL.
  const uint8_t au[] = {0, 0, 0, 2, 0xAA, 0xBB};
  writer.WriteAccessUnit(au, sizeof(au));
  // Expect AUD + that NAL.
  CHECK(Eq(captured, Bytes({0, 0, 0, 1, 0x09, 0xF0, 0, 0, 0, 1, 0xAA, 0xBB})));
}

}  // namespace

int main() {
  TestAppendNal();
  TestAppendNalAppends();
  TestAppendAud();
  TestAvccSingleNal();
  TestAvccMultipleNals();
  TestAvccEmpty();
  TestAvccTruncatedStops();
  TestWriterAccessUnit();

  if (g_failures > 0) {
    std::fprintf(stderr, "%d check(s) failed\n", g_failures);
    return EXIT_FAILURE;
  }
  std::printf("annexb_writer tests passed\n");
  return EXIT_SUCCESS;
}
