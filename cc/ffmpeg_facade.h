// @Author: WangQixiang (wangqx@mpreader.com) 
// @Date: 2016/08/23
//
// *NOTE THAT*:
//   The FFmpegFacade is *NOT* thread-safe!!!
//   It should be Per thread has own FFmpegFacade.
//
// Usage example:
//
//   FFmpegFacade* ffmpeg_facade = FFmpegFace::GetInstance();
//   Status status = ffmpeg_facade->Initialize(command_line);
//   if (!status.ok()) {
//     fail... 
//   }
//
//   status = ffmpeg_facade->ReadyDataIO();
//
//   if (!status.ok()) {
//     fail...
//   }
//
//   FFmpegFacade::MetaData meta_data = ffmpeg_facade->meta_data();
//   meta_data.input_file
//   meta_data.output_file
//   meta_data.input_file_duration
//   ...
//
//   class TranscodeReporter : public FFmpegFacade {
//    public:
//     virtual void OnTranscodeStep(const std::string& info) override {
//       LOG(INFO) << "Done the data: " << info;
//     } 
//   };
//
//   TranscodeReporter reporter;
//
//   ffmpeg_facade->RegisterTranscodeObserver(&reporter);
//
//   // Take a long time
//   status = ffmpeg_facade->Transcode();
//
//
#ifndef CC_FFMPEG_FACADE_H_
#define CC_FFMPEG_FACADE_H_

#include "base/macros.h"
#include "base/status.h"
#include "base/command_line.h"

#include "cc/ffmpeg_c.h"
#include "cc/ffmpeg_commandline.h"
#include "cc/ffmpeg_io.h"

#include <vector>
#include <mutex>

namespace ffmpeg {

class FFmpegFacade {
 public:

  enum State {
    UNINITILIAZED,
    START,
    READY,
    TRANSCODING,
    STOP,
    DONE,
  };

  class TranscodeObserver {
   public:
    virtual ~TranscodeObserver() {}
    virtual void OnTranscodeStep(const std::string& info) = 0;
  };

  struct MetaData {
  
  };

  static FFmpegFacade* GetInstance();
  // parse arguments to internal
  base::Status Initialize(const base::CommandLine::StringVector& args);
  // open input/output files (codec/encode...)
  base::Status ReadyDataIO();
  MetaData meta_data();

  // transcode
  bool RegisterTranscodeObserver(TranscodeObserver* observer);
  void UnregisterTranscodeObserver(TranscodeObserver* observer);
  base::Status Transcode();

  State state();

  void Reset();
  void Desotry();
 private:

  std::mutex mutex_;
  State state_;
  base::CommandLine::StringVector args_;

  std::vector<TranscodeObserver*> observer_list_;
  std::unique_ptr<FFmpegCommandLine> ffmpeg_command_line_;

  FFmpegFacade();
  ~FFmpegFacade();
  DISALLOW_COPY_AND_ASSIGN(FFmpegFacade);
};

} // namespace ffmpeg
#endif // CC_FFMPEG_FACADE_H_
