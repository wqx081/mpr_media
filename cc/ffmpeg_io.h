//@Author: WangQixiang (wangqx@mpreader.com)
//@Date: 2016/08/23
#ifndef CC_FFMPEG_IO_H_
#define CC_FFMPEG_IO_H_
#include "base/status.h"
#include "base/macros.h"

#include "cc/ffmpeg_cmdutils.h"

namespace ffmpeg {

class FFmpegIOInterface {
 public:
  virtual ~FFmpegIOInterface() {}
  virtual base::Status OpenInputFiles() = 0;
  virtual base::Status InitComplexFilters() = 0;
  virtual base::Status OpenOutputFiles() = 0;
  virtual base::Status ConfigureComplexFilters() = 0;

};

/////

class FFmpegIO : public FFmpegIOInterface {
 public:
  virtual ~FFmpegIO() {}

  explicit FFmpegIO(const OptionParseContext& option_parse_context)
    : option_parse_context_(option_parse_context) {}

  virtual base::Status OpenInputFiles() override;
  virtual base::Status InitComplexFilters() override;
  virtual base::Status OpenOutputFiles() override;
  virtual base::Status ConfigureComplexFilters() override;

 private:
  OptionParseContext option_parse_context_;

  DISALLOW_COPY_AND_ASSIGN(FFmpegIO);  
};

} // namespace ffmpeg
#endif // CC_FFMPEG_IO_H_
