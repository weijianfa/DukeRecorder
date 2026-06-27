#ifndef DUKE_ENCODE_X264_ENCODER_H_
#define DUKE_ENCODE_X264_ENCODER_H_

#include <x264.h>

#include "core/config.h"
#include "core/frame.h"
#include "encode/encoder.h"

namespace duke {

// x264 software H.264 encoder. Emits AVCC-framed NALs (b_annexb=0) per access
// unit via the NAL sink. Input is NV12 (BGRA must be converted first — see
// color_convert). Falls back gracefully: an init failure returns false and
// logs, never crashes (the software path is itself the hardware-failure
// fallback per the design philosophy).
class X264Encoder : public Encoder {
 public:
  X264Encoder();
  ~X264Encoder() override;

  X264Encoder(const X264Encoder&) = delete;
  X264Encoder& operator=(const X264Encoder&) = delete;

  bool Initialize(const RecorderConfig& config) override;
  bool Encode(const uint8_t* nv12, const FrameDesc& desc) override;
  void SetNalSink(EncodedNalSink sink) override;

 private:
  RecorderConfig config_{};
  EncodedNalSink sink_;
  x264_t* encoder_ = nullptr;
  x264_param_t* param_ = nullptr;
  x264_picture_t* pic_in_ = nullptr;
  int64_t pts_ = 0;
};

}  // namespace duke

#endif  // DUKE_ENCODE_X264_ENCODER_H_
