#include "transcoder/ffmpeg_launch.h"
#include <vector>
#include <string>
#include <gtest/gtest.h>

namespace ffmpeg {

class TranscoderReporter : public FFmpegLaunch::TranscoderObserver {
 public:
  virtual void OnTranscodeStep(const std::string& info) override {
    LOG(INFO) << "Get From FFmpeg: " << info;
  }
};

TEST(FFmpegLaunch, Basic) {
  std::vector<std::string> argv;
  argv.push_back(FFmpegLaunch::kFFmpegBinPath);
  argv.push_back("-loglevel");
  argv.push_back("panic");
  argv.push_back("-y");
  argv.push_back("-i");
  argv.push_back("/tmp/test.mp4");
  argv.push_back("-codec:v");
  argv.push_back("libx264");
  argv.push_back("-codec:a");
  argv.push_back("libfaac");
  argv.push_back("/tmp/out.flv");

  std::unique_ptr<FFmpegLaunch> ffmpeg_launch = FFmpegLaunch::CreateWithArgv(argv);

  TranscoderReporter reporter;
  EXPECT_TRUE(ffmpeg_launch->RegisterTranscoderObserver(&reporter));
  EXPECT_TRUE(ffmpeg_launch->Transcode().ok());
  int exit_code = -1;
  EXPECT_TRUE(ffmpeg_launch->WaitForExit(exit_code)); 
  EXPECT_EQ(exit_code, 0);
}

} // namespace ffmpeg
