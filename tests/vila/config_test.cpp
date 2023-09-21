/******************************************************************************
 * INTEL CONFIDENTIAL
 *
 * Copyright (C) 2023 Intel Corporation. All Rights Reserved.
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
