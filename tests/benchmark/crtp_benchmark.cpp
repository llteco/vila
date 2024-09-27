/******************************************************************************
 * INTEL CONFIDENTIAL
 *
 * Copyright (C) 2024 Intel Corporation. All Rights Reserved.
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
#include <benchmark/benchmark.h>

namespace vf {
class Base {
 public:
  virtual void doSomething() = 0;
};

class Derived : public Base {
 public:
  void doSomething() override { volatile int i = 0; }
};

void callDoSomething(Base* obj) { obj->doSomething(); }
}  // namespace vf

namespace crtp {
template <typename T>
class Base {
 public:
  void doSomething() { static_cast<T*>(this)->doSomethingImpl(); }
};

class Derived : public Base<Derived> {
 public:
  void doSomethingImpl() { volatile int i = 0; }
};

void callDoSomething(Derived& obj) { obj.doSomething(); }
}  // namespace crtp

static void BM_VirtualFunctionCall(benchmark::State& state) {
  vf::Derived obj;
  for (auto _ : state) {
    vf::callDoSomething(&obj);
  }
}

BENCHMARK(BM_VirtualFunctionCall);

static void BM_CRTPFunctionCall(benchmark::State& state) {
  crtp::Derived obj;
  for (auto _ : state) {
    crtp::callDoSomething(obj);
  }
}

BENCHMARK(BM_CRTPFunctionCall);
