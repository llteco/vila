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
 * Description: Test barrier object
 ****************************************/
#include "clim/barrier.h"

#include <gtest/gtest.h>

#include <thread>

TEST(Barrier, SingleThread) {
  Barrier b(1);
  EXPECT_FALSE(b.Notified());
  b.Notify();
  b.Wait();
  EXPECT_FALSE(b.Notified());

  Barrier bb(2);
  EXPECT_FALSE(bb.Notified());
  EXPECT_FALSE(bb.WaitFor(std::chrono::seconds(0)));
  EXPECT_FALSE(bb.Notified());
  bb.Notify();
  bb.Notify();
  EXPECT_TRUE(bb.WaitFor(std::chrono::seconds(0)));
  EXPECT_TRUE(bb.Notified());
}

TEST(Barrier, TwinThreads) {
  Barrier b(1);

  std::thread waiter([&]() { b.Wait(); });
  std::thread publisher([&]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    b.Notify();
  });

  waiter.join();
  publisher.join();
  b.Wait();
  EXPECT_TRUE(b.Notified());
}
