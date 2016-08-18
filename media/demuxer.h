#ifndef MEDIA_DEMUXER_H_
#define MEDIA_DEMUXER_H_

#include <stdint.h>

#include <memory>
#include <vector>

#include "base/macros.h"
#include "base/time.h"

#include "media/input_stream.h"
#include "media/demuxer_stream.h"
#include "media/demuxer_stream_provider.h"
#include "media/ranges.h"

namespace media {

class DemuxerHost {
 public:
  virtual void OnBufferedTimedRangesChanged(
      const Ranges<base::TimeDelta>& ranges) = 0;
  virtual void SetDuration(base::TimeDelta duration) = 0;
 protected:
  virtual ~DemuxerHost();
};



} // namespace media
#endif // MEDIA_DEMUXER_H_
