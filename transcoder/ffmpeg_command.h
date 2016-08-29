#ifndef TRANSCODER_FFMEPG_COMMAND_H_
#define TRANSCODER_FFMEPG_COMMAND_H_
#include <map>
#include <string>

namespace ffmpeg {

class FFmpegCommand {
 public:
  static FFmpegCommand DefaultCommand() {
    FFmpegCommand command;
    return command;
  }  
 private:
  std::map<std::string, std::string> kv_;

  void DoDefaultInit();
};

class TranscodeFFmpegCommand : public FFmpegCommand {

};

class SegmentFFmpegCommand : public FFmpegCommand {

};

} // namespace ffmpeg
#endif // TRANSCODER_FFMEPG_COMMAND_H_
