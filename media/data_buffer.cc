#include "media/data_buffer.h"

#include <glog/logging.h>

namespace media {

DataBuffer::DataBuffer(std::unique_ptr<uint8_t[]> buffer, int buffer_size)
    : Buffer(base::TimeDelta(), base::TimeDelta()),
      data_(std::move(buffer)),
      buffer_size_(buffer_size),
      data_size_(buffer_size) {
}

DataBuffer::DataBuffer(int buffer_size)
    : Buffer(base::TimeDelta(), base::TimeDelta()),
      buffer_size_(buffer_size),
      data_size_(0) {
  Initialize();
}

DataBuffer::DataBuffer(const uint8_t* data, int data_size)
    : Buffer(base::TimeDelta(), base::TimeDelta()),
      buffer_size_(data_size),
      data_size_(data_size) {
  Initialize();
  memcpy(data_.get(), data, data_size_);
}

DataBuffer::~DataBuffer() {}

void DataBuffer::Initialize() {
  // Prevent arbitrary pointers.
  if (buffer_size_ <= 0) {
    buffer_size_ = data_size_ = 0;
    data_.reset();
    return;
  }

  data_.reset(new uint8_t[buffer_size_]);
}

const uint8_t* DataBuffer::GetData() const {
  return data_.get();
}

int DataBuffer::GetDataSize() const {
  return data_size_;
}

uint8_t* DataBuffer::GetWritableData() {
  return data_.get();
}

void DataBuffer::SetDataSize(int data_size) {
  DCHECK_LE(data_size, buffer_size_);
  data_size_ = data_size;
}

int DataBuffer::GetBufferSize() const {
  return buffer_size_;
}

}  // namespace media
