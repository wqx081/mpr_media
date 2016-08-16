#include "media/ffmpeg.h"

#include <glog/logging.h>

namespace media {

void PrintFFmpegError(const char* filename, int err) {
  char buf[128];
  const char* ptr = buf;
  if (::av_strerror(err, buf, sizeof(buf)) < 0) {
  }
  LOG(ERROR) << filename << ": " << ptr;
}

} // namespace media
