// A simple implementation of Buffer that takes ownership of the given data
// pointer.
//
// DataBuffer assumes that memory was allocated with new uint8_t[].

#ifndef MEDIA_BASE_DATA_BUFFER_H_
#define MEDIA_BASE_DATA_BUFFER_H_

#include "base/ref_counted.h"
#include "media/buffers.h"

namespace media {

class DataBuffer : public Buffer {
 public:
  // Assumes valid data of size |buffer_size|.
  DataBuffer(std::unique_ptr<uint8_t[]> buffer, int buffer_size);

  // Allocates buffer of size |buffer_size|. If |buffer_size| is 0, |data_| is
  // set to a NULL ptr.
  explicit DataBuffer(int buffer_size);

  // Allocates buffer of size |data_size|, copies [data,data+data_size) to
  // the allocated buffer and sets data size to |data_size|.
  DataBuffer(const uint8_t* data, int data_size);

  // Buffer implementation.
  virtual const uint8_t* GetData() const override;
  virtual int GetDataSize() const override;

  // Returns a read-write pointer to the buffer data.
  virtual uint8_t* GetWritableData();

  // Updates the size of valid data in bytes, which must be less than or equal
  // to GetBufferSize().
  virtual void SetDataSize(int data_size);

  // Returns the size of the underlying buffer.
  virtual int GetBufferSize() const;

 protected:
  virtual ~DataBuffer();

 private:
  // Constructor helper method for memory allocations.
  void Initialize();

  std::unique_ptr<uint8_t[]> data_;
  int buffer_size_;
  int data_size_;

  DISALLOW_COPY_AND_ASSIGN(DataBuffer);
};

}  // namespace media

#endif  // MEDIA_BASE_DATA_BUFFER_H_
