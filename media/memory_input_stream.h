#ifndef MEDIA_FILTERS_MEMORY_DATA_SOURCE_H_
#define MEDIA_FILTERS_MEMORY_DATA_SOURCE_H_

#include <stdint.h>

#include "base/macros.h"
#include "media/input_stream.h"

namespace media {

// Basic data source that treats the URL as a file path, and uses the file
// system to read data for a media pipeline.
class MemoryInputStream : public InputStream {
 public:
  // Construct MemoryDataSource with |data| and |size|. The data is guaranteed
  // to be valid during the lifetime of MemoryDataSource.
  MemoryInputStream(const uint8_t* data, size_t size);
  ~MemoryInputStream() final;

  // Implementation of DataSource.
  void Read(int64_t position,
            int size,
            uint8_t* data,
            const InputStream::ReadCB& read_cb) final;
  void Stop() final;
  bool GetSize(int64_t* size_out) final;
  bool IsStreaming() final;
  void SetBitrate(int bitrate) final;

 private:
  const uint8_t* data_ = nullptr;
  size_t size_ = 0;

  bool is_stopped_ = false;

  DISALLOW_COPY_AND_ASSIGN(MemoryInputStream);
};

}  // namespace media

#endif  // MEDIA_FILTERS_MEMORY_DATA_SOURCE_H_
