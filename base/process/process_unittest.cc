#include "base/command_line.h"
#include "base/file_path.h"
#include "base/file_util.h"
#include "base/scoped_file.h"

#include "base/process/kill.h"
#include "base/process/launch.h"
#include "base/process/process.h"

#include "base/command_line.h"

#include "base/eintr_wrapper.h"
#include "base/test/multiprocess_func_list.h"

#include <gtest/gtest.h>
#include <glog/logging.h>

#include <malloc.h>
#include <sched.h>
#include <sys/syscall.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <sched.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <thread>
#include <chrono>

using base::FilePath;

namespace {

const char kSignalFileSlow[] = "SlowChildProcess.die";
const char kSignalFileKill[] = "KilledChildProcess.die";

const char kSignalFileTerm[] = "TerminatedChildProcess.die";

const char kShellPath[] = "/bin/sh";
const char kPosixShell[] = "sh";

const int kExpectedStillRunningExitCode = 0;

void WaitToDie(const char* filename) {
  FILE* fp;
  do {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    fp = fopen(filename, "r");
  } while (!fp);
  fclose(fp);
}

void SignalChildren(const char* filename) {
  FILE* fp = fopen(filename, "w");
  fclose(fp);
}

base::TerminationStatus WaitForChildTermination(base::ProcessHandle handle,
                                                int* exit_code) {
  base::TerminationStatus status = base::TERMINATION_STATUS_STILL_RUNNING;
  const base::TimeDelta kInterval = base::TimeDelta::FromMilliseconds(20);
  base::TimeDelta waited;
  do {
    status = base::GetTerminationStatus(handle, exit_code);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    waited += kInterval;
  } while (status == base::TERMINATION_STATUS_STILL_RUNNING &&
                      waited < base::TimeDelta::FromMilliseconds(6000)) ; //TestTimeouts::action_max_timeout());

  return status;
}

} // namespace

using namespace base;

Process SpawnMultiProcessTestChild(
    const std::string& procname,
    const CommandLine& command_line,
    const LaunchOptions& options);

CommandLine GetMultiProcessTestChildBaseCommandLine();

class MultiProcessTest : public testing::Test {

 public:
  MultiProcessTest();

 protected:
  Process SpawnChild(const std::string& procname);

  Process SpawnChildWithOptions(const std::string& procname,
                                const LaunchOptions& options);

  virtual CommandLine MakeCmdLine(const std::string& procname);

 private:
  DISALLOW_COPY_AND_ASSIGN(MultiProcessTest);

};

const char kTestChildProcess[] = "test-child-process";

Process SpawnMultiProcessTestChild(
    const std::string& procname,
    const CommandLine& base_command_line,
    const LaunchOptions& options) {
  CommandLine command_line(base_command_line);
  if (!command_line.HasSwitch(kTestChildProcess))
    command_line.AppendSwitchASCII(kTestChildProcess, procname);
    
  return LaunchProcess(command_line, options);
} 

CommandLine GetMultiProcessTestChildBaseCommandLine() { 
  CommandLine::Init(0, nullptr);
  CommandLine cmd_line = *CommandLine::ForCurrentProcess();
  cmd_line.SetProgram(MakeAbsoluteFilePath(cmd_line.GetProgram()));
  return cmd_line;
}


MultiProcessTest::MultiProcessTest() { }

Process MultiProcessTest::SpawnChild(const std::string& procname) { 
  LaunchOptions options;
  return SpawnChildWithOptions(procname, options);
}

Process MultiProcessTest::SpawnChildWithOptions(
    const std::string& procname,
    const LaunchOptions& options) { 
  return SpawnMultiProcessTestChild(procname, MakeCmdLine(procname), options);
}

CommandLine MultiProcessTest::MakeCmdLine(const std::string& procname) { 
  CommandLine command_line = GetMultiProcessTestChildBaseCommandLine();
  command_line.AppendSwitchASCII(kTestChildProcess, procname);
  return command_line;
}

const int kSuccess = 0;

class ProcessUtilTest : public MultiProcessTest { 
 public:
  int CountOpenFDsInChild();
  static std::string GetSignalFilePath(const char* filename);
};

