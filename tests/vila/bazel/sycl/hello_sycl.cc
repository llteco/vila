/*
 * Copyright (C) 2025-2026 The VILA Authors.
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
