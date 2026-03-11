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
#include <gtest/gtest.h>

#include <sstream>

#include "vila/config/configuration.h"

TEST(Json, ParseJson) {
  std::stringstream doc;
  doc << R"({
    "a": 1,
    "b": true,
    "c": "x",
    "d": [],
    "e": {}
  })";

  auto root = vila::JsonFromStream(doc);
  EXPECT_EQ(root["a"].get<int>(), 1);
  bool b = root["b"];
  EXPECT_TRUE(b);
  std::string c;
  root["c"].get_to(c);
  EXPECT_EQ(c, "x");
  EXPECT_TRUE(root["d"].empty());
  EXPECT_EQ(root["e"], nlohmann::json::object());
}
