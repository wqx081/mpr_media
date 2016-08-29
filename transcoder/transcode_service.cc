#include "transcoder/transcode_service.h"
#include "transcoder/ffmpeg_launch.h"

#include "base/string_util.h"
#include "base/string_split.h"

namespace ffmpeg {

grpc::Status TranscoderImpl::Transcode(grpc::ServerContext* context,
                                       const transcoder::TranscodeRequest* request,
      grpc::ServerWriter<transcoder::TranscodeResponse>* writer) {
  (void) context;
  writer_ = writer;
  FFmpegLaunch::StringVector args;
  args.push_back(FFmpegLaunch::kFFmpegBinPath);
  args.push_back("-loglevel");
  args.push_back("panic");
  args.push_back("-y");
  args.push_back("-i");
  args.push_back(request->media_source_path());
  
// CoDec
  args.push_back("-codec:v");  
  args.push_back("libx264");
  args.push_back("-codec:a");
  args.push_back("libfaac");
  
// Audio
  args.push_back("-ar");
  args.push_back(request->audio_data().sample());

// Video
  args.push_back("-r"); // Frame rate, e.g. 25 fps
  args.push_back(request->video_data().frame_rate());
  args.push_back("-s"); // Frame size, e.g. 320x240
  args.push_back(request->video_data().frame_size());
  args.push_back("-aspect"); // Frame aspect, e.g. 4:3
  args.push_back(request->video_data().frame_aspect());
  args.push_back("-b:v"); // Bit rate  800k
  args.push_back(request->video_data().rate_bit());

// x264 DON'T TOUCH ME!!!
  args.push_back("-flags");     args.push_back("+loop");
  args.push_back("-subq");      args.push_back("5");
  args.push_back("-trellis");   args.push_back("1");
  args.push_back("-refs");      args.push_back("1");
  args.push_back("-coder");     args.push_back("0");
  args.push_back("-me_range");  args.push_back("16");
  args.push_back("-minrate");   args.push_back(request->video_data().rate_bit());
  args.push_back("-maxrate");   args.push_back(request->video_data().rate_bit());
  args.push_back("-bufsize");   args.push_back(request->video_data().rate_bit());
  args.push_back("-qmax");      args.push_back("51");

  args.push_back("-g");     args.push_back("25"); // Maybe 250???
  args.push_back("-level"); args.push_back("30");
  args.push_back("-async"); args.push_back("2");

  args.push_back("-map");
  args.push_back("0");

// Segment info
  args.push_back("-f");
  args.push_back("segment");

  args.push_back("-segment_time");
  args.push_back(request->segment_data().time());
  args.push_back("-segment_list_entry_prefix");
  args.push_back(request->segment_data().url_prefix());
  args.push_back("-segment_list");
  args.push_back(request->segment_data().m3u8_name());
  args.push_back("-segment_list_flags");
  args.push_back("+live");
  args.push_back(request->media_target_path() + "my%d.ts");
  
  std::string cmd;
  for (auto c: args) {
    cmd += " " + c;
  }
  LOG(INFO) << "FFmpeg command: " << cmd;

  std::unique_ptr<FFmpegLaunch> ffmpeg_lauch = FFmpegLaunch::CreateWithArgv(args);
  if (!ffmpeg_lauch->RegisterTranscoderObserver(this)) {
    LOG(ERROR) << "FFmpegLaunch: RegisterTranscoderObserver";
    return grpc::Status(grpc::INTERNAL, "FFmpegLaunch: RegisterTranscoderObserver");
  }

  // Callback to OnTranscodeStep
  if (!ffmpeg_lauch->Transcode().ok()) {
    LOG(ERROR) << "FFmpegLaunch: Transcode()";
    return grpc::Status(grpc::INTERNAL, "FFmpegLaunch: Transcode()");
  }

  int exit_code = -1;
  if (!ffmpeg_lauch->WaitForExit(exit_code)) {
    LOG(ERROR) << "FFmpegLaunch: WaitForExit()";
    return grpc::Status(grpc::INTERNAL, "FFmpegLaunch: WaitForExit()");
  }
  if (exit_code != 0) {
    LOG(ERROR) << "FFmpegLaunch: FFmpeg Internal";
    return grpc::Status(grpc::INTERNAL, "FFmpegLaunch: FFmpeg Internal");
  }
  return grpc::Status::OK;  
}

void TranscoderImpl::OnTranscodeStep(const std::string& info) {
  transcoder::TranscodeResponse response;  
  base::StringPairs kv_pairs;
  base::SplitStringIntoKeyValuePairs(info,
                                     '=',
                                     ';',
                                     &kv_pairs);
  for (auto kv : kv_pairs) {
    base::TrimWhitespaceASCII(kv.first, base::TRIM_ALL, &kv.first);  
    base::TrimWhitespaceASCII(kv.second, base::TRIM_ALL, &kv.second);  
    if (kv.first == "duration") response.set_duration(kv.second);
    if (kv.first == "out_time_ms") response.set_out_time(kv.second);

    if (kv.first == "frame") response.set_frame(kv.second);
    if (kv.first == "fps") response.set_fps(kv.second);
    if (kv.first == "size") response.set_size(kv.second);
    if (kv.first == "time") response.set_time(kv.second);
    if (kv.first == "bitrate") response.set_bitrate(kv.second);
    if (kv.first == "speed") response.set_speed(kv.second);
  }
  // Grpc
  writer_->Write(response);  
}

} // namespace ffmpeg
