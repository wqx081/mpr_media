#include "media/ranges.h"

namespace media {

template<>
void Ranges<base::TimeDelta>::DCheckLT(const base::TimeDelta& lhs,
                                       const base::TimeDelta& rhs) const {
  DCHECK(lhs < rhs) << lhs.ToInternalValue() << " < " << rhs.ToInternalValue();
}

}  // namespace media
