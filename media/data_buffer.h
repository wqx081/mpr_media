#ifndef MEDIA_DATA_BUFFER_H_
#define MEDIA_DATA_BUFFER_H_
#include <stdint.h>
#include <memory>

#include "base/macros.h"
#include "base/ref_counted.h"
#include "base/time.h"

namespace media {

class DataBuffer : public base::RefCountedThreadSafe<DataBuffer> {
 public:
  explicit DataBuffer(int buffer_size);
  DataBuffer(std::unique_ptr<uint8_t[]> buffer, int buffer_size);

  static scoped_ref_ptr<DataBuffer> CopyFrom(const uint8_t* data, int size);
  static scoped_ref_ptr<DataBuffer> CreateEOSBuffer();

  base::TimeDelta timestamp() const;
  void set_timestamp(const base::TimeDelta& timestamp);
  base::TimeDelta duration() const;
  void set_duration(const base::TimeDelta& duration);
  const uint8_t* data() const;
  uint8_t* writable_data();

  bool end_of_stream() const;

 private:
  friend class base::RefCountedThreadSafe<DataBuffer>;

  DataBuffer(const uint8_t* data, int data_size);
  virtual ~DataBuffer();

 private:
  base::TimeDelta timestamp_;
  base::TimeDelta duration_;

  std::unique_ptr<uint8_t[]> data_;
  int buffer_size_;
  int data_size_;

  DISALLOW_COPY_AND_ASSIGN(DataBuffer);
};

} // namespace media
#endif // MEDIA_DATA_BUFFER_H_
