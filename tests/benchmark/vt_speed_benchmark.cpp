/*
 * Copyright (C) 2023-2026 The VILA Authors.
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

#include "clim/vt/vt.h"

// NOLINTBEGIN(misc-use-anonymous-namespace,readability-identifier-naming)

static void BM_median_std_priority_queue(benchmark::State& state) {
  constexpr size_t kQueueLength = 1000000;
  auto data = vt::RandomN<std::vector<float>>(kQueueLength);
  for (auto _ : state) {
    float median = vt::MedianMinHeap(data);
    benchmark::DoNotOptimize(median);
  }
}

BENCHMARK(BM_median_std_priority_queue)->Iterations(10);

static void BM_median_qsort(benchmark::State& state) {
  constexpr size_t kQueueLength = 1000000;
  auto data = vt::RandomN<std::vector<float>>(kQueueLength);
  for (auto _ : state) {
    float median = vt::Median(data);
    benchmark::DoNotOptimize(median);
  }
}

BENCHMARK(BM_median_qsort)->Iterations(10);

static void BM_median_std_sort(benchmark::State& state) {
  constexpr size_t kQueueLength = 1000000;
  auto data = vt::RandomN<std::vector<float>>(kQueueLength);
  for (auto _ : state) {
    std::sort(data.begin(), data.end());
    state.PauseTiming();
    vt::RandomN<std::vector<float>>(kQueueLength);
    state.ResumeTiming();
  }
}

BENCHMARK(BM_median_std_sort)->Iterations(10);

// NOLINTEND(misc-use-anonymous-namespace,readability-identifier-naming)
