#include "media/ffmpeg.h"

#include <gtest/gtest.h>
#include <glog/logging.h>

const char* kDefaultVideoPath = "test_data/test.mp4";

/**
 * At its very basic level, dealing with video and audio streams is very easy:
 *
 * video_stream = open_video_stream("video.avi");
 * while ((frame = read_packet(video_stream))) {
 *   if frame not complete
 *     continue;
 *   // DO SOMETHING
 * }
 */
TEST(FFmpeg, Basic) {

  ::av_register_all();

  AVFormatContext* ctx = nullptr;
  CHECK_EQ(avformat_open_input(&ctx, kDefaultVideoPath, nullptr, nullptr), 0);
  
  // retrieve stream information
  DCHECK(avformat_find_stream_info(ctx, nullptr) >= 0);

  ::av_dump_format(ctx, 0, kDefaultVideoPath, 0);

  EXPECT_TRUE(true);
}
