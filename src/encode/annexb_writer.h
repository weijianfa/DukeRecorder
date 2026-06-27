#ifndef DUKE_ENCODE_ANNEXB_WRITER_H_
#define DUKE_ENCODE_ANNEXB_WRITER_H_

#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

namespace duke {

// Annex-B framing constants. H.264 NAL units are prefixed with a 4-byte start
// code (00 00 00 01) in the Annex-B byte stream format, as opposed to the
// length-prefixed (AVCC) layout x264 emits when b_annexb=0.
constexpr uint8_t kAnnexBStartCode[] = {0x00, 0x00, 0x00, 0x01};
constexpr size_t kAnnexBStartCodeLen = 4;

// Access Unit Delimiter (AUD) NAL: nal_unit_type=9, primary_pic_type=0 (I).
// Inserting an AUD before each access unit lets a downstream player resync on
// fragmented streams. primary_pic_type=0 is conservative (I-slice); the muxer
// calls per access unit so this is a safe default.
constexpr uint8_t kAudNal[] = {0x09, 0xF0};
constexpr size_t kAudNalLen = 2;

// Append a 4-byte start code + the NAL bytes to dst.
void AppendNal(std::vector<uint8_t>& dst, const uint8_t* nal, size_t len);

// Append an AUD (start code + AUD bytes) to dst.
void AppendAud(std::vector<uint8_t>& dst);

// Convert an AVCC (4-byte big-endian length-prefixed) NAL buffer to Annex-B.
// Processes NALs sequentially; stops cleanly if a declared length exceeds the
// remaining bytes (emits the available trailing bytes under a final start
// code rather than reading out of bounds). Empty input yields empty output.
std::vector<uint8_t> AvccToAnnexB(const uint8_t* avcc, size_t len);

// Streaming Annex-B writer. Forwards complete access units to a sink callback
// (typically the raw-ES file sink), inserting an AUD before each. Using a sink
// keeps memory bounded — bytes are written as produced, never accumulated for
// the whole recording (the 8h+ design constraint).
class AnnexBWriter {
 public:
  using Sink = std::function<void(const uint8_t* data, size_t len)>;

  explicit AnnexBWriter(Sink sink) : sink_(std::move(sink)) {}

  // Write one access unit: AUD + each length-prefixed NAL, converted to
  // Annex-B, forwarded to the sink in a single call.
  void WriteAccessUnit(const uint8_t* avcc_nals, size_t len);

 private:
  Sink sink_;
};

}  // namespace duke

#endif  // DUKE_ENCODE_ANNEXB_WRITER_H_
