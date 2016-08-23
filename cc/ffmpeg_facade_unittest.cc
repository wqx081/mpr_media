#include "cc/ffmpeg_facade.h"
#include "base/command_line.h"

#include <vector>
#include <gtest/gtest.h>

namespace ffmpeg {

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

  base::Status status = ffmpeg_facade->Initialize(argv);
  EXPECT_TRUE(status.ok());

  status = ffmpeg_facade->ReadyDataIO();
  EXPECT_TRUE(status.ok());
  
}

} // namespace ffmpeg
