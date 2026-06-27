#include "capture/dxgi_capture.h"

#ifdef DUKE_PLATFORM_WINDOWS

#include <chrono>

#include <d3d11.h>
#include <dxgi1_2.h>
#include <spdlog/spdlog.h>

namespace duke {

DxgiCaptureEngine::DxgiCaptureEngine() = default;

DxgiCaptureEngine::~DxgiCaptureEngine() { Stop(); }

bool DxgiCaptureEngine::Initialize(const RecorderConfig& config) {
  config_ = config;

  if (!InitD3D11()) {
    spdlog::error("Failed to initialize D3D11");
    return false;
  }

  // Staging texture for CPU readback.
  D3D11_TEXTURE2D_DESC desc = texture_desc_;
  desc.Usage = D3D11_USAGE_STAGING;
  desc.BindFlags = 0;
  desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
  desc.MiscFlags = 0;

  HRESULT hr = d3d_device_->CreateTexture2D(&desc, nullptr, &staging_texture_);
  if (FAILED(hr)) {
    spdlog::error("Failed to create staging texture: 0x{:X}", hr);
    return false;
  }

  spdlog::info("DXGI capture initialized: {}x{}@{}", output_width_,
               output_height_, config_.fps);
  return true;
}

bool DxgiCaptureEngine::InitD3D11() {
  HRESULT hr = S_OK;

  D3D_FEATURE_LEVEL feature_level;
  hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                         D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0,
                         D3D11_SDK_VERSION, &d3d_device_, &feature_level,
                         &d3d_context_);
  if (FAILED(hr)) {
    spdlog::warn("Hardware D3D11 failed, trying WARP...");
    hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr,
                           D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0,
                           D3D11_SDK_VERSION, &d3d_device_, &feature_level,
                           &d3d_context_);
    if (FAILED(hr)) return false;
  }

  Microsoft::WRL::ComPtr<IDXGIDevice> dxgi_device;
  hr = d3d_device_->QueryInterface(IID_PPV_ARGS(&dxgi_device));
  if (FAILED(hr)) return false;

  Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
  hr = dxgi_device_->GetAdapter(&adapter);
  if (FAILED(hr)) return false;

  Microsoft::WRL::ComPtr<IDXGIOutput> output;
  hr = adapter->EnumOutputs(0, &output);  // primary output
  if (FAILED(hr)) return false;

  Microsoft::WRL::ComPtr<IDXGIOutput1> output1;
  hr = output->QueryInterface(IID_PPV_ARGS(&output1));
  if (FAILED(hr)) return false;

  hr = output1->DuplicateOutput(d3d_device_.Get(), &duplication_);
  if (FAILED(hr)) {
    if (hr == DXGI_ERROR_NOT_CURRENTLY_AVAILABLE) {
      spdlog::error(
          "Desktop Duplication already in use by another application");
    }
    return false;
  }

  DXGI_OUTPUT_DESC output_desc;
  output->GetDesc(&output_desc);
  output_width_ = output_desc.DesktopCoordinates.right -
                  output_desc.DesktopCoordinates.left;
  output_height_ = output_desc.DesktopCoordinates.bottom -
                   output_desc.DesktopCoordinates.top;

  texture_desc_.Width = output_width_;
  texture_desc_.Height = output_height_;
  texture_desc_.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  texture_desc_.ArraySize = 1;
  texture_desc_.MipLevels = 1;
  texture_desc_.SampleDesc.Count = 1;
  texture_desc_.Usage = D3D11_USAGE_DEFAULT;
  texture_desc_.BindFlags =
      D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

  return true;
}

bool DxgiCaptureEngine::Start() {
  if (running_.exchange(true)) return false;

  start_time_us_ = now_us();
  capture_thread_ = std::thread(&DxgiCaptureEngine::CaptureThread, this);
  SetThreadPriority(capture_thread_.native_handle(),
                    THREAD_PRIORITY_TIME_CRITICAL);

  spdlog::info("DXGI capture started");
  return true;
}

bool DxgiCaptureEngine::Stop() {
  if (!running_.exchange(false)) return false;

  if (capture_thread_.joinable()) {
    capture_thread_.join();
  }

  duplication_.Reset();
  staging_texture_.Reset();
  d3d_context_.Reset();
  d3d_device_.Reset();

  spdlog::info("DXGI capture stopped, total frames: {}", frame_count_.load());
  return true;
}

void DxgiCaptureEngine::SetCallback(RawFrameCallback callback) {
  callback_ = std::move(callback);
}

void DxgiCaptureEngine::GetEncoderParams(uint32_t& width, uint32_t& height,
                                         uint32_t& fps) {
  width = output_width_;
  height = output_height_;
  fps = config_.fps;
}

