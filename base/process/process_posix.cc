#include "base/process/process.h"

#include <errno.h>
#include <stdint.h>
#include <sys/resource.h>
#include <sys/wait.h>

#include <glog/logging.h>

#include "base/scoped_file.h"
#include "base/eintr_wrapper.h"
#include "base/process/kill.h"
#include "base/time.h"

namespace {

bool WaitpidWithTimeout(base::ProcessHandle handle,
                        int* status,
                        base::TimeDelta wait) {
  // This POSIX version of this function only guarantees that we wait no less
  // than |wait| for the process to exit.  The child process may
  // exit sometime before the timeout has ended but we may still block for up
  // to 256 milliseconds after the fact.
  //
  // waitpid() has no direct support on POSIX for specifying a timeout, you can
  // either ask it to block indefinitely or return immediately (WNOHANG).
  // When a child process terminates a SIGCHLD signal is sent to the parent.
  // Catching this signal would involve installing a signal handler which may
  // affect other parts of the application and would be difficult to debug.
  //
  // Our strategy is to call waitpid() once up front to check if the process
  // has already exited, otherwise to loop for |wait|, sleeping for
  // at most 256 milliseconds each time using usleep() and then calling
  // waitpid().  The amount of time we sleep starts out at 1 milliseconds, and
  // we double it every 4 sleep cycles.
  //
  // usleep() is speced to exit if a signal is received for which a handler
  // has been installed.  This means that when a SIGCHLD is sent, it will exit
  // depending on behavior external to this function.
  //
  // This function is used primarily for unit tests, if we want to use it in
  // the application itself it would probably be best to examine other routes.

  if (wait == base::TimeDelta::Max()) {
    return HANDLE_EINTR(waitpid(handle, status, 0)) > 0;
  }

  pid_t ret_pid = HANDLE_EINTR(waitpid(handle, status, WNOHANG));
  static const int64_t kMaxSleepInMicroseconds = 1 << 18;  // ~256 milliseconds.
  int64_t max_sleep_time_usecs = 1 << 10;                  // ~1 milliseconds.
  int64_t double_sleep_time = 0;

  // If the process hasn't exited yet, then sleep and try again.
  base::TimeTicks wakeup_time = base::TimeTicks::Now() + wait;
  while (ret_pid == 0) {
    base::TimeTicks now = base::TimeTicks::Now();
    if (now > wakeup_time)
      break;
    // Guaranteed to be non-negative!
    int64_t sleep_time_usecs = (wakeup_time - now).InMicroseconds();
    // Sleep for a bit while we wait for the process to finish.
    if (sleep_time_usecs > max_sleep_time_usecs)
      sleep_time_usecs = max_sleep_time_usecs;

    // usleep() will return 0 and set errno to EINTR on receipt of a signal
    // such as SIGCHLD.
    usleep(sleep_time_usecs);
    ret_pid = HANDLE_EINTR(waitpid(handle, status, WNOHANG));

    if ((max_sleep_time_usecs < kMaxSleepInMicroseconds) &&
        (double_sleep_time++ % 4 == 0)) {
      max_sleep_time_usecs *= 2;
    }
  }

  return ret_pid > 0;
}

bool WaitForExitWithTimeoutImpl(base::ProcessHandle handle,
                                int* exit_code,
                                base::TimeDelta timeout) {
  base::ProcessHandle parent_pid = base::GetParentProcessId(handle);
  base::ProcessHandle our_pid = base::GetCurrentProcessHandle();
  if (parent_pid != our_pid) {
    // Currently on Linux we can't handle non child processes.
    //NOTIMPLEMENTED();
  }

  int status;
  if (!WaitpidWithTimeout(handle, &status, timeout))
    return false;
  if (WIFSIGNALED(status)) {
    if (exit_code)
      *exit_code = -1;
    return true;
  }
  if (WIFEXITED(status)) {
    if (exit_code)
      *exit_code = WEXITSTATUS(status);
    return true;
  }
  return false;
}

}  // namespace

namespace base {

Process::Process(ProcessHandle handle) : process_(handle) {
}

Process::~Process() {
}

Process::Process(Process&& other) : process_(other.process_) {
  other.Close();
}

Process& Process::operator=(Process&& other) {
  DCHECK_NE(this, &other);
  process_ = other.process_;
  other.Close();
  return *this;
}

// static
Process Process::Current() {
  return Process(GetCurrentProcessHandle());
}

// static
Process Process::Open(ProcessId pid) {
  if (pid == GetCurrentProcId())
    return Current();

  // On POSIX process handles are the same as PIDs.
  return Process(pid);
}

// static
Process Process::OpenWithExtraPrivileges(ProcessId pid) {
  // On POSIX there are no privileges to set.
  return Open(pid);
}

// static
Process Process::DeprecatedGetProcessFromHandle(ProcessHandle handle) {
  DCHECK_NE(handle, GetCurrentProcessHandle());
  return Process(handle);
}

bool Process::IsValid() const {
  return process_ != kNullProcessHandle;
}

ProcessHandle Process::Handle() const {
  return process_;
}

Process Process::Duplicate() const {
  if (is_current())
    return Current();

  return Process(process_);
}

ProcessId Process::Pid() const {
  DCHECK(IsValid());
  return GetProcId(process_);
}

bool Process::is_current() const {
  return process_ == GetCurrentProcessHandle();
}

void Process::Close() {
  process_ = kNullProcessHandle;
  // if the process wasn't terminated (so we waited) or the state
  // wasn't already collected w/ a wait from process_utils, we're gonna
  // end up w/ a zombie when it does finally exit.
}

bool Process::Terminate(int exit_code, bool wait) const {
  (void) exit_code;
  // exit_code isn't supportable.
  DCHECK(IsValid());
  CHECK_GT(process_, 0);

  bool result = kill(process_, SIGTERM) == 0;
  if (result && wait) {
    int tries = 60;

    unsigned sleep_ms = 4;

    // The process may not end immediately due to pending I/O
    bool exited = false;
    while (tries-- > 0) {
      pid_t pid = HANDLE_EINTR(waitpid(process_, NULL, WNOHANG));
      if (pid == process_) {
        exited = true;
        break;
      }
      if (pid == -1) {
        if (errno == ECHILD) {
          // The wait may fail with ECHILD if another process also waited for
          // the same pid, causing the process state to get cleaned up.
          exited = true;
          break;
        }
        LOG(ERROR) << "Error waiting for process " << process_;
      }

      usleep(sleep_ms * 1000);
      const unsigned kMaxSleepMs = 1000;
      if (sleep_ms < kMaxSleepMs)
        sleep_ms *= 2;
    }

    // If we're waiting and the child hasn't died by now, force it
    // with a SIGKILL.
    if (!exited)
      result = kill(process_, SIGKILL) == 0;
  }

  if (!result)
    LOG(ERROR) << "Unable to terminate process " << process_;

  return result;
}

bool Process::WaitForExit(int* exit_code) {
  (void) exit_code;
  return WaitForExitWithTimeout(TimeDelta::Max(), exit_code);
}

bool Process::WaitForExitWithTimeout(TimeDelta timeout, int* exit_code) {
  return WaitForExitWithTimeoutImpl(Handle(), exit_code, timeout);
}

int Process::GetPriority() const {
  DCHECK(IsValid());
  return getpriority(PRIO_PROCESS, process_);
}

}  // namespace base
