#include "ffmpeg/container.h"
#include "ffmpeg/ffmpeg.h"

#include <glog/logging.h>

namespace media {

OutputContainer::OutputContainer(const std::string& file_name,
                                 const OutputContainer::Options& options)
    : file_name_(file_name),
      options_(options),
      output_context_(nullptr),
      file_oformat_(nullptr) {}

OutputContainer::~OutputContainer() {
  //TODO
}

base::Status OutputContainer::Open() {
  int ret;
  
  ret = ::avformat_alloc_output_context2(&output_context_,
                                         nullptr,
                                         options_.format.c_str(),
                                         file_name_.c_str());
  if (!output_context_) {
    PrintFFmpegError(file_name_.c_str(), ret);
    return base::Status(base::Code::RESOURCE_EXHAUSTED, "could not allocated");
  }

  if (options_.recording_time != INT64_MAX) {
    output_context_->duration = options_.recording_time;
  } 

  file_oformat_ = output_context_->oformat;
  
  //TODO
  return base::Status::OK();
}

base::Status OutputContainer::Close() {
  return base::Status::OK();
}

} // namespace media
