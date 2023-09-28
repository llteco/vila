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
