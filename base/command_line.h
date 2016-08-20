#ifndef BASE_COMMAND_LINE_H_
#define BASE_COMMAND_LINE_H_
#include <stddef.h>
#include <map>
#include <string>
#include <vector>

#include "base/string_piece.h"

namespace base {

class FilePath;

class CommandLine {
 public:
  using StringType = std::string;
  using CharType = StringType::value_type;
  using StringVector = std::vector<StringType>;
  using SwitchMap = std::map<std::string, StringType>;
  using StringPieceSwitchMap = std::map<StringPiece, const StringType*>;

  enum NoProgram { NO_PROGRAM };
  explicit CommandLine(NoProgram no_program);
  explicit CommandLine(const FilePath& program);
  CommandLine(int argc, const CharType* const* argv);
  explicit CommandLine(const StringVector& argv);

  CommandLine(const CommandLine& other);
  CommandLine& operator=(const CommandLine& other);

  ~CommandLine();

  static bool Init(int argc, const char* const* argv);
  static void Reset();
  static CommandLine* ForCurrentProcess();
  static bool InitializedForCurrentProcess();
  
  void InitFromArgv(int argc, const CharType* const* argv);
  void InitFromArgv(const StringVector& argv);

  StringType GetCommandLineString() const {
    return GetCommandLineStringInternal(false);
  }

  StringType GetArgumentsString() const {
    return GetArgumentsStringInternal(false);
  }

  const StringVector& argv() const { return argv_; }

  FilePath GetProgram() const;
  void SetProgram(const FilePath& program);

  bool HasSwitch(const StringPiece& switch_string) const;
  bool HasSwitch(const char switch_constant[]) const;

  std::string GetSwitchValueASCII(const StringPiece& switch_string) const;
  FilePath GetSwitchValuePath(const StringPiece& switch_string) const;
  StringType GetSwitchValueNative(const StringPiece& switch_string) const;

  const SwitchMap& GetSwitches() const { return switches_; }


  void AppendSwitch(const std::string& switch_string);
  void AppendSwitchPath(const std::string& switch_string,
                        const FilePath& path);
  void AppendSwitchNative(const std::string& switch_string,
                          const StringType& value);
  void AppendSwitchASCII(const std::string& switch_string,
                         const std::string& value);

  void CopySwitchesFrom(const CommandLine& source,
                        const char* const switches[],
                        size_t count);

  StringVector GetArgs() const;

  void AppendArg(const std::string& value);
  void AppendArgPath(const FilePath& value);
  void AppendArgNative(const StringType& value);
  void AppendArguments(const CommandLine& other, bool include_program);
  void PrependWrapper(const StringType& wrapper);


 private:
  CommandLine();
  StringType GetCommandLineStringInternal(bool quote_placeholders) const;
  StringType GetArgumentsStringInternal(bool quote_placeholders) const;
  void ResetStringPieces();
  static CommandLine* current_process_commandline_;
  StringVector argv_;
  SwitchMap switches_;
  StringPieceSwitchMap switches_by_stringpiece_;
  size_t begin_args_;
};

} // namespace base
#endif // BASE_COMMAND_LINE_H_
