#include "media/decoder_buffer.h"

#include <glog/logging.h>
#include "base/aligned_memory.h"

namespace media {

DecoderBuffer::DecoderBuffer(int buffer_size)
    : Buffer(base::TimeDelta(), base::TimeDelta()),
      buffer_size_(buffer_size) {
  Initialize();
}

DecoderBuffer::DecoderBuffer(const uint8_t* data, int buffer_size)
    : Buffer(base::TimeDelta(), base::TimeDelta()),
      buffer_size_(buffer_size) {
  // Prevent invalid allocations.  Also used to create end of stream buffers.
  if (!data) {
    buffer_size_ = 0;
    data_ = NULL;
    return;
  }

  Initialize();
  memcpy(data_, data, buffer_size_);
}

DecoderBuffer::~DecoderBuffer() {
  base::AlignedFree(data_);
}

void DecoderBuffer::Initialize() {
  DCHECK_GE(buffer_size_, 0);
  data_ = reinterpret_cast<uint8_t*>(
      base::AlignedAlloc(buffer_size_ + kPaddingSize, kAlignmentSize));
  memset(data_ + buffer_size_, 0, kPaddingSize);
}

scoped_ref_ptr<DecoderBuffer> DecoderBuffer::CopyFrom(const uint8_t* data,
                                                     int data_size) {
  DCHECK(data);
  return make_scoped_ref_ptr(new DecoderBuffer(data, data_size));
}

scoped_ref_ptr<DecoderBuffer> DecoderBuffer::CreateEOSBuffer() {
  return make_scoped_ref_ptr(new DecoderBuffer(NULL, 0));
}

const uint8_t* DecoderBuffer::GetData() const {
  return data_;
}

int DecoderBuffer::GetDataSize() const {
  return buffer_size_;
}

uint8_t* DecoderBuffer::GetWritableData() {
  return data_;
}

}  // namespace media
