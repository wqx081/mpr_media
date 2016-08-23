#include "cc/ffmpeg_facade.h"

#include "cc/config.h"
#include "cc/ffmpeg_c.h"
#include "cc/ffmpeg_cmdutils.h"
#include "cc/ffmpeg_common.h"

#include <mutex>

namespace ffmpeg {

// static
FFmpegFacade* FFmpegFacade::GetInstance() {
  static FFmpegFacade instance;
  return &instance;
}

// private
FFmpegFacade::FFmpegFacade() {
  std::lock_guard<std::mutex> lock(mutex_);
  // register ffmpeg
  av_register_all();
  state_ = State::UNINITILIAZED;
}

FFmpegFacade::~FFmpegFacade() {}

void FFmpegFacade::Reset() {
  //TODO
  std::lock_guard<std::mutex> lock(mutex_);  
  DCHECK(state_ == State::STOP);

  state_ = State::UNINITILIAZED; 
}

void FFmpegFacade::Desotry() {
  //TODO:
  // ffmpeg_cleanup
  std::lock_guard<std::mutex> lock(mutex_);  
  if (state_ != State::UNINITILIAZED) {  
    FFmpegCleanup_C();                                    
    state_ = State::UNINITILIAZED;
  }                                       
}

// APIs
base::Status FFmpegFacade::Initialize(
		const base::CommandLine::StringVector& args) {
  std::lock_guard<std::mutex> lock(mutex_);  
  DCHECK(state_ == State::UNINITILIAZED);
  
  //args_ = command_line.GetArgs();
  args_ = args; ;

  // Transcode args to internal respresentation
  // Apply the global options
  base::Status status;
  ffmpeg_command_line_.reset(std::move(FFmpegCommandLine::FromArgs(args_)).release());
  status = ffmpeg_command_line_->Split();
  if (!status.ok()) {
    return status;
  }
  
  status = ffmpeg_command_line_->ApplyGlobalOptions();
  if (!status.ok()) {
    return status;
  }
  
  // Tail
  state_ = State::START;
  return base::Status::OK();
}

base::Status FFmpegFacade::ReadyDataIO() {
  std::lock_guard<std::mutex> lock(mutex_);  
  DCHECK(state_ == State::START);
  
  // Parse options
  // Open Input/Output files and FilterGraphs(filters)
  std::unique_ptr<FFmpegIO> ffmpeg_io = 
         base::MakeUnique<FFmpegIO>(ffmpeg_command_line_->GetOptionParseContext());
  base::Status status = ffmpeg_io->OpenInputFiles();
  if (!status.ok()) {
    return status;
  }
  status = ffmpeg_io->InitComplexFilters();
  if (!status.ok()) {
    return status;
  }
  status = ffmpeg_io->OpenOutputFiles();
  if (!status.ok()) {
    return status;
  }
  status = ffmpeg_io->ConfigureComplexFilters();
  if (!status.ok()) {
    return status;
  }
  ///
  state_ = State::READY;
  return base::Status::OK();
}

base::Status FFmpegFacade::Transcode() {
  std::lock_guard<std::mutex> lock(mutex_);  
  DCHECK(state_ == State::READY);
  state_ = State::TRANSCODING;

  state_ = State::DONE;
  return base::Status::Status::OK();
}

FFmpegFacade::State FFmpegFacade::state() {
  std::lock_guard<std::mutex> lock(mutex_);  
  return state_;
}

} // namespace ffmpeg
