#ifndef MEDIA_FILE_INPUT_STREAM_H_
#define MEDIA_FILE_INPUT_STREAM_H_
#include <stdint.h>
#include <string>

#include "base/file.h"
#include "base/file_path.h"
#include "base/memory_mapped_file.h"
#include "base/macros.h"
#include "media/input_stream.h"

namespace media {

class FileInputStream : public InputStream {
 public:
  FileInputStream();
  explicit FileInputStream(base::File file);

  ~FileInputStream() override;
  
  bool Initialize(const base::FilePath& file_path);

  // 
  void Stop() override;
  void Read(int64_t position,
            int size,
            uint8_t* data,
            const InputStream::ReadCB& read_cb) override;
  bool GetSize(int64_t* size) override;
  bool IsStreaming() override;
  void SetBitrate(int bitrate) override;

 private:
  base::MemoryMappedFile file_;

  bool force_read_errors_;
  bool force_streaming_;
  uint64_t bytes_read_;

  DISALLOW_COPY_AND_ASSIGN(FileInputStream);
};

} // namespace media
#endif // MEDIA_FILE_INPUT_STREAM_H_
