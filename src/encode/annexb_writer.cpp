#include "encode/annexb_writer.h"

#include <cstring>

namespace duke {

void AppendNal(std::vector<uint8_t>& dst, const uint8_t* nal, size_t len) {
  dst.insert(dst.end(), kAnnexBStartCode, kAnnexBStartCode + kAnnexBStartCodeLen);
  if (len > 0 && nal != nullptr) {
    dst.insert(dst.end(), nal, nal + len);
  }
}

void AppendAud(std::vector<uint8_t>& dst) {
  AppendNal(dst, kAudNal, kAudNalLen);
}

std::vector<uint8_t> AvccToAnnexB(const uint8_t* avcc, size_t len) {
  std::vector<uint8_t> out;
  size_t i = 0;
  while (i + 4 <= len) {
    // 4-byte big-endian length prefix.
    const size_t nal_len = (static_cast<size_t>(avcc[i]) << 24) |
                           (static_cast<size_t>(avcc[i + 1]) << 16) |
                           (static_cast<size_t>(avcc[i + 2]) << 8) |
                           static_cast<size_t>(avcc[i + 3]);
    i += 4;
    const size_t available = len - i;
    const size_t take = (nal_len <= available) ? nal_len : available;
    AppendNal(out, avcc + i, take);
    i += take;
    // If the declared length was truncated, stop — do not interpret trailing
    // bytes as another length prefix.
    if (nal_len > available) {
      break;
    }
  }
  return out;
}

void AnnexBWriter::WriteAccessUnit(const uint8_t* avcc_nals, size_t len) {
  std::vector<uint8_t> au;
  au.reserve(len + 16);
  AppendAud(au);
  std::vector<uint8_t> nals = AvccToAnnexB(avcc_nals, len);
  au.insert(au.end(), nals.begin(), nals.end());
  if (!au.empty()) {
    sink_(au.data(), au.size());
  }
}

}  // namespace duke
