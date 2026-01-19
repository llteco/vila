/************************************************************************
 * Copyright (C) 2026 The CLIM Authors.
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
#ifndef CLIM_VT_VT_GEMM_H_
#define CLIM_VT_VT_GEMM_H_
#include "vt.h"

namespace vt {
//! @addtogroup vt
//! @{

/**
 * @brief General Matrix-Matrix Multiplication (GEMM).
 * $ out = a @ b $
 *
 * @tparam Shape: an STL container of integer type for vector dimensions
 * @tparam Container: an STL container for vector data
 * @param a: input matrix a of shape [..., M, K]
 * @param as: the shape of matrix a
 * @param b: input matrix b of shape [..., K, N]
 * @param bs: the shape of matrix b
 * @return result of a @ b of shape [..., M, N]
 */
template <
    class Shape,
    class Container,
    must_be_int<typename Shape::value_type> = 0>
Container Gemm(
    const Container& a, const Shape& as, const Container& b, const Shape& bs
) {
  typedef typename Shape::value_type U;
  typedef typename Container::value_type T;
  if (as.size() < 2 || bs.size() < 2) {
    printf("%s: a or b must be at least 2-D tensor!!\n", __FUNCTION__);
    return {};
  }

  U B = PI(as.begin(), as.end() - 2);
  if (B != PI(bs.begin(), bs.end() - 2)) {
    printf("%s: Batch size mismatch!!\n", __FUNCTION__);
    return {};
  }

  Shape shape_a(as.end() - 2, as.end());
  Shape shape_b(bs.end() - 2, bs.end());
  if (shape_a[1] != shape_b[0]) {
    printf("%s: Dimension on reduce axis is not same!!\n", __FUNCTION__);
    return {};
  }
  shape_a.insert(shape_a.begin(), B);
  shape_b.insert(shape_b.begin(), B);

  U M = shape_a[1];
  U K = shape_a[2];
  U N = shape_b[2];
  Container y(B * M * N, T(0));
  Shape shape_y{B, M, N};
  for (U i = 0; i < B; i++) {
    for (U m = 0; m < M; m++) {
      for (U n = 0; n < N; n++) {
        T tmp = T(0);
        for (U k = 0; k < K; k++) {
          tmp += a[Index({i, m, k}, shape_a)] * b[Index({i, k, n}, shape_b)];
        }
        y[Index({i, m, n}, shape_y)] = tmp;
      }
    }
  }
  return y;
}

/**
 * @brief General Matrix-Matrix Multiplication (GEMM).
 * $ out = a @ b + c $
 *
 * @tparam Shape: an STL container of integer type for vector dimensions
 * @tparam Container: an STL container for vector data
 * @param a: input matrix a of shape [..., M, K]
 * @param as: the shape of matrix a
 * @param b: input matrix b of shape [..., K, N]
 * @param bs: the shape of matrix b
 * @param c: input matrix c of shape [..., M, N]
 * @param cs: the shape of matrix c
 * @return c after accumulation with a @ b
 */
template <
    class Shape,
    class Container,
    must_be_int<typename Shape::value_type> = 0>
Container Gemm(
    const Container& a,
    const Shape& as,
    const Container& b,
    const Shape& bs,
    Container& c,
    const Shape& cs
) {
  typedef typename Shape::value_type U;
  typedef typename Container::value_type T;
  Container y = Gemm<Shape, Container>(a, as, b, bs);
  if (y.size() != c.size()) {
    printf("%s: Accumulate size mismatch!!\n", __FUNCTION__);
    return {};
  }
  for (U i = 0; i < y.size(); i++) {
    c[i] += y[i];
  }
  return c;
}

template <
    class Shape,
    class Container,
    must_be_int<typename Shape::value_type> = 0>
