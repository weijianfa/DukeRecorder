#ifndef DUKE_CORE_RING_BUFFER_H_
#define DUKE_CORE_RING_BUFFER_H_

#include <atomic>
#include <cstddef>
#include <thread>
#include <vector>

#include "frame.h"
#include "platform.h"

namespace duke {

// Lock-free single-producer single-consumer ring buffer over a fixed byte
// array. Used for high-throughput capture -> encode transfer where the
// payload is a contiguous byte range.
//
// One slot is permanently unused to distinguish full from empty, so the
// usable capacity is Capacity - 1 bytes. SPSC only: exactly one writer
// thread and one reader thread.
template <size_t Capacity>
class RingBuffer {
 public:
  RingBuffer() : head_(0), tail_(0) {}

  // Producer: write len bytes. Returns false if insufficient space.
  bool Write(const uint8_t* data, size_t len) {
    const size_t tail = tail_.load(std::memory_order_relaxed);
    const size_t head = head_.load(std::memory_order_acquire);

    if (AvailableWrite(head, tail) < len) return false;

    for (size_t i = 0; i < len; ++i) {
      buffer_[(tail + i) % Capacity] = data[i];
    }
    tail_.store((tail + len) % Capacity, std::memory_order_release);
    return true;
  }

  // Consumer: read len bytes into out. Returns false if insufficient data.
  bool Read(uint8_t* out, size_t len) {
    const size_t head = head_.load(std::memory_order_relaxed);
    const size_t tail = tail_.load(std::memory_order_acquire);

    if (AvailableRead(head, tail) < len) return false;

    for (size_t i = 0; i < len; ++i) {
      out[i] = buffer_[(head + i) % Capacity];
    }
    head_.store((head + len) % Capacity, std::memory_order_release);
    return true;
  }

  size_t AvailableRead() const {
    const size_t head = head_.load(std::memory_order_relaxed);
    const size_t tail = tail_.load(std::memory_order_relaxed);
    return (tail + Capacity - head) % Capacity;
  }

 private:
  size_t AvailableWrite(size_t head, size_t tail) const {
    return (head + Capacity - tail - 1) % Capacity;
  }
  size_t AvailableRead(size_t head, size_t tail) const {
    return (tail + Capacity - head) % Capacity;
  }

  alignas(64) std::atomic<size_t> head_;
  alignas(64) std::atomic<size_t> tail_;
  uint8_t buffer_[Capacity]{};
};

// Variable-length frame buffer built on a fixed pool of slots. Each slot is a
// self-contained (desc + bytes) unit handed from producer to consumer.
//
// SPSC lifecycle: producer AcquireWriteSlot() -> fills data/desc ->
// CommitWrite(); consumer AcquireReadSlot() -> reads -> ReleaseReadSlot().
// The fixed slot count bounds memory regardless of producer rate (the core
// design constraint for 8h+ recording): a slow consumer causes the producer
// to spin/yield in AcquireWriteSlot, never to grow unbounded.
//
// Ordering: producer and consumer advance over the slot array via monotonic
// cursors (tail_ / head_), so frames are always handed off in FIFO order.
// (The design-doc sketch scanned slots from index 0 on every read, which
// reorders frames when the producer recycles an early slot before the
// consumer drains later ones — fixed here.)
class FrameBuffer {
 public:
  struct Slot {
    std::atomic<bool> ready{false};
    std::vector<uint8_t> data;
    FrameDesc desc;
  };

  explicit FrameBuffer(size_t slot_count = 8,
                       size_t max_frame_size = 10 * 1024 * 1024)
      : slots_(slot_count), max_frame_size_(max_frame_size) {
    for (auto& slot : slots_) {
      slot.data.reserve(max_frame_size);
    }
  }

  // Acquire the next writable slot (blocks until the slot at the producer
  // cursor is free). SPSC: called only by the single producer.
  Slot* AcquireWriteSlot() {
    const size_t tail = tail_.load(std::memory_order_relaxed);
    Slot& slot = slots_[tail % slots_.size()];
    while (slot.ready.load(std::memory_order_acquire)) {
      // Slot still holds an unread frame; yield rather than grow the pool.
      std::this_thread::yield();
    }
    return &slot;
  }

  void CommitWrite(Slot* /*slot*/) {
    // slot is the one at the current tail cursor; mark then advance.
    slots_[tail_.load(std::memory_order_relaxed) % slots_.size()]
        .ready.store(true, std::memory_order_release);
    tail_.fetch_add(1, std::memory_order_release);
  }

  // Non-blocking acquire of the next readable slot; returns nullptr if the
  // slot at the consumer cursor is not yet committed.
  Slot* AcquireReadSlot() {
    const size_t head = head_.load(std::memory_order_relaxed);
    Slot& slot = slots_[head % slots_.size()];
    if (!slot.ready.load(std::memory_order_acquire)) {
      return nullptr;
    }
    return &slot;
  }

  void ReleaseReadSlot(Slot* /*slot*/) {
    slots_[head_.load(std::memory_order_relaxed) % slots_.size()]
        .ready.store(false, std::memory_order_release);
    head_.fetch_add(1, std::memory_order_release);
  }

 private:
  std::vector<Slot> slots_;
  size_t max_frame_size_;
  alignas(64) std::atomic<size_t> head_{0};  // consumer cursor
  alignas(64) std::atomic<size_t> tail_{0};  // producer cursor
};

}  // namespace duke

#endif  // DUKE_CORE_RING_BUFFER_H_
