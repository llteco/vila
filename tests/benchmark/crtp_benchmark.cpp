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
