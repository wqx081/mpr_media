// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file contains functions for launching subprocesses.

#ifndef BASE_PROCESS_LAUNCH_H_
#define BASE_PROCESS_LAUNCH_H_

#include <stddef.h>

#include <string>
#include <utility>
#include <vector>

#include "base/environment.h"
#include "base/macros.h"
#include "base/process/process.h"
#include "base/process/process_handle.h"
#include "base/string_piece.h"

#include "base/file_descriptor_shuffle.h"

namespace base {

class CommandLine;

// TODO(viettrungluu): Only define this on POSIX?
typedef std::vector<std::pair<int, int> > FileHandleMappingVector;

// Options for launching a subprocess that are passed to LaunchProcess().
// The default constructor constructs the object with default options.
struct  LaunchOptions {
  // Delegate to be run in between fork and exec in the subprocess (see
  // pre_exec_delegate below)
  class  PreExecDelegate {
   public:
    PreExecDelegate() {}
    virtual ~PreExecDelegate() {}

    // Since this is to be run between fork and exec, and fork may have happened
    // while multiple threads were running, this function needs to be async
    // safe.
    virtual void RunAsyncSafe() = 0;

   private:
    DISALLOW_COPY_AND_ASSIGN(PreExecDelegate);
  };

  LaunchOptions();
  LaunchOptions(const LaunchOptions&);
  ~LaunchOptions();

  // If true, wait for the process to complete.
  bool wait = false;

  // If not empty, change to this directory before executing the new process.
  base::FilePath current_directory;

  // Set/unset environment variables. These are applied on top of the parent
  // process environment.  Empty (the default) means to inherit the same
  // environment. See AlterEnvironment().
  EnvironmentMap environ;

  // Clear the environment for the new process before processing changes from
  // |environ|.
  bool clear_environ = false;

  // If non-null, remap file descriptors according to the mapping of
  // src fd->dest fd to propagate FDs into the child process.
  // This pointer is owned by the caller and must live through the
  // call to LaunchProcess().
  const FileHandleMappingVector* fds_to_remap = nullptr;

  // Each element is an RLIMIT_* constant that should be raised to its
  // rlim_max.  This pointer is owned by the caller and must live through
  // the call to LaunchProcess().
  const std::vector<int>* maximize_rlimits = nullptr;

  // If true, start the process in a new process group, instead of
  // inheriting the parent's process group.  The pgid of the child process
  // will be the same as its pid.
  bool new_process_group = false;

  // If non-zero, start the process using clone(), using flags as provided.
  // Unlike in clone, clone_flags may not contain a custom termination signal
  // that is sent to the parent when the child dies. The termination signal will
  // always be set to SIGCHLD.
  int clone_flags = 0;

  // By default, child processes will have the PR_SET_NO_NEW_PRIVS bit set. If
  // true, then this bit will not be set in the new child process.
  bool allow_new_privs = false;

  // Sets parent process death signal to SIGKILL.
  bool kill_on_parent_death = false;

  // If non-null, a delegate to be run immediately prior to executing the new
  // program in the child process.
  //
  // WARNING: If LaunchProcess is called in the presence of multiple threads,
  // code running in this delegate essentially needs to be async-signal safe
  // (see man 7 signal for a list of allowed functions).
  PreExecDelegate* pre_exec_delegate = nullptr;

};

// Launch a process via the command line |cmdline|.
// See the documentation of LaunchOptions for details on |options|.
//
// Returns a valid Process upon success.
//
// Unix-specific notes:
// - All file descriptors open in the parent process will be closed in the
//   child process except for any preserved by options::fds_to_remap, and
//   stdin, stdout, and stderr. If not remapped by options::fds_to_remap,
//   stdin is reopened as /dev/null, and the child is allowed to inherit its
//   parent's stdout and stderr.
// - If the first argument on the command line does not contain a slash,
//   PATH will be searched.  (See man execvp.)
Process LaunchProcess(const CommandLine& cmdline,
                      const LaunchOptions& options);

// A POSIX-specific version of LaunchProcess that takes an argv array
// instead of a CommandLine.  Useful for situations where you need to
// control the command line arguments directly, but prefer the
// CommandLine version if launching Chrome itself.
Process LaunchProcess(const std::vector<std::string>& argv,
                      const LaunchOptions& options);

// Close all file descriptors, except those which are a destination in the
// given multimap. Only call this function in a child process where you know
// that there aren't any other threads.
void CloseSuperfluousFds(const InjectiveMultimap& saved_map);


// Executes the application specified by |cl| and wait for it to exit. Stores
// the output (stdout) in |output|. Redirects stderr to /dev/null. Returns true
// on success (application launched and exited cleanly, with exit code
// indicating success).
bool GetAppOutput(const CommandLine& cl, std::string* output);

// Like GetAppOutput, but also includes stderr.
bool GetAppOutputAndError(const CommandLine& cl,
                                      std::string* output);

// A POSIX-specific version of GetAppOutput that takes an argv array
// instead of a CommandLine.  Useful for situations where you need to
// control the command line arguments directly.
bool GetAppOutput(const std::vector<std::string>& argv,
                  std::string* output);

// A version of |GetAppOutput()| which also returns the exit code of the
// executed command. Returns true if the application runs and exits cleanly. If
// this is the case the exit code of the application is available in
// |*exit_code|.
bool GetAppOutputWithExitCode(const CommandLine& cl,
                              std::string* output, int* exit_code);

// If supported on the platform, and the user has sufficent rights, increase
// the current process's scheduling priority to a high priority.
void RaiseProcessToHighPriority();


// Creates a LaunchOptions object suitable for launching processes in a test
// binary. This should not be called in production/released code.
LaunchOptions LaunchOptionsForTest();

// A wrapper for clone with fork-like behavior, meaning that it returns the
// child's pid in the parent and 0 in the child. |flags|, |ptid|, and |ctid| are
// as in the clone system call (the CLONE_VM flag is not supported).
//
// This function uses the libc clone wrapper (which updates libc's pid cache)
// internally, so callers may expect things like getpid() to work correctly
// after in both the child and parent. An exception is when this code is run
// under Valgrind. Valgrind does not support the libc clone wrapper, so the libc
// pid cache may be incorrect after this function is called under Valgrind.
//
// As with fork(), callers should be extremely careful when calling this while
// multiple threads are running, since at the time the fork happened, the
// threads could have been in any state (potentially holding locks, etc.).
// Callers should most likely call execve() in the child soon after calling
// this.
pid_t ForkWithFlags(unsigned long flags, pid_t* ptid, pid_t* ctid);

}  // namespace base

#endif  // BASE_PROCESS_LAUNCH_H_