std::string ProcessUtilTest::GetSignalFilePath(const char* filename) { 
  return filename;
}

MULTIPROCESS_TEST_MAIN(SimpleChildProcess) { 
  return kSuccess;
}

TEST_F(ProcessUtilTest, SpawnChild) { 
  base::Process process = SpawnChild("SimpleChildProcess");
  ASSERT_TRUE(process.IsValid());
  int exit_code;
  EXPECT_TRUE(process.WaitForExitWithTimeout(
              base::TimeDelta::FromMilliseconds(60000), &exit_code));
}

MULTIPROCESS_TEST_MAIN(SlowChildProcess) { 
  WaitToDie(ProcessUtilTest::GetSignalFilePath(kSignalFileSlow).c_str());
  return kSuccess;
}


////////////////////////
namespace { 

std::string TestLaunchProcess(const std::vector<std::string>& args,
                              const base::EnvironmentMap& env_changes,
                              const bool clear_environ,
                              const int clone_flags) { 
  base::FileHandleMappingVector fds_to_remap;
  int fds[2];
  CHECK(pipe(fds) == 0);
  fds_to_remap.push_back(std::make_pair(fds[1], 1));
  base::LaunchOptions options;
  options.wait = true;
  options.environ = env_changes;
  options.clear_environ = clear_environ;
  options.fds_to_remap = &fds_to_remap;
  options.clone_flags = clone_flags;
  EXPECT_TRUE(base::LaunchProcess(args, options).IsValid());
  CHECK(IGNORE_EINTR(close(fds[1])) == 0);
  char buf[512];
  const ssize_t n = HANDLE_EINTR(read(fds[0], buf, sizeof(buf)));
  CHECK(IGNORE_EINTR(close(fds[0])) == 0);
  return std::string(buf, n);

 }

const char kLargeString[] = 
"0123456789012345678901234567890123456789012345678901234567890123456789"
"0123456789012345678901234567890123456789012345678901234567890123456789"
"0123456789012345678901234567890123456789012345678901234567890123456789"
"0123456789012345678901234567890123456789012345678901234567890123456789"
"0123456789012345678901234567890123456789012345678901234567890123456789"
"0123456789012345678901234567890123456789012345678901234567890123456789"
"0123456789012345678901234567890123456789012345678901234567890123456789";

}  // namespace

TEST_F(ProcessUtilTest, LaunchProcess) { 
  base::EnvironmentMap env_changes;
  std::vector<std::string> echo_base_test;
  echo_base_test.push_back(kPosixShell);
  echo_base_test.push_back("-c");
  echo_base_test.push_back("echo $BASE_TEST");

  std::vector<std::string> print_env;
  print_env.push_back("/usr/bin/env");
  const int no_clone_flags = 0;
  const bool no_clear_environ = false;
  const char kBaseTest[] = "BASE_TEST";

  env_changes[kBaseTest] = "bar";
  EXPECT_EQ("bar\n",
  TestLaunchProcess(
    echo_base_test, env_changes, no_clear_environ, no_clone_flags));
  env_changes.clear();

  EXPECT_EQ(0, setenv(kBaseTest, "testing", 1 /* override */));
  EXPECT_EQ("testing\n",
  TestLaunchProcess(
                echo_base_test, env_changes, no_clear_environ, no_clone_flags));

  env_changes[kBaseTest] = std::string();
  EXPECT_EQ("\n",
    TestLaunchProcess(
     echo_base_test, env_changes, no_clear_environ, no_clone_flags));

  env_changes[kBaseTest] = "foo";
  EXPECT_EQ("foo\n",
           TestLaunchProcess(
                echo_base_test, env_changes, no_clear_environ, no_clone_flags));

  env_changes.clear();
  EXPECT_EQ(0, setenv(kBaseTest, kLargeString, 1 /* override */));
  EXPECT_EQ(std::string(kLargeString) + "\n",
  TestLaunchProcess(
    echo_base_test, env_changes, no_clear_environ, no_clone_flags));

  env_changes[kBaseTest] = "wibble";
  EXPECT_EQ("wibble\n",
  TestLaunchProcess(
  echo_base_test, env_changes, no_clear_environ, no_clone_flags));
  //if (!RunningOnValgrind()) { 
    EXPECT_EQ("wibble\n", TestLaunchProcess(echo_base_test, env_changes,
    no_clear_environ, CLONE_FS));
  //} 

  EXPECT_EQ(
      "BASE_TEST=wibble\n",
      TestLaunchProcess(
          print_env, env_changes, true /* clear_environ */, no_clone_flags));
  env_changes.clear();
  EXPECT_EQ(
      "",
      TestLaunchProcess(
          print_env, env_changes, true /* clear_environ */, no_clone_flags));
}

