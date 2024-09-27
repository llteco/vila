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

#include <memory>

class PImplFoo {
 public:
  PImplFoo();
  ~PImplFoo() = default;

  int64_t& GetValue() noexcept;

 private:
  class FooImpl;
  std::unique_ptr<FooImpl> impl_;
};

class PImplFoo::FooImpl {
 public:
  FooImpl() = default;
  ~FooImpl() = default;

  int64_t& GetValue() noexcept { return value_; }

 private:
  int64_t value_ = 0;
};

PImplFoo::PImplFoo() : impl_(std::make_unique<FooImpl>()) {}

int64_t& PImplFoo::GetValue() noexcept { return impl_->GetValue(); }

class NativeFoo {
 public:
  NativeFoo() = default;
  ~NativeFoo() = default;

  int64_t& GetValue() noexcept;

 private:
  int64_t value_ = 0;
};

int64_t& NativeFoo::GetValue() noexcept { return value_; }

class PImplBar {
 public:
  PImplBar();
  ~PImplBar();

  int64_t& GetValue() noexcept;

 private:
  class BazImpl;
  BazImpl* impl_;
};

class PImplBar::BazImpl {
 public:
  BazImpl() = default;
  ~BazImpl() = default;

  int64_t& GetValue() noexcept { return value_; }

 private:
  int64_t value_ = 0;
};

PImplBar::PImplBar() : impl_(new BazImpl()) {}

PImplBar::~PImplBar() { delete impl_; }

int64_t& PImplBar::GetValue() noexcept { return impl_->GetValue(); }

class PureAbstractFoo {
 public:
  static std::shared_ptr<PureAbstractFoo> Create();

  virtual int64_t& GetValue() noexcept = 0;
};

class FooImpl final : public PureAbstractFoo {
 public:
  int64_t& GetValue() noexcept override { return value_; }

 private:
  int64_t value_ = 0;
};

std::shared_ptr<PureAbstractFoo> PureAbstractFoo::Create() {
  return std::make_shared<FooImpl>();
}

static void BM_PImplGetValue(benchmark::State& state) {
  PImplFoo foo;
  for (auto _ : state) {
    foo.GetValue()++;
  }
}

static void BM_NativeGetValue(benchmark::State& state) {
  NativeFoo bar;
  for (auto _ : state) {
    bar.GetValue()++;
  }
}

static void BM_PImplNoUniquePtrGetValue(benchmark::State& state) {
  PImplBar baz;
  for (auto _ : state) {
    baz.GetValue()++;
  }
}

static void BM_PureAbstractGetValue(benchmark::State& state) {
  auto foo = PureAbstractFoo::Create();
  for (auto _ : state) {
    foo->GetValue()++;
  }
}

static void BM_PureAbstractCastGetValue(benchmark::State& state) {
  auto foo = PureAbstractFoo::Create();
  auto bar = std::dynamic_pointer_cast<FooImpl>(foo);
  for (auto _ : state) {
    bar->GetValue()++;
  }
}

BENCHMARK(BM_PImplGetValue);
BENCHMARK(BM_NativeGetValue);
BENCHMARK(BM_PImplNoUniquePtrGetValue);
BENCHMARK(BM_PureAbstractGetValue);
BENCHMARK(BM_PureAbstractCastGetValue);
