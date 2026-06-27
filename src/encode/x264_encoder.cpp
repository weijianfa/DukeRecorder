#include "encode/x264_encoder.h"

#include <cstring>
#include <new>

#include <x264.h>

#include <spdlog/spdlog.h>

#include "core/config.h"
#include "core/frame.h"

namespace duke {

namespace {

// Append one NAL to an AVCC buffer: 4-byte big-endian length + payload.
void AppendAvccNal(std::vector<uint8_t>& out, const uint8_t* payload,
                   int payload_len) {
  const size_t len = static_cast<size_t>(payload_len);
  out.push_back(static_cast<uint8_t>((len >> 24) & 0xFF));
  out.push_back(static_cast<uint8_t>((len >> 16) & 0xFF));
  out.push_back(static_cast<uint8_t>((len >> 8) & 0xFF));
  out.push_back(static_cast<uint8_t>(len & 0xFF));
  out.insert(out.end(), payload, payload + len);
}

}  // namespace

X264Encoder::X264Encoder() {
  param_ = new (std::nothrow) x264_param_t{};
  pic_in_ = new (std::nothrow) x264_picture_t{};
}

X264Encoder::~X264Encoder() {
  if (encoder_ != nullptr) {
    x264_encoder_close(encoder_);
  }
  delete param_;
  delete pic_in_;
}

bool X264Encoder::Initialize(const RecorderConfig& config) {
  config_ = config;
  if (param_ == nullptr || pic_in_ == nullptr) {
    spdlog::error("x264: allocation failed");
    return false;
  }

  // ultrafast + zerolatency: low-latency screen recording; tune zerolatency
  // disables frame reordering so each input frame yields one access unit.
  if (x264_param_default_preset(param_, "ultrafast", "zerolatency") < 0) {
    spdlog::error("x264: param_default_preset failed");
    return false;
  }

  param_->i_width = config.width;
  param_->i_height = config.height;
  param_->i_csp = X264_CSP_NV12;
  param_->i_fps_num = config.fps;
  param_->i_fps_den = 1;
  param_->i_keyint_max =
      static_cast<int>(config.gop_seconds) * static_cast<int>(config.fps);
  param_->rc.i_bitrate = static_cast<int>(config.bitrate_kbps);
  param_->b_repeat_headers = 1;  // SPS/PPS before each IDR (fragmented play)
  param_->b_annexb = 0;          // raw NALs; we AVCC-frame ourselves
  param_->i_log_level = X264_LOG_ERROR;

  encoder_ = x264_encoder_open(param_);
  if (encoder_ == nullptr) {
    spdlog::error("x264: encoder_open failed ({}x{} @ {}kbps)", config.width,
                  config.height, config.bitrate_kbps);
    return false;
  }

  x264_picture_init(pic_in_);
  pic_in_->img.i_csp = X264_CSP_NV12;
  pic_in_->img.i_plane = 2;  // NV12: Y + interleaved UV
  pts_ = 0;
  return true;
}

void X264Encoder::SetNalSink(EncodedNalSink sink) { sink_ = std::move(sink); }

bool X264Encoder::Encode(const uint8_t* nv12, const FrameDesc& desc) {
  if (encoder_ == nullptr || nv12 == nullptr) {
    return false;
  }
  const int width = static_cast<int>(desc.width);
  const int height = static_cast<int>(desc.height);

  pic_in_->img.i_stride[0] = width;  // Y stride
  pic_in_->img.i_stride[1] = width;  // UV stride (interleaved, same as width)
  pic_in_->img.plane[0] = const_cast<uint8_t*>(nv12);
  pic_in_->img.plane[1] = const_cast<uint8_t*>(nv12 + static_cast<size_t>(width) * height);
  pic_in_->i_pts = desc.timestamp_us;

  x264_nal_t* nals = nullptr;
  int i_nals = 0;
  x264_picture_t pic_out;
  const int frame_size =
      x264_encoder_encode(encoder_, &nals, &i_nals, pic_in_, &pic_out);
  if (frame_size < 0) {
    spdlog::error("x264: encode failed at pts={}", desc.timestamp_us);
    return false;
  }
  if (frame_size == 0 || i_nals <= 0 || nals == nullptr) {
    // No output yet (e.g. buffering); not an error for the caller.
    ++pts_;
    return true;
  }

  if (sink_) {
    std::vector<uint8_t> avcc;
    avcc.reserve(static_cast<size_t>(frame_size) + 4);
    for (int i = 0; i < i_nals; ++i) {
      AppendAvccNal(avcc, nals[i].p_payload, nals[i].i_payload);
    }
    if (!avcc.empty()) {
      sink_(avcc.data(), avcc.size());
    }
  }
  ++pts_;
  return true;
}

}  // namespace duke
