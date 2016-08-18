#ifndef MEDIA_DEMUXER_STREAM_PROVIDER_H_
#define MEDIA_DEMUXER_STREAM_PROVIDER_H_

/**
 * @Author: WangQixiang (wangqx@mpreader.com)  
 *
 * Usage example:
 *  DemuxerStreamProvider* provider = new DemuxerStreamProvider;
 *  DemuxerStream* stream = provider->GetStream(DemuxerStreamProvider::Type::Stream);
 *
 *  switch (stream->type()) {
 *    case DemuxerStream::Type::VIDEO:
 *      stream->Read(VideoReadCB);
 *    case DemuxerStream::Type::Audio:
 *      stream->Read(AudioReadCB);
 *    ...
 *  }
 */
#include "base/macros.h"
#include "media/demuxer_stream.h"

namespace media {

class DemuxerStreamProvider {
 public:
  enum Type {
    STREAM,
    // URL,
  };

  DemuxerStreamProvider();
  virtual ~DemuxerStreamProvider();

  virtual DemuxerStream* GetStream(DemuxerStream::Type type) = 0;
  //TODO
  //virtual std::string GetUrl() const;

  virtual DemuxerStreamProvider::Type GetType() const;

 private:
  DISALLOW_COPY_AND_ASSIGN(DemuxerStreamProvider);
};

} // namespace media
#endif // MEDIA_DEMUXER_STREAM_PROVIDER_H_
