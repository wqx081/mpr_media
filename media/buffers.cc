#include "media/buffers.h"

namespace media {

Buffer::Buffer(base::TimeDelta timestamp, base::TimeDelta duration)
    : timestamp_(timestamp),
      duration_(duration) {
}

Buffer::~Buffer() {}

bool Buffer::IsEndOfStream() const {
  return GetData() == NULL;
}

}  // namespace media
