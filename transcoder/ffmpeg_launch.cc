#include "transcoder/ffmpeg_launch.h"

#include "base/eintr_wrapper.h"
#include "base/ptr_util.h"
#include "base/string_util.h"
#include "base/string_split.h"
#include "base/process/launch.h"
#include "base/process/process.h"

namespace ffmpeg {

const std::string FFmpegLaunch::kFFmpegBinPath("/home/mpr/projects/mpr_media/bin/FFmpeg");

// private
FFmpegLaunch::FFmpegLaunch() {}
FFmpegLaunch::FFmpegLaunch(const StringVector& argv)
    : argv_(argv) {}

std::unique_ptr<FFmpegLaunch> FFmpegLaunch::Create() {
  return base::MakeUnique<FFmpegLaunch>();
}

std::unique_ptr<FFmpegLaunch> FFmpegLaunch::CreateWithArgv(const StringVector& argv) {
  return base::MakeUnique<FFmpegLaunch>(argv);
}

bool FFmpegLaunch::RegisterTranscoderObserver(FFmpegLaunch::TranscoderObserver* observer) {
  for (auto it = observer_list_.begin();
       it != observer_list_.end();
       ++it) {
    if (*it == observer) {
      return false;
    }
  }
  observer_list_.push_back(observer);
  return true;
}

void FFmpegLaunch::UnregisterTranscoderObserver(FFmpegLaunch::TranscoderObserver* observer) {
  for (auto it = observer_list_.begin();
       it != observer_list_.end();
       ++it) {
    if (observer == *it) {
      observer_list_.erase(it);
    }
  }
}

base::Status FFmpegLaunch::Transcode() {
  base::FileHandleMappingVector fds_to_remap;
  int fds[2];
  DCHECK(pipe(fds) == 0);
  fds_to_remap.push_back(std::make_pair(fds[1], 1));
  base::LaunchOptions options;
  options.clone_flags = 0;
  options.fds_to_remap = &fds_to_remap;

  process_ = base::LaunchProcess(argv(), options);

  if (!process_.IsValid()) {
    return base::Status(base::Code::INTERNAL, "LaunchProcess");
  }

  DCHECK(IGNORE_EINTR(close(fds[1])) == 0);
  char buf[1024] = {0};
  ssize_t n = 0;
  std::string remain = "";
  std::string input;
  do {
    n = HANDLE_EINTR(read(fds[0], buf, 1024));  
    std::string str(buf, n);
    auto pos = str.find_last_of('\n');
    if (pos == std::string::npos) {
      remain += str;
    } else {
      input = remain + str.substr(0, pos);
      std::vector<std::string> result = base::SplitString(input, 
                                                          "\n", 
                                                          base::TRIM_WHITESPACE, 
                                                          base::SPLIT_WANT_ALL); 
      remain = "";
      for (size_t i=0; i < result.size(); ++i) {
        //LOG(INFO) << "get from ffmpeg: " << result[i] << "\n";
        // Notify observers
        for (size_t i = 0; i < observer_list_.size(); ++i) {
          observer_list_[i]->OnTranscodeStep(result[i]);
        }
#if 0
        base::StringPairs kv_pairs;
        base::SplitStringIntoKeyValuePairs(result[i],
                                           '=',
                                           ';',
                                           &kv_pairs);
          for (auto kv : kv_pairs) {
            base::TrimWhitespaceASCII(kv.first, base::TRIM_ALL, &kv.first);
            base::TrimWhitespaceASCII(kv.second, base::TRIM_ALL, &kv.second);
            LOG(INFO) << kv.first << "," << kv.second;
          }
#endif
      }
    }
  } while (n != -1 && n != 0);
  return base::Status::OK();
}

bool FFmpegLaunch::WaitForExit(int& exit_code) {
  return process_.WaitForExit(&exit_code);
}

} // namespace ffmpeg
