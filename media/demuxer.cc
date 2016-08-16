#include "media/demuxer.h"

#include <glog/logging.h>

namespace media {

DemuxerHost::~DemuxerHost() {}

Demuxer::Demuxer() {}

Demuxer::~Demuxer() {}

void Demuxer::SetPlaybackRate(float playback_rate) {
  (void)playback_rate;
}

void Demuxer::Seek(base::TimeDelta time, const PipelineStatusCB& status_cb) {
  DCHECK(!status_cb.is_null());
  status_cb.Run(PIPELINE_OK);
  (void)time;
}

void Demuxer::Stop(const base::Closure& callback) {
  DCHECK(!callback.is_null());
  callback.Run();
}

void Demuxer::OnAudioRendererDisabled() {}

}  // namespace media
