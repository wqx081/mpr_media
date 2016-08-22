#include "base/process/process.h"

#include <errno.h>
#include <sys/resource.h>

#include <glog/logging.h>

#include "base/file_util.h"
#include "base/lazy_instance.h"
#include "base/string_split.h"
#include "base/string_printf.h"

namespace base {

namespace {

const int kForegroundPriority = 0;

const int kBackgroundPriority = 5;

struct CheckForNicePermission {
  CheckForNicePermission() : can_reraise_priority(false) {
    // We won't be able to raise the priority if we don't have the right rlimit.
    // The limit may be adjusted in /etc/security/limits.conf for PAM systems.
    struct rlimit rlim;
    if ((getrlimit(RLIMIT_NICE, &rlim) == 0) &&
        (20 - kForegroundPriority) <= static_cast<int>(rlim.rlim_cur)) {
        can_reraise_priority = true;
    }
  };

  bool can_reraise_priority;
};

}  // namespace

// static
bool Process::CanBackgroundProcesses() {
  static LazyInstance<CheckForNicePermission> check_for_nice_permission =
      LAZY_INSTANCE_INITIALIZER;
  return check_for_nice_permission.Get().can_reraise_priority;
}

bool Process::IsProcessBackgrounded() const {
  DCHECK(IsValid());

  return GetPriority() == kBackgroundPriority;
}

bool Process::SetProcessBackgrounded(bool background) {
  DCHECK(IsValid());

  if (!CanBackgroundProcesses())
    return false;

  int priority = background ? kBackgroundPriority : kForegroundPriority;
  int result = setpriority(PRIO_PROCESS, process_, priority);
  DCHECK(result == 0);
  return result == 0;
}

}  // namespace base