TEST_F(ProcessUtilTest, GetAppOutput) { 
  std::string output;
  EXPECT_TRUE(base::GetAppOutput(base::CommandLine(FilePath("true")),
                                 &output));
  EXPECT_STREQ("", output.c_str());
  EXPECT_FALSE(base::GetAppOutput(base::CommandLine(FilePath("false")),
               &output));

  std::vector<std::string> argv;
  argv.push_back("/bin/echo");
  argv.push_back("-n");
  argv.push_back("foobar42");
  EXPECT_TRUE(base::GetAppOutput(base::CommandLine(argv), &output));
  EXPECT_STREQ("foobar42", output.c_str());
}

TEST_F(ProcessUtilTest, GetAppOutputWithExitCode) {
  std::vector<std::string> argv;
  std::string output;
  int exit_code;
  argv.push_back(std::string(kShellPath));  // argv[0]
  argv.push_back("-c");  // argv[1]
  argv.push_back("echo foo");  // argv[2];
  EXPECT_TRUE(base::GetAppOutputWithExitCode(base::CommandLine(argv), &output,
                                             &exit_code));
  EXPECT_STREQ("foo\n", output.c_str());
  EXPECT_EQ(exit_code, kSuccess);

  output.clear();
  argv[2] = "echo foo; exit 2";
  EXPECT_TRUE(base::GetAppOutputWithExitCode(base::CommandLine(argv), &output,
                                             &exit_code));
  EXPECT_STREQ("foo\n", output.c_str());
  EXPECT_EQ(exit_code, 2);
}

TEST_F(ProcessUtilTest, GetParentProcessId) {
  base::ProcessId ppid =
      base::GetParentProcessId(base::GetCurrentProcessHandle());
  EXPECT_EQ(ppid, getppid());
}

//ffmpeg -y -i /tmp/test.mp4 -codec:v libx264 -codec:a libfaac out.flv
TEST_F(ProcessUtilTest, FFmpeg) {
  std::string output;
  std::vector<std::string> argv;
  argv.push_back("/tmp/FFmpeg");
  argv.push_back("-loglevel");
  argv.push_back("panic");
  argv.push_back("-y");
  argv.push_back("-i");
  argv.push_back("/tmp/test.mp4");
  argv.push_back("-codec:v");
  argv.push_back("libx264");
  argv.push_back("-codec:a");
  argv.push_back("libfaac");
  argv.push_back("/tmp/out.flv");

  base::FileHandleMappingVector fds_to_remap;

  base::EnvironmentMap env_changes;

//  int fds[2];
//  DCHECK(pipe(fds) == 0);

//  fds_to_remap.push_back(std::make_pair(fds[1], 1));
  base::LaunchOptions options;
//  options.wait = true;
  options.clone_flags = 0;
  options.environ = env_changes;
  options.fds_to_remap = &fds_to_remap;
  options.clone_flags = 0;
  // LauchProcess
  base::Process process = base::LaunchProcess(argv, options);
  EXPECT_TRUE(process.IsValid());
  int child_exit_code = 0;
  CHECK(process.WaitForExit(&child_exit_code));
  LOG(INFO) << "--------child_exit_code: " << child_exit_code;

#if 0
  PCHECK(IGNORE_EINTR(close(fds[1])) == 0);

  char buf[10];
  ssize_t n = 0;
  do {
    n = HANDLE_EINTR(read(fds[0], buf, sizeof(buf)));
    LOG(INFO) << "get from ffmpeg: " << std::string(buf, n);
  } while (n != -1 && n != 0);

  PCHECK(IGNORE_EINTR(close(fds[0])) == 0);
#endif
}
