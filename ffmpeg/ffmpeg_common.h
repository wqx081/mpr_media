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

#include "ffmpeg/ffmpeg_deleters.h"

namespace media {

inline void ScopedPtrAVFree::operator()(void* x) const {
  av_free(x);
} 

inline void ScopedPtrAVFreePacket::operator()(void* x) const {
  AVPacket* packet = static_cast<AVPacket*>(x);
  av_packet_unref(packet);
  delete packet;
} 

inline void ScopedPtrAVFreeContext::operator()(void* x) const {
  AVCodecContext* codec_context = static_cast<AVCodecContext*>(x);
  avcodec_free_context(&codec_context);
} 

inline void ScopedPtrAVFreeFrame::operator()(void* x) const {
  AVFrame* frame = static_cast<AVFrame*>(x);
  av_frame_free(&frame);
} 

} // namespace media
#endif //FFMPEG_FFMPEG_COMMON_H_
