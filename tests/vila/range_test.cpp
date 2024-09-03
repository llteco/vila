/******************************************************************************
 * INTEL CONFIDENTIAL
 *
 * Copyright (C) 2024 Intel Corporation. All Rights Reserved.
 *
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel Corporation
 * or licensors. Title to the Material remains with Intel
 * Corporation or its licensors. The Material contains trade
 * secrets and proprietary and confidential information of Intel or its
 * licensors. The Material is protected by worldwide copyright
 * and trade secret laws and treaty provisions. No part of the Material may
 * be used, copied, reproduced, modified, published, uploaded, posted,
 * transmitted, distributed, or disclosed in any way without Intel's prior
 * express written permission.
 *
 * No License under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or
 * delivery of the Materials, either expressly, by implication, inducement,
 * estoppel or otherwise. Any license under such intellectual property rights
 * must be express and approved by Intel in writing.
 ******************************************************************************/
#include <gtest/gtest.h>

#include <ranges>
#include <string_view>
#include <vector>

#include "range/v3/view/filter.hpp"
#include "range/v3/view/join.hpp"
#include "range/v3/view/split.hpp"
#include "range/v3/view/transform.hpp"
#include "range/v3/view/zip.hpp"

using namespace ranges;

TEST(RangeView, Filter) {
  std::vector<int> const vi{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  auto rng = vi | views::filter([](int i) { return i % 2 == 0; }) |
             views::transform([](int i) { return std::to_string(i); });
  // prints: [2,4,6,8,10]
  std::cout << rng << '\n';

  auto const ints = {0, 1, 2, 3, 4, 5};
  auto even = [](int i) { return 0 == i % 2; };
  auto square = [](int i) { return i * i; };

  // the "pipe" syntax of composing the views:
  for (int i : ints | std::views::filter(even) | std::views::transform(square))
    std::cout << i << ' ';

  std::cout << '\n';

  // a traditional "functional" composing syntax:
  for (int i : std::views::transform(std::views::filter(ints, even), square))
    std::cout << i << ' ';

  std::cout << '\n';
}

TEST(RangeView, Zip) {
  std::vector<int> const vi{1, 3, 5, 7, 9};
  std::vector<double> const vii{2., 4., 6., 8.};
  for (auto &&[i, ii] : views::zip(vi, vii)) {
    EXPECT_EQ(ii - i, 1);
  }
}

TEST(RangeView, Split) {
  using std::operator""sv;
  constexpr auto words{"Hello^_^C++^_^20^_^!"sv};
  constexpr auto delim{"^_^"sv};
  constexpr auto words_view = views::split(words, delim);

  for (const auto word : words_view) {
    // with string_view's C++23 range constructor:
    for (const auto ch : word) std::cout << ch;
    std::cout << ' ';
  }
  std::cout << '\n';
}

TEST(RangeView, Join) {
  using std::operator""sv;
  const auto bits = {"https:"sv, "//"sv, "cppreference"sv, "."sv, "com"sv};
  for (char const c : bits | views::join) std::cout << c;
  std::cout << '\n';

  const std::vector<std::vector<int>> v{
      {1, 2},
      {3, 4, 5},
      {6},
      {7, 8, 9}
  };
  auto jv = join_view(v);
  for (int const e : jv) std::cout << e << ' ';
  std::cout << '\n';
}

TEST(RangeView, All) {
  std::vector<int> v{0, 1, 2, 3, 4, 5};
  for (int n : std::views::all(v) | std::views::take(2)) std::cout << n << ' ';
  std::cout << '\n';

  static_assert(
      std::is_same<
          decltype(std::views::single(42)), std::ranges::single_view<int>>{}
  );

  static_assert(std::is_same<
                decltype(std::views::all(v)),
                std::ranges::ref_view<std::vector<int, std::allocator<int>>>>{}
  );

  int a[]{1, 2, 3, 4};
  static_assert(
      std::is_same<decltype(std::views::all(a)), std::ranges::ref_view<int[4]>>{
      }
  );

  static_assert(
      std::is_same<
          decltype(std::ranges::subrange{std::begin(a) + 1, std::end(a) - 1}),
          std::ranges::subrange<int *, int *, std::ranges::subrange_kind(1)>>{}
  );
}
