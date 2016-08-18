#include "ffmpeg/ffmpeg_glue.h"
#include "base/lazy_instance.h"
#include "ffmpeg/ffmpeg_common.h"
#include "base/lock.h"
#include "media/container_names.h"

#include <glog/logging.h>

namespace media {

enum { kBufferSize = 32 * 1024 };

static int AVIOReadOperation(void* opaque, uint8_t* buf, int buf_size) {
  FFmpegURLProtocol* protocol = reinterpret_cast<FFmpegURLProtocol*>(opaque);
  int result = protocol->Read(buf_size, buf);
  if (result < 0)
    result = AVERROR(EIO);
  return result;
}

static int64_t AVIOSeekOperation(void* opaque, int64_t offset, int whence) {
  FFmpegURLProtocol* protocol = reinterpret_cast<FFmpegURLProtocol*>(opaque);
  int64_t new_offset = AVERROR(EIO);
  switch (whence) {
    case SEEK_SET:
      if (protocol->SetPosition(offset))
        protocol->GetPosition(&new_offset);
      break;

    case SEEK_CUR:
      int64_t pos;
      if (!protocol->GetPosition(&pos))
        break;
      if (protocol->SetPosition(pos + offset))
        protocol->GetPosition(&new_offset);
      break;

    case SEEK_END:
      int64_t size;
      if (!protocol->GetSize(&size))
        break;
      if (protocol->SetPosition(size + offset))
        protocol->GetPosition(&new_offset);
      break;

    case AVSEEK_SIZE:
      protocol->GetSize(&new_offset);
      break;

    default:
      DCHECK(false);
  }
  if (new_offset < 0)
    new_offset = AVERROR(EIO);
  return new_offset;
}

static int LockManagerOperation(void** lock, enum AVLockOp op) {
  switch (op) {
    case AV_LOCK_CREATE:
      *lock = new base::Lock();
      return 0;

    case AV_LOCK_OBTAIN:
      static_cast<base::Lock*>(*lock)->Acquire();
      return 0;

    case AV_LOCK_RELEASE:
      static_cast<base::Lock*>(*lock)->Release();
      return 0;

    case AV_LOCK_DESTROY:
      delete static_cast<base::Lock*>(*lock);
      *lock = NULL;
      return 0;
  }
  return 1;
}


class FFmpegInitializer {
 public:
  bool initialized() { return initialized_; }

 private:
  friend struct base::DefaultLazyInstanceTraits<FFmpegInitializer>;

  FFmpegInitializer()
      : initialized_(false) {
    av_log_set_level(AV_LOG_QUIET);

    if (av_lockmgr_register(&LockManagerOperation) != 0)
      return;

    av_register_all();

    initialized_ = true;
  }

  ~FFmpegInitializer() {
    DCHECK(false) << "FFmpegInitializer should be leaky!";
  }

  bool initialized_;

  DISALLOW_COPY_AND_ASSIGN(FFmpegInitializer);
};


void FFmpegGlue::InitializeFFmpeg() {
  static base::LazyInstance<FFmpegInitializer>::Leaky li =
      LAZY_INSTANCE_INITIALIZER;
  CHECK(li.Get().initialized());
}

FFmpegGlue::FFmpegGlue(FFmpegURLProtocol* protocol)
    : open_called_(false) {
  InitializeFFmpeg();

  format_context_ = avformat_alloc_context();
  avio_context_.reset(avio_alloc_context(
      static_cast<unsigned char*>(av_malloc(kBufferSize)), kBufferSize, 0,
      protocol, &AVIOReadOperation, NULL, &AVIOSeekOperation));

  avio_context_->seekable =
      protocol->IsStreaming() ? 0 : AVIO_SEEKABLE_NORMAL;

  avio_context_->write_flag = 0;

  format_context_->flags |= AVFMT_FLAG_CUSTOM_IO;
  format_context_->pb = avio_context_.get();
}

bool FFmpegGlue::OpenContext() {
  DCHECK(!open_called_) << "OpenContext() should't be called twice.";

  open_called_ = true;
  std::unique_ptr<std::vector<uint8_t>> buffer(new std::vector<uint8_t>(8192));
  
  int64_t pos = AVIOSeekOperation(avio_context_.get()->opaque, 0, SEEK_CUR);
  AVIOSeekOperation(avio_context_.get()->opaque, 0, SEEK_SET);
  int numRead = AVIOReadOperation(
        avio_context_.get()->opaque, buffer.get()->data(), buffer.get()->size());
  AVIOSeekOperation(avio_context_.get()->opaque, pos, SEEK_SET);
  if (numRead > 0) {
    //container_names::MediaContainerName container =
    //      container_names::DetermineContainer(buffer.get()->data(), numRead);
    //  UMA_HISTOGRAM_SPARSE_SLOWLY("Media.DetectedContainer", container);
  }

  return avformat_open_input(&format_context_, nullptr, nullptr, nullptr) == 0;
}

FFmpegGlue::~FFmpegGlue() {
  if (!format_context_) {
    av_free(avio_context_->buffer);
    return;
  }

  if (!open_called_) {
    avformat_free_context(format_context_);
    av_free(avio_context_->buffer);
    return;
  }

  if (format_context_->streams) {
    for (int i = format_context_->nb_streams - 1; i >= 0; --i) {
      AVStream* stream = format_context_->streams[i];

      if (stream && stream->codec && stream->codec->codec) {
        stream->discard = AVDISCARD_ALL;
        avcodec_close(stream->codec);
      }
    }
  }

  avformat_close_input(&format_context_);
  av_free(avio_context_->buffer);
}


} // namespace media
