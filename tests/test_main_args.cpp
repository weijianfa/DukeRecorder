// Standalone test for src/cli (argument parsing). No deps; fully local.
// main.cpp's real record path (Recorder + x264 + DXGI) is CI-verified.

#include <cstdio>
#include <cstdlib>
#include <string>

#include "cli.h"

namespace {

int g_failures = 0;

#define CHECK(cond)                                                        \
  do {                                                                     \
    if (!(cond)) {                                                         \
      std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
      ++g_failures;                                                        \
    }                                                                      \
  } while (0)

duke::CliOptions Parse(int argc, char** argv, bool expect_ok = true) {
  duke::CliOptions out;
  std::string err;
  bool ok = duke::ParseArgs(argc, argv, out, err);
  CHECK(ok == expect_ok);
  if (!expect_ok) CHECK(!err.empty());
  return out;
}

void TestVersion() {
  char* a[] = {(char*)"duke", (char*)"--version"};
  auto o = Parse(2, a);
  CHECK(o.mode == duke::CliMode::Version);
}

void TestHelp() {
  char* a[] = {(char*)"duke", (char*)"--help"};
  auto o = Parse(2, a);
  CHECK(o.mode == duke::CliMode::Help);
}

void TestDefaultIsVersion() {
  char* a[] = {(char*)"duke"};
  auto o = Parse(1, a);
  CHECK(o.mode == duke::CliMode::Version);
}

void TestRecordBasic() {
  char* a[] = {(char*)"duke", (char*)"--record", (char*)"--out",
               (char*)"x.h264", (char*)"--duration", (char*)"5"};
  auto o = Parse(6, a);
  CHECK(o.mode == duke::CliMode::Record);
  CHECK(o.output_path == "x.h264");
  CHECK(o.duration_seconds == 5);
  CHECK(o.config.output_path == "x.h264");
}

void TestRecordDurationZero() {
  char* a[] = {(char*)"duke", (char*)"--record", (char*)"--out",
               (char*)"x.h264", (char*)"--duration", (char*)"0"};
  auto o = Parse(6, a);
  CHECK(o.duration_seconds == 0);  // 0 = until SIGINT
}

void TestRecordFpsBitrate() {
  char* a[] = {(char*)"duke", (char*)"--record", (char*)"--out",
               (char*)"x.h264", (char*)"--fps", (char*)"60",
               (char*)"--bitrate", (char*)"8000"};
  auto o = Parse(8, a);
  CHECK(o.config.fps == 60);
  CHECK(o.config.bitrate_kbps == 8000);
}

void TestRecordWidthHeight() {
  char* a[] = {(char*)"duke", (char*)"--record",     (char*)"--out",
               (char*)"x.h264", (char*)"--width",     (char*)"1280",
               (char*)"--height", (char*)"720"};
  auto o = Parse(8, a);
  CHECK(o.config.width == 1280);
  CHECK(o.config.height == 720);
}

void TestRecordMissingOutFails() {
  char* a[] = {(char*)"duke", (char*)"--record", (char*)"--duration", (char*)"5"};
  Parse(4, a, /*expect_ok=*/false);
}

void TestUnknownArgFails() {
  char* a[] = {(char*)"duke", (char*)"--bogus"};
  Parse(2, a, /*expect_ok=*/false);
}

void TestBadNumberFails() {
  char* a[] = {(char*)"duke", (char*)"--record", (char*)"--out",
               (char*)"x.h264", (char*)"--fps", (char*)"notanumber"};
  Parse(6, a, /*expect_ok=*/false);
}

}  // namespace

int main() {
  TestVersion();
  TestHelp();
  TestDefaultIsVersion();
  TestRecordBasic();
  TestRecordDurationZero();
  TestRecordFpsBitrate();
  TestRecordWidthHeight();
  TestRecordMissingOutFails();
  TestUnknownArgFails();
  TestBadNumberFails();

  if (g_failures > 0) {
    std::fprintf(stderr, "%d check(s) failed\n", g_failures);
    return EXIT_FAILURE;
  }
  std::printf("cli (main args) tests passed\n");
  return EXIT_SUCCESS;
}
