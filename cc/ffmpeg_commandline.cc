#include "cc/ffmpeg_commandline.h"
#include "cc/ffmpeg_cmdutils.h"
#include "cc/ffmpeg_c.h"

namespace ffmpeg {

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
