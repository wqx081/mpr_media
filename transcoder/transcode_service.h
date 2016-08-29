#ifndef TRANSCODE_SERVICE_H_
#define TRANSCODE_SERVICE_H_

#include "base/macros.h"

#include <glog/logging.h>

#include "transcoder/ffmpeg_launch.h"

#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>

#include "protos/transcode.grpc.pb.h"

namespace ffmpeg {


class TranscoderImpl final 
  : public transcoder::Transcoder::Service ,
    public FFmpegLaunch::TranscoderObserver {
 public:
//  TranscoderImpl(FFmpegLaunch* ffmpeg_launch)
//    : ffmpeg_launch_(ffmpeg_launch_) {}
//
  grpc::Status Transcode(grpc::ServerContext* context,
                const transcoder::TranscodeRequest* request,
                grpc::ServerWriter<transcoder::TranscodeResponse>* writer) override;
  void OnTranscodeStep(const std::string& info) override;

 private:
  grpc::ServerWriter<transcoder::TranscodeResponse>* writer_;

//  FFmpegLaunch* ffmpeg_launch_;  
};


class TranscoderServer {
 public:
  TranscoderServer(const std::string& host, int port)
    : host_(host), port_(port) {}

  void Run() {
    std::string server_address(host_+":"+std::to_string(port_));  
    TranscoderImpl service;

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address,
                             grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    LOG(INFO) << "Server listening on " << server_address;
    server->Wait();
  }

 private:
  std::string host_;
  int port_;

  DISALLOW_COPY_AND_ASSIGN(TranscoderServer);
};

} // namespace ffmpeg
#endif // TRANSCODE_SERVICE_H_
