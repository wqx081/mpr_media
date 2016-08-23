#include "cc/ffmpeg_commandline.h"
#include "cc/ffmpeg_cmdutils.h"
#include "cc/ffmpeg_c.h"
#include "cc/ffmpeg_c_api.h"

#include <stdlib.h>
#include <stdio.h>

namespace ffmpeg {

FFmpegCommandLine::~FFmpegCommandLine() {
  if (argc_ > 0) {
    for (int i=0; i < argc_; ++i) {
      if (argv_[i]) {
        ::free(argv_[i]);
      }
    }
    ::free(argv_);
  }
  // C API
  //UninitParseContext_C(&option_parse_context_);
}

void FFmpegCommandLine::DoInit(const base::CommandLine::StringVector& args) {
  argv_ = (char **) malloc((args.size() + 1) * sizeof(char *));
  DCHECK(argv_);
  for (size_t i=0; i < args.size(); ++i) {
    argv_[i] = (char*)malloc(args[i].size() + 1);
    DCHECK(argv_[i]);
    ::strcpy(argv_[i], args[i].c_str());
  }
  argv_[args.size()] = nullptr;
  argc_ = args.size(); 
  ::memset(&option_parse_context_, 0, sizeof(option_parse_context_));
  // C API
  PrepareAppArguments_C(&argc_, &argv_);
  InitParseContext_C(&option_parse_context_); 
}


base::Status FFmpegCommandLine::Split() {
  // C_API
  if (!SplitCommandLine_C(&option_parse_context_, argc_, argv_)) {
    return base::Status(base::Code::INVALID_ARGUMENT, "C_API:SplitCommandLine Error");
  }
  return base::Status::OK();
}

base::Status FFmpegCommandLine::ApplyGlobalOptions() {
  // C_API
  if (!ParseOptGroup_C(&option_parse_context_.global_opts)) {
    return base::Status(base::Code::INVALID_ARGUMENT, "C_API:ParseOptGroup Error");
  }
  return base::Status::OK();
}

OptionParseContext FFmpegCommandLine::GetOptionParseContext() {
  return option_parse_context_;
}

} // namespace media
