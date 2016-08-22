// This file contains methods to iterate over processes on the system.

#ifndef BASE_PROCESS_PROCESS_ITERATOR_H_
#define BASE_PROCESS_PROCESS_ITERATOR_H_

#include <stddef.h>

#include <list>
#include <string>
#include <vector>

#include "base/file_path.h"
#include "base/macros.h"
#include "base/process/process.h"

#include <dirent.h>

namespace base {

struct ProcessEntry {
  ProcessEntry();
  ProcessEntry(const ProcessEntry& other);
  ~ProcessEntry();

  ProcessId pid() const { return pid_; }
  ProcessId parent_pid() const { return ppid_; }
  ProcessId gid() const { return gid_; }
  const char* exe_file() const { return exe_file_.c_str(); }
  const std::vector<std::string>& cmd_line_args() const {
    return cmd_line_args_;
  }

  ProcessId pid_;
  ProcessId ppid_;
  ProcessId gid_;
  std::string exe_file_;
  std::vector<std::string> cmd_line_args_;
};

// Used to filter processes by process ID.
class ProcessFilter {
 public:
  // Returns true to indicate set-inclusion and false otherwise.  This method
  // should not have side-effects and should be idempotent.
  virtual bool Includes(const ProcessEntry& entry) const = 0;

 protected:
  virtual ~ProcessFilter() {}
};

// This class provides a way to iterate through a list of processes on the
// current machine with a specified filter.
// To use, create an instance and then call NextProcessEntry() until it returns
// false.
class ProcessIterator {
 public:
  typedef std::list<ProcessEntry> ProcessEntries;

  explicit ProcessIterator(const ProcessFilter* filter);
  virtual ~ProcessIterator();

  // If there's another process that matches the given executable name,
  // returns a const pointer to the corresponding PROCESSENTRY32.
  // If there are no more matching processes, returns NULL.
  // The returned pointer will remain valid until NextProcessEntry()
  // is called again or this NamedProcessIterator goes out of scope.
  const ProcessEntry* NextProcessEntry();

  // Takes a snapshot of all the ProcessEntry found.
  ProcessEntries Snapshot();

 protected:
  virtual bool IncludeEntry();
  const ProcessEntry& entry() { return entry_; }

 private:
  // Determines whether there's another process (regardless of executable)
  // left in the list of all processes.  Returns true and sets entry_ to
  // that process's info if there is one, false otherwise.
  bool CheckForNextProcess();

  // Initializes a PROCESSENTRY32 data structure so that it's ready for
  // use with Process32First/Process32Next.
  void InitProcessEntry(ProcessEntry* entry);

  DIR* procfs_dir_;
  ProcessEntry entry_;
  const ProcessFilter* filter_;

  DISALLOW_COPY_AND_ASSIGN(ProcessIterator);
};

// This class provides a way to iterate through the list of processes
// on the current machine that were started from the given executable
// name.  To use, create an instance and then call NextProcessEntry()
// until it returns false.
class NamedProcessIterator : public ProcessIterator {
 public:
  NamedProcessIterator(const FilePath::StringType& executable_name,
                       const ProcessFilter* filter);
  ~NamedProcessIterator() override;

 protected:
  bool IncludeEntry() override;

 private:
  FilePath::StringType executable_name_;

  DISALLOW_COPY_AND_ASSIGN(NamedProcessIterator);
};

// Returns the number of processes on the machine that are running from the
// given executable name.  If filter is non-null, then only processes selected
// by the filter will be counted.
int GetProcessCount(const FilePath::StringType& executable_name,
                                const ProcessFilter* filter);

}  // namespace base

#endif  // BASE_PROCESS_PROCESS_ITERATOR_H_
