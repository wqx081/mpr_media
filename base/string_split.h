#ifndef BASE_STRING_SPLIT_H_
#define BASE_STRING_SPLIT_H_
#include <string>
#include <utility>
#include <vector>
  
#include "base/string_piece.h"

namespace base {

enum WhitespaceHandling {
  KEEP_WHITESPACE,
  TRIM_WHITESPACE,
};

enum SplitResult {
  SPLIT_WANT_ALL,
  SPLIT_WANT_NONEMPTY,
};

std::vector<std::string> SplitString(
    StringPiece input,
    StringPiece separators,
    WhitespaceHandling whitespace,
    SplitResult result_type);

std::vector<StringPiece> SplitStringPiece(
      StringPiece input,
      StringPiece separators,
      WhitespaceHandling whitespace,
      SplitResult result_type);

using StringPairs = std::vector<std::pair<std::string, std::string>>;

bool SplitStringIntoKeyValuePairs(StringPiece input,
                                  char key_value_delimiter,
                                  char key_value_pair_delimiter,
                                  StringPairs* key_value_pairs);

void SplitStringUsingSubstr(StringPiece input,
                            StringPiece delimiter,
                            std::vector<std::string>* result);
                                        
std::vector<StringPiece> SplitStringPieceUsingSubstr(
    StringPiece input,
    StringPiece delimiter,
    WhitespaceHandling whitespace,
    SplitResult result_type);

} // namespace base
#endif // BASE_STRING_SPLIT_H_
