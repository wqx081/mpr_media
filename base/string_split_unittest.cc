#include "base/string_piece.h"
#include "base/string_split.h"
#include <string>
#include <gtest/gtest.h>

namespace base {

TEST(StringPlist, Basic) {
  std::vector<std::string> r;

  r = SplitString(std::string(), "\n", KEEP_WHITESPACE, SPLIT_WANT_ALL);
  EXPECT_TRUE(r.empty());

  r = SplitString("a\n", "\n", TRIM_WHITESPACE, SPLIT_WANT_ALL);
  EXPECT_TRUE(r.size() == 1);
  EXPECT_EQ("a", r[0]);
}

} // namespace base
