# Spec: P2 — Capture + Software Encode

> Status: **DRAFT — awaiting human review** (Phase 1 of spec-driven development).
> Do not plan or implement until this spec is approved.

## Objective

Land the first runnable recording pipeline for DukeRecorder: capture desktop
frames (Windows DXGI Desktop Duplication) → software-encode to H.264 (x264) →
write a raw H.264 Annex-B elementary stream to disk. This proves the
capture→encode spine of the 4-thread design against real frames and produces a
decodable artifact, without depending on the fMP4 muxer (P4) or control
protocol (P3).

**User:** the engineer building DukeRecorder (and, transitively, the Electron
front-end that will later drive it). **Success:** a CLI invocation records N
seconds of the desktop and emits a `.h264` file that `ffprobe` reports as a
valid H.264 stream with the expected frame count and duration (±1 frame).

### In scope
- `CaptureEngine` abstract interface + DXGI Desktop Duplication backend (Windows).
- `Encoder` abstract interface + x264 software backend.
- BGRA→NV12 color conversion via libyuv (x264 input format).
- `CaptureLoop` and `EncodeLoop` threads wired through the existing `FrameBuffer`.
- An Annex-B elementary-stream file sink standing in for the (P4) muxer.
- A `TestCaptureEngine` synthetic frame source for pipeline testing without a
  live desktop session.
- Basic cursor composition (position + bitmap overlay) — alpha blending
  simplified, per the design doc's flagged gap.

### Out of scope (later phases)
- macOS ScreenCaptureKit backend (interface pluggable; stubbed for P2).
- Hardware encoders (MediaFoundation / VideoToolbox) — interface pluggable.
- fMP4 muxing (P4) — P2 writes raw ES, not a container.
- Upload loop (P5).
- Control protocol / pause-resume signaling (P3). The EncodeLoop API is shaped
  to accept pause/resume later, but no control channel triggers it in P2.
- mp4 parsing/repair, IPC shared-memory zero-copy.

## Tech Stack
- C++17, namespace `duke`, Google style (existing convention).
- x264 (software H.264 encoder) — vcpkg `encoding` feature.
- libyuv (BGRA→NV12 conversion) — vcpkg `encoding` feature.
- spdlog (logging) — already linked via `duke_core`.
- Windows: DXGI 1.2+ / Direct3D 11 (`d3d11`, `dxgi`, `dxguid` — already in
  `DUKE_PLATFORM_LIBS`).

## Commands
P2 has no locally-buildable command in this WSL env (no cmake/vcpkg/x264/DXGI).
All build/test commands run on the Windows CI runner (or a Windows dev box):

```
Configure:  cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake
Build:      cmake --build build --config Release
Tests:      ctest --test-dir build --output-on-failure
Run:        ./build/Release/duke_recorder --record --duration 5 --out out.h264
Style:      cmake --build build --target duke_format_check duke_tidy
Probe:      ffprobe -v error -count_frames -select_streams v:0 \
              -show_entries stream=nb_read_frames -of default=out.h264
```

Local (Linux/g++ only, no deps): compiles the portable pure-logic units
(Annex-B NAL writer + any header-only helpers) directly:
```
g++ -std=c++17 -Wall -Wextra -Werror -Isrc tests/test_<unit>.cpp -o /tmp/test_<unit>
```

## Project Structure
New files added in P2 (existing P1 files shown for context):

```
src/
  core/                 (P1) platform.h config.h frame.h ring_buffer.h
  capture/
    capture_engine.h        CaptureEngine abstract interface + RawFrameCallback
    dxgi_capture.h          DXGI Desktop Duplication backend (Windows)
    dxgi_capture.cpp        .cpp implementation
    test_capture.h          Synthetic frame source (color bars / moving pattern)
    test_capture.cpp        — used by tests + diagnostic builds
  encode/
    encoder.h               Encoder abstract interface + EncodedFrame sink
    x264_encoder.h          x264 software backend
    x264_encoder.cpp
    annexb_writer.h         Annex-B NAL stream writer (PORTABLE — unit tested)
    annexb_writer.cpp
  mux/
    raw_es_sink.h           File sink: append Annex-B NALs to output_path
    raw_es_sink.cpp         (stands in for the P4 fmp4_muxer)
  main.cpp              (P1, extended) --record/--duration/--out wiring
tests/
  test_ring_buffer.cpp      (P1)
  test_annexb_writer.cpp    NEW — NAL framing / Annex-B conversion
  test_pipeline.cpp         NEW — TestCaptureEngine → x264 → file (CI only)
docs/specs/
  P2-capture-encode.md      THIS FILE
```

