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
 * Description:
 ****************************************/
#include "clim/ringbuffer.h"

#include <gtest/gtest.h>

TEST(Ringbuffer, EmptyCheck) {
  RingBuffer<int> rb;
  ASSERT_EQ(rb.Capacity(), 2);
  EXPECT_TRUE(rb.Empty());
  rb.Push(1);
  EXPECT_FALSE(rb.Empty());
  rb.Push(2);
  rb.Push(3);
  rb.Pop();
  rb.Pop();
  EXPECT_TRUE(rb.Empty());
}

TEST(Ringbuffer, Fifo) {
  RingBuffer<int> rb;
  rb.Push(1);
  rb.Push(2);
  EXPECT_EQ(rb.Pop(), 1);
  EXPECT_EQ(rb.Pop(), 2);
  rb.Push(1);
  rb.Push(2);
  rb.Push(3);
  EXPECT_EQ(rb.Pop(), 2);
  EXPECT_EQ(rb.Pop(), 3);
  EXPECT_TRUE(rb.Empty());
}
