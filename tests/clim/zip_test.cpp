/************************************************************************
 * Copyright (C) 2026 The CLIM Authors.
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
 ************************************************************************/
#include "clim/zip.h"

#include <gtest/gtest.h>

#include <vector>

TEST(ZipTest, ConstReference) {
  std::tuple<int, double, std::string> t1{1, 2.5, "hello"};
  std::tuple<int, double, std::string> t2{10, 20.5, "world"};

  auto zipped = tuple_zip(t1, t2);
  static_assert(std::tuple_size_v<decltype(zipped)> == 3);

  EXPECT_EQ(std::get<0>(std::get<0>(zipped)), 1);
  EXPECT_EQ(std::get<1>(std::get<0>(zipped)), 10);
  EXPECT_EQ(std::get<0>(std::get<1>(zipped)), 2.5);
  EXPECT_EQ(std::get<1>(std::get<1>(zipped)), 20.5);
  EXPECT_EQ(std::get<0>(std::get<2>(zipped)), std::string("hello"));
  EXPECT_EQ(std::get<1>(std::get<2>(zipped)), std::string("world"));
}

TEST(ZipTest, Modify) {
  std::tuple<int, double, std::string> t1{1, 2.5, "hello"};
  std::tuple<int, double, std::string> t2{10, 20.5, "world"};

  auto zipped = tuple_zip(t1, t2);
  std::get<0>(t1) = 42;
  EXPECT_EQ(std::get<0>(std::get<0>(zipped)), 42);
  EXPECT_EQ(std::get<1>(std::get<0>(zipped)), 10);
  std::get<1>(std::get<0>(zipped)) = 20;
  EXPECT_EQ(std::get<0>(std::get<0>(zipped)), 42);
  EXPECT_EQ(std::get<1>(std::get<0>(zipped)), 20);
}

TEST(ZipTest, RvalueReference) {
  std::tuple<int, double, std::string> t1{1, 2.5, "hello"};
  std::tuple<int, double, std::string> t2{10, 20.5, "world"};
  // Pass tuples as rvalues: tuple_zip should decay elements to values.
  auto zipped = tuple_zip(std::move(t1), std::move(t2));
  // Capture the state of the source tuples immediately after move.
  auto t1_after_move = t1;
  auto t2_after_move = t2;
  std::get<0>(std::get<0>(zipped)) = 42;
  EXPECT_EQ(std::get<0>(zipped), std::make_pair(42, 10));
  std::get<1>(std::get<0>(zipped)) = 20;
  EXPECT_EQ(std::get<0>(zipped), std::make_pair(42, 20));
}

TEST(ZipTest, VectorZip) {
  std::vector<float> lhs{1, 2, 3};
  std::vector<int> rhs{10, 20};

  auto zipped = zip(lhs, rhs);
  ASSERT_EQ(zipped.size(), 2u);
  EXPECT_EQ(std::get<0>(zipped[0]), 1.0f);
  EXPECT_EQ(std::get<1>(zipped[0]), 10);
  EXPECT_EQ(std::get<0>(zipped[1]), 2.0f);
  EXPECT_EQ(std::get<1>(zipped[1]), 20);
}

TEST(ZipTest, VectorZipModifyReference) {
  std::vector<int> lhs{1, 2, 3};
  std::vector<int> rhs{10, 20, 30};

  auto zipped = zip(lhs, rhs);
  lhs[0] = 42;
  EXPECT_EQ(std::get<0>(zipped[0]), 42);
  EXPECT_EQ(std::get<1>(zipped[0]), 10);

  zipped[0].second = 99;
  EXPECT_EQ(rhs[0], 99);
}

TEST(ZipTest, VectorZipRvalue) {
  auto zipped = zip(std::vector<int>{1, 2, 3}, std::vector<int>{10, 20, 30});

  ASSERT_EQ(zipped.size(), 3u);
  EXPECT_EQ(std::get<0>(zipped[0]), 1);
  EXPECT_EQ(std::get<1>(zipped[0]), 10);
  zipped[0].first = 42;
  EXPECT_EQ(std::get<0>(zipped[0]), 42);
  EXPECT_EQ(std::get<1>(zipped[0]), 10);
}
