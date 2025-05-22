/******************************************************************************
 * INTEL CONFIDENTIAL
 *
 * Copyright (C) 2025 Intel Corporation. All Rights Reserved.
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

#include <sycl/sycl.hpp>
#include <vector>

int main() {
  sycl::queue queue;
  std::vector<int> a(1024, 1), b(1024, 2), c(1024, 0);
  {
    sycl::buffer buffer_a(a), buffer_b(b), buffer_c(c);
    queue.submit([&buffer_c, &buffer_a, &buffer_b](sycl::handler& h) {
      sycl::accessor a(buffer_a, h, sycl::read_only);
      sycl::accessor b(buffer_b, h, sycl::read_only);
      sycl::accessor c(buffer_c, h, sycl::write_only, sycl::no_init);

      h.parallel_for(sycl::range<1>(1024), [=](auto i) { c[i] = a[i] + b[i]; });
    });
    queue.wait();
    // note to release sycl::buffer to write data back to host memory
  }
  EXPECT_EQ(c, std::vector<int>(1024, 3));
  return 0;
}
