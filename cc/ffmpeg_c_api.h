//@Author: WangQixiang (wangqx@mpreader.com)
//@Date: 2016/08/23
//
#ifndef CC_FFMPEG_C_API_H_
#define CC_FFMPEG_C_API_H_

#include "cc/ffmpeg_facade.h"
#include "cc/ffmpeg_c.h"
#include "cc/ffmpeg_cmdutils.h"
#include "base/status.h"

#include <vector>

namespace ffmpeg {

// wqx
// implemented in ffmpeg_cmdutils.cc
extern const OptionGroupDef groups[];
void PrepareAppArguments_C(int* argc, char*** argv_ptr);
void InitParseContext_C(OptionParseContext* option_parse_context);
void UninitParseContext_C(OptionParseContext* option_parse_context);
bool SplitCommandLine_C(OptionParseContext* option_parse_context, 
		        int argc, char** argv);
bool ParseOptGroup_C(OptionGroup* group);

// wqx Steal From ffmpeg_opt.c : ffmpeg_parse_options()
// implemented in ffmpeg_opt.cc
bool OpenInputFiles_C(OptionParseContext* option_parse_context);
bool OpenOutputFiles_C(OptionParseContext* option_parse_context);
bool InitComplexFilters_C();
bool ConfigureComplexFilters_C();

// wqx
// implemeneted ffmpeg_c.cc
void FFmpegCleanup_C(int ret=0);
base::Status Transcode_C(int argc,
		         char** argv,
  std::vector<FFmpegFacade::TranscodeObserver*>& observer_list);

} // namespace ffmpeg
#endif // CC_FFMPEG_C_API_H_
