#ifndef MEDIA_BASE_TIMESTAMP_CONSTANTS_H_
#define MEDIA_BASE_TIMESTAMP_CONSTANTS_H_

#include <stdint.h>

#include <limits>

#include "base/time.h"

namespace media {

// Indicates an invalid or missing timestamp.
const base::TimeDelta kNoTimestamp =
    base::TimeDelta::FromMicroseconds(std::numeric_limits<int64_t>::min());

// Represents an infinite stream duration.
const base::TimeDelta kInfiniteDuration =
    base::TimeDelta::FromMicroseconds(std::numeric_limits<int64_t>::max());

}  // namespace media

#endif  // MEDIA_BASE_TIMESTAMP_CONSTANTS_H_
