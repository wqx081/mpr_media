// This file contains routines to kill processes and get the exit code and
// termination status.

#ifndef BASE_PROCESS_KILL_H_
#define BASE_PROCESS_KILL_H_

#include "base/file_path.h"
#include "base/process/process.h"
#include "base/process/process_handle.h"
#include "base/time.h"

namespace base {

class ProcessFilter;

// Return status values from GetTerminationStatus.  Don't use these as
// exit code arguments to KillProcess*(), use platform/application
// specific values instead.
enum TerminationStatus {
  TERMINATION_STATUS_NORMAL_TERMINATION,   // zero exit status
  TERMINATION_STATUS_ABNORMAL_TERMINATION, // non-zero exit status
  TERMINATION_STATUS_PROCESS_WAS_KILLED,   // e.g. SIGKILL or task manager kill
  TERMINATION_STATUS_PROCESS_CRASHED,      // e.g. Segmentation fault
  TERMINATION_STATUS_STILL_RUNNING,        // child hasn't exited yet
  TERMINATION_STATUS_LAUNCH_FAILED,        // child process never launched
  TERMINATION_STATUS_OOM,                  // Process died due to oom
  TERMINATION_STATUS_MAX_ENUM
};

// Attempts to kill all the processes on the current machine that were launched
// from the given executable name, ending them with the given exit code.  If
// filter is non-null, then only processes selected by the filter are killed.
// Returns true if all processes were able to be killed off, false if at least
// one couldn't be killed.
bool KillProcesses(const FilePath::StringType& executable_name,
                               int exit_code,
                               const ProcessFilter* filter);

// Attempts to kill the process group identified by |process_group_id|. Returns
// true on success.
bool KillProcessGroup(ProcessHandle process_group_id);

// Get the termination status of the process by interpreting the
// circumstances of the child process' death. |exit_code| is set to
// the status returned by waitpid() on POSIX, and from
// GetExitCodeProcess() on Windows.  |exit_code| may be NULL if the
// caller is not interested in it.  Note that on Linux, this function
// will only return a useful result the first time it is called after
// the child exits (because it will reap the child and the information
// will no longer be available).
TerminationStatus GetTerminationStatus(ProcessHandle handle,
                                                   int* exit_code);

// Send a kill signal to the process and then wait for the process to exit
// and get the termination status.
//
// This is used in situations where it is believed that the process is dead
// or dying (because communication with the child process has been cut).
// In order to avoid erroneously returning that the process is still running
// because the kernel is still cleaning it up, this will wait for the process
// to terminate. In order to avoid the risk of hanging while waiting for the
// process to terminate, send a SIGKILL to the process before waiting for the
// termination status.
//
// Note that it is not an option to call WaitForExitCode and then
// GetTerminationStatus as the child will be reaped when WaitForExitCode
// returns, and this information will be lost.
//
TerminationStatus GetKnownDeadTerminationStatus(
    ProcessHandle handle, int* exit_code);

// Wait for all the processes based on the named executable to exit.  If filter
// is non-null, then only processes selected by the filter are waited on.
// Returns after all processes have exited or wait_milliseconds have expired.
// Returns true if all the processes exited, false otherwise.
bool WaitForProcessesToExit(
    const FilePath::StringType& executable_name,
    base::TimeDelta wait,
    const ProcessFilter* filter);

// Waits a certain amount of time (can be 0) for all the processes with a given
// executable name to exit, then kills off any of them that are still around.
// If filter is non-null, then only processes selected by the filter are waited
// on.  Killed processes are ended with the given exit code.  Returns false if
// any processes needed to be killed, true if they all exited cleanly within
// the wait_milliseconds delay.
bool CleanupProcesses(const FilePath::StringType& executable_name,
                                  base::TimeDelta wait,
                                  int exit_code,
                                  const ProcessFilter* filter);

// This method ensures that the specified process eventually terminates, and
// then it closes the given process handle.
//
// It assumes that the process has already been signalled to exit, and it
// begins by waiting a small amount of time for it to exit.  If the process
// does not appear to have exited, then this function starts to become
// aggressive about ensuring that the process terminates.
//
// On Linux this method does not block the calling thread.
// On OS X this method may block for up to 2 seconds.
//
// NOTE: The process must have been opened with the PROCESS_TERMINATE and
// SYNCHRONIZE permissions.
//
void EnsureProcessTerminated(Process process);

// The nicer version of EnsureProcessTerminated() that is patient and will
// wait for |pid| to finish and then reap it.
void EnsureProcessGetsReaped(ProcessId pid);

}  // namespace base

#endif  // BASE_PROCESS_KILL_H_
