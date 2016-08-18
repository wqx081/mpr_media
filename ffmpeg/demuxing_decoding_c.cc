#include "base/macros.h"

#include "base/file_path.h"
#include "base/file.h"
#include "base/file_path.h"

#include <string>

#include <gtest/gtest.h>

extern "C" {
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
}

#if 0
class InputContainer {
 public:

  ScopedAVFormatContext() 
    : ctx_(nullptr),
      video_stream_index_(-1),
      audio_stream_index_(-1),
      video_dec_ctx_(nullptr),
      audio_dec_ctx_(nullptr) {}    

  ~ScopedAVFormatContext() {
    if (ctx_) {
      ::avformat_close_input(&ctx_);
    }
  }

  bool Open(base::FilePath path) {

    DCHECK(!path.value().empty());
    // TODO
    // Add the Options
    if (::avformat_open_input(&ctx_, 
                              path.value().c_str(), 
                              nullptr, 
                              nullptr) < 0) {
      return false;  
    }
    if (::avformat_find_stream_info(ctx_, nullptr) < 0) {
      return false;
    }

    // VideoDecodeContext* CreateContext
    return true;
  }  

  bool IsValid() const { return ctx_ != nullptr; }

  
  AVFormatContext* get() {
    return ctx_;
  }

 private:
  AVFormatContext* ctx_;

  int video_stream_index_;
  int audio_stream_index_;
  AVCodecContext* video_dec_ctx_;
  AVCodecContext* audio_dec_ctx_;

  DISALLOW_COPY_AND_ASSIGN(ScopedAVFormatContext);

};

/////////////////////////////

class Demuxer {
 public:
  virtual ~Demuxer() {}
  virtual bool Demuxing() = 0;
  virtual bool Decoding() = 0; 
};

class FFmpegDemuxer : public Demuxer {
 public:
  explicit FFmpegDemuxer(const base::FilePath& path)
      : media_path_(path) , format_context_() {
  }

  bool Initialize() {
    return format_context_.Open(media_path_);
  }

  virtual bool Demuxing() override {
    return true;
  }

  virtual bool Decoding() override {
    return true;
  }

  base::FilePath file_path() const {
    return media_path_;
  }

 private:
  base::FilePath media_path_;
  ScopedAVFormatContext format_context_;
};

namespace {

class InitializeFFmpegLib {
 public:
  InitializeFFmpegLib() {
    ::av_register_all();
  }
};

static InitializeFFmpegLib init_;
} // namespace

TEST(Demuxer, Basic) {
  base::FilePath kMediaPath("test_data/test.mp4");
  ScopedAVFormatContext format_context;

  EXPECT_FALSE(format_context.IsValid());
  EXPECT_TRUE(format_context.Open(kMediaPath));
  EXPECT_TRUE(format_context.IsValid());

  EXPECT_TRUE(true);
}
#endif

/**
 *  class DecodedFrame;
 *  class 
 */
TEST(Dmuexer, Basic) {
  EXPECT_TRUE(true);
}
