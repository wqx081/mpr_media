#include "base/process/internal_linux.h"

#include <limits.h>
#include <unistd.h>

#include <map>
#include <string>
#include <vector>

#include <glog/logging.h>

#include "base/file_util.h"
#include "base/numbers.h"

#include "base/string_split.h"
#include "base/string_util.h"
#include "base/time.h"


namespace base {

namespace internal {

const char kProcDir[] = "/proc";

const char kStatFile[] = "stat";

FilePath GetProcPidDir(pid_t pid) {
  return FilePath(kProcDir).Append(std::to_string(pid));
}

pid_t ProcDirSlotToPid(const char* d_name) {
  int i;
  for (i = 0; i < NAME_MAX && d_name[i]; ++i) {
    if (!IsAsciiDigit(d_name[i])) {
      return 0;
    }
  }
  if (i == NAME_MAX)
    return 0;

  pid_t pid;
  std::string pid_string(d_name);
  //TODO
  if (!base::StringAsValue(pid_string, &pid)) {
    //NOTREACHED();
    DCHECK(false);
    return 0;
  }
  return pid;
}

bool ReadProcFile(const FilePath& file, std::string* buffer) {
  buffer->clear();
//TODO
//  ThreadRestrictions::ScopedAllowIO allow_io;
  
  if (!ReadFileToString(file, buffer)) {
    DLOG(WARNING) << "Failed to read " << file.value(); // << file.MaybeAsASCII();
    return false; 
  } 
  return !buffer->empty();
} 

bool ReadProcStats(pid_t pid, std::string* buffer) {
  FilePath stat_file = internal::GetProcPidDir(pid).Append(kStatFile);
  return ReadProcFile(stat_file, buffer);
} 

bool ParseProcStats(const std::string& stats_data,
                    std::vector<std::string>* proc_stats) {
  if (stats_data.empty())
    return false;
    
    
  size_t open_parens_idx = stats_data.find(" (");
  size_t close_parens_idx = stats_data.rfind(") ");
  if (open_parens_idx == std::string::npos ||
            close_parens_idx == std::string::npos ||
            open_parens_idx > close_parens_idx) { 
    DLOG(WARNING) << "Failed to find matched parens in '" << stats_data << "'";
    //NOTREACHED(); 
    DCHECK(false);
    return false; 
  } 
  open_parens_idx++;
  
  proc_stats->clear();
  proc_stats->push_back(stats_data.substr(0, open_parens_idx));
  proc_stats->push_back(
      stats_data.substr(open_parens_idx + 1,
                        close_parens_idx - (open_parens_idx + 1)));
                        
  std::vector<std::string> other_stats = SplitString(
      stats_data.substr(close_parens_idx + 2), " ",
      base::TRIM_WHITESPACE, base::SPLIT_WANT_ALL);
  for (size_t i = 0; i < other_stats.size(); ++i)
    proc_stats->push_back(other_stats[i]);
  return true;
} 

typedef std::map<std::string, std::string> ProcStatMap;
void ParseProcStat(const std::string& contents, ProcStatMap* output) {
  StringPairs key_value_pairs;
  SplitStringIntoKeyValuePairs(contents, ' ', '\n', &key_value_pairs);
  for (size_t i = 0; i < key_value_pairs.size(); ++i) {
    output->insert(key_value_pairs[i]);
  } 
} 

int64_t GetProcStatsFieldAsInt64(const std::vector<std::string>& proc_stats,
                                 ProcStatsFields field_num) {
  DCHECK_GE(field_num, VM_PPID); 
  CHECK_LT(static_cast<size_t>(field_num), proc_stats.size());
  
  int64_t value;
  return StringAsValue(proc_stats[field_num], &value) ? value : 0;
} 

size_t GetProcStatsFieldAsSizeT(const std::vector<std::string>& proc_stats,
                                ProcStatsFields field_num) {
  DCHECK_GE(field_num, VM_PPID);
  CHECK_LT(static_cast<size_t>(field_num), proc_stats.size());
  
  size_t value;
  return StringAsValue(proc_stats[field_num], &value) ? value : 0;
}

int64_t ReadProcStatsAndGetFieldAsInt64(pid_t pid, ProcStatsFields field_num) {
  std::string stats_data;
  if (!ReadProcStats(pid, &stats_data))
    return 0;
  std::vector<std::string> proc_stats;
  if (!ParseProcStats(stats_data, &proc_stats))
    return 0;
  return GetProcStatsFieldAsInt64(proc_stats, field_num);
}

size_t ReadProcStatsAndGetFieldAsSizeT(pid_t pid,
                                       ProcStatsFields field_num) {
  std::string stats_data;
  if (!ReadProcStats(pid, &stats_data))
    return 0;
  std::vector<std::string> proc_stats;
  if (!ParseProcStats(stats_data, &proc_stats))
    return 0;
  return GetProcStatsFieldAsSizeT(proc_stats, field_num);
}

Time GetBootTime() {
  FilePath path("/proc/stat");
  std::string contents;
  if (!ReadProcFile(path, &contents))
    return Time();
  ProcStatMap proc_stat;
  ParseProcStat(contents, &proc_stat);
  ProcStatMap::const_iterator btime_it = proc_stat.find("btime");
  if (btime_it == proc_stat.end())
    return Time();
  int btime;
  if (!StringAsValue(btime_it->second, &btime))
    return Time();
  return Time::FromTimeT(btime);
}

TimeDelta ClockTicksToTimeDelta(int clock_ticks) {
  static const int kHertz = sysconf(_SC_CLK_TCK);
  
  return TimeDelta::FromMicroseconds(
      Time::kMicrosecondsPerSecond * clock_ticks / kHertz);
}     

} // namespace internal
} // namespace base
