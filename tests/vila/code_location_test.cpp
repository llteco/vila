/*
 * Copyright (C) 2023-2026 The VILA Authors.
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
 *
 */
#include "vila/logging/code_location.h"

#include <gtest/gtest.h>

template <uint32_t level>
std::vector<std::string> Func() {
  auto stacks = Func<level - 1>();
  stacks.push_back(vila::CaptureBackTrace().front());
  return stacks;
}

template <>
std::vector<std::string> Func<0>() {
  return {};
}

TEST(CodeLocation, GetBacktrace) {
  if (vila::CaptureBackTrace().empty()) {
    GTEST_SKIP();
  }
  auto stacks = Func<3>();
#ifdef _DEBUG
  EXPECT_EQ(stacks[0].substr(0, 10), "at Func<1>");
  EXPECT_EQ(stacks[1].substr(0, 10), "at Func<2>");
  EXPECT_EQ(stacks[2].substr(0, 10), "at Func<3>");
#endif
  for (const auto& sym : stacks) {
    std::cout << sym << std::endl;
  }
}
