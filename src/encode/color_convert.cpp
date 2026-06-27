#include "encode/color_convert.h"

#include <libyuv/convert.h>

namespace duke {

bool BgraToNv12Converter::Initialize(uint32_t width, uint32_t height) {
  if (width == 0 || height == 0) return false;
  if ((width & 1) != 0 || (height & 1) != 0) return false;  // NV12 needs even
  width_ = width;
  height_ = height;
  // NV12: Y plane (w*h) + interleaved UV plane (w*h/2).
  nv12_.assign(static_cast<size_t>(width) * height * 3 / 2, 0);
  return true;
}

bool BgraToNv12Converter::Convert(const uint8_t* bgra, uint32_t bgra_stride,
                                  const uint8_t** out_nv12, size_t* out_len) {
  if (width_ == 0 || height_ == 0 || bgra == nullptr || bgra_stride == 0) {
    return false;
  }

  uint8_t* dst_y = nv12_.data();
  const size_t y_size = static_cast<size_t>(width_) * height_;
  uint8_t* dst_uv = nv12_.data() + y_size;

  // libyuv's "ARGB" is BGRA in memory byte order — exactly DXGI's layout.
  const int result = ARGBToNV12(
      bgra, static_cast<int>(bgra_stride), dst_y, static_cast<int>(width_),
      dst_uv, static_cast<int>(width_), static_cast<int>(width_),
      static_cast<int>(height_));

  if (result != 0) {
    return false;
  }

  if (out_nv12 != nullptr) *out_nv12 = nv12_.data();
  if (out_len != nullptr) *out_len = nv12_.size();
  return true;
}

}  // namespace duke
