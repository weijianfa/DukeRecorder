#ifndef DUKE_CORE_FRAME_H_
#define DUKE_CORE_FRAME_H_

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "platform.h"

namespace duke {

// Description of a single video frame. Carries geometry, layout and timing
// alongside the raw/encoded bytes (held separately). timestamp_us is the
// capture instant from now_us(); it is NOT the encoder PTS/DTS — see
// EncodedFrame.
struct FrameDesc {
  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t stride = 0;  // bytes per row
  uint32_t format = 0;  // FourCC: NV12, BGRA, etc.
  TimestampUs timestamp_us = 0;  // capture timestamp
  TimestampUs duration_us = 0;   // frame duration
  bool is_keyframe = false;
};

// An encoded frame ready for muxing. pts/dts are encoder timestamps in the
// recording timeline (pause-compensated — see pause/resume design); they are
// distinct from FrameDesc::timestamp_us, which is the raw capture instant.
struct EncodedFrame {
  std::vector<uint8_t> data;
  FrameDesc desc;
  int64_t pts = 0;  // presentation timestamp
  int64_t dts = 0;  // decode timestamp
};

using ErrorCallback = std::function<void(int code, const std::string& msg)>;
using FrameCallback = std::function<void(const EncodedFrame& frame)>;

}  // namespace duke

#endif  // DUKE_CORE_FRAME_H_