Container SDPA(
    const Container& query,
    const Shape& qs,
    const Container& key,
    const Shape& ks,
    const Container& value,
    const Shape& vs,
    const Container& mask,
    const Shape& ms,
    typename Container::value_type scale = 1.0f
) {
  typedef typename Shape::value_type U;
  typedef typename Container::value_type T;

  U batch = qs[0];
  U head_q = PI(qs.begin(), qs.end() - 2);
  U head_k = PI(ks.begin(), ks.end() - 2);
  U head_v = PI(vs.begin(), vs.end() - 2);
  U seq_q = *(qs.rbegin() + 1);
  U seq_kv = *(ks.rbegin() + 1);
  U dim_q = *(qs.rbegin());
  U dim_k = *(ks.rbegin());
  U dim_v = *(vs.rbegin());

  Container out(head_v * seq_q * dim_v, T(0));
  for (U i = 0; i < head_q; i++) {
    for (U j = 0; j < seq_q; j++) {
      T max_qk = std::numeric_limits<T>::lowest();
      T sum_exp = T(0);
      Container qk(seq_kv, T(0));
      for (U k = 0; k < seq_kv; k++) {
        T& tmp = qk[k];
        for (U d = 0; d < dim_q; d++) {
          tmp += query[Index<Shape>({i, j, d}, {head_q, seq_q, dim_q})] *
                 key[Index<Shape>({i, k, d}, {head_k, seq_kv, dim_k})];
        }
        tmp *= scale;
        tmp += mask[Index({j, k}, ms)];
        max_qk = std::max(max_qk, tmp);
      }
      // sync qk_max
      for (U k = 0; k < seq_kv; k++) {
        T tmp = std::exp(qk[k] - max_qk);
        qk[k] = tmp;
        sum_exp += tmp;
      }
      // sync sum_exp
      for (U k = 0; k < seq_kv; k++) {
        qk[k] /= sum_exp;
      }
      // softmax done for ij
      for (U k = 0; k < seq_kv; k++) {
        for (U d = 0; d < dim_v; d++) {
          T& tmp = out[Index<Shape>({i, j, d}, {head_v, seq_q, dim_v})];
          tmp +=
              qk[k] * value[Index<Shape>({i, k, d}, {head_v, seq_kv, dim_v})];
        }
      }
    }
  }
  return out;
}

template <
    int BlockSize,
    class Shape,
    class Container,
    must_be_int<typename Shape::value_type> = 0>
Container FlashAttention(
    const Container& query,
    const Shape& qs,
    const Container& key,
    const Shape& ks,
    const Container& value,
    const Shape& vs,
    const Container& mask,
    const Shape& ms,
    typename Container::value_type scale = 1.0f
) {
  typedef typename Shape::value_type U;
  typedef typename Container::value_type T;

  U batch = qs[0];
  U head_q = PI(qs.begin(), qs.end() - 2);
  U head_k = PI(ks.begin(), ks.end() - 2);
  U head_v = PI(vs.begin(), vs.end() - 2);
  U seq_q = *(qs.rbegin() + 1);
  U seq_kv = *(ks.rbegin() + 1);
  U dim_q = *(qs.rbegin());
  U dim_k = *(ks.rbegin());
  U dim_v = *(vs.rbegin());
  const U kBlockKV = seq_kv / BlockSize;

  Container out(head_v * seq_q * dim_v, T(0));
  for (U i = 0; i < head_q; i++) {
    for (U j = 0; j < seq_q; j++) {
      T max_qk = std::numeric_limits<T>::lowest();
      T sum_exp = T(0);
      for (U k1 = 0; k1 < kBlockKV; k1++) {
        Container qkblock(BlockSize, T(0));
        T max_qblock = std::numeric_limits<T>::lowest();
        T sum_exp_block = T(0);
        for (U k2 = 0; k2 < BlockSize; k2++) {
          U k = k1 * BlockSize + k2;
          T& tmp = qkblock[k2];
          for (U d = 0; d < dim_q; d++) {
            tmp += query[Index<Shape>({i, j, d}, {head_q, seq_q, dim_q})] *
                   key[Index<Shape>({i, k, d}, {head_k, seq_kv, dim_k})];
          }
          tmp *= scale;
          tmp += mask[Index({j, k}, ms)];
          max_qblock = std::max(max_qblock, tmp);
        }
        T max_qk_new = std::max(max_qk, max_qblock);
        for (U k2 = 0; k2 < BlockSize; k2++) {
          U k = k1 * BlockSize + k2;
          T tmp = std::exp(qkblock[k2] - max_qblock);
          qkblock[k2] = tmp;
          sum_exp_block += tmp;
        }
        T sum_exp_new = sum_exp * std::exp(max_qk - max_qk_new) +
                        sum_exp_block * std::exp(max_qblock - max_qk_new);
        for (U d = 0; d < dim_v; d++) {
          T& tmp = out[Index<Shape>({i, j, d}, {head_v, seq_q, dim_v})];
          tmp *= std::exp(max_qk - max_qk_new) * sum_exp / sum_exp_new;
          for (U k2 = 0; k2 < BlockSize; k2++) {
            U k = k1 * BlockSize + k2;
            tmp += std::exp(max_qblock - max_qk_new) / sum_exp_new *
                   qkblock[k2] *
                   value[Index<Shape>({i, k, d}, {head_v, seq_kv, dim_v})];
          }
        }
        max_qk = max_qk_new;
        sum_exp = sum_exp_new;
      }
    }
  }
  return out;
}

//! @}
}  // namespace vt
#endif /* CLIM_VT_VT_GEMM_H_ */
