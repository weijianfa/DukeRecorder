#ifndef DUKE_RECORDER_H_
#define DUKE_RECORDER_H_

#include <atomic>
#include <memory>
#include <thread>

#include "capture/capture_engine.h"
#include "core/config.h"
#include "core/ring_buffer.h"
#include "encode/annexb_writer.h"
#include "encode/color_convert.h"
#include "encode/encoder.h"
#include "mux/raw_es_sink.h"

namespace duke {

// Orchestrates the capture -> encode -> Annex-B-ES pipeline on two threads:
// the capture engine's own thread writes raw BGRA frames into a bounded
// FrameBuffer (dropping under backpressure rather than blocking), and an
// EncodeLoop drains the buffer, converts BGRA->NV12, encodes, and writes
// Annex-B NALs to the file sink. Memory stays bounded (fixed slot pool,
// reusable NV12 buffer) for the 8h+ recording case.
//
// The file sink is injected so the fMP4 muxer (P4) can replace it later
// without changing this orchestration.
class Recorder {
 public:
  Recorder(std::unique_ptr<CaptureEngine> capture,
           std::unique_ptr<Encoder> encoder,
           std::unique_ptr<RawEsSink> sink);
  ~Recorder();

  Recorder(const Recorder&) = delete;
  Recorder& operator=(const Recorder&) = delete;

  // Initialize capture + encoder (using actual capture dims), wire the NAL
  // sink through AnnexBWriter to the file sink, open the output file.
  bool Initialize(const RecorderConfig& config);

  // Start capture + encode thread.
  bool Start();

  // Stop capture, drain + join the encode thread, close the file. Safe to call
  // once; also called by the destructor.
  void Stop();

  uint64_t dropped_frames() const { return dropped_.load(); }

 private:
  void EncodeLoop();
  void OnCapturedFrame(const uint8_t* data, const FrameDesc& desc);

  std::unique_ptr<CaptureEngine> capture_;
  std::unique_ptr<Encoder> encoder_;
  std::unique_ptr<RawEsSink> sink_;
  std::unique_ptr<FrameBuffer> frame_buffer_;
  std::unique_ptr<AnnexBWriter> writer_;
  BgraToNv12Converter converter_;

  RecorderConfig config_{};
  std::atomic<bool> running_{false};
  std::thread encode_thread_;
  std::atomic<uint64_t> dropped_{0};
  bool converter_ready_ = false;
};

}  // namespace duke

#endif  // DUKE_RECORDER_H_
