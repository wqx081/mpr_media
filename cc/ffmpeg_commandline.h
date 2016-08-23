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
#include "cc/ffmpeg_c.h"
#include "cc/ffmpeg_cmdutils.h"

#include <glog/logging.h>

#include <stdlib.h>
#include <stdio.h>

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
  virtual ~FFmpegCommandLine() override {
    if (argc_ > 0) {
      for (int i=0; i < argc_; ++i) {
        if (argv_[i]) {
          ::free(argv_[i]);
        }
      }
      ::free(argv_);
    }
    // C API
    UninitParseContext_C(&option_parse_context_);
  }

  static std::unique_ptr<FFmpegCommandLine> FromArgs(const base::CommandLine::StringVector& args) {
    if (args.empty()) {
      return nullptr;
    }
    return base::MakeUnique<FFmpegCommandLine>(args);
  }

  FFmpegCommandLine(const base::CommandLine::StringVector& args): argv_(nullptr), argc_(0) {
    DoInit(args);     
  }

  virtual base::Status Split() override;
  virtual base::Status ApplyGlobalOptions() override;
  virtual OptionParseContext GetOptionParseContext() override;

 private:
  char** argv_;
  int argc_;
  OptionParseContext option_parse_context_;

  void DoInit(const base::CommandLine::StringVector& args) {
    argv_ = (char **) malloc((args.size() + 1) * sizeof(char *));
    DCHECK(argv_);
    for (int i=0; i < args.size(); ++i) {
      argv_[i] = (char*)malloc(args[i].size() + 1);
      DCHECK(argv_[i]);
      ::strcpy(argv_[i], args[i].c_str());
    }
    argv_[args.size()] = nullptr;
    argc_ = args.size(); 
    ::memset(&option_parse_context_, 0, sizeof(option_parse_context_));
    // C API
    // Init C argv and context
    PrepareAppArguments_C(&argc_, &argv_);
    InitParseContext_C(&option_parse_context_); 
  }

};

} // namespace ffmpeg
#endif // CC_FFMPEG_COMMANDLINE_H_
