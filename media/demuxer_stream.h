#ifndef MEDIA_DEMUXER_H_
#define MEDIA_DEMUXER_H_
#include "base/callback.h"
#include "base/ref_counted.h"
#include "base/time.h"
#include "media/video_rotation.h"

namespace media {

class AudioDecoderConfig;
class DecoderBuffer;
class VideoDecoderConfig;

class DemuxerStream {
 public:
  enum Type {
    UNKNOWN,
    AUDIO,
    VIDEO,
    TEXT,
    NUM_TYPES,
  };
  enum Liveness {
    LIVENESS_UNKNOWN,
    LIVENESS_RECORDED,
    LIVENESS_LIVE,
  };

  enum Status {
    kOk,
    kAborted,
    kConfigChanged,
  };

  // Request a buffer to returned via the provided callback.
  typedef base::Callback<void(Status,
                              const scoped_ref_ptr<DecoderBuffer>&)> ReadCB;
  virtual void Read(const ReadCB& read_cb) = 0;
  virtual AudioDecoderConfig audio_decoder_config() = 0;
  virtual VideoDecoderConfig video_decoder_config() = 0;

  virtual Type type() const = 0;
  virtual Liveness liveness() const;
  virtual void EnableBitstreamConverter();
  virtual bool SupportsConfigChanges() = 0;
  virtual VideoRotation video_rotation() = 0;
  virtual bool enabled() const = 0;
  virtual void set_enabled(bool enabled, base::TimeDelta timestamp) = 0;

  using StreamRestartedCB =
     base::Callback<void(DemuxerStream*, base::TimeDelta)>;
  virtual void SetStreamRestartedCB(const StreamRestartedCB& cb) = 0;

 protected:
  virtual ~DemuxerStream();
};

} // namespace
#endif // MEDIA_DEMUXER_H_
