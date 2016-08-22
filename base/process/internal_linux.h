#ifndef BASE_PROCESS_INTERNAL_LINUX_H_
#define BASE_PROCESS_INTERNAL_LINUX_H_
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

#include "base/file_path.h"

namespace base {

class Time;
class TimeDelta;

namespace internal {

extern const char kProcDir[];

extern const char kStatFile[];

base::FilePath GetProcPidDir(pid_t pid);
pid_t ProcDirSlotToPid(const char* d_name);

bool ReadProcStats(pid_t pid, std::string* buffer);

bool ParseProcStats(const std::string& stats_data,
                                        std::vector<std::string>* proc_stats);
                    
enum ProcStatsFields {
  VM_COMM           = 1,   // Filename of executable, without parentheses.
  VM_STATE          = 2,   // Letter indicating the state of the process.
  VM_PPID           = 3,   // PID of the parent.
  VM_PGRP           = 4,   // Process group id.
  VM_UTIME          = 13,  // Time scheduled in user mode in clock ticks.
  VM_STIME          = 14,  // Time scheduled in kernel mode in clock ticks.
  VM_NUMTHREADS     = 19,  // Number of threads.
  VM_STARTTIME      = 21,  // The time the process started in clock ticks.
  VM_VSIZE          = 22,  // Virtual memory size in bytes.
  VM_RSS            = 23,  // Resident Set Size in pages.
};

int64_t GetProcStatsFieldAsInt64(const std::vector<std::string>& proc_stats,
                                 ProcStatsFields field_num);
                                 
size_t GetProcStatsFieldAsSizeT(const std::vector<std::string>& proc_stats,
                                ProcStatsFields field_num);
                                
int64_t ReadProcStatsAndGetFieldAsInt64(pid_t pid, ProcStatsFields field_num);

size_t ReadProcStatsAndGetFieldAsSizeT(pid_t pid,
                                       ProcStatsFields field_num);

Time GetBootTime();

TimeDelta ClockTicksToTimeDelta(int clock_ticks);

} // namespace internal
} // namespace base

#endif // BASE_PROCESS_INTERNAL_LINUX_H_
