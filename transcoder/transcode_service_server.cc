#include "transcoder/transcode_service.h"

int main() {
  ffmpeg::TranscoderServer server("localhost", 50051); 
  server.Run();
  return 0;
}
