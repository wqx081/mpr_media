#include "cc/ffmpeg_facade.h"
#include "base/command_line.h"

#include <vector>
#include <gtest/gtest.h>

namespace ffmpeg {

#if 0
TEST(FFmpegFacade, Create) {
  FFmpegFacade* ffmpeg_facade = FFmpegFacade::GetInstance();
  EXPECT_EQ(FFmpegFacade::UNINITILIAZED, ffmpeg_facade->state());
  EXPECT_EQ(ffmpeg_facade,
            FFmpegFacade::GetInstance());

  std::vector<std::string> argv;
  argv.push_back("FFmpeg");
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

  base::Status status;
  status = ffmpeg_facade->Initialize(argv);
  EXPECT_TRUE(status.ok());

  status = ffmpeg_facade->ReadyDataIO();
  EXPECT_TRUE(status.ok());
  
  ffmpeg_facade->Desotry();

  status = ffmpeg_facade->Initialize(argv);
  EXPECT_TRUE(status.ok());

  status = ffmpeg_facade->ReadyDataIO();
  EXPECT_TRUE(status.ok());

  ffmpeg_facade->Desotry();
}

#endif

class SimpleTranscodeObserver : public FFmpegFacade::TranscodeObserver {
 public:
  virtual ~SimpleTranscodeObserver() {}
  virtual void OnTranscodeStep(const std::string& info) override {
    LOG(INFO) << "Transcode: " << info;  
  }
};


TEST(FFmpegFacade, Transcode) {
  FFmpegFacade* ffmpeg_facade = FFmpegFacade::GetInstance();
  EXPECT_EQ(FFmpegFacade::UNINITILIAZED, ffmpeg_facade->state());

  std::vector<std::string> argv;
  argv.push_back("FFmpeg");
//  argv.push_back("-loglevel");
//  argv.push_back("panic");
  argv.push_back("-y");
  argv.push_back("-i");
  argv.push_back("/tmp/test.mp4");
  argv.push_back("-codec:v");
  argv.push_back("libx264");
  argv.push_back("-codec:a");
  argv.push_back("libfaac");
  argv.push_back("/tmp/out.flv");

  base::Status status;
  status = ffmpeg_facade->Initialize(argv);
  EXPECT_TRUE(status.ok());

  status = ffmpeg_facade->ReadyDataIO();
  EXPECT_TRUE(status.ok());

  SimpleTranscodeObserver observer;
  EXPECT_TRUE(ffmpeg_facade->RegisterTranscodeObserver(&observer));

  status = ffmpeg_facade->Transcode();
  LOG(INFO) << "transcode: " << status.ToString();
  EXPECT_TRUE(status.ok());

//  ffmpeg_facade->Desotry();
}

} // namespace ffmpeg
