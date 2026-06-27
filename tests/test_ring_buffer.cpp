// Standalone test for src/core/ring_buffer.h.
//
// No external test framework (gtest is not yet a vcpkg dependency); a tiny
// CHECK macro drives assertions and main() returns nonzero on any failure so
// CTest reports the regression. Add gtest later if test volume warrants it.

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

#include "core/ring_buffer.h"

namespace {

int g_failures = 0;

#define CHECK(cond)                                                       \
  do {                                                                    \
    if (!(cond)) {                                                        \
      std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
      ++g_failures;                                                       \
    }                                                                     \
  } while (0)

// RingBuffer: basic write/read round-trip and content integrity.
void TestRingBufferBasic() {
  duke::RingBuffer<16> rb;
  const std::string payload = "hello";
  CHECK(rb.Write(reinterpret_cast<const uint8_t*>(payload.data()),
                 payload.size()));
  CHECK(rb.AvailableRead() == payload.size());

  uint8_t out[8] = {};
  CHECK(rb.Read(out, payload.size()));
  CHECK(std::memcmp(out, payload.data(), payload.size()) == 0);
  CHECK(rb.AvailableRead() == 0);
}

// RingBuffer: capacity is Capacity - 1 (one slot distinguishes full/empty).
void TestRingBufferFull() {
  duke::RingBuffer<8> rb;  // usable: 7 bytes
  uint8_t buf[7] = {1, 2, 3, 4, 5, 6, 7};
  CHECK(rb.Write(buf, 7));
  CHECK(!rb.Write(buf, 1));  // full
  CHECK(rb.AvailableRead() == 7);
}

// RingBuffer: wraparound across the array boundary.
void TestRingBufferWraparound() {
  duke::RingBuffer<8> rb;
  uint8_t seed[5] = {0, 1, 2, 3, 4};
  CHECK(rb.Write(seed, 5));
  uint8_t drain[5] = {};
  CHECK(rb.Read(drain, 5));
  // Tail now at 5; writing 5 more wraps past the end.
  uint8_t next[5] = {5, 6, 7, 8, 9};
  CHECK(rb.Write(next, 5));
  uint8_t out[5] = {};
  CHECK(rb.Read(out, 5));
  CHECK(std::memcmp(out, next, 5) == 0);
}

// FrameBuffer: slot acquire/commit/read/release lifecycle.
void TestFrameBufferLifecycle() {
  duke::FrameBuffer fb(2, 1024);
  auto* slot = fb.AcquireWriteSlot();
  CHECK(slot != nullptr);
  CHECK(fb.AcquireReadSlot() == nullptr);  // not committed yet

  const std::string bytes = "frame0";
  slot->data.assign(reinterpret_cast<const uint8_t*>(bytes.data()),
                    reinterpret_cast<const uint8_t*>(bytes.data()) +
                        bytes.size());
  slot->desc.width = 1920;
  slot->desc.height = 1080;
  slot->desc.is_keyframe = true;
  fb.CommitWrite(slot);

  auto* read = fb.AcquireReadSlot();
  CHECK(read != nullptr);
  CHECK(read->desc.width == 1920);
  CHECK(read->desc.height == 1080);
  CHECK(read->desc.is_keyframe);
  CHECK(read->data.size() == bytes.size());
  CHECK(std::memcmp(read->data.data(), bytes.data(), bytes.size()) == 0);

  fb.ReleaseReadSlot(read);
  CHECK(fb.AcquireReadSlot() == nullptr);  // released
}

// FrameBuffer: SPSC transfer under load (producer/consumer threads).
void TestFrameBufferSpSc() {
  constexpr int kFrames = 1000;
  duke::FrameBuffer fb(8, 64 * 1024);

  std::thread producer([&] {
    for (int i = 0; i < kFrames; ++i) {
      auto* s = fb.AcquireWriteSlot();
      s->data.resize(sizeof(int));
      std::memcpy(s->data.data(), &i, sizeof(int));
      s->desc.timestamp_us = i;
      fb.CommitWrite(s);
    }
  });

  std::thread consumer([&] {
    int received = 0;
    while (received < kFrames) {
      if (auto* s = fb.AcquireReadSlot()) {
        int value = -1;
        std::memcpy(&value, s->data.data(), sizeof(int));
        CHECK(value == received);          // FIFO order preserved
        CHECK(s->desc.timestamp_us == received);
        fb.ReleaseReadSlot(s);
        ++received;
      } else {
        std::this_thread::yield();
      }
    }
  });

  producer.join();
  consumer.join();
}

}  // namespace

int main() {
  TestRingBufferBasic();
  TestRingBufferFull();
  TestRingBufferWraparound();
  TestFrameBufferLifecycle();
  TestFrameBufferSpSc();

  if (g_failures > 0) {
    std::fprintf(stderr, "%d check(s) failed\n", g_failures);
    return EXIT_FAILURE;
  }
  std::printf("ring_buffer tests passed\n");
  return EXIT_SUCCESS;
}
