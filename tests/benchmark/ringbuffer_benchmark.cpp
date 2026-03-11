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
 * Description: benchmark rinbuffer vs deque
 ****************************************/
#include <benchmark/benchmark.h>

#include <deque>

#include "clim/ringbuffer.h"

struct Dummy {
  int x;
};

static void BM_Ringbuffer(benchmark::State& state) {
  RingBuffer<Dummy> rb(10);
  for (auto _ : state) {
    rb.Push(Dummy{});
    rb.Pop();
  }
}

BENCHMARK(BM_Ringbuffer)->Range(1, 1000000);

static void BM_deque(benchmark::State& state) {
  std::deque<Dummy> dq;
  for (auto _ : state) {
    dq.push_back(Dummy{});
    dq.pop_front();
  }
}

BENCHMARK(BM_deque)->Range(1, 1000000);
