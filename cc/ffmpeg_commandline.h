//@Author: WangQixiang (wangqixiang@mpreader.com)
//@Date: 2016/08/23
//
// Usage example:
//
//   std::unique_ptr<FFmpegCommandLine> ffmpeg_command_line 
//       = FFmpegCommandLine::FromStringVector(args);
//   Status status = ffmpeg_command_line->Split();
//   if (!status.ok()) {
//     fail...
//   }
//
//   OptionParseContext option_parse_context = ffmpeg_command_line->GetOptionParseContext();
//
//
//   std::unique_ptr<FFmpegIO> ffmpeg_io = FFmpegIO::Create(option_parse_context);
//
//   Status status;
//   status = ffmpeg_io->OpenInputFiles();
//   status = ffmpeg_io->InitComplexFilters();
//   status = ffmpeg_io->OpenOutputFiles();
//   status = ffmpeg_io->ConfigureComplexFilters();
//   ...
//
#ifndef CC_FFMPEG_COMMANDLINE_H_
#define CC_FFMPEG_COMMANDLINE_H_

#include <memory>
#include "base/ptr_util.h"
#include "base/command_line.h"
#include "cc/ffmpeg_cmdutils.h"

#include <glog/logging.h>

namespace ffmpeg {

class FFmpegCommandLineInterface {
 public:
  virtual ~FFmpegCommandLineInterface() {}
  virtual base::Status Split() = 0;  
  virtual base::Status ApplyGlobalOptions() = 0;
  virtual OptionParseContext GetOptionParseContext() = 0;
};

////////////////////
//
class FFmpegCommandLine : public FFmpegCommandLineInterface {
 public:
  virtual ~FFmpegCommandLine() override;

  static std::unique_ptr<FFmpegCommandLine> FromArgs(
		  const base::CommandLine::StringVector& args) {
    if (args.empty()) {
      return nullptr;
    }
    return base::MakeUnique<FFmpegCommandLine>(args);
  }

  FFmpegCommandLine(const base::CommandLine::StringVector& args)
      : argv_(nullptr), argc_(0) {
    DoInit(args);     
  }

  virtual base::Status Split() override;
  virtual base::Status ApplyGlobalOptions() override;
  virtual OptionParseContext GetOptionParseContext() override;

  char** argv() {
    return argv_;
  }

  int argc() {
    return argc_;
  }

 private:
  char** argv_;
  int argc_;
  OptionParseContext option_parse_context_;

  void DoInit(const base::CommandLine::StringVector& args);
};

} // namespace ffmpeg
#endif // CC_FFMPEG_COMMANDLINE_H_
