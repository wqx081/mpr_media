#ifndef BASE_PROCESS_PROCESS_HANDLE_H_
#define BASE_PROCESS_PROCESS_HANDLE_H_

#include <stdint.h>
#include <sys/types.h>

#include "base/file_path.h"


namespace base {

// ProcessHandle is a platform specific type which represents the underlying OS
// handle to a process.
// ProcessId is a number which identifies the process in the OS.
// On POSIX, our ProcessHandle will just be the PID.
typedef pid_t ProcessHandle;
typedef pid_t ProcessId;
const ProcessHandle kNullProcessHandle = 0;
const ProcessId kNullProcessId = 0;

// Returns the id of the current process.
// Note that on some platforms, this is not guaranteed to be unique across
// processes (use GetUniqueIdForProcess if uniqueness is required).
ProcessId GetCurrentProcId();

// Returns a unique ID for the current process. The ID will be unique across all
// currently running processes within the chrome session, but IDs of terminated
// processes may be reused. This returns an opaque value that is different from
// a process's PID.
uint32_t GetUniqueIdForProcess();

// When a process is started in a different PID namespace from the browser
// process, this function must be called with the process's PID in the browser's
// PID namespace in order to initialize its unique ID. Not thread safe.
// WARNING: To avoid inconsistent results from GetUniqueIdForProcess, this
// should only be called very early after process startup - ideally as soon
// after process creation as possible.
void InitUniqueIdForProcessInPidNamespace(
    ProcessId pid_outside_of_namespace);

// Returns the ProcessHandle of the current process.
ProcessHandle GetCurrentProcessHandle();

// Returns the process ID for the specified process. This is functionally the
// same as Windows' GetProcessId(), but works on versions of Windows before Win
// XP SP1 as well.
// DEPRECATED. New code should be using Process::Pid() instead.
// Note that on some platforms, this is not guaranteed to be unique across
// processes.
ProcessId GetProcId(ProcessHandle process);

// Returns the ID for the parent of the given process.
ProcessId GetParentProcessId(ProcessHandle process);

// Returns the path to the executable of the given process.
FilePath GetProcessExecutablePath(ProcessHandle process);

}  // namespace base

#endif  // BASE_PROCESS_PROCESS_HANDLE_H_
