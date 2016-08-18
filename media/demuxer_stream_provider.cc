#include "media/demuxer_stream_provider.h"

namespace media {

DemuxerStreamProvider::DemuxerStreamProvider() {}
DemuxerStreamProvider::~DemuxerStreamProvider() {}

DemuxerStreamProvider::Type DemuxerStreamProvider::GetType() const {
  return STREAM;
}

} // namespace media
