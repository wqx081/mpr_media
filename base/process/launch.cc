#include "base/process/launch.h"

namespace base {

LaunchOptions::LaunchOptions() = default;

LaunchOptions::LaunchOptions(const LaunchOptions& other) = default;

LaunchOptions::~LaunchOptions() = default;

LaunchOptions LaunchOptionsForTest() {
  LaunchOptions options;
  // To prevent accidental privilege sharing to an untrusted child, processes
  // are started with PR_SET_NO_NEW_PRIVS. Do not set that here, since this
  // new child will be used for testing only.
  options.allow_new_privs = true;
  return options;
}

}  // namespace base
