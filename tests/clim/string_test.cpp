/*
 * Copyright (C) 2021-2026 The VILA Authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/****************************************
 * Description: test string functions
 ****************************************/
#include <gtest/gtest.h>

#include "clim/str_replace.h"
#include "clim/str_split.h"
#include "clim/strip.h"

TEST(String, StripTest) {
  EXPECT_EQ(Strip("   abcd   "), "abcd") << "Strip abcd";
  EXPECT_EQ(Strip("hello world"), "hello world") << "Strip hello world";
  EXPECT_EQ(Strip(""), "") << "Strip empty string";
}

TEST(String, SplitTest) {
  EXPECT_EQ(
      StrSplit("1,2,3,4", ","), std::vector<std::string>({"1", "2", "3", "4"})
  ) << "single sep";
  EXPECT_EQ(
      StrSplit("1::2::3::4", "::"),
      std::vector<std::string>({"1", "2", "3", "4"})
  ) << "double sep";
  EXPECT_EQ(StrSplit("", "."), std::vector<std::string>()) << "empty string";
  EXPECT_EQ(StrSplit("", ""), std::vector<std::string>()) << "empty sep";
  EXPECT_EQ(StrSplit("abc", ""), std::vector<std::string>({"abc"}))
      << "empty sep";
  EXPECT_EQ(
      StrSplitStringView("1,2,3,4", ','),
      std::vector<std::string_view>({"1", "2", "3", "4"})
  ) << "single char sep";
}

TEST(String, ReplaceTest) {
  std::string test_str("alice loves bob and bob does not love alice");
  auto ans1 = StrReplace(test_str, "bob", "clark");
  EXPECT_EQ(ans1, "alice loves clark and clark does not love alice");
  EXPECT_NE(ans1, test_str);
  auto ans2 = StrReplaceInplace(test_str, "love", "hate");
  EXPECT_EQ(ans2, test_str);
  EXPECT_EQ(ans2, "alice hates bob and bob does not hate alice");
  EXPECT_EQ(
      StrReplace("helloworld", "hello", "prependhello"), "prependhelloworld"
  );
  EXPECT_EQ(StrReplace("helloworld", "hello", ""), "world");
  EXPECT_EQ(StrReplace("hello world", "o", "lo"), "helllo wlorld");
}
