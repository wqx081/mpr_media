#include "media/pipeline_status.h"

//#include "base/bind.h"

namespace media {

#if 0
static void ReportAndRun(const std::string& name,
                         const PipelineStatusCB& cb,
                         PipelineStatus status) {
  (void) name;
  cb.Run(status);
}

PipelineStatusCB CreateUMAReportingPipelineCB(const std::string& name,
                                              const PipelineStatusCB& cb) {
  return base::Bind(&ReportAndRun, name, cb);
}
#endif

}  // namespace media
