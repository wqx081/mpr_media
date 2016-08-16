#include "media/container.h"

#include <gtest/gtest.h>
#include <glog/logging.h>

namespace media {

TEST(Container, Basic) {

  ::av_register_all();

  InputContainer* input_container = new InputContainer("test_data/test.mp4",
                                                  InputContainer::Options::Default());
  
  EXPECT_TRUE(input_container->Open().ok());
  LOG(INFO) << "duration: " << input_container->duration();
  LOG(INFO) << "bit_rate: " << input_container->bit_rate();
  LOG(INFO) << "start_time: " << input_container->start_time();

  EXPECT_TRUE(input_container->Close().ok());
}

} // namespace media
