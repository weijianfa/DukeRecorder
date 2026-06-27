# Plan: P2 — Capture + Software Encode

Source spec: `docs/specs/P2-capture-encode.md`
Verification tiers (honest given local env = g++ only, no cmake/vcpkg/x264/DXGI):
- **[LOCAL]** — full RED→GREEN→build loop runnable here with `g++ -std=c++17`.
- **[STUB]** — logic unit-tested locally against a stub of the dep (x264/libyuv);
  real-dep integration confirmed on CI only. I will NOT claim local GREEN for
  the real-dep path.
- **[CI]** — cannot build/test locally at all (DXGI, real x264, end-to-end).
  Written + reviewed, verified on the Windows runner.

Defaults adopted (from spec open questions, unless vetoed):
Annex-B ES output; refresh CI vcpkg pin; standalone CHECK tests (no gtest);
`--duration 0` = record until SIGINT; `i_keyint_max = gop_seconds * fps`.

## Tasks (dependency order)

- [x] **T1. Annex-B NAL writer [LOCAL]**
  - Acceptance: converts length-prefixed (AVCC) NALs → Annex-B (start codes),
    inserts AUD before each access unit, pure C++ no deps.
  - Verify: `g++ ... tests/test_annexb_writer.cpp` passes.
  - Files: src/encode/annexb_writer.h, src/encode/annexb_writer.cpp,
    tests/test_annexb_writer.cpp

- [x] **T2. Raw ES file sink [LOCAL]**
  - Acceptance: appends Annex-B NALs to output_path; opens/creates/truncates
    correctly; flushes on close; no unbounded buffering (bounded internal buffer).
  - Verify: `g++ ... tests/test_raw_es_sink.cpp` — write known NALs, read back
    bytes, assert equality.
  - Files: src/mux/raw_es_sink.h, src/mux/raw_es_sink.cpp,
    tests/test_raw_es_sink.cpp

- [x] **T3. TestCaptureEngine synthetic source [LOCAL]**
  - Acceptance: implements CaptureEngine; emits N frames at a set fps as BGRA
    with a moving test pattern; Stop() halts cleanly; honors duration.
  - Verify: `g++ ... tests/test_test_capture.cpp` — run 30 frames @ 30fps,
    assert count, desc fields, callback thread safety.
  - Files: src/capture/capture_engine.h (interface), src/capture/test_capture.h,
    src/capture/test_capture.cpp, tests/test_test_capture.cpp

- [x] **T4. Encoder interface + x264 wrapper [STUB]**
  - Acceptance: abstract Encoder; x264_encoder sets params from RecorderConfig
    (keyint = gop_seconds*fps, bitrate, fps), feeds NV12, emits AVCC NALs to a
    sink, handles init failure (logs + returns false, no crash).
  - Verify (local, stub x264.h): param-mapping + error-path unit tests pass.
    Verify (CI): real x264 build + encode of TestCaptureEngine frames.
  - Files: src/encode/encoder.h, src/encode/x264_encoder.h,
    src/encode/x264_encoder.cpp, tests/test_x264_encoder.cpp (+ tmp stub)

- [x] **T5. DXGI capture backend [CI]**
  - Acceptance: DxgiCaptureEngine implements CaptureEngine; AcquireNextFrame →
    map BGRA texture → callback; basic ComposeCursor overlay (alpha simplified);
    GetEncoderParams reports actual output w/h/fps; Stop logs frame count.
  - Verify: CI Windows build + manual end-to-end run. NOT locally buildable.
  - Files: src/capture/dxgi_capture.h, src/capture/dxgi_capture.cpp

- [x] **T6. BGRA→NV12 conversion [STUB]**
  - Acceptance: thin libyuv wrapper (ARGBToNV12); fallback path documented.
  - Verify (local, stub libyuv): wrapper dispatch test. Verify (CI): real libyuv.
  - Files: src/encode/color_convert.h, src/encode/color_convert.cpp,
    tests/test_color_convert.cpp (+ tmp stub)

- [x] **T7. Pipeline orchestration (CaptureLoop + EncodeLoop) [STUB]**
  - Acceptance: wires CaptureEngine → FrameBuffer → Encoder → RawEsSink on two
    threads; bounded (producer yields, never grows); clean Stop joins threads;
    SIGINT triggers clean shutdown.
  - Verify (local): TestCaptureEngine + stub Encoder end-to-end into a tmp file,
    assert frame count. Verify (CI): real x264.
  - Files: src/recorder.h, src/recorder.cpp, tests/test_recorder.cpp

- [ ] **T8. main.cpp CLI wiring [LOCAL+CI]**
  - Acceptance: parse --record/--duration/--out/--fps/--bitrate; --version/--help
    retained; --duration 0 = until SIGINT; constructs Recorder, runs, exits 0.
  - Verify (local): arg-parsing unit test (no deps). Verify (CI): real run.
  - Files: src/main.cpp, tests/test_main_args.cpp

- [ ] **T9. CMake + CI wiring [CI]**
  - Acceptance: add capture/encode/mux static libs to CMakeLists; link
    DUKE_PLATFORM_LIBS + x264 + libyuv; widen duke_tidy scope; register new test
    targets; refresh CI vcpkg pin; add ffprobe validation step.
  - Verify: CI configure + build + ctest + ffprobe.
  - Files: CMakeLists.txt, cmake/*.cmake, vcpkg.json, .github/workflows/ci.yml

## Dependency notes
- T1 before T2 (sink writes Annex-B). T3 defines CaptureEngine interface (T5/T7
  depend on it). T4 before T7 (pipeline needs Encoder). T6 before T7 (encode
  needs NV12). T8/T9 last (integration). T5 (CI-only) can be written anytime
  after T3's interface lands but verified only in CI.

## Out of scope for this run
- fMP4 muxer (P4), upload (P5), control protocol/pause-resume (P3), macOS
  ScreenCaptureKit, hardware encoders, mp4 parsing/repair.
