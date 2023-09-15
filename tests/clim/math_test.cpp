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

#include "clim/numerical.h"
#include "clim/quat.h"

TEST(Numbers, Constant) {
  EXPECT_NEAR(numbers::pi, 3.141592654, 1e-6);
  EXPECT_NEAR(numbers::e, 2.718281828459045, 1e-6);
  // numbers::pi_v<int>  // should compile error
}

TEST(Quaternion, Identity) {
  Quaternion_d iden = Quaternion_d::Identity();
  EXPECT_EQ(iden, Quaternion_d(0, 0, 0, 1));
  EXPECT_EQ(iden, Quaternion_d());
  auto rot = iden.Apply(1., 2., 3.);
  EXPECT_EQ(rot, std::make_tuple(1., 2., 3.));
}

TEST(Quaternion, FromEuler) {
  Quaternion_d q(0, 1, 2, EulerType::xyz);
  EXPECT_NEAR(q.X(), 0.40342268, 1e-5);
  EXPECT_NEAR(q.Y(), 0.25903472, 1e-5);
  EXPECT_NEAR(q.Z(), 0.73846026, 1e-5);
  EXPECT_NEAR(q.W(), 0.47415988, 1e-5);
  Quaternion_d p(0, 1, 2, EulerType::zyx);
  EXPECT_NEAR(p.X(), -0.40342268, 1e-5);
  EXPECT_NEAR(p.Y(), 0.25903472, 1e-5);
  EXPECT_NEAR(p.Z(), 0.73846026, 1e-5);
  EXPECT_NEAR(p.W(), 0.47415988, 1e-5);
}

TEST(Quaternion, Mul) {
  Quaternion_d p(0, 1, 0), q(0, 2, 0);
  Quaternion_d r = p * q;
  EXPECT_DOUBLE_EQ(r.X(), 0);
  EXPECT_DOUBLE_EQ(r.Y(), 0.99749498660405456);
  EXPECT_DOUBLE_EQ(r.Z(), 0);
  EXPECT_DOUBLE_EQ(r.W(), 0.070737201667702976);
}

TEST(Quaternion, Inversion) {
  Quaternion_d rot1(Degree<double>(0), Degree<double>(45), Degree<double>(0));
  auto rot2 = rot1.Reciprocal();
  EXPECT_EQ(
      rot2,
      Quaternion_d(Degree<double>(0), Degree<double>(-45), Degree<double>(0))
  );
}

TEST(Quaternion, Magnitude) {
  Quaternion_d rot(0, deg2rad(10.0), 0);
  EXPECT_EQ(Quaternion_d().Magnitude(), 0);
  EXPECT_DOUBLE_EQ(rot.Magnitude(), 0.17453292519943295);
}
