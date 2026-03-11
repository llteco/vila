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
 * Description: unit test of StringviewHashmap
 ****************************************/
#include <gtest/gtest.h>

#include "clim/stringview_hash_map.h"

TEST(Hashmap, EmplaceNewValue) {
  StringviewHashmap<int> ht;
  ht.try_emplace("a", 0);
  ht.try_emplace("b", 1);
  ht.try_emplace("c", 2);
  ht.try_emplace("d", 3);
  std::string e("e");
  ht.try_emplace(e, 4);
  EXPECT_EQ(ht.find("a")->second, 0);
  EXPECT_EQ(ht.find("b")->second, 1);
  EXPECT_EQ(ht.find("c")->second, 2);
  EXPECT_EQ(ht.find("d")->second, 3);
  EXPECT_EQ(ht.find("e")->second, 4);
  EXPECT_EQ(ht.find("A"), ht.end());
  // the key holds on hashmap shouldn't depend on callers.
  EXPECT_NE(ht.find("e")->first.data(), e.data());
}

TEST(Hashmap, EraseValue) {
  StringviewHashmap<int> ht;
  ht["a"] = 0;
  ht["b"] = 1;
  ht.erase("b");
  EXPECT_EQ(ht.count("a"), 1);
  EXPECT_EQ(ht.count("b"), 0);
  EXPECT_EQ(ht.count("c"), 0);
}
