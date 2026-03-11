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
 * Description: Provide arithmetic calculation to std::array.
 ****************************************/
#ifndef CLIM_ARRAY_ARITHMETIC_H_
#define CLIM_ARRAY_ARITHMETIC_H_
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>

// NOLINTBEGIN(readability-identifier-naming)
template <typename T, size_t size>
std::array<T, size> operator-(const std::array<T, size>& lhs) {
  std::array<T, size> ans;
  for (size_t i = 0; i < size; i++) {
    ans[i] = -lhs[i];
  }
  return ans;
}

template <typename T, size_t size>
std::array<T, size> operator+(
    const std::array<T, size>& lhs, const std::array<T, size>& rhs
) {
  std::array<T, size> ans;
  for (size_t i = 0; i < size; i++) {
    ans[i] = lhs[i] + rhs[i];
  }
  return ans;
}

template <typename T, size_t size>
std::array<T, size> operator-(
    const std::array<T, size>& lhs, const std::array<T, size>& rhs
) {
  std::array<T, size> ans;
  for (size_t i = 0; i < size; i++) {
    ans[i] = lhs[i] - rhs[i];
  }
  return ans;
}

template <typename T, size_t size>
std::array<T, size> operator*(
    const std::array<T, size>& lhs, const std::array<T, size>& rhs
) {
  std::array<T, size> ans;
  for (size_t i = 0; i < size; i++) {
    ans[i] = lhs[i] * rhs[i];
  }
  return ans;
}

template <typename T, size_t size>
std::array<T, size> operator/(
    const std::array<T, size>& lhs, const std::array<T, size>& rhs
) {
  std::array<T, size> ans;
  for (size_t i = 0; i < size; i++) {
    ans[i] = lhs[i] / rhs[i];
  }
  return ans;
}

template <typename T, size_t size>
std::array<T, size> operator+(const std::array<T, size>& lhs, T rhs) {
  std::array<T, size> ans;
  for (size_t i = 0; i < size; i++) {
    ans[i] = lhs[i] + rhs;
  }
  return ans;
}

template <typename T, size_t size>
std::array<T, size> operator-(const std::array<T, size>& lhs, T rhs) {
  std::array<T, size> ans;
  for (size_t i = 0; i < size; i++) {
    ans[i] = lhs[i] - rhs;
  }
  return ans;
}

template <typename T, size_t size>
std::array<T, size> operator*(const std::array<T, size>& lhs, T rhs) {
  std::array<T, size> ans;
  for (size_t i = 0; i < size; i++) {
    ans[i] = lhs[i] * rhs;
  }
  return ans;
}

template <typename T, size_t size>
std::array<T, size> operator/(const std::array<T, size>& lhs, T rhs) {
  std::array<T, size> ans;
  for (size_t i = 0; i < size; i++) {
    ans[i] = lhs[i] / rhs;
  }
  return ans;
}

template <typename T, size_t size>
std::array<T, size> operator+(T lhs, const std::array<T, size>& rhs) {
  return rhs + lhs;
}

template <typename T, size_t size>
std::array<T, size> operator-(T lhs, const std::array<T, size>& rhs) {
  return -rhs + lhs;
}

template <typename T, size_t size>
std::array<T, size> operator*(T lhs, const std::array<T, size>& rhs) {
  return rhs * lhs;
}

template <typename T, size_t size>
std::array<T, size> operator/(T lhs, const std::array<T, size>& rhs) {
  std::array<T, size> ans;
  for (size_t i = 0; i < size; i++) {
    ans[i] = lhs / rhs[i];
  }
  return ans;
}

template <typename T, size_t size>
std::array<T, size>& operator+=(
    std::array<T, size>& lhs, const std::array<T, size>& rhs
) {
  lhs = lhs + rhs;
  return lhs;
}

template <typename T, size_t size>
std::array<T, size>& operator-=(
    std::array<T, size>& lhs, const std::array<T, size>& rhs
) {
  lhs = lhs - rhs;
  return lhs;
}

template <typename T, size_t size>
std::array<T, size>& operator*=(
    std::array<T, size>& lhs, const std::array<T, size>& rhs
) {
  lhs = lhs * rhs;
  return lhs;
}

