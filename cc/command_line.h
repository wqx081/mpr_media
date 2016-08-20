#ifndef CC_COMMAND_LINE_H_
#define CC_COMMAND_LINE_H_

#include <string>
#include <functional>
#include <vector>
#include <map>

#include "base/status.h"
#include "base/macros.h"
#include "cc/ffmpeg_common.h"

namespace media {

struct SpecifierOpt {
  std::string specifier;
  union {
    std::string str;
    int i;
    int64_t i64;
    float f;
    double dbl;
  } u;
};

typedef std::function<int(void*, const char*, const char*)> OptionCallback;

struct OptionDef {
  enum Flag {
    HAS_ARG    = 0x0001,
    OPT_BOOL   = 0x0002,
    OPT_EXPERT = 0x0004,
    OPT_STRING = 0x0008,
    OPT_VIDEO  = 0x0010,
    OPT_AUDIO  = 0x0020,
    OPT_INT    = 0x0080,
    OPT_FLOAT  = 0x0100,
    OPT_SUBTITLE = 0x0200,
    OPT_INT64  = 0x0400,
    OPT_EXIT   = 0x0800,
    OPT_DATA   = 0x1000,
    OPT_PERFILE= 0x2000, 
    OPT_OFFSET = 0x4000,
    OPT_SPEC   = 0x8000,
    OPT_TIME   = 0x10000,
    OPT_DOUBLE = 0x20000,
    OPT_INPUT  = 0x40000,
    OPT_OUTPUT = 0x80000,
  };
  std::string name;
  int flags;
  void* dst_ptr;
  OptionCallback func_arg;
  size_t off;
  std::string help;
  std::string argname;
};

struct Option {
  OptionDef* opt;
  std::map<std::string, std::string> kv;
};

struct OptionGroupDef {
  std::string name;
  std::string sep;
  int flags;
};

struct OptionGroup {
  const OptionGroupDef* group_def;
  std::string arg;

  std::vector<Option> opts;

  AVDictionary* codec_opts;
  AVDictionary* format_opts;
  AVDictionary* resample_opts;
  AVDictionary* sws_dict;
  AVDictionary* swr_opts;
};

struct OptionGroupList {
  const OptionGroupDef* group_def;
  std::vector<OptionGroup> groups;
};

struct OptionParseContext {
  OptionGroup global_opts;

  std::vector<OptionGroupList> groups;

  /* Parsing state */
  OptionGroup cur_group;
};

class CommandLine {
 public:
  explicit CommandLine();
  ~CommandLine();

  base::Status Initialize(int argc, char* argv[]);
  // split_command_line
  base::Status Parse();

 private: 
  OptionParseContext octx_;
  DISALLOW_COPY_AND_ASSIGN(CommandLine);
};

} // namespace media
#endif // CC_COMMAND_LINE_H_
