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
 * Description: benchmark stl containers
 ****************************************/
#include <benchmark/benchmark.h>
#include <fmt/format.h>

#include <deque>
#include <string>
#include <unordered_set>

// NOLINTNEXTLINE(*-avoid-c-arrays)
const std::string kDocs[]{
    "{}hello-world",
    "{}it was not profitable but very fashionable to write poetry. It also "
    "provided credibility to his talent as a writer and helped to enhance his "
    "social standing. It seems writing poetry was something he greatly enjoyed "
    "and did mainly for himself at times when he was not consumed with writing "
    "a play. Because of their more private nature, few poems, particularly "
    "long-form poems, have been published.",
    R"({}Alas, 'tis true I have gone here and there
       And made myself a motley to the view,
       Gor'd mine own thoughts, sold cheap what is most dear,
       Made old offences of affections new.
       Most true it is that I have look'd on truth
       Askance and strangely: but, by all above,
       These blenches gave my heart another youth,
       And worse essays prov'd thee my best of love.
       Now all is done, have what shall have no end!
       Mine appetite, I never more will grind
       On newer proof, to try an older friend,
       A god in love, to whom I am confin'd.
       Then give me welcome, next my heaven the best,
       Even to thy pure and most most loving breast.)",
};

static void IterationIntOverStlDeque(benchmark::State& state) {
  std::deque<int> deq;
  for (int64_t i = 0; i < state.range(); i++) {
    deq.push_back(i);
  }
  for (auto _ : state) {
    for (auto&& value : deq) {
      benchmark::DoNotOptimize(value);
    }
  }
}

static void IterationIntOverStlHashset(benchmark::State& state) {
  std::unordered_set<int> hset;
  for (int64_t i = 0; i < state.range(); i++) {
    hset.insert(i);
  }
  for (auto _ : state) {
    for (auto&& value : hset) {
      benchmark::DoNotOptimize(value);
    }
  }
}

static void IterationStrOverStlDeque(benchmark::State& state) {
  std::deque<std::string> deq;
  for (int64_t i = 0; i < state.range(); i++) {
    deq.push_back(fmt::format(kDocs[i % 3], i));
  }
  for (auto _ : state) {
    for (auto&& value : deq) {
      benchmark::DoNotOptimize(value);
    }
  }
}

static void IterationStrOverStlHashset(benchmark::State& state) {
  std::unordered_set<std::string> hset;
  for (int64_t i = 0; i < state.range(); i++) {
    hset.insert(fmt::format(kDocs[i % 3], i));
  }
  for (auto _ : state) {
    for (auto&& value : hset) {
      benchmark::DoNotOptimize(value);
    }
  }
}

BENCHMARK(IterationIntOverStlDeque)->Range(1, 1 << 20);
BENCHMARK(IterationStrOverStlDeque)->Range(1, 1 << 20);
BENCHMARK(IterationIntOverStlHashset)->Range(1, 1 << 20);
BENCHMARK(IterationStrOverStlHashset)->Range(1, 1 << 20);
