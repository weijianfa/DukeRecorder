// DukeRecorder entry point.
//
// P1 scope: parse command-line arguments, print version, exit cleanly. The
// full capture -> encode -> mux -> upload pipeline lands in later phases
// (see docs/context.md). Keeping this minimal lets the build system, style
// targets and CI smoke test come online before any backend code exists.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <string_view>

#include <spdlog/spdlog.h>

#include "core/config.h"

namespace {

// Tracks CMakeLists.txt PROJECT_VERSION. Update both together.
constexpr std::string_view kVersion = "1.0.0";

void PrintVersion() {
  std::printf("DukeRecorder %.*s\n", static_cast<int>(kVersion.size()),
              kVersion.data());
}

void PrintUsage() {
  std::printf(
      "DukeRecorder %.*s — enterprise-grade screen recording engine\n\n"
      "Usage: duke_recorder [options]\n\n"
      "Options:\n"
      "  -h, --help     Show this help and exit\n"
      "  -v, --version  Print version and exit\n",
      static_cast<int>(kVersion.size()), kVersion.data());
}

}  // namespace

int main(int argc, char** argv) {
  spdlog::info("DukeRecorder starting (version {})", kVersion);

  for (int i = 1; i < argc; ++i) {
    const std::string arg(argv[i]);
    if (arg == "-v" || arg == "--version") {
      PrintVersion();
      return EXIT_SUCCESS;
    }
    if (arg == "-h" || arg == "--help") {
      PrintUsage();
      return EXIT_SUCCESS;
    }
    spdlog::warn("Unknown argument ignored: {}", arg);
  }

  // P1 has no recording pipeline yet; emit config defaults and exit cleanly
  // so the CI smoke test has a meaningful no-op run.
  duke::RecorderConfig config;
  spdlog::info("Default config: {}x{} @ {}fps, bitrate={}kbps, fragment={}s",
               config.width, config.height, config.fps, config.bitrate_kbps,
               config.fragment_seconds);

  PrintVersion();
  return EXIT_SUCCESS;
}
