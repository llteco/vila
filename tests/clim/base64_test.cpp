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
 * Description: base64 decoding test
 ****************************************/
#include "clim/base64.h"

#include <gtest/gtest.h>

#include "clim/vt/vt.h"

TEST(Base64, Decode) {
  auto binary0 = base64::decode("AA==\n", 5);
  EXPECT_EQ(binary0.size(), 1);
  EXPECT_EQ(
      std::vector<uint8_t>(binary0.begin(), binary0.begin() + 1),
      std::vector<uint8_t>({0})
  );
  auto binary1 = base64::decode("AB0sPw==\n", 9);
  EXPECT_EQ(binary1.size(), 4);
  EXPECT_EQ(
      std::vector<uint8_t>(binary1.begin(), binary1.begin() + 4),
      std::vector<uint8_t>({0x00, 0x1d, 0x2c, 0x3f})
  );
}

TEST(Base64, IllegalInput) {
  std::string long_text;
  long_text.resize(100000);
  for (int16_t ch = std::numeric_limits<char>::lowest();
       ch <= std::numeric_limits<char>::max(); ch++) {
    std::fill(long_text.begin(), long_text.end(), static_cast<char>(ch));
    base64::decode(long_text.c_str(), long_text.size());
  }
}

TEST(Base64, Encode) {
  auto test = vt::RandomN<std::vector<uint8_t>>({1024});
  auto enc = base64::encode(test.data(), test.size());
  auto dec = base64::decode(enc.data(), enc.size());
  EXPECT_EQ(test, dec);
}
