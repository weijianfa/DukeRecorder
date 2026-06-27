#ifndef DUKE_CAPTURE_CAPTURE_ENGINE_H_
#define DUKE_CAPTURE_CAPTURE_ENGINE_H_

#include <cstdint>
#include <functional>

#include "core/config.h"
#include "core/frame.h"

namespace duke {

// Pixel format FourCC codes, packed little-endian (first char in low byte).
// Matches the layout used by DXGI (BGRA) and libyuv (ARGB/NV12) so frame
// buffers can be handed between modules without reinterpreting.
inline constexpr uint32_t MakeFourCC(char a, char b, char c, char d) {
  return static_cast<uint32_t>(a) | (static_cast<uint32_t>(b) << 8) |
         (static_cast<uint32_t>(c) << 16) | (static_cast<uint32_t>(d) << 24);
}
constexpr uint32_t kFourCCBGRA = MakeFourCC('B', 'G', 'R', 'A');
constexpr uint32_t kFourCCNV12 = MakeFourCC('N', 'V', '1', '2');

// Per-frame raw capture callback. data is valid only for the duration of the
// call; the consumer must copy if it needs the bytes beyond the call.
using RawFrameCallback =
    std::function<void(const uint8_t* data, const FrameDesc& desc)>;

// Abstract capture source. Backends: DXGI Desktop Duplication (Windows),
// ScreenCaptureKit (macOS, stubbed in P2), TestCaptureEngine (synthetic).
class CaptureEngine {
 public:
  virtual ~CaptureEngine() = default;
  virtual bool Initialize(const RecorderConfig& config) = 0;
  virtual bool Start() = 0;
  virtual bool Stop() = 0;
  virtual void SetCallback(RawFrameCallback callback) = 0;

  // Report the actual capture geometry/framerate (may differ from the
  // requested config — e.g. desktop resolution). The encoder uses these.
  virtual void GetEncoderParams(uint32_t& width, uint32_t& height,
                                uint32_t& fps) = 0;
};

}  // namespace duke

#endif  // DUKE_CAPTURE_CAPTURE_ENGINE_H_
