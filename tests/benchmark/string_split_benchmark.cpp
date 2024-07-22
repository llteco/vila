/*
 * INTEL CONFIDENTIAL
 *
 * Copyright (C) 2021-2023 Intel Corporation
 *
 * This software and the related documents are Intel copyrighted materials,
 * and your use of them is governed by the express license under which they
 * were provided to you ("License"). Unless the License provides otherwise,
 * you may not use, modify, copy, publish, distribute, disclose or transmit
 * this software or the related documents without Intel's prior written
 * permission. This software and the related documents are provided as is, with
 * no express or implied warranties, other than those that are expressly stated
 * in the License.
 */
/****************************************
 * Description: bench string split
 ****************************************/
#include <benchmark/benchmark.h>

#include "clim/str_split.h"

std::string MakeString(size_t len, char sep = ';') {
  std::string str(len, 'x');
  for (int i = 7; i < len; i += 7) {
    str[i] = sep;
  }
  return str;
}

static void BM_split_string(benchmark::State& state) {
  std::string str = MakeString(state.range(0));
  for (auto _ : state) {
    auto v = StrSplit(str, ";");
    benchmark::DoNotOptimize(v);
  }
}

BENCHMARK(BM_split_string)->Range(1, 100000);

static void BM_split_stringview(benchmark::State& state) {
  std::string str = MakeString(state.range(0));
  for (auto _ : state) {
    auto v = StrSplitStringView(str, ";");
    benchmark::DoNotOptimize(v);
  }
}

BENCHMARK(BM_split_stringview)->Range(1, 100000);

static void BM_split_stringview_by_char(benchmark::State& state) {
  std::string str = MakeString(state.range(0));
  for (auto _ : state) {
    auto v = StrSplitStringView(str, ';');
    benchmark::DoNotOptimize(v);
  }
}

BENCHMARK(BM_split_stringview_by_char)->Range(1, 100000);
