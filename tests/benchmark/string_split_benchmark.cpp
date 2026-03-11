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
