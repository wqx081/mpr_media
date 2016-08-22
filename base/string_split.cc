#include "base/string_split.h"

#include <stddef.h>

#include "base/string_util.h"
#include <glog/logging.h>

namespace base {

namespace { 

template<typename Str, typename OutputType>
OutputType PieceToOutputType(BasicStringPiece<Str> piece) {
  return piece;
}

template<>  // Convert StringPiece to std::string
std::string PieceToOutputType<std::string, std::string>(StringPiece piece) {
  return piece.as_string();
}

 template<typename Str> BasicStringPiece<Str> WhitespaceForType();

template<> StringPiece WhitespaceForType<std::string>() {
  return kWhitespaceASCII;
}

size_t FindFirstOf(StringPiece piece, char c, size_t pos) {
  return piece.find(c, pos);
}

size_t FindFirstOf(StringPiece piece, StringPiece one_of, size_t pos) {
  return piece.find_first_of(one_of, pos);
}

template<typename Str, typename OutputStringType, typename DelimiterType>
           static std::vector<OutputStringType> SplitStringT(
    BasicStringPiece<Str> str,
    DelimiterType delimiter,
    WhitespaceHandling whitespace,
    SplitResult result_type) {
  std::vector<OutputStringType> result;
  if (str.empty())
    return result;
    
  size_t start = 0;
  while (start != Str::npos) {
    size_t end = FindFirstOf(str, delimiter, start);
    
    BasicStringPiece<Str> piece;
    if (end == Str::npos) {
      piece = str.substr(start);
      start = Str::npos;
    } else {
      piece = str.substr(start, end - start);
      start = end + 1;
    }

    if (whitespace == TRIM_WHITESPACE)
      piece = TrimString(piece, WhitespaceForType<Str>(), TRIM_ALL);

    if (result_type == SPLIT_WANT_ALL || !piece.empty())
      result.push_back(PieceToOutputType<Str, OutputStringType>(piece));
  }
  return result;
}

bool AppendStringKeyValue(StringPiece input,
                          char delimiter,
                          StringPairs* result) {
  result->resize(result->size() + 1);
  auto& result_pair = result->back();
  
  size_t end_key_pos = input.find_first_of(delimiter);
  if (end_key_pos == std::string::npos) {
    DVLOG(1) << "cannot find delimiter in: " << input;
    return false;    // No delimiter.
  } 
  input.substr(0, end_key_pos).CopyToString(&result_pair.first);
  
  StringPiece remains = input.substr(end_key_pos, input.size() - end_key_pos);
  size_t begin_value_pos = remains.find_first_not_of(delimiter); 
  if (begin_value_pos == StringPiece::npos) {
    DVLOG(1) << "cannot parse value from input: " << input;
    return false;   // No value.
  } 
  remains.substr(begin_value_pos, remains.size() - begin_value_pos)
      .CopyToString(&result_pair.second);
      
  return true;
} 

template <typename Str, typename OutputStringType>
void SplitStringUsingSubstrT(BasicStringPiece<Str> input,
                             BasicStringPiece<Str> delimiter,
                             WhitespaceHandling whitespace,
                             SplitResult result_type,
                             std::vector<OutputStringType>* result) {
  using Piece = BasicStringPiece<Str>;
  using size_type = typename Piece::size_type;
  
  result->clear();
  for (size_type begin_index = 0, end_index = 0; end_index != Piece::npos;
              begin_index = end_index + delimiter.size()) {
    end_index = input.find(delimiter, begin_index);
    Piece term = end_index == Piece::npos
                     ? input.substr(begin_index)
                     : input.substr(begin_index, end_index - begin_index);
                     
    if (whitespace == TRIM_WHITESPACE)
      term = TrimString(term, WhitespaceForType<Str>(), TRIM_ALL);
      
    if (result_type == SPLIT_WANT_ALL || !term.empty())
      result->push_back(PieceToOutputType<Str, OutputStringType>(term));
  }   
} 

}  // namespace

std::vector<std::string> SplitString(StringPiece input,
                                     StringPiece separators,
                                     WhitespaceHandling whitespace,
                                     SplitResult result_type) {
  if (separators.size() == 1) {
    return SplitStringT<std::string, std::string, char>(
        input, separators[0], whitespace, result_type);
  }
  return SplitStringT<std::string, std::string, StringPiece>(
      input, separators, whitespace, result_type);
}

std::vector<StringPiece> SplitStringPiece(StringPiece input,
                                          StringPiece separators,
                                          WhitespaceHandling whitespace,
                                          SplitResult result_type) {
  if (separators.size() == 1) {
    return SplitStringT<std::string, StringPiece, char>(
        input, separators[0], whitespace, result_type);
  }
  return SplitStringT<std::string, StringPiece, StringPiece>(
      input, separators, whitespace, result_type);
}

bool SplitStringIntoKeyValuePairs(StringPiece input,
                                  char key_value_delimiter,
                                  char key_value_pair_delimiter,
                                  StringPairs* key_value_pairs) { 
  key_value_pairs->clear();

  std::vector<StringPiece> pairs = SplitStringPiece(
      input, std::string(1, key_value_pair_delimiter),
      TRIM_WHITESPACE, SPLIT_WANT_NONEMPTY);
  key_value_pairs->reserve(pairs.size());
  bool success = true;
  for (const StringPiece& pair : pairs) { 
    if (!AppendStringKeyValue(pair, key_value_delimiter, key_value_pairs)) { 
      success = false;
    } 
  } 
  return success;
}

void SplitStringUsingSubstr(StringPiece input,
                            StringPiece delimiter,
                            std::vector<std::string>* result) {
  SplitStringUsingSubstrT(input, delimiter, TRIM_WHITESPACE, SPLIT_WANT_ALL,
                          result); 
}

std::vector<StringPiece> SplitStringPieceUsingSubstr(
    StringPiece input,
    StringPiece delimiter,
    WhitespaceHandling whitespace,
    SplitResult result_type) {
  std::vector<StringPiece> result;
  SplitStringUsingSubstrT(input, delimiter, whitespace, result_type, &result);
  return result;
}


} // namespace base
