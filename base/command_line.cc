#include "base/command_line.h"

#include <algorithm>
#include <ostream>

#include <glog/logging.h>

#include "base/file_path.h"
#include "base/macros.h"
#include "base/string_split.h"
#include "base/string_util.h"

namespace base {

CommandLine* CommandLine::current_process_commandline_ = NULL;

namespace {

const CommandLine::CharType kSwitchTerminator[] = FILE_PATH_LITERAL("--");
const CommandLine::CharType kSwitchValueSeparator[] = FILE_PATH_LITERAL("=");

// Since we use a lazy match, make sure that longer versions (like "--") are
// listed before shorter versions (like "-") of similar prefixes.
// Unixes don't use slash as a switch.
const CommandLine::CharType* const kSwitchPrefixes[] = {"--", "-"};
size_t switch_prefix_count = arraysize(kSwitchPrefixes);

size_t GetSwitchPrefixLength(const CommandLine::StringType& string) {
  for (size_t i = 0; i < switch_prefix_count; ++i) {
    CommandLine::StringType prefix(kSwitchPrefixes[i]);
    if (string.compare(0, prefix.length(), prefix) == 0)
      return prefix.length();
  }
  return 0;
}

// Fills in |switch_string| and |switch_value| if |string| is a switch.
// This will preserve the input switch prefix in the output |switch_string|.
bool IsSwitch(const CommandLine::StringType& string,
              CommandLine::StringType* switch_string,
              CommandLine::StringType* switch_value) {
  switch_string->clear();
  switch_value->clear();
  size_t prefix_length = GetSwitchPrefixLength(string);
  if (prefix_length == 0 || prefix_length == string.length())
    return false;

  const size_t equals_position = string.find(kSwitchValueSeparator);
  *switch_string = string.substr(0, equals_position);
  if (equals_position != CommandLine::StringType::npos)
    *switch_value = string.substr(equals_position + 1);
  return true;
}

// Append switches and arguments, keeping switches before arguments.
void AppendSwitchesAndArguments(CommandLine* command_line,
                                const CommandLine::StringVector& argv) {
  bool parse_switches = true;
  for (size_t i = 1; i < argv.size(); ++i) {
    CommandLine::StringType arg = argv[i];
    TrimWhitespaceASCII(arg, TRIM_ALL, &arg);

    CommandLine::StringType switch_string;
    CommandLine::StringType switch_value;
    parse_switches &= (arg != kSwitchTerminator);
    if (parse_switches && IsSwitch(arg, &switch_string, &switch_value)) {
      command_line->AppendSwitchNative(switch_string, switch_value);
    } else {
      command_line->AppendArgNative(arg);
    }
  }
}


}  // namespace

CommandLine::CommandLine(NoProgram no_program)
    : argv_(1),
      begin_args_(1) {
  (void) no_program;
}

CommandLine::CommandLine(const FilePath& program)
    : argv_(1),
      begin_args_(1) {
  SetProgram(program);
}

CommandLine::CommandLine(int argc, const CommandLine::CharType* const* argv)
    : argv_(1),
      begin_args_(1) {
  InitFromArgv(argc, argv);
}

CommandLine::CommandLine(const StringVector& argv)
    : argv_(1),
      begin_args_(1) {
  InitFromArgv(argv);
}

CommandLine::CommandLine(const CommandLine& other)
    : argv_(other.argv_),
      switches_(other.switches_),
      begin_args_(other.begin_args_) {
  ResetStringPieces();
}

CommandLine& CommandLine::operator=(const CommandLine& other) {
  argv_ = other.argv_;
  switches_ = other.switches_;
  begin_args_ = other.begin_args_;
  ResetStringPieces();
  return *this;
}

CommandLine::~CommandLine() {
}

// static
bool CommandLine::Init(int argc, const char* const* argv) {
  if (current_process_commandline_) {
    // If this is intentional, Reset() must be called first. If we are using
    // the shared build mode, we have to share a single object across multiple
    // shared libraries.
    return false;
  }

  current_process_commandline_ = new CommandLine(NO_PROGRAM);
  current_process_commandline_->InitFromArgv(argc, argv);

  return true;
}

// static
void CommandLine::Reset() {
  DCHECK(current_process_commandline_);
  delete current_process_commandline_;
  current_process_commandline_ = NULL;
}

// static
CommandLine* CommandLine::ForCurrentProcess() {
  DCHECK(current_process_commandline_);
  return current_process_commandline_;
}

// static
bool CommandLine::InitializedForCurrentProcess() {
  return !!current_process_commandline_;
}

void CommandLine::InitFromArgv(int argc,
                               const CommandLine::CharType* const* argv) {
  StringVector new_argv;
  for (int i = 0; i < argc; ++i)
    new_argv.push_back(argv[i]);
  InitFromArgv(new_argv);
}

void CommandLine::InitFromArgv(const StringVector& argv) {
  argv_ = StringVector(1);
  switches_.clear();
  switches_by_stringpiece_.clear();
  begin_args_ = 1;
  SetProgram(argv.empty() ? FilePath() : FilePath(argv[0]));
  AppendSwitchesAndArguments(this, argv);
}

FilePath CommandLine::GetProgram() const {
  return FilePath(argv_[0]);
}

void CommandLine::SetProgram(const FilePath& program) {
  TrimWhitespaceASCII(program.value(), TRIM_ALL, &argv_[0]);
}

bool CommandLine::HasSwitch(const base::StringPiece& switch_string) const {
  DCHECK_EQ(ToLowerASCII(switch_string), switch_string);
  return switches_by_stringpiece_.find(switch_string) !=
         switches_by_stringpiece_.end();
}

bool CommandLine::HasSwitch(const char switch_constant[]) const {
  return HasSwitch(base::StringPiece(switch_constant));
}

std::string CommandLine::GetSwitchValueASCII(
    const base::StringPiece& switch_string) const {
  StringType value = GetSwitchValueNative(switch_string);
#if 0
  if (!IsStringASCII(value)) {
    DLOG(WARNING) << "Value of switch (" << switch_string << ") must be ASCII.";
    return std::string();
  }
#endif
  return value;
}

FilePath CommandLine::GetSwitchValuePath(
    const base::StringPiece& switch_string) const {
  return FilePath(GetSwitchValueNative(switch_string));
}

CommandLine::StringType CommandLine::GetSwitchValueNative(
    const base::StringPiece& switch_string) const {
  DCHECK_EQ(ToLowerASCII(switch_string), switch_string);
  auto result = switches_by_stringpiece_.find(switch_string);
  return result == switches_by_stringpiece_.end() ? StringType()
                                                  : *(result->second);
}

void CommandLine::AppendSwitch(const std::string& switch_string) {
  AppendSwitchNative(switch_string, StringType());
}

void CommandLine::AppendSwitchPath(const std::string& switch_string,
                                   const FilePath& path) {
  AppendSwitchNative(switch_string, path.value());
}

void CommandLine::AppendSwitchNative(const std::string& switch_string,
                                     const CommandLine::StringType& value) {
  const std::string& switch_key = switch_string;
  StringType combined_switch_string(switch_key);
  size_t prefix_length = GetSwitchPrefixLength(combined_switch_string);
  auto insertion =
      switches_.insert(make_pair(switch_key.substr(prefix_length), value));
  if (!insertion.second)
    insertion.first->second = value;
  switches_by_stringpiece_[insertion.first->first] = &(insertion.first->second);
  // Preserve existing switch prefixes in |argv_|; only append one if necessary.
  if (prefix_length == 0)
    combined_switch_string = kSwitchPrefixes[0] + combined_switch_string;
  if (!value.empty())
    combined_switch_string += kSwitchValueSeparator + value;
  // Append the switch and update the switches/arguments divider |begin_args_|.
  argv_.insert(argv_.begin() + begin_args_++, combined_switch_string);
}

void CommandLine::AppendSwitchASCII(const std::string& switch_string,
                                    const std::string& value_string) {
  AppendSwitchNative(switch_string, value_string);
}

void CommandLine::CopySwitchesFrom(const CommandLine& source,
                                   const char* const switches[],
                                   size_t count) {
  for (size_t i = 0; i < count; ++i) {
    if (source.HasSwitch(switches[i]))
      AppendSwitchNative(switches[i], source.GetSwitchValueNative(switches[i]));
  }
}

CommandLine::StringVector CommandLine::GetArgs() const {
  // Gather all arguments after the last switch (may include kSwitchTerminator).
  StringVector args(argv_.begin() + begin_args_, argv_.end());
  // Erase only the first kSwitchTerminator (maybe "--" is a legitimate page?)
  StringVector::iterator switch_terminator =
      std::find(args.begin(), args.end(), kSwitchTerminator);
  if (switch_terminator != args.end())
    args.erase(switch_terminator);
  return args;
}

void CommandLine::AppendArg(const std::string& value) {
  AppendArgNative(value);
}

void CommandLine::AppendArgPath(const FilePath& path) {
  AppendArgNative(path.value());
}

void CommandLine::AppendArgNative(const CommandLine::StringType& value) {
  argv_.push_back(value);
}

void CommandLine::AppendArguments(const CommandLine& other,
                                  bool include_program) {
  if (include_program)
    SetProgram(other.GetProgram());
  AppendSwitchesAndArguments(this, other.argv());
}

void CommandLine::PrependWrapper(const CommandLine::StringType& wrapper) {
  if (wrapper.empty())
    return;
  // The wrapper may have embedded arguments (like "gdb --args"). In this case,
  // we don't pretend to do anything fancy, we just split on spaces.
  StringVector wrapper_argv = SplitString(
      wrapper, FilePath::StringType(1, ' '), base::TRIM_WHITESPACE,
      base::SPLIT_WANT_ALL);
  // Prepend the wrapper and update the switches/arguments |begin_args_|.
  argv_.insert(argv_.begin(), wrapper_argv.begin(), wrapper_argv.end());
  begin_args_ += wrapper_argv.size();
}

CommandLine::StringType CommandLine::GetCommandLineStringInternal(
    bool quote_placeholders) const {
  StringType string(argv_[0]);
  StringType params(GetArgumentsStringInternal(quote_placeholders));
  if (!params.empty()) {
    string.append(StringType(FILE_PATH_LITERAL(" ")));
    string.append(params);
  }
  return string;
}

CommandLine::StringType CommandLine::GetArgumentsStringInternal(
    bool quote_placeholders) const {
  (void) quote_placeholders;
  StringType params;
  // Append switches and arguments.
  bool parse_switches = true;
  for (size_t i = 1; i < argv_.size(); ++i) {
    StringType arg = argv_[i];
    StringType switch_string;
    StringType switch_value;
    parse_switches &= arg != kSwitchTerminator;
    if (i > 1)
      params.append(StringType(FILE_PATH_LITERAL(" ")));
    if (parse_switches && IsSwitch(arg, &switch_string, &switch_value)) {
      params.append(switch_string);
      if (!switch_value.empty()) {
        params.append(kSwitchValueSeparator + switch_value);
      }
    } else {
      params.append(arg);
    }
  }
  return params;
}

void CommandLine::ResetStringPieces() {
  switches_by_stringpiece_.clear();
  for (const auto& entry : switches_)
    switches_by_stringpiece_[entry.first] = &(entry.second);
}

}  // namespace base
