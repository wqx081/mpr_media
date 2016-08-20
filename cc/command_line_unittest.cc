#include "cc/command_line.h"
              
#include <gtest/gtest.h>

namespace media {
              
char* argv[] = { "ffmpeg", "-loglevel", "panic", "-i", "test.mp4" };
  
TEST(CommandLine, Basic) {                         
  CommandLine command_line;                        
  EXPECT_TRUE(command_line.Initialize(sizeof(argv), argv).ok());
}                                                  
                                                     
} // namespace media

