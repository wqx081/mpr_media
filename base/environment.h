#ifndef BASE_ENVIRONMENT_H_
#define BASE_ENVIRONMENT_H_

#include <map>
#include <memory>
#include <string>

#include "base/string_piece.h"

namespace base {

namespace env_vars {

extern const char kHome[];

}  // namespace env_vars

class  Environment {
 public:
  virtual ~Environment();

  // Returns the appropriate platform-specific instance.
  static std::unique_ptr<Environment> Create();

  // Gets an environment variable's value and stores it in |result|.
  // Returns false if the key is unset.
  virtual bool GetVar(StringPiece variable_name, std::string* result) = 0;

  // Syntactic sugar for GetVar(variable_name, nullptr);
  virtual bool HasVar(StringPiece variable_name);

  // Returns true on success, otherwise returns false.
  virtual bool SetVar(StringPiece variable_name,
                      const std::string& new_value) = 0;

  // Returns true on success, otherwise returns false.
  virtual bool UnSetVar(StringPiece variable_name) = 0;
};



typedef std::string NativeEnvironmentString;
typedef std::map<NativeEnvironmentString, NativeEnvironmentString>
    EnvironmentMap;

// See general comments for the Windows version above.
//
// This Posix version takes and returns a Posix-style environment block, which
// is a null-terminated list of pointers to null-terminated strings. The
// returned array will have appended to it the storage for the array itself so
// there is only one pointer to manage, but this means that you can't copy the
// array without keeping the original around.
 std::unique_ptr<char* []> AlterEnvironment(
    const char* const* env,
    const EnvironmentMap& changes);


}  // namespace base

#endif  // BASE_ENVIRONMENT_H_
