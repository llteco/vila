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
