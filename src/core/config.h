#ifndef DUKE_CORE_CONFIG_H_
#define DUKE_CORE_CONFIG_H_

#include <cstdint>
#include <string>

namespace duke {

// Recorder configuration. Populated from CLI args / control protocol and
// consumed by every downstream module (capture, encode, mux, upload).
//
// Defaults reflect a 1080p30 recording with 4s fMP4 fragments, which is the
// design-doc baseline (see docs/context.md). Fields are intentionally
// value-initialized so a default-constructed RecorderConfig is usable.
struct RecorderConfig {
  uint32_t width = 1920;
  uint32_t height = 1080;
  uint32_t fps = 30;
  uint32_t bitrate_kbps = 4000;
  uint32_t gop_seconds = 2;       // GOP length (seconds)
  uint32_t fragment_seconds = 4;  // fMP4 fragment length (seconds)
  std::string output_path;        // local output path
  bool enable_streaming = true;   // enable streaming upload
  std::string upload_endpoint;    // upload endpoint
};

}  // namespace duke

#endif  // DUKE_CORE_CONFIG_H_
