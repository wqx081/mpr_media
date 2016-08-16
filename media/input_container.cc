#include "media/container.h"
#include "media/ffmpeg.h"

#include "base/status.h"

namespace media {

InputContainer::InputContainer(const std::string& filename,
                               const InputContainer::Options& options)
    : file_name_(filename),
      options_(options),
      input_context_(nullptr),
      file_iformat_(nullptr) {}

InputContainer::~InputContainer() {
  if (input_context_ != nullptr) {
    ::avformat_close_input(&input_context_);
  }
}

base::Status InputContainer::Open() {
  int ret;

  if (file_name_.empty()) {
    LOG(ERROR) << "empty input file";
    return base::Status(base::Code::NOT_FOUND, "file missed");
  }

  if (!options_.format.empty()) {
    if (!::av_find_input_format(options_.format.c_str())) {
      LOG(ERROR) << "Unknown input format";
      return base::Status(base::Code::UNIMPLEMENTED, 
                          "Unknown input format: " + options_.format);
    }
  }

  input_context_ = ::avformat_alloc_context(); 
  if (!input_context_) {
    return base::Status(base::Code::RESOURCE_EXHAUSTED, "avformat_alloc_context");
  }
  
  //TODO
  input_context_->video_codec_id = AV_CODEC_ID_NONE;
  input_context_->audio_codec_id = AV_CODEC_ID_NONE;
  input_context_->subtitle_codec_id = AV_CODEC_ID_NONE;

  input_context_->flags |= AVFMT_FLAG_NONBLOCK;
  
  if ((ret = ::avformat_open_input(&input_context_, 
                            file_name_.c_str(), 
                            file_iformat_, 
                            nullptr)) < 0) {
    
    PrintFFmpegError(file_name_.c_str(), ret);
    return base::Status(base::Code::DATA_LOSS, "no stream: " + file_name_);
  }
  
  // Find stream info
  if (::avformat_find_stream_info(input_context_, nullptr) < 0) {
    #if 0
    if (input_context_->nb_streams == 0) {
      ::avformat_close_input(&input_context_);
    }
    #endif
    LOG(ERROR) << "avformat_find_stream_info";
    return base::Status(base::Code::DATA_LOSS, "no stream: " + file_name_);
  }
  
  return base::Status::OK();
}

base::Status InputContainer::Seek(int64_t start_time, int64_t seek_timestamp) {
  int64_t timestamp;
  timestamp = (start_time == AV_NOPTS_VALUE) ? 0 : start_time;
  // Add the stream start time
  if (!seek_timestamp && input_context_->start_time != AV_NOPTS_VALUE) {
    timestamp += input_context_->start_time;
  }
  
  // If seeking requested, we execute it
  if (start_time != AV_NOPTS_VALUE) {
    int64_t new_seek_timestamp = timestamp;
    if (!(input_context_->iformat->flags & AVFMT_SEEK_TO_PTS)) {
      int dts_heuristic = 0;
      for (unsigned int i = 0; i < input_context_->nb_streams; ++i) {
        AVCodecContext* avctx = input_context_->streams[i]->codec;
        if (avctx->has_b_frames) {
          dts_heuristic = 1;
        }
      }
      if (dts_heuristic) {
        new_seek_timestamp -= 3 * AV_TIME_BASE / 23;
      }
    }
    if (::avformat_seek_file(input_context_,
                             -1,
                             INT64_MIN,
                             new_seek_timestamp,
                             new_seek_timestamp,
                             0) < 0) {
      LOG(WARNING) << file_name_ << ": could not seek to position %0.3f\n" <<
          (double)timestamp / AV_TIME_BASE;
      return base::Status(base::Code::UNKNOWN, "could not seek");
    }
  }
  return base::Status::OK();
}

base::Status InputContainer::Close() {
  return base::Status::OK();
}

} // namespace media
