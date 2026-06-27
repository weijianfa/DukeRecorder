#ifndef DUKE_CORE_PLATFORM_H_
#define DUKE_CORE_PLATFORM_H_

#include <cstdint>
#include <chrono>

// Platform macros consumed via #ifdef by backend implementations.
#ifdef _WIN32
#define DUKE_PLATFORM_WINDOWS 1
#elif defined(__APPLE__)
#define DUKE_PLATFORM_MACOS 1
#endif

namespace duke {

// Microsecond-resolution monotonic timestamp. Used for capture timestamps;
// not to be confused with encoder PTS/DTS (see frame.h).
using TimestampUs = int64_t;

// Returns the current monotonic time in microseconds. Monotonic (never goes
// backwards) but pauses with neither wall clock nor the recording timeline.
inline TimestampUs now_us() {
  return std::chrono::duration_cast<std::chrono::microseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
}

}  // namespace duke

#endif  // DUKE_CORE_PLATFORM_H_
