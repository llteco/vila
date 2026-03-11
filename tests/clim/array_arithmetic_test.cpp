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
 * Description: math test
 ****************************************/
#include "clim/array_arithmetic.h"

#include <gtest/gtest.h>

#include <type_traits>

#include "clim/alpha_beta_filter.h"
#include "clim/bounding_box.h"
#include "clim/kalman_filter.h"

TEST(Arithmetic, ArrayAddMul) {
  std::array<double, 4> a{1, 2, 3, 4};
  std::array<double, 4> b{9, 8, 7, 1};
  const std::array<double, 4> kSum{10, 10, 10, 5};
  const std::array<double, 4> kSub{-8, -6, -4, 3};
  const std::array<double, 4> kProd{9, 16, 21, 4};
  const std::array<double, 4> kScale3{27, 24, 21, 3};
  const std::array<double, 4> kAdd2{3, 4, 5, 6};

  EXPECT_EQ(a + b, kSum);
  EXPECT_EQ(a - b, kSub);
  EXPECT_EQ(a * b, kProd);
  EXPECT_EQ(a + 2.0, kAdd2);
  EXPECT_EQ(a - (-2.0), kAdd2);
  EXPECT_EQ(2.0 + a, kAdd2);
  EXPECT_EQ(2.0 - (-a), kAdd2);
  EXPECT_EQ(b * 3.0, kScale3);
  EXPECT_EQ(3.0 * b, kScale3);
  EXPECT_EQ(b / (1.0 / 3.0), kScale3);
}

TEST(Arithmetic, ArrayBinaryAssign) {
  std::array<double, 4> a{1, 2, 3, 4};
  std::array<double, 4> b{9, 8, 7, 1};
  const std::array<double, 4> kSum{10, 10, 10, 5};

  a += b;
  EXPECT_EQ(a, kSum);
}

TEST(Arithmetic, ArrayAbsClipL1L2) {
  std::array<double, 4> a{-1, 2, -3, 4};
  const std::array<double, 4> kAbs{1, 2, 3, 4};
  const std::array<double, 4> kClamp02{0, 2, 0, 2};
  const double kL1Norm = 10, kL2Squared = 30;

  EXPECT_EQ(abs(a), kAbs);
  EXPECT_EQ(clip(a, 0.0, 2.0), kClamp02);
  EXPECT_EQ(l1(a), kL1Norm);
  EXPECT_EQ(l2(a) * l2(a), kL2Squared);
}

TEST(Arithmetic, ArrayBoolean) {
  std::array<double, 4> a{-1, 2, -3, 4};
  std::array<double, 4> b{1, 1, 1, 1};

  EXPECT_TRUE(b == 1.0);
  EXPECT_TRUE(a >= -3.0);
  EXPECT_TRUE(a <= 4.0);
  EXPECT_TRUE(b > 0.0);
  EXPECT_TRUE(b < 2.0);
  EXPECT_TRUE(b != 0.0);
  EXPECT_FALSE(a != 2.0);
}

TEST(KalmanFilter, Linear) {
  KalmanFilter<1> kf({}, {});
  kf.Priori();
  kf.Posteriori({0}, 1, {}, {});
}
