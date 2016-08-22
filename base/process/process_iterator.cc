#include "base/process/process_iterator.h"

namespace base {

ProcessEntry::ProcessEntry() : pid_(0), ppid_(0), gid_(0) {}
ProcessEntry::ProcessEntry(const ProcessEntry& ) = default;
ProcessEntry::~ProcessEntry() {}

const ProcessEntry* ProcessIterator::NextProcessEntry() {
  bool result = false;
  do {
    result = CheckForNextProcess();
  } while (result && !IncludeEntry());
  if (result)
    return &entry_;
  return NULL;
}

ProcessIterator::ProcessEntries ProcessIterator::Snapshot() {
  ProcessEntries found;
  while (const ProcessEntry* process_entry = NextProcessEntry()) {
    found.push_back(*process_entry);
  }
  return found;
}

bool ProcessIterator::IncludeEntry() {
  return !filter_ || filter_->Includes(entry_);
}

NamedProcessIterator::NamedProcessIterator(
    const FilePath::StringType& executable_name,
    const ProcessFilter* filter) : ProcessIterator(filter),
                                   executable_name_(executable_name) {
}

NamedProcessIterator::~NamedProcessIterator() {
}

int GetProcessCount(const FilePath::StringType& executable_name,
                    const ProcessFilter* filter) {
  int count = 0;
  NamedProcessIterator iter(executable_name, filter);
  while (iter.NextProcessEntry())
    ++count;
  return count;
}

}  // namespace base