## Code Style
Match existing P1 conventions (`duke` namespace, `snake_case` files, PascalCase
types, include guards `#ifndef DUKE_CORE_X_H_`, spdlog for logging,
`std::unique_ptr` ownership via `Create*` factories). Example of the interface
shape P2 follows (mirrors the doc's `CaptureEngine`):

```cpp
// src/capture/capture_engine.h
#ifndef DUKE_CAPTURE_CAPTURE_ENGINE_H_
#define DUKE_CAPTURE_CAPTURE_ENGINE_H_

#include <cstdint>

#include "core/frame.h"

namespace duke {

using RawFrameCallback = std::function<void(const uint8_t* data, const FrameDesc& desc)>;

class CaptureEngine {
 public:
  virtual ~CaptureEngine() = default;
  virtual bool Initialize(const RecorderConfig& config) = 0;
  virtual bool Start() = 0;
  virtual bool Stop() = 0;
  virtual void SetCallback(RawFrameCallback callback) = 0;
  virtual void GetEncoderParams(uint32_t& width, uint32_t& height, uint32_t& fps) = 0;
};

}  // namespace duke

#endif  // DUKE_CAPTURE_CAPTURE_ENGINE_H_
```

Platform code is guarded with `#ifdef DUKE_PLATFORM_WINDOWS`; the macOS
ScreenCaptureKit path is declared but `NOT_IMPLEMENTED`-stubbed for P2.

## Testing Strategy
- **Framework:** no gtest yet (not in vcpkg.json). Continue the P1 pattern:
  standalone `assert`-based CTest binaries with a tiny `CHECK` macro. Add
  gtest only if test volume demands it (open question).
- **Levels:**
  - *Unit (portable, run locally AND in CI):* `test_annexb_writer` — NAL
    length-prefix parsing, start-code emission, AUD insertion. Pure C++, no
    platform deps.
  - *Integration (CI only — Windows):* `test_pipeline` — `TestCaptureEngine`
    produces N synthetic frames → x264 encodes → `RawEsSink` writes file →
    `ffprobe` validates frame count. Exercises the real encoder + sink without
    a DXGI desktop session.
  - *End-to-end (CI only — Windows, manual/gated):* `duke_recorder --record`
    against the live desktop; validate with `ffprobe`. Gated because GH Windows
    runners' desktop-session availability for DXGI is uncertain (see Risks).
- **Coverage expectation:** every new module has at least one test at the
  lowest feasible level. DXGI backend (not unit-testable without a session) is
  covered by the end-to-end run + a compile/link smoke check.

## Boundaries
- **Always do:** run `ctest` and `duke_format_check` before declaring a task
  done; guard platform code with `DUKE_PLATFORM_*` macros; use spdlog for all
  logging; bound all buffers (no unbounded `std::vector` growth in the hot
  path — reuse `FrameBuffer` slots).
- **Ask first:** adding a new vcpkg dependency (e.g. gtest, ffmpeg for
  probing); changing `DUKE_PLATFORM_LIBS`; modifying the `FrameBuffer`/`RecorderConfig`
  contracts from P1; altering CI workflow.
- **Never do:** commit build artifacts or `out.h264` test outputs; bypass the
  `Encoder`/`CaptureEngine` interfaces with concrete-type calls in the
  pipeline; introduce an MP4/container writer (that is P4's boundary — P2
  writes raw ES only).

## Success Criteria
1. `cmake --build` succeeds on the Windows CI runner (Release).
2. `ctest` passes: `ring_buffer`, `annexb_writer`, `pipeline` (synthetic).
3. `duke_recorder --record --duration 5 --out out.h264` exits 0 and produces a
   non-empty `out.h264`.
4. `ffprobe` on that file reports codec `h264`, frame count within ±1 of
   `5 * fps`, and no decode errors.
5. Memory stays bounded over a 60s synthetic run: `FrameBuffer` slot count
   never grows (producer yields, never allocates) — asserted in `test_pipeline`.
6. `duke_format_check` and `duke_tidy` (scope widened to `src/capture`,
   `src/encode`, `src/mux`) are clean.
7. Encoder falls back gracefully: if x264 init fails, the run logs an error and
   exits nonzero (no crash) — demonstrates the "故障降级" philosophy's error
   path (hardware→software fallback itself is a later phase).

## Risks & Mitigations
- **GH Windows runner desktop session for DXGI.** Desktop Duplication requires
  an active session; CI may not provide one. *Mitigation:* the end-to-end
  desktop test is gated/manual; CI runs the synthetic `test_pipeline` against
  `TestCaptureEngine`, which needs no session. If DXGI works in CI, promote the
  desktop test to required.
- **x264/libyuv API drift across vcpkg versions.** *Mitigation:* pin vcpkg
  baseline (the existing `vcpkgGitCommitId: 2024-01-01` in CI — flagged
  separately as pre-existing and possibly stale).
- **Local verification gap.** Only g++ is available locally; most of P2 cannot
  be built here. *Mitigation:* maximize portable units (Annex-B writer) that
  ARE g++-verifiable; rely on CI for the rest; keep the spec's Verify steps
  explicit about local-vs-CI.
- **Cursor alpha blending.** Doc flags `ComposeCursor` as simplified. *Mitigation:*
  ship basic overlay in P2, document the simplification, defer correct
  per-pixel alpha to a follow-up.

## Open Questions
1. **Output format confirmation:** raw H.264 Annex-B ES (`.h264`) for P2,
   fMP4 deferred to P4 — as this spec assumes. Confirm.
2. **gtest adoption:** add gtest to vcpkg now, or keep the standalone `CHECK`
   pattern? P2 adds ~2-3 test files; standalone is still fine.
3. **CI vcpkg pin:** the existing `vcpkgGitCommitId: 2024-01-01` predates this
   work and may not resolve current x264/libyuv cleanly. Refresh it as part of
   P2, or leave untouched?
4. **Duration flag semantics:** `--duration 0` = record until Ctrl-C (signal
   handling), or = invalid? P2 needs to pick one for clean shutdown.
5. **GOP / keyframe cadence:** use `RecorderConfig::gop_seconds` to set x264
   keyframe interval (`i_keyint_max = gop_seconds * fps`)? Confirm mapping.
