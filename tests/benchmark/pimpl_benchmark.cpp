/*
 * Copyright (C) 2024-2026 The VILA Authors.
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
