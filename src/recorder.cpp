#include "recorder.h"

#include <utility>

#include <spdlog/spdlog.h>

namespace duke {

Recorder::Recorder(std::unique_ptr<CaptureEngine> capture,
                   std::unique_ptr<Encoder> encoder,
                   std::unique_ptr<RawEsSink> sink)
    : capture_(std::move(capture)),
      encoder_(std::move(encoder)),
      sink_(std::move(sink)) {}

Recorder::~Recorder() { Stop(); }

bool Recorder::Initialize(const RecorderConfig& config) {
  config_ = config;

  if (!capture_ || !encoder_ || !sink_) {
    spdlog::error("Recorder: null dependency");
    return false;
  }

  if (!capture_->Initialize(config)) {
    spdlog::error("Recorder: capture init failed");
    return false;
  }

  // Use the capture's actual reported geometry for the encoder (DXGI may
  // report desktop dims differing from the requested config).
  uint32_t cw = 0, ch = 0, cfps = 0;
  capture_->GetEncoderParams(cw, ch, cfps);
  RecorderConfig enc_cfg = config;
  if (cw != 0) enc_cfg.width = cw;
  if (ch != 0) enc_cfg.height = ch;
  if (cfps != 0) enc_cfg.fps = cfps;

  if (!encoder_->Initialize(enc_cfg)) {
    spdlog::error("Recorder: encoder init failed");
    return false;
  }

  const size_t max_frame = static_cast<size_t>(enc_cfg.width) * enc_cfg.height * 4;
  frame_buffer_ = std::make_unique<FrameBuffer>(8, max_frame);

  // Wire encoder NALs -> AnnexBWriter -> file sink.
  writer_ = std::make_unique<AnnexBWriter>(
      [this](const uint8_t* data, size_t len) {
        sink_->Write(data, len);
      });
  encoder_->SetNalSink(
      [this](const uint8_t* avcc, size_t len) {
        writer_->WriteAccessUnit(avcc, len);
      });

  capture_->SetCallback(
      [this](const uint8_t* data, const FrameDesc& desc) {
        OnCapturedFrame(data, desc);
      });

  if (!sink_->Open(config.output_path)) {
    spdlog::error("Recorder: failed to open output: {}", config.output_path);
    return false;
  }

  spdlog::info("Recorder initialized: {}x{}@{}fps -> {}", enc_cfg.width,
               enc_cfg.height, enc_cfg.fps, config.output_path);
  return true;
}

bool Recorder::Start() {
  if (running_.exchange(true)) return false;
  if (!frame_buffer_) return false;

  encode_thread_ = std::thread(&Recorder::EncodeLoop, this);
  if (!capture_->Start()) {
    running_.store(false);
    if (encode_thread_.joinable()) encode_thread_.join();
    return false;
  }
  return true;
}

void Recorder::Stop() {
  if (!running_.exchange(false)) {
    return;
  }
  // Capture thread never blocks in the callback (TryAcquireWriteSlot), so this
  // joins promptly; the encode thread then drains and joins.
  capture_->Stop();
  if (encode_thread_.joinable()) {
    encode_thread_.join();
  }
  sink_->Close();
  spdlog::info("Recorder stopped (dropped {} frames)", dropped_.load());
}

void Recorder::OnCapturedFrame(const uint8_t* data, const FrameDesc& desc) {
  if (!running_.load()) return;
  FrameBuffer::Slot* slot = frame_buffer_->TryAcquireWriteSlot();
  if (slot == nullptr) {
    dropped_.fetch_add(1);  // backpressure: drop rather than grow/block
    return;
  }
  const size_t bytes = static_cast<size_t>(desc.stride) * desc.height;
  slot->desc = desc;
  slot->data.assign(data, data + bytes);
  frame_buffer_->CommitWrite(slot);
}

void Recorder::EncodeLoop() {
  while (running_.load()) {
    FrameBuffer::Slot* slot = frame_buffer_->AcquireReadSlot();
    if (slot == nullptr) {
      std::this_thread::yield();
      continue;
    }
    if (!converter_ready_) {
      if (!converter_.Initialize(slot->desc.width, slot->desc.height)) {
        spdlog::error("NV12 converter init failed: {}x{}", slot->desc.width,
                      slot->desc.height);
        frame_buffer_->ReleaseReadSlot(slot);
        continue;
      }
      converter_ready_ = true;
    }
    const uint8_t* nv12 = nullptr;
    size_t len = 0;
    if (converter_.Convert(slot->data.data(), slot->desc.stride, &nv12, &len)) {
      encoder_->Encode(nv12, slot->desc);
    } else {
      spdlog::warn("BGRA->NV12 convert failed");
    }
    frame_buffer_->ReleaseReadSlot(slot);
  }

  // Drain any frames committed before the stop signal.
  while (auto* slot = frame_buffer_->AcquireReadSlot()) {
    const uint8_t* nv12 = nullptr;
    size_t len = 0;
    if (converter_ready_ &&
        converter_.Convert(slot->data.data(), slot->desc.stride, &nv12, &len)) {
      encoder_->Encode(nv12, slot->desc);
    }
    frame_buffer_->ReleaseReadSlot(slot);
  }
}

}  // namespace duke
