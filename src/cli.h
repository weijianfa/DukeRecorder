#ifndef DUKE_CLI_H_
#define DUKE_CLI_H_

#include <cstdint>
#include <string>

#include "core/config.h"

namespace duke {

enum class CliMode { Help, Version, Record };

// Parsed command-line options. Dependency-free so the parser is unit-testable
// without pulling in capture/encode/mux.
struct CliOptions {
  CliMode mode = CliMode::Version;  // default mirrors P1 (print version, exit)
  RecorderConfig config{};
  uint32_t duration_seconds = 0;  // 0 = record until SIGINT
  std::string output_path;        // --out target (also written to config.output_path)
};

// Parse argv into options. Returns false (with err set) on unknown flags,
// missing values, bad numbers, or record mode without --out. On success err
// is left empty.
bool ParseArgs(int argc, char** argv, CliOptions& out, std::string& err);

}  // namespace duke

#endif  // DUKE_CLI_H_
