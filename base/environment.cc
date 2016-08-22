#include "base/environment.h"

#include <stddef.h>

#include <vector>

#if 0
#include "base/memory/ptr_util.h"
#include "base/strings/string_piece.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "build/build_config.h"
#endif

#include "base/ptr_util.h"
#include "base/string_util.h"

#include <stdlib.h>

namespace base {

namespace {

class EnvironmentImpl : public Environment {
 public:
  bool GetVar(StringPiece variable_name, std::string* result) override {
    if (GetVarImpl(variable_name, result))
      return true;

    // Some commonly used variable names are uppercase while others
    // are lowercase, which is inconsistent. Let's try to be helpful
    // and look for a variable name with the reverse case.
    // I.e. HTTP_PROXY may be http_proxy for some users/systems.
    char first_char = variable_name[0];
    std::string alternate_case_var;
    if (IsAsciiLower(first_char))
      alternate_case_var = ToUpperASCII(variable_name);
    else if (IsAsciiUpper(first_char))
      alternate_case_var = ToLowerASCII(variable_name);
    else
      return false;
    return GetVarImpl(alternate_case_var.c_str(), result);
  }

  bool SetVar(StringPiece variable_name,
              const std::string& new_value) override {
    return SetVarImpl(variable_name, new_value);
  }

  bool UnSetVar(StringPiece variable_name) override {
    return UnSetVarImpl(variable_name);
  }

 private:
  bool GetVarImpl(StringPiece variable_name, std::string* result) {
    const char* env_value = getenv(variable_name.data());
    if (!env_value)
      return false;
    // Note that the variable may be defined but empty.
    if (result)
      *result = env_value;
    return true;
  }

  bool SetVarImpl(StringPiece variable_name, const std::string& new_value) {
    return !setenv(variable_name.data(), new_value.c_str(), 1);
  }

  bool UnSetVarImpl(StringPiece variable_name) {
    return !unsetenv(variable_name.data());
  }
};

// Parses a null-terminated input string of an environment block. The key is
// placed into the given string, and the total length of the line, including
// the terminating null, is returned.
size_t ParseEnvLine(const NativeEnvironmentString::value_type* input,
                    NativeEnvironmentString* key) {
  // Skip to the equals or end of the string, this is the key.
  size_t cur = 0;
  while (input[cur] && input[cur] != '=')
    cur++;
  *key = NativeEnvironmentString(&input[0], cur);

  // Now just skip to the end of the string.
  while (input[cur])
    cur++;
  return cur + 1;
}

}  // namespace

namespace env_vars {

// On Posix systems, this variable contains the location of the user's home
// directory. (e.g, /home/username/).
const char kHome[] = "HOME";

}  // namespace env_vars

Environment::~Environment() {}

// static
std::unique_ptr<Environment> Environment::Create() {
  return MakeUnique<EnvironmentImpl>();
}

bool Environment::HasVar(StringPiece variable_name) {
  return GetVar(variable_name, nullptr);
}

std::unique_ptr<char* []> AlterEnvironment(const char* const* const env,
                                           const EnvironmentMap& changes) {
  std::string value_storage;  // Holds concatenated null-terminated strings.
  std::vector<size_t> result_indices;  // Line indices into value_storage.

  // First build up all of the unchanged environment strings. These are
  // null-terminated of the form "key=value".
  std::string key;
  for (size_t i = 0; env[i]; i++) {
    size_t line_length = ParseEnvLine(env[i], &key);

    // Keep only values not specified in the change vector.
    EnvironmentMap::const_iterator found_change = changes.find(key);
    if (found_change == changes.end()) {
      result_indices.push_back(value_storage.size());
      value_storage.append(env[i], line_length);
    }
  }

  // Now append all modified and new values.
  for (EnvironmentMap::const_iterator i = changes.begin();
       i != changes.end(); ++i) {
    if (!i->second.empty()) {
      result_indices.push_back(value_storage.size());
      value_storage.append(i->first);
      value_storage.push_back('=');
      value_storage.append(i->second);
      value_storage.push_back(0);
    }
  }

  size_t pointer_count_required =
      result_indices.size() + 1 +  // Null-terminated array of pointers.
      (value_storage.size() + sizeof(char*) - 1) / sizeof(char*);  // Buffer.
  std::unique_ptr<char* []> result(new char*[pointer_count_required]);

  // The string storage goes after the array of pointers.
  char* storage_data = reinterpret_cast<char*>(
      &result.get()[result_indices.size() + 1]);
  if (!value_storage.empty())
    memcpy(storage_data, value_storage.data(), value_storage.size());

  // Fill array of pointers at the beginning of the result.
  for (size_t i = 0; i < result_indices.size(); i++)
    result[i] = &storage_data[result_indices[i]];
  result[result_indices.size()] = 0;  // Null terminator.

  return result;
}

}  // namespace base
