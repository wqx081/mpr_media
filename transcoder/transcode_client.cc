#include <grpc/grpc.h>
#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>

#include <glog/logging.h>
#include "protos/transcode.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::Status;

using transcoder::Transcoder;
using transcoder::TranscodeResponse;
using transcoder::TranscodeRequest;
using transcoder::AudioData;
using transcoder::VideoData;
using transcoder::SegmentData;

class TranscodeClient {
 public:
  TranscodeClient(std::shared_ptr<Channel> channel)
      : stub_(Transcoder::NewStub(channel)) {}

  void Transcode() {
    ClientContext context;
    TranscodeResponse response;
    TranscodeRequest request;
    AudioData* audio_data = new AudioData();
    VideoData* video_data = new VideoData();
    SegmentData* segment_data = new SegmentData();

    request.set_media_source_path("/tmp/test.mp4");

    audio_data->set_sample("48000");

    video_data->set_frame_size("320x240");
    video_data->set_frame_aspect("4:3");
    video_data->set_frame_rate("25");
    video_data->set_rate_bit("800k");

    segment_data->set_time("10"); // 10s
    segment_data->set_url_prefix("https://172.16.2.129:80/media/");
    segment_data->set_m3u8_name("/tmp/wqx/test/test.m3u8");

    request.set_allocated_audio_data(audio_data);
    request.set_allocated_video_data(video_data);
    request.set_allocated_segment_data(segment_data);
    request.set_media_target_path("/tmp/wqx/test/");

    std::unique_ptr<ClientReader<TranscodeResponse>> reader(
      stub_->Transcode(&context, request));
    while (reader->Read(&response)) {
      LOG(INFO) << "duration: " << response.duration() << " , out_time: " << response.out_time();
    }   
    Status status = reader->Finish();
    if (status.ok()) {
      LOG(INFO) << "Transcode RPC succeeded";
    } else {
      LOG(INFO) << "Transcode RPC failed";
    }
  }

 private:
  std::unique_ptr<Transcoder::Stub> stub_;
};

int main() {
  TranscodeClient client(
    grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));

  client.Transcode();

  return 0;
}

