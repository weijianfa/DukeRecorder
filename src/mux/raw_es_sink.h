#ifndef DUKE_MUX_RAW_ES_SINK_H_
#define DUKE_MUX_RAW_ES_SINK_H_

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>

namespace duke {

// Append-only sink for raw H.264 Annex-B elementary stream bytes. Stands in
// for the fMP4 muxer (P4) in P2: it just writes NAL bytes to a file as they
// arrive. Write-through (no unbounded internal buffer — bytes go to the
// stream's own buffer and are flushed on Close), so memory stays bounded for
// the 8h+ recording case.
class RawEsSink {
 public:
  RawEsSink() = default;
  ~RawEsSink();

  RawEsSink(const RawEsSink&) = delete;
  RawEsSink& operator=(const RawEsSink&) = delete;

  // Open path for writing, truncating any existing file. Returns false on
  // failure (bad path, permissions, etc.).
  bool Open(const std::string& path);

  // Append len bytes. Returns false if the sink is not open or a write error
  // occurs.
  bool Write(const uint8_t* data, size_t len);

  // Flush and close. Idempotent.
  void Close();

 private:
  std::ofstream stream_;
};

}  // namespace duke

#endif  // DUKE_MUX_RAW_ES_SINK_H_
