#include "cli.h"

#include <cstdlib>
#include <string>

namespace duke {

namespace {

// Parse a non-negative integer; returns false on failure.
bool ParseUint(const char* s, uint32_t& out) {
  if (s == nullptr || *s == '\0') return false;
  char* end = nullptr;
  const long v = std::strtol(s, &end, 10);
  if (*end != '\0' || v < 0) return false;
  out = static_cast<uint32_t>(v);
  return true;
}

}  // namespace

bool ParseArgs(int argc, char** argv, CliOptions& out, std::string& err) {
  out = CliOptions{};
  for (int i = 1; i < argc; ++i) {
    const std::string arg(argv[i]);

    if (arg == "-v" || arg == "--version") {
      out.mode = CliMode::Version;
    } else if (arg == "-h" || arg == "--help") {
      out.mode = CliMode::Help;
    } else if (arg == "--record") {
      out.mode = CliMode::Record;
    } else if (arg == "--out") {
      if (++i >= argc) {
        err = "--out requires a path";
        return false;
      }
      out.output_path = argv[i];
      out.config.output_path = out.output_path;
    } else if (arg == "--duration") {
      if (++i >= argc || !ParseUint(argv[i], out.duration_seconds)) {
        err = "--duration requires a non-negative integer";
        return false;
      }
    } else if (arg == "--fps") {
      if (++i >= argc || !ParseUint(argv[i], out.config.fps) || out.config.fps == 0) {
        err = "--fps requires a positive integer";
        return false;
      }
    } else if (arg == "--bitrate") {
      if (++i >= argc || !ParseUint(argv[i], out.config.bitrate_kbps)) {
        err = "--bitrate requires a non-negative integer";
        return false;
      }
    } else if (arg == "--width") {
      if (++i >= argc || !ParseUint(argv[i], out.config.width) || out.config.width == 0) {
        err = "--width requires a positive integer";
        return false;
      }
    } else if (arg == "--height") {
      if (++i >= argc || !ParseUint(argv[i], out.config.height) || out.config.height == 0) {
        err = "--height requires a positive integer";
        return false;
      }
    } else {
      err = "unknown argument: " + arg;
      return false;
    }
  }

  if (out.mode == CliMode::Record && out.output_path.empty()) {
    err = "--record requires --out <path>";
    return false;
  }
  return true;
}

}  // namespace duke
