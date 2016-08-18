#include "media/memory_input_stream.h"

#include <algorithm>

#include <glog/logging.h>

namespace media {

MemoryInputStream::MemoryInputStream(const uint8_t* data, size_t size)
    : data_(data), size_(size) {}

MemoryInputStream::~MemoryInputStream() {}

void MemoryInputStream::Read(int64_t position,
                            int size,
                            uint8_t* data,
                            const InputStream::ReadCB& read_cb) {
  DCHECK(!read_cb.is_null());

  if (is_stopped_ || size < 0 || position < 0 ||
      static_cast<size_t>(position) > size_) {
    read_cb.Run(kReadError);
    return;
  }

  // Cap size within bounds.
  size_t clamped_size = std::min(static_cast<size_t>(size),
                                 size_ - static_cast<size_t>(position));

  if (clamped_size > 0) {
    DCHECK(data);
    memcpy(data, data_ + position, clamped_size);
  }

  read_cb.Run(clamped_size);
}

void MemoryInputStream::Stop() {
  is_stopped_ = true;
}

bool MemoryInputStream::GetSize(int64_t* size_out) {
  *size_out = size_;
  return true;
}

bool MemoryInputStream::IsStreaming() {
  return false;
}

void MemoryInputStream::SetBitrate(int bitrate) { (void) bitrate; }

}  // namespace media
