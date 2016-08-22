#include "base/process/kill.h"
#include "base/process/process_iterator.h"

namespace base {

bool KillProcesses(const FilePath::StringType& executable_name,
                   int exit_code,
                   const ProcessFilter* filter) {
  bool result = true;
  NamedProcessIterator iter(executable_name, filter);
  while (const ProcessEntry* entry = iter.NextProcessEntry()) {
    Process process = Process::Open(entry->pid());
    result &= process.Terminate(exit_code, true);
  }
  return result;
}

}  // namespace base
