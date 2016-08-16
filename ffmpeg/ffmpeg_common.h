#ifndef FFMPEG_FFMPEG_COMMON_H_
#define FFMPEG_FFMPEG_COMMON_H_
#include <cerrno>

extern "C" {

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/avutil.h>
#include <libavutil/mathematics.h>
#include <libavutil/log.h>
#include <libavutil/imgutils.h>

} // extern "C"

namespace media {

class ScopedPtrAVFree {
 public:
  inline void operator()(void* x) const {
    av_free(x);
  }
};

class ScopedPtrAVFreePacket {
 public:
  inline void operator()(void* x) const {
    AVPacket* packet = static_cast<AVPacket*>(x);
    av_packet_unref(packet);
    delete packet;
  }
};

} // namespace media
#endif //FFMPEG_FFMPEG_COMMON_H_
