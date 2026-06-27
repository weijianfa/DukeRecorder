#ifndef DUKE_ENCODE_COLOR_CONVERT_H_
#define DUKE_ENCODE_COLOR_CONVERT_H_

#include <cstddef>
#include <cstdint>
#include <vector>

namespace duke {

// Converts BGRA frames (DXGI's in-memory layout, which libyuv calls ARGB) to
// NV12 (x264's input colorspace) via libyuv. Owns a reusable NV12 buffer so
// memory stays bounded across frames — no per-frame allocation in steady state.
class BgraToNv12Converter {
 public:
  // width and height must be even (NV12 4:2:0 chroma subsampling). Returns
  // false otherwise or on bad dimensions.
  bool Initialize(uint32_t width, uint32_t height);

  // Convert one BGRA frame to NV12 in the internal buffer. out_nv12 points
  // into the buffer and is valid until the next Convert call. Returns false
  // on error (not initialized, null input, bad stride).
  bool Convert(const uint8_t* bgra, uint32_t bgra_stride,
               const uint8_t** out_nv12, size_t* out_len);

  size_t nv12_size() const { return nv12_.size(); }

 private:
  uint32_t width_ = 0;
  uint32_t height_ = 0;
  std::vector<uint8_t> nv12_;  // Y plane (w*h) + interleaved UV plane (w*h/2)
};

}  // namespace duke

#endif  // DUKE_ENCODE_COLOR_CONVERT_H_
