#include "media/demuxer_stream.h"

namespace media {

DemuxerStream::~DemuxerStream() {}

DemuxerStream::Liveness DemuxerStream::liveness() const {
  return DemuxerStream::LIVENESS_UNKNOWN;
}

void DemuxerStream::EnableBitstreamConverter() {}

} // namespace media
