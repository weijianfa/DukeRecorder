#ifndef DUKE_CAPTURE_DXGI_CAPTURE_H_
#define DUKE_CAPTURE_DXGI_CAPTURE_H_

#include "capture/capture_engine.h"

#ifdef DUKE_PLATFORM_WINDOWS

#include <atomic>
#include <cstdint>
#include <mutex>
#include <thread>
#include <vector>

#include <d3d11.h>
#include <dxgi1_2.h>
#include <wrl/client.h>

#include "core/config.h"
#include "core/frame.h"
#include "core/platform.h"

namespace duke {

// Windows DXGI Desktop Duplication capture backend. Acquires desktop frames as
// BGRA textures, copies to a staging texture for CPU read, composes the cursor
// (simplified source-over blend, color cursors only), and delivers raw BGRA
// to the RawFrameCallback. Runs its own capture thread at the configured fps.
class DxgiCaptureEngine : public CaptureEngine {
 public:
  DxgiCaptureEngine();
  ~DxgiCaptureEngine() override;

  DxgiCaptureEngine(const DxgiCaptureEngine&) = delete;
  DxgiCaptureEngine& operator=(const DxgiCaptureEngine&) = delete;

  bool Initialize(const RecorderConfig& config) override;
  bool Start() override;
  bool Stop() override;
  void SetCallback(RawFrameCallback callback) override;
  void GetEncoderParams(uint32_t& width, uint32_t& height, uint32_t& fps) override;

 private:
  void CaptureThread();
  bool ProcessFrame();
  bool InitD3D11();

  // Cursor composition (resolves DXGI cursor desync by blending the cursor
  // into the BGRA frame on the CPU side).
  void ComposeCursor(uint8_t* frame_data, uint32_t pitch,
                     const DXGI_OUTDUPL_FRAME_INFO& frame_info);

  RecorderConfig config_{};
  RawFrameCallback callback_;
  std::atomic<bool> running_{false};
  std::thread capture_thread_;

  // D3D11 / DXGI
  Microsoft::WRL::ComPtr<ID3D11Device> d3d_device_;
  Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3d_context_;
  Microsoft::WRL::ComPtr<IDXGIOutputDuplication> duplication_;
  Microsoft::WRL::ComPtr<ID3D11Texture2D> staging_texture_;

  D3D11_TEXTURE2D_DESC texture_desc_{};
  uint32_t output_width_ = 0;
  uint32_t output_height_ = 0;

  // Cursor state
  std::mutex cursor_mutex_;
  std::vector<uint8_t> cursor_bitmap_;
  DXGI_OUTDUPL_POINTER_SHAPE_INFO cursor_info_{};
  POINT cursor_position_{0, 0};
  bool cursor_visible_ = false;

  // Stats
  std::atomic<uint64_t> frame_count_{0};
  TimestampUs start_time_us_ = 0;
};

}  // namespace duke

#endif  // DUKE_PLATFORM_WINDOWS

#endif  // DUKE_CAPTURE_DXGI_CAPTURE_H_