void DxgiCaptureEngine::CaptureThread() {
  const auto frame_interval =
      std::chrono::microseconds(1000000 / config_.fps);
  auto next_frame_time = std::chrono::steady_clock::now();

  while (running_.load()) {
    auto now = std::chrono::steady_clock::now();
    if (now < next_frame_time) {
      std::this_thread::sleep_until(next_frame_time);
    }
    next_frame_time += frame_interval;

    if (!ProcessFrame()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }
}

bool DxgiCaptureEngine::ProcessFrame() {
  if (!duplication_) return false;

  Microsoft::WRL::ComPtr<IDXGIResource> resource;
  DXGI_OUTDUPL_FRAME_INFO frame_info;

  HRESULT hr = duplication_->AcquireNextFrame(100, &frame_info, &resource);
  if (hr == DXGI_ERROR_WAIT_TIMEOUT) {
    return false;  // no new frame (screen unchanged)
  }
  if (FAILED(hr)) {
    spdlog::error("AcquireNextFrame failed: 0x{:X}", hr);
    return false;
  }

  Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
  hr = resource->QueryInterface(IID_PPV_ARGS(&texture));
  if (FAILED(hr)) {
    duplication_->ReleaseFrame();
    return false;
  }

  d3d_context_->CopyResource(staging_texture_.Get(), texture.Get());

  D3D11_MAPPED_SUBRESOURCE mapped;
  hr = d3d_context_->Map(staging_texture_.Get(), 0, D3D11_MAP_READ, 0, &mapped);
  if (FAILED(hr)) {
    duplication_->ReleaseFrame();
    return false;
  }

  FrameDesc desc;
  desc.width = output_width_;
  desc.height = output_height_;
  desc.stride = mapped.RowPitch;
  desc.format = kFourCCBGRA;
  desc.timestamp_us = now_us();
  desc.duration_us = 1000000 / config_.fps;

  if (frame_info.LastMouseUpdateTime.QuadPart != 0) {
    ComposeCursor(static_cast<uint8_t*>(mapped.pData), mapped.RowPitch,
                  frame_info);
  }

  if (callback_) {
    callback_(static_cast<uint8_t*>(mapped.pData), desc);
  }

  d3d_context_->Unmap(staging_texture_.Get(), 0);
  duplication_->ReleaseFrame();

  frame_count_.fetch_add(1);
  return true;
}

void DxgiCaptureEngine::ComposeCursor(uint8_t* frame_data, uint32_t pitch,
                                      const DXGI_OUTDUPL_FRAME_INFO& frame_info) {
  if (frame_info.PointerShapeBufferSize > 0) {
    std::lock_guard<std::mutex> lock(cursor_mutex_);
    cursor_bitmap_.resize(frame_info.PointerShapeBufferSize);
    UINT required_size = 0;
    duplication_->GetFramePointerShape(frame_info.PointerShapeBufferSize,
                                       cursor_bitmap_.data(), &required_size,
                                       &cursor_info_);
  }

  cursor_position_ = {frame_info.PointerPosition.Position.x,
                      frame_info.PointerPosition.Position.y};
  cursor_visible_ = frame_info.PointerPosition.Visible != 0;

  if (!cursor_visible_ || cursor_bitmap_.empty()) return;

  // Simplified: source-over blend for color cursors only. Monochrome/masked
  // cursor types are not blended in P2 (flagged in the spec for follow-up).
  if (cursor_info_.Type != DXGI_OUTDUPL_POINTER_SHAPE_TYPE_COLOR) return;

  const int cw = static_cast<int>(cursor_info_.Width);
  const int ch = static_cast<int>(cursor_info_.Height);
  const int cx = cursor_position_.x;
  const int cy = cursor_position_.y;

  for (int y = 0; y < ch; ++y) {
    const int fy = cy + y;
    if (fy < 0 || fy >= static_cast<int>(output_height_)) continue;
    for (int x = 0; x < cw; ++x) {
      const int fx = cx + x;
      if (fx < 0 || fx >= static_cast<int>(output_width_)) continue;
      const size_t co = (static_cast<size_t>(y) * cw + x) * 4;
      if (co + 3 >= cursor_bitmap_.size()) break;
      const size_t fo = static_cast<size_t>(fy) * pitch +
                        static_cast<size_t>(fx) * 4;
      uint8_t* dst = frame_data + fo;
      const uint8_t* src = cursor_bitmap_.data() + co;
      const uint8_t a = src[3];
      if (a == 0) continue;
      if (a == 255) {
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
      } else {
        dst[0] = static_cast<uint8_t>((src[0] * a + dst[0] * (255 - a)) / 255);
        dst[1] = static_cast<uint8_t>((src[1] * a + dst[1] * (255 - a)) / 255);
        dst[2] = static_cast<uint8_t>((src[2] * a + dst[2] * (255 - a)) / 255);
      }
    }
  }
}

}  // namespace duke

#endif  // DUKE_PLATFORM_WINDOWS
