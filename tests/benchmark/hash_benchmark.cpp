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
 * Description: bench hashers
 ****************************************/
#include <benchmark/benchmark.h>

#include <string>
#include <utility>

#include "clim/hasher/hash.h"

static std::string MakeString(size_t len) { return std::string(len, 'x'); }

static void BM_std_hash(benchmark::State& state) {
  std::string str = MakeString(state.range(0));
  auto hasher = std::hash<std::string>();
  for (auto _ : state) {
    size_t hash = hasher(str);
    benchmark::DoNotOptimize(hash);
  }
}

BENCHMARK(BM_std_hash)->RangeMultiplier(2)->Range(1, 4096);

static void BM_city_hash(benchmark::State& state) {
  std::string str = MakeString(state.range(0));
  auto hasher = city::Hash<std::string>();
  for (auto _ : state) {
    size_t hash = hasher(str);
    benchmark::DoNotOptimize(hash);
  }
}

BENCHMARK(BM_city_hash)->RangeMultiplier(2)->Range(1, 4096);

static void BM_murmur3_hash(benchmark::State& state) {
  std::string str = MakeString(state.range(0));
  auto hasher = murmur3::Hash<std::string>();
  for (auto _ : state) {
    size_t hash = hasher(str);
    benchmark::DoNotOptimize(hash);
  }
}

BENCHMARK(BM_murmur3_hash)->RangeMultiplier(2)->Range(1, 4096);

static void BM_mix_hash(benchmark::State& state) {
  std::string str = MakeString(state.range(0));
  auto hasher = MixStringHash();
  for (auto _ : state) {
    size_t hash = hasher(str);
    benchmark::DoNotOptimize(hash);
  }
}

BENCHMARK(BM_mix_hash)->RangeMultiplier(2)->Range(1, 4096);
