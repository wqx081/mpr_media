#include "media/file_input_stream.h"

#include <algorithm>
#include <utility>

#include <glog/logging.h>

namespace media {

FileInputStream::FileInputStream()
    : force_read_errors_(false),
      force_streaming_(false),
      bytes_read_(0) {}

FileInputStream::FileInputStream(base::File file)
    : force_read_errors_(false),
      force_streaming_(false),
      bytes_read_(0) {
  file_.Initialize(std::move(file));
}

bool FileInputStream::Initialize(const base::FilePath& file_path) {
  DCHECK(!file_.IsValid());
  return file_.Initialize(file_path);
}

void FileInputStream::Stop() {}

void FileInputStream::Read(int64_t position,
                           int size,
                           uint8_t* data,
                           const InputStream::ReadCB& read_cb) {
  if (force_read_errors_ || !file_.IsValid()) {
    read_cb.Run(kReadError);
    return;
  }
  
  int64_t file_size = file_.length();
  
  CHECK_GE(file_size, 0);
  CHECK_GE(position, 0);
  CHECK_GE(size, 0);
  
  position = std::min(position, file_size);
  int64_t clamped_size =
    std::min(static_cast<int64_t>(size), file_size - position);
  
  memcpy(data, file_.data() + position, clamped_size);
  bytes_read_ += clamped_size;
  read_cb.Run(clamped_size);
}

bool FileInputStream::GetSize(int64_t* size) {
  *size = file_.length();
  return true;
}

bool FileInputStream::IsStreaming() {
  return force_streaming_;
}

void FileInputStream::SetBitrate(int bitrate) { (void) bitrate; }

FileInputStream::~FileInputStream() {}

} // namespace media