template <typename T, size_t size>
std::array<T, size>& operator/=(
    std::array<T, size>& lhs, const std::array<T, size>& rhs
) {
  lhs = lhs / rhs;
  return lhs;
}

template <typename T, size_t size>
std::array<T, size>& operator+=(std::array<T, size>& lhs, T rhs) {
  lhs = lhs + rhs;
  return lhs;
}

template <typename T, size_t size>
std::array<T, size>& operator-=(std::array<T, size>& lhs, T rhs) {
  lhs = lhs - rhs;
  return lhs;
}

template <typename T, size_t size>
std::array<T, size>& operator*=(std::array<T, size>& lhs, T rhs) {
  lhs = lhs * rhs;
  return lhs;
}

template <typename T, size_t size>
std::array<T, size>& operator/=(std::array<T, size>& lhs, T rhs) {
  lhs = lhs / rhs;
  return lhs;
}

template <typename T, size_t size>
std::array<T, size> abs(const std::array<T, size>& lhs) {
  auto ans = lhs;
  std::for_each(ans.begin(), ans.end(), [&](T& x) { x = std::abs(x); });
  return ans;
}

template <typename T, size_t size>
std::array<T, size> clip(
    const std::array<T, size>& lhs,
    T low,
    T high = std::numeric_limits<T>::max()
) {
  auto ans = lhs;
  std::for_each(ans.begin(), ans.end(), [&](T& x) {
    x = std::max(std::min(x, high), low);
  });
  return ans;
}

template <typename T, size_t size>
std::array<T, size> clip(
    const std::array<T, size>& lhs,
    const std::array<T, size>& low,
    const std::array<T, size>& high
) {
  auto ans = lhs;
  auto lowit = low.begin();
  auto highit = high.begin();
  std::for_each(ans.begin(), ans.end(), [&](T& x) {
    x = std::max(std::min(x, *highit++), *lowit++);
  });
  return ans;
}

template <typename T, size_t size>
T l1(const std::array<T, size>& lhs) {
  T ans = 0;
  std::for_each(lhs.begin(), lhs.end(), [&ans](const T& v) {
    ans += ::abs(v);
  });
  return ans;
}

template <typename T, size_t size>
double l2(const std::array<T, size>& lhs) {
  double ans = 0;
  std::for_each(lhs.begin(), lhs.end(), [&ans](const T& v) { ans += v * v; });
  return sqrt(ans);
}

template <typename T, size_t size>
bool operator==(const std::array<T, size>& lhs, T rhs) {
  bool ans = true;
  std::for_each(lhs.begin(), lhs.end(), [&](const T& x) { ans &= x == rhs; });
  return ans;
}

template <typename T, size_t size>
bool operator!=(const std::array<T, size>& lhs, T rhs) {
  bool ans = true;
  std::for_each(lhs.begin(), lhs.end(), [&](const T& x) { ans &= x != rhs; });
  return ans;
}

template <typename T, size_t size>
bool operator<(const std::array<T, size>& lhs, T rhs) {
  bool ans = true;
  std::for_each(lhs.begin(), lhs.end(), [&](const T& x) { ans &= x < rhs; });
  return ans;
}

template <typename T, size_t size>
bool operator>(const std::array<T, size>& lhs, T rhs) {
  bool ans = true;
  std::for_each(lhs.begin(), lhs.end(), [&](const T& x) { ans &= x > rhs; });
  return ans;
}

template <typename T, size_t size>
bool operator<=(const std::array<T, size>& lhs, T rhs) {
  bool ans = true;
  std::for_each(lhs.begin(), lhs.end(), [&](const T& x) { ans &= x <= rhs; });
  return ans;
}

template <typename T, size_t size>
bool operator>=(const std::array<T, size>& lhs, T rhs) {
  bool ans = true;
  std::for_each(lhs.begin(), lhs.end(), [&](const T& x) { ans &= x >= rhs; });
  return ans;
}

// NOLINTEND(readability-identifier-naming)
#endif  // CLIM_ARRAY_ARITHMETIC_H_
