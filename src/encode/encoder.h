#ifndef DUKE_ENCODE_ENCODER_H_
#define DUKE_ENCODE_ENCODER_H_

#include <cstddef>
#include <cstdint>
#include <functional>

#include "core/config.h"
#include "core/frame.h"

namespace duke {

// Sink for one access unit's NALs, AVCC-framed: each NAL preceded by a 4-byte
// big-endian length, concatenated. The pipeline converts this to Annex-B via
// AnnexBWriter before writing. Using AVCC here keeps the encoder backend
// independent of the output container/format.
using EncodedNalSink = std::function<void(const uint8_t* avcc, size_t len)>;

// Abstract video encoder. Backends: x264 software (P2), hardware
// MediaFoundation/VideoToolbox (later).
class Encoder {
 public:
  virtual ~Encoder() = default;
  virtual bool Initialize(const RecorderConfig& config) = 0;

  // Feed one input frame in the encoder's native format (x264: NV12). The
  // buffer must remain valid for the duration of the call. Returns false on
  // encode error.
  virtual bool Encode(const uint8_t* frame, const FrameDesc& desc) = 0;

  virtual void SetNalSink(EncodedNalSink sink) = 0;
};

}  // namespace duke

#endif  // DUKE_ENCODE_ENCODER_H_
