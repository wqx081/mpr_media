#include <stdint.h>
#include <glog/logging.h>
#include <unistd.h>

#include "base/process/process_handle.h"
#include "base/file_path.h"
#include "base/file_util.h"
#include "base/process/internal_linux.h"


namespace base {

namespace {
bool g_have_unique_id = false;
uint32_t g_unique_id;

// The process which set |g_unique_id|.
ProcessId g_procid;

// Mangle IDs so that they are not accidentally used as PIDs, e.g. as an
// argument to kill or waitpid.
uint32_t MangleProcessId(ProcessId process_id) {
  // Add a large power of 10 so that the pid is still the pid is still readable
  // inside the mangled id.
  return static_cast<uint32_t>(process_id) + 1000000000U;
}

}  // namespace

uint32_t GetUniqueIdForProcess() {
  if (!g_have_unique_id) {
    return MangleProcessId(GetCurrentProcId());
  }

  // Make sure we are the same process that set |g_procid|. This check may have
  // false negatives (if a process ID was reused) but should have no false
  // positives.
  DCHECK_EQ(GetCurrentProcId(), g_procid);
  return g_unique_id;
}


void InitUniqueIdForProcessInPidNamespace(ProcessId pid_outside_of_namespace) {
  g_unique_id = MangleProcessId(pid_outside_of_namespace);
  g_procid = GetCurrentProcId();
  g_have_unique_id = true;
}

ProcessId GetCurrentProcId() {
  return getpid();
}

ProcessHandle GetCurrentProcessHandle() {
  return GetCurrentProcId();
}

ProcessId GetProcId(ProcessHandle process) {
  return process;
}

ProcessId GetParentProcessId(ProcessHandle process) {
  ProcessId pid =
      internal::ReadProcStatsAndGetFieldAsInt64(process, internal::VM_PPID);
  if (pid)
    return pid;
  return -1;
}

FilePath GetProcessExecutablePath(ProcessHandle process) {
  FilePath stat_file = internal::GetProcPidDir(process).Append("exe");
  FilePath exe_name;
  if (!ReadSymbolicLink(stat_file, &exe_name)) {
    return FilePath();
  }
  return exe_name;
}

}  // namespace base
