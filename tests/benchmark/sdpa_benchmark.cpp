/************************************************************************
 * Copyright (C) 2026 The VILA Authors.
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
 ************************************************************************/

#include <benchmark/benchmark.h>

#include <vector>

#include "clim/vt/vt_gemm.h"
#include "clim/vt/vt_nn.h"

static void BM_Scaled_Dot_Product_Attention_Math(benchmark::State& state) {
  using Tensor = std::vector<float>;
  using Shape = std::vector<int64_t>;
  for (auto _ : state) {
    state.PauseTiming();
    Tensor query, key, value, mask;
    auto num_heads = state.range(0);
    auto seq_len = state.range(1);
    auto head_dim = state.range(2);
    Shape qs{num_heads, seq_len, head_dim};
    Shape ks{num_heads, seq_len, head_dim};
    Shape vs{num_heads, seq_len, head_dim};
    query = vt::RandomN<Tensor>(qs);
    key = vt::RandomN<Tensor>(ks);
    value = vt::RandomN<Tensor>(vs);
    mask = vt::Ones<Tensor, Shape>({seq_len, seq_len});
    state.ResumeTiming();
    auto kt = vt::Transpose(key, ks, {0, 2, 1});
    auto qk = vt::Gemm(query, qs, kt, {num_heads, head_dim, seq_len});
    float scale = 1.0f / std::sqrt(static_cast<float>(head_dim));
    qk = vt::Mul(qk, scale);
    for (int i = 0; i < qk.size(); i++) {
      qk[i] += mask[i % (seq_len * seq_len)];
    }
    qk = vt::Softmax<Tensor, Shape>(qk, {num_heads, seq_len, seq_len});
    auto attn = vt::Gemm(qk, {num_heads, seq_len, seq_len}, value, vs);
    benchmark::DoNotOptimize(attn);
  }
}

BENCHMARK(BM_Scaled_Dot_Product_Attention_Math)
    ->RangeMultiplier(8)
    ->Ranges({
        { 1,   8}, // num_heads
        {16, 256}, // seq_len
        {16, 512}  // head_dim
});

static void BM_Scaled_Dot_Product_Attention_SDPA(benchmark::State& state) {
  using Tensor = std::vector<float>;
  using Shape = std::vector<int64_t>;
  for (auto _ : state) {
    state.PauseTiming();
    Tensor query, key, value, mask;
    auto num_heads = state.range(0);
    auto seq_len = state.range(1);
    auto head_dim = state.range(2);
    Shape qs{num_heads, seq_len, head_dim};
    Shape ks{num_heads, seq_len, head_dim};
    Shape vs{num_heads, seq_len, head_dim};
    query = vt::RandomN<Tensor>(qs);
    key = vt::RandomN<Tensor>(ks);
    value = vt::RandomN<Tensor>(vs);
    mask = vt::Ones<Tensor, Shape>({seq_len, seq_len});
    state.ResumeTiming();
    auto attn = vt::SDPA(
        query, qs, key, ks, value, vs, mask, Shape{seq_len, seq_len},
        1.0f / std::sqrt(static_cast<float>(head_dim))
    );
    benchmark::DoNotOptimize(attn);
  }
}

BENCHMARK(BM_Scaled_Dot_Product_Attention_SDPA)
    ->RangeMultiplier(8)
    ->Ranges({
        { 1,   8}, // num_heads
        {16, 256}, // seq_len
        {16, 512}  // head_dim
});

static void BM_Scaled_Dot_Product_Attention_Flash(benchmark::State& state) {
  using Tensor = std::vector<float>;
  using Shape = std::vector<int64_t>;
  for (auto _ : state) {
    state.PauseTiming();
    Tensor query, key, value, mask;
    auto num_heads = state.range(0);
    auto seq_len = state.range(1);
    auto head_dim = state.range(2);
    Shape qs{num_heads, seq_len, head_dim};
    Shape ks{num_heads, seq_len, head_dim};
    Shape vs{num_heads, seq_len, head_dim};
    query = vt::RandomN<Tensor>(qs);
    key = vt::RandomN<Tensor>(ks);
    value = vt::RandomN<Tensor>(vs);
    mask = vt::Ones<Tensor, Shape>({seq_len, seq_len});
    state.ResumeTiming();
    auto attn = vt::FlashAttention<4>(
        query, qs, key, ks, value, vs, mask, {seq_len, seq_len},
        1.0f / std::sqrt(static_cast<float>(head_dim))
    );
    benchmark::DoNotOptimize(attn);
  }
}

BENCHMARK(BM_Scaled_Dot_Product_Attention_Flash)
    ->RangeMultiplier(8)
    ->Ranges({
        { 1,   8}, // num_heads
        {16, 256}, // seq_len
        {16, 512}  // head_dim
});
