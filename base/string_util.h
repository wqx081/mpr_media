#ifndef BASE_STRING_UTIL_H_
#define BASE_STRING_UTIL_H_

#include <ctype.h>
#include <stdarg.h>   // va_list
#include <stddef.h>
#include <stdint.h>

#include <string>
#include <vector>

#include "base/macros.h"
#include "base/string_piece.h"  // For implicit conversions.

namespace base {

extern const char kWhitespaceASCII[];

template <typename Char>
inline bool IsAsciiWhitespace(Char c) {
  return c == ' ' || c == '\r' || c == '\n' || c == '\t';
}
template <typename Char>
inline bool IsAsciiAlpha(Char c) {
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}
template <typename Char>
inline bool IsAsciiUpper(Char c) {
  return c >= 'A' && c <= 'Z';
}
template <typename Char>
inline bool IsAsciiLower(Char c) {
  return c >= 'a' && c <= 'z';
}                   
template <typename Char>
inline bool IsAsciiDigit(Char c) {
  return c >= '0' && c <= '9';
}                   
                    
template <typename Char> 
inline bool IsHexDigit(Char c) {
  return (c >= '0' && c <= '9') ||
         (c >= 'A' && c <= 'F') ||
         (c >= 'a' && c <= 'f');
} 

inline char ToLowerASCII(char c) {
  return (c >= 'A' && c <= 'Z') ? (c + ('a' - 'A')) : c;
}

inline char ToUpperASCII(char c) {
  return (c >= 'a' && c <= 'z') ? (c + ('A' - 'a')) : c;
}

std::string ToLowerASCII(StringPiece str);
std::string ToUpperASCII(StringPiece str);


template<typename Char> struct CaseInsensitiveCompareASCII {
 public:
  bool operator()(Char x, Char y) const {
    return ToLowerASCII(x) == ToLowerASCII(y);
  }
};

bool LowerCaseEqualsASCII(StringPiece str, StringPiece lowercase_ascii);

enum class CompareCase {
  SENSITIVE,
  INSENSITIVE_ASCII,
};

bool StartsWith(StringPiece str,
		StringPiece search_for,
		CompareCase case_sensitivity);

bool EndsWith(StringPiece str,
              StringPiece search_for,
	      CompareCase case_sensitivity);

char* WriteInto(std::string* str, size_t length_with_null);

// Trim
enum TrimPositions {
  TRIM_NONE    = 0,
  TRIM_LEADING = 1,
  TRIM_TRAILING= 2,
  TRIM_ALL     = TRIM_LEADING | TRIM_TRAILING,
};

bool TrimString(const std::string& input,
                StringPiece trim_chars,
                std::string* output);

StringPiece TrimString(StringPiece input,
                       const StringPiece& trim_chars,
                       TrimPositions positions);

TrimPositions TrimWhitespaceASCII(const std::string& input,
                                  TrimPositions positions,
                                  std::string* output);
StringPiece TrimWhitespaceASCII(StringPiece input,
                                TrimPositions positions);

} // namespace
#endif // BASE_STRING_UTIL_H_
