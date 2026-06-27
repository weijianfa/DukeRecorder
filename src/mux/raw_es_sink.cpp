#include "mux/raw_es_sink.h"

namespace duke {

RawEsSink::~RawEsSink() { Close(); }

bool RawEsSink::Open(const std::string& path) {
  Close();
  stream_.open(path, std::ios::binary | std::ios::trunc);
  return stream_.is_open() && stream_.good();
}

bool RawEsSink::Write(const uint8_t* data, size_t len) {
  if (!stream_.is_open() || !stream_.good()) {
    return false;
  }
  if (len > 0 && data != nullptr) {
    stream_.write(reinterpret_cast<const char*>(data),
                  static_cast<std::streamsize>(len));
  }
  return stream_.good();
}

void RawEsSink::Close() {
  if (stream_.is_open()) {
    stream_.flush();
    stream_.close();
  }
}

}  // namespace duke
