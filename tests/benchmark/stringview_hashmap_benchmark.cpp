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
 * Description: benchmark hash map
 ****************************************/
#include <benchmark/benchmark.h>

#include <deque>
#include <list>
#include <map>
#include <unordered_map>
#include <vector>

#include "clim/hasher/hash.h"
#include "clim/stringview_hash_map.h"

static void BM_std_unordered_map(benchmark::State& state) {
  StringHashmap<int> ht;
  std::string key(state.range(0), 'a');
  ht[key] = 1;
  for (auto _ : state) {
    int v = ht.at(key.c_str());
    benchmark::DoNotOptimize(v);
  }
}

BENCHMARK(BM_std_unordered_map)->Range(1, 10000);

static void BM_std_unordered_map_wo_str_alloc(benchmark::State& state) {
  StringHashmap<int> ht;
  std::string key(state.range(0), 'a');
  ht[key] = 1;
  for (auto _ : state) {
    int v = ht.at(key);
    benchmark::DoNotOptimize(v);
  }
}

BENCHMARK(BM_std_unordered_map_wo_str_alloc)->Range(1, 10000);

static void BM_stringview_hash_map(benchmark::State& state) {
  StringviewHashmap<int> ht;
  std::string key(state.range(0), 'a');
  ht[key] = 1;
  for (auto _ : state) {
    int v = ht.at(key.c_str());
    benchmark::DoNotOptimize(v);
  }
}

BENCHMARK(BM_stringview_hash_map)->Range(1, 10000);

static void BM_std_unordered_map_mixhash(benchmark::State& state) {
  StringHashmap<int, MixStringHash> ht;
  std::string key(state.range(0), 'a');
  ht[key] = 1;
  for (auto _ : state) {
    int v = ht.at(key.c_str());
    benchmark::DoNotOptimize(v);
  }
}

BENCHMARK(BM_std_unordered_map_mixhash)->Range(1, 10000);

static void BM_stringview_hash_map_mixhash(benchmark::State& state) {
  StringviewHashmap<int, MixStringHash> ht;
  std::string key(state.range(0), 'a');
  ht[key] = 1;
  for (auto _ : state) {
    int v = ht.at(key.c_str());
    benchmark::DoNotOptimize(v);
  }
}

BENCHMARK(BM_stringview_hash_map_mixhash)->Range(1, 10000);

static void BM_stringview_hash_map_iteration(benchmark::State& state) {
  StringviewHashmap<int, MixStringHash> maps;
  for (int i = 0; i < state.range(); ++i) {
    maps[std::to_string(i)] = int{};
  }
  for (auto _ : state) {
    for (auto& value : maps) {
      auto v = value.second;
      benchmark::DoNotOptimize(v);
    }
  }
}

BENCHMARK(BM_stringview_hash_map_iteration)->Range(100, 10000);

static void BM_std_map_iteration(benchmark::State& state) {
  std::map<std::string, int> maps;
  for (int i = 0; i < state.range(); ++i) {
    maps[std::to_string(i)] = int{};
  }
  for (auto _ : state) {
    for (auto& value : maps) {
      auto v = value.second;
      benchmark::DoNotOptimize(v);
    }
  }
}

BENCHMARK(BM_std_map_iteration)->Range(100, 10000);

static void BM_std_list_iteration(benchmark::State& state) {
  std::list<int> lists;
  for (int i = 0; i < state.range(); ++i) {
    lists.push_back(int{});
  }
  for (auto _ : state) {
    for (auto& value : lists) {
      benchmark::DoNotOptimize(value);
    }
  }
}

BENCHMARK(BM_std_list_iteration)->Range(100, 10000);

static void BM_std_vector_iteration(benchmark::State& state) {
  std::vector<int> lists;
  lists.reserve(state.range());
  for (int i = 0; i < state.range(); ++i) {
    lists.push_back(int{});
  }
  for (auto _ : state) {
    for (auto& value : lists) {
      benchmark::DoNotOptimize(value);
    }
  }
}

BENCHMARK(BM_std_vector_iteration)->Range(100, 10000);

static void BM_std_deque_iteration(benchmark::State& state) {
  std::deque<int> lists;
  for (int i = 0; i < state.range(); ++i) {
    lists.push_back(int{});
  }
  for (auto _ : state) {
    for (auto& value : lists) {
      benchmark::DoNotOptimize(value);
    }
  }
}

BENCHMARK(BM_std_deque_iteration)->Range(100, 10000);
