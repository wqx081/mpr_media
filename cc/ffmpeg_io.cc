#include "cc/ffmpeg_cmdutils.h"
#include "cc/ffmpeg_io.h"

namespace ffmpeg {

base::Status FFmpegIO::OpenInputFiles() {
  if (!OpenInputFiles_C(&option_parse_context_)) {
    return base::Status(base::Code::INTERNAL, "C:OpenInputFiles_C Error");
  }
  return base::Status::OK();
}

base::Status FFmpegIO::InitComplexFilters() {
  if (!InitComplexFilters_C()) {
    return base::Status(base::Code::INTERNAL, "C:InitComplexFilters_C Error");
  }
  return base::Status::OK();
}

base::Status FFmpegIO::OpenOutputFiles() {
  if (!OpenOutputFiles_C(&option_parse_context_)) {
    return base::Status(base::Code::INTERNAL, "C:OpenOutputFiles_C Error");
  }
  return base::Status::OK();
}

base::Status FFmpegIO::ConfigureComplexFilters() {
  if (!ConfigureComplexFilters_C()) {
    return base::Status(base::Code::INTERNAL, "C:ConfigureComplexFilters_C Error");
  }
  return base::Status::OK();
}

} // namespace ffmpeg
