#ifndef BASE_STRING_SPLIT_H_
#define BASE_STRING_SPLIT_H_

namespace base {

enum WhitespaceHandling {
  KEEP_WHITESPACE,
  TRIM_WHITESPACE,
};

enum SplitResult {
  SPLIT_WANT_ALL,
  SPLIT_WANT_NONEMPTY,
};

} // namespace base
#endif // BASE_STRING_SPLIT_H_
