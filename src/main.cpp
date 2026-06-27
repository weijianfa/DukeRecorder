// DukeRecorder entry point.
//
// P2: --record drives the full capture -> encode -> Annex-B-ES pipeline via
// Recorder. --version/--help remain dep-free so the CI smoke test works
// without the recording backends. The real record path (DXGI + x264) is built
// only where those deps are available (Windows CI).

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>
#include <string_view>
#include <thread>

#include <spdlog/spdlog.h>

#include "capture/capture_engine.h"
#include "capture/dxgi_capture.h"
#include "cli.h"
#include "core/config.h"
#include "encode/x264_encoder.h"
#include "mux/raw_es_sink.h"
#include "recorder.h"

namespace {

// Tracks CMakeLists.txt PROJECT_VERSION. Update both together.
constexpr std::string_view kVersion = "1.0.0";

std::atomic<bool> g_interrupted{false};

extern "C" void OnSignal(int /*sig*/) { g_interrupted.store(true); }

void PrintVersion() {
  std::printf("DukeRecorder %.*s\n", static_cast<int>(kVersion.size()),
              kVersion.data());
}

void PrintUsage() {
  std::printf(
      "DukeRecorder %.*s — enterprise-grade screen recording engine\n\n"
      "Usage: duke_recorder [options]\n\n"
      "Options:\n"
      "  -h, --help        Show this help and exit\n"
      "  -v, --version     Print version and exit\n"
      "  --record          Record to --out (raw H.264 Annex-B ES in P2)\n"
      "  --out <path>      Output file (required with --record)\n"
      "  --duration <sec>  Stop after N seconds (0 = until Ctrl-C)\n"
      "  --width <px>      Capture width (default 1920)\n"
      "  --height <px>     Capture height (default 1080)\n"
      "  --fps <n>         Frame rate (default 30)\n"
      "  --bitrate <kbps>  Target bitrate (default 4000)\n",
      static_cast<int>(kVersion.size()), kVersion.data());
}

// Block until the duration elapses or SIGINT is received. duration_seconds == 0
// means "until Ctrl-C".
void WaitForStop(uint32_t duration_seconds) {
  using clock = std::chrono::steady_clock;
  const auto start = clock::now();
  const auto dur = std::chrono::seconds(duration_seconds);
  while (!g_interrupted.load()) {
    if (duration_seconds > 0) {
      const auto elapsed = clock::now() - start;
      if (elapsed >= dur) break;
      auto remaining = dur - elapsed;
      if (remaining > std::chrono::milliseconds(100)) {
        remaining = std::chrono::milliseconds(100);
      }
      std::this_thread::sleep_for(remaining);
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
}

std::unique_ptr<duke::CaptureEngine> MakeCapture() {
#ifdef DUKE_PLATFORM_WINDOWS
  return std::make_unique<duke::DxgiCaptureEngine>();
#else
  // macOS ScreenCaptureKit is not implemented in P2.
  return nullptr;
#endif
}

int RunRecord(const duke::CliOptions& opts) {
  auto capture = MakeCapture();
  if (capture == nullptr) {
    spdlog::error("No capture backend on this platform (P2 supports Windows)");
    return EXIT_FAILURE;
  }
  auto encoder = std::make_unique<duke::X264Encoder>();
  auto sink = std::make_unique<duke::RawEsSink>();

  duke::Recorder recorder(std::move(capture), std::move(encoder),
                          std::move(sink));
  if (!recorder.Initialize(opts.config)) {
    return EXIT_FAILURE;
  }
  if (!recorder.Start()) {
    return EXIT_FAILURE;
  }

  spdlog::info("Recording (duration {}s, 0 = Ctrl-C to stop)",
               opts.duration_seconds);
  WaitForStop(opts.duration_seconds);
  recorder.Stop();
  return EXIT_SUCCESS;
}

}  // namespace

int main(int argc, char** argv) {
  std::signal(SIGINT, OnSignal);

  duke::CliOptions opts;
  std::string err;
  if (!duke::ParseArgs(argc, argv, opts, err)) {
    std::fprintf(stderr, "error: %s\n", err.c_str());
    PrintUsage();
    return EXIT_FAILURE;
  }

  switch (opts.mode) {
    case duke::CliMode::Help:
      PrintUsage();
      return EXIT_SUCCESS;
    case duke::CliMode::Version:
      PrintVersion();
      return EXIT_SUCCESS;
    case duke::CliMode::Record:
      return RunRecord(opts);
  }
  return EXIT_SUCCESS;
}
