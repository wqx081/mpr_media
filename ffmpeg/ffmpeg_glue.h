#ifndef MEDIA_FFMPEG_GLUE_H_
#define MEDIA_FFMPEG_GLUE_H_
#include <stdint.h>

#include <memory>

#include "base/macros.h"
#include "base/ref_counted.h"
#include "ffmpeg/ffmpeg_common.h"


struct AVFormatContext;
struct AVIOContext;

namespace media {

class FFmpegURLProtocol {
 public:
  virtual int Read(int size, uint8_t* data) = 0;
  virtual bool GetPosition(int64_t* position) = 0;
  virtual bool SetPosition(int64_t position) = 0;
  virtual bool GetSize(int64_t* size) = 0;
  virtual bool IsStreaming() = 0;
};

class FFmpegGlue {
 public:
  static void InitializeFFmpeg();
  explicit FFmpegGlue(FFmpegURLProtocol* protocol);
  ~FFmpegGlue();

  bool OpenContext();
  AVFormatContext* format_context() { return format_context_; }

 private:
  bool open_called_;
  AVFormatContext* format_context_;
  std::unique_ptr<AVIOContext, ScopedPtrAVFree> avio_context_;

  DISALLOW_COPY_AND_ASSIGN(FFmpegGlue);
};

} // namespace media
#endif // MEDIA_FFMPEG_GLUE_H_
