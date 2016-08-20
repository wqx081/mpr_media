#ifndef CC_CONTEXT_H_
#define CC_CONTEXT_H_

#include "base/macros.h"

#include "cc/ffmpeg_common.h"

namespace media {

class Context {
 public:
  explicit Context();  
  ~Context();

  // For command_line.{h,cc}
  AVCodecContext* avcodec_opts[AVMEDIA_TYPE_NB];
  AVFormatContext* avformat_opts;
  AVDictionary* sws_dict;
  AVDictionary* swr_opts;
  AVDictionary* format_opts;
  AVDictionary* codec_opts;
  AVDictionary* resample_opts;
  int hide_banner;


 private:
  void DoDestroy();

  DISALLOW_COPY_AND_ASSIGN(Context);
};

} // namespace media
#endif // CC_CONTEXT_H_
