// Standalone test for src/mux/raw_es_sink.

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

#include "mux/raw_es_sink.h"

namespace {

int g_failures = 0;

#define CHECK(cond)                                                        \
  do {                                                                     \
    if (!(cond)) {                                                         \
      std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
      ++g_failures;                                                        \
    }                                                                      \
  } while (0)

std::vector<uint8_t> ReadFile(const std::string& path) {
  std::ifstream f(path, std::ios::binary);
  return std::vector<uint8_t>((std::istreambuf_iterator<char>(f)),
                              std::istreambuf_iterator<char>());
}

void TestWriteAndClose() {
  const std::string path = "/tmp/duke_test_es_1.h264";
  std::remove(path.c_str());
  duke::RawEsSink sink;
  CHECK(sink.Open(path));
  const uint8_t bytes[] = {0, 0, 0, 1, 0x09, 0xF0};
  CHECK(sink.Write(bytes, sizeof(bytes)));
  sink.Close();
  auto got = ReadFile(path);
  CHECK(got.size() == sizeof(bytes));
  CHECK(std::memcmp(got.data(), bytes, sizeof(bytes)) == 0);
  std::remove(path.c_str());
}

void TestMultipleWritesAccumulate() {
  const std::string path = "/tmp/duke_test_es_2.h264";
  std::remove(path.c_str());
  duke::RawEsSink sink;
  CHECK(sink.Open(path));
  const uint8_t a[] = {1, 2, 3};
  const uint8_t b[] = {4, 5};
  CHECK(sink.Write(a, sizeof(a)));
  CHECK(sink.Write(b, sizeof(b)));
  sink.Close();
  auto got = ReadFile(path);
  CHECK(got.size() == 5);
  CHECK(got[0] == 1 && got[4] == 5);
  std::remove(path.c_str());
}

void TestReopenTruncates() {
  const std::string path = "/tmp/duke_test_es_3.h264";
  std::remove(path.c_str());
  duke::RawEsSink sink;
  CHECK(sink.Open(path));
  const uint8_t big[] = {1, 2, 3, 4, 5, 6, 7, 8};
  CHECK(sink.Write(big, sizeof(big)));
  sink.Close();
  // Reopen and write fewer bytes — file must be truncated, not appended.
  CHECK(sink.Open(path));
  const uint8_t small[] = {9, 9};
  CHECK(sink.Write(small, sizeof(small)));
  sink.Close();
  auto got = ReadFile(path);
  CHECK(got.size() == 2);
  CHECK(got[0] == 9 && got[1] == 9);
  std::remove(path.c_str());
}

void TestCloseIdempotent() {
  const std::string path = "/tmp/duke_test_es_4.h264";
  std::remove(path.c_str());
  duke::RawEsSink sink;
  CHECK(sink.Open(path));
  const uint8_t b[] = {0x42};
  CHECK(sink.Write(b, sizeof(b)));
  sink.Close();
  sink.Close();  // must not crash / double-close.
  auto got = ReadFile(path);
  CHECK(got.size() == 1 && got[0] == 0x42);
  std::remove(path.c_str());
}

void TestWriteBeforeOpenFails() {
  duke::RawEsSink sink;
  const uint8_t b[] = {1};
  CHECK(!sink.Write(b, sizeof(b)));  // not open
}

void TestOpenBadPathFails() {
  duke::RawEsSink sink;
  // A path under a nonexistent directory cannot be opened.
  CHECK(!sink.Open("/nonexistent_dir_xyz/duke/out.h264"));
}

void TestDestructorCloses() {
  const std::string path = "/tmp/duke_test_es_5.h264";
  std::remove(path.c_str());
  {
    duke::RawEsSink sink;
    CHECK(sink.Open(path));
    const uint8_t b[] = {0x77};
    CHECK(sink.Write(b, sizeof(b)));
    // destructor closes
  }
  auto got = ReadFile(path);
  CHECK(got.size() == 1 && got[0] == 0x77);
  std::remove(path.c_str());
}

}  // namespace

int main() {
  TestWriteAndClose();
  TestMultipleWritesAccumulate();
  TestReopenTruncates();
  TestCloseIdempotent();
  TestWriteBeforeOpenFails();
  TestOpenBadPathFails();
  TestDestructorCloses();

  if (g_failures > 0) {
    std::fprintf(stderr, "%d check(s) failed\n", g_failures);
    return EXIT_FAILURE;
  }
  std::printf("raw_es_sink tests passed\n");
  return EXIT_SUCCESS;
}
