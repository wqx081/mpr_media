//@Author: WangQixiang (wangqx@mpreader.com)
//@Date: 2016/08/24
//
// Usage example:
//   
//   std::vector<std::string> argv;
//   argv.push_back(...);
//   ...
//
//   std::unique_ptr<FFmpegLauch> ffmpeg_lanuch = FFmpegLanuch::FromArgv(argv);
//
#ifndef TRANSCODER_FFMPEG_LAUNCH_H_
#define TRANSCODER_FFMPEG_LAUNCH_H_

#include "base/macros.h"
#include "base/status.h"
#include "base/process/process.h"

#include <vector>
#include <string>

namespace ffmpeg {

class Transcoder {
 public:
  virtual ~Transcoder(){}
  virtual base::Status Initialize() = 0;
  virtual base::Status Start() = 0;
  virtual base::Status Stop() = 0;
};

class FFmpegLaunch {
 public:
  using StringVector = std::vector<std::string>;
  static const std::string kFFmpegBinPath ;

  ~FFmpegLaunch() {};

  FFmpegLaunch();
  FFmpegLaunch(const StringVector& argv);

  static std::unique_ptr<FFmpegLaunch> Create();
  static std::unique_ptr<FFmpegLaunch> CreateWithArgv(const StringVector& argv);

  class TranscoderObserver {
   public:
    virtual ~TranscoderObserver() {}
    virtual void OnTranscodeStep(const std::string& info) = 0;
  };
  
  bool RegisterTranscoderObserver(TranscoderObserver* observer);
  void UnregisterTranscoderObserver(TranscoderObserver* observer);

  base::Status Transcode();
  bool WaitForExit(int& exit_code);  
  
  void set_argv(const StringVector& v) {
    argv_ = v;
  }
  StringVector argv() {
    return argv_;
  }

 private:
  std::vector<TranscoderObserver*> observer_list_;
  StringVector argv_;
  base::Process process_;

  DISALLOW_COPY_AND_ASSIGN(FFmpegLaunch);
};

} // namespace ffmpeg
#endif // TRANSCODER_FFMPEG_LAUNCH_H_
