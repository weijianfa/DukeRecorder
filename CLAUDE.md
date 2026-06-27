# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Repository status

This is a **greenfield project**. The repository currently contains **no source code** — only the design document `docs/context.md` (written in Chinese), which specifies the full architecture for **DukeRecorder**, a planned enterprise-grade cross-platform desktop screen-recording engine. There is no build system, no tests, and no installed dependencies yet.

When asked to implement, treat `docs/context.md` as the authoritative spec. It contains reference header/source listings (C++) for every module plus a CMakeLists.txt skeleton; these are design sketches, not committed code — they contain intentional simplifications and `// 省略` (omitted) markers flagged for production completion. Do not assume they compile as written.

`docs/Google的C++编码规范 中文.PDF` is Google's C++ style guide (Chinese edition) — the coding-standard reference.

## Planned architecture (from docs/context.md)

- **Language/standard:** C++17, namespace `duke`. Cross-platform: Windows 10+ (DXGI Desktop Duplication + Windows Graphics Capture) and macOS 12+ (ScreenCaptureKit). Platform macros `DUKE_PLATFORM_WINDOWS` / `DUKE_PLATFORM_MACOS`.
- **Pipeline (4 threads, bounded resources):** `CaptureLoop → EncodeLoop → MuxLoop → UploadLoop`, connected by a slot-based `FrameBuffer` and mutex+condvar queues. Bounded by fixed ring buffers and a fixed-slot shared-memory region — never accumulate unbounded memory (the core design constraint for 8h+ recording).
- **Modules** (planned as static libs in CMake): `core` (platform, ring_buffer, thread_pool, timer), `capture`, `encode` (hardware-first: MediaFoundation/VideoToolbox, with x264 software fallback), `mux` (self-written fMP4 muxer), `ipc` (shared memory + UDS/Named Pipe for zero-copy Electron interop).
- **fMP4 muxer is the centerpiece:** streaming `moof`/`mdat` fragments (~4s each), moov front-loaded, each fragment starts on an IDR frame so it is independently decodable/playable (enables 边录边传 / record-while-streaming). Fragment boundary = keyframe + time threshold.
- **Pause/Resume:** keeps encoder context alive; on resume forces an IDR and applies timestamp compensation (`total_pause_duration_` subtracted from PTS/DTS) to keep timestamps monotonic/continuous.
- **Control protocol:** Protobuf (`proto/control.proto`) over UDS for strongly-typed signaling.
- **Design philosophy:** "资源有界，流式可控，故障降级" (bounded resources, streaming & controllable, graceful degradation) — hardware→software encoder fallback, SHM-failure disables IPC, upload-failure backs off to local cache.

## Build / test (not yet established)

Per the design doc, the intended toolchain is **CMake + vcpkg**, C++17, dependencies `spdlog` and `protobuf` (CONFIG), plus `x264` and platform SDKs (d3d11/dxgi/MediaFoundation on Windows; CoreVideo/CoreMedia/VideoToolbox on macOS). Once implemented, the conventional flow would be:

```bash
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake
cmake --build build
ctest --test-dir build
```

No build or test commands will work until the code is actually written.

## Notes for implementation work

- The design doc's C++ listings are illustrative and contain known gaps (e.g. `Fmp4Muxer::WriteMoov`/`WriteMoof` are stubbed, `ComposeCursor` alpha-blending is simplified, hardware encoders are unimplemented). These are called out in the "待完善项（生产级）" section at the end of the doc.
- Match the codebase's planned conventions once code exists: `duke` namespace, `snake_case` files, `PascalCase` classes, `spdlog` for logging, `std::unique_ptr` ownership via module factory functions (`CreateCaptureEngine`, `CreateHardwareEncoder`, etc.).
