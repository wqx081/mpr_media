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

class Demuxer : public DemuxerStreamProvider {
 public:
  Demuxer();
  virtual ~Demuxer() override;

  virtual std::string GetDisplayName() const = 0;
  virtual void Initialize(DemuxerHost* host) = 0;
  virtual void StartWaitingForSeek(base::TimeDelta seek_time) = 0;
  virtual void CancelPendingSeek(base::TimeDelta seek_time) = 0;
  virtual void Seek(base::TimeDelta time) = 0;
  virtual void Stop() = 0;
  virtual base::TimeDelta GetStartTime() const = 0;
  virtual base::Time GetTimelineOffset() const = 0;
  virtual int64_t GetMemoryUsage() const = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(Demuxer);
};

} // namespace media
#endif // MEDIA_DEMUXER_H_
