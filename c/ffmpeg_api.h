#ifndef C_FFMPEG_API_H_
#define C_FFMPEG_API_H_

enum FFmpegStatus {
  OK = 0,
  // For option parser
  INVALID_ARGUMENT,
  // For Decoder
  DECODER_AUDIO_ERROR,
  DECODER_VIDEO_ERROR,
  // TODO
};

int FFmpegTranscode(int argc, char** argv);

#endif // C_FFMPEG_API_H_
