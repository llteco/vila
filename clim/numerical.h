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
 * Description: Math functions for simple algebra
 ****************************************/
#ifndef CLIM_NUMERICAL_H_
#define CLIM_NUMERICAL_H_
#include <algorithm>
#include <cmath>
#include <limits>

#include "clim/type_traits.h"

/// NOLINTBEGIN(readability-identifier-naming)
/// mathematic constant numbers
namespace numbers {
template <class T = double, must_be_float<T> = 0>
inline constexpr T pi_v = static_cast<T>(3.141592653589793);

template <class T = double, must_be_float<T> = 0>
inline constexpr T e_v = static_cast<T>(2.718281828459045);

constexpr double pi = pi_v<double>;
constexpr double e = e_v<double>;
}  // namespace numbers

/**
 * @brief rounded up integer division
 *
 * @tparam T: data type, must be an integer type.
 * @param a: dividend.
 * @param b: divisor.
 * @return y = round_up(a / b)
 */
template <class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
inline constexpr T round_div(T a, T b) {
  return (a + b - 1) / b;
}

/**
 * @brief numeric division for any input type
 *
 * @tparam T: output data type, must be a floating point.
 * @tparam U1: dividend type.
 * @tparam U2: divisor type.
 * @param a: dividend.
 * @param b: divisor.
 * @return y = a / b
 */
template <
    class T,
    class U1,
    class U2,
    std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
inline constexpr T numeric_div(U1 a, U2 b) {
  T tmp = static_cast<T>(b);
  if (fabs(tmp) <= std::numeric_limits<T>::epsilon()) {
    tmp = std::numeric_limits<T>::epsilon() * (tmp < 0 ? -1.0f : 1.0f);
  }
  return static_cast<T>(a) / tmp;
}

/**
 * @brief rounded down an integer to be divisible by the aligned integer.
 *
 * @tparam T: data type, must be an integer.
 * @param value: input integer.
 * @param align_value: aligned integer.
 * @return y that y mod `align_value` === 0.
 */
template <class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
inline constexpr T align_floor(T value, T align_value) {
  return value / align_value * align_value;
}

/**
 * @brief rounded up an integer to be divisible by the aligned integer.
 *
 * @tparam T: data type, must be an integer.
 * @param value: input integer.
 * @param align_value: aligned integer.
 * @return y that y mod `align_value` === 0.
 * @note this will align an divisible integer to a larger one.
 * i.e. align_ceil(0, 4)=4, align_ceil(4, 4)=8.
 */
template <class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
inline constexpr T align_ceil(T value, T align_value) {
  return align_floor(value + align_value, align_value);
}

/**
 * @brief rounded up an integer to be divisible by the aligned integer.
 *
 * @tparam T: data type, must be an integer.
 * @param value: input integer.
 * @param align_value: aligned integer.
 * @return y that y mod `align_value` === 0.
 */
template <class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
inline constexpr T align(T value, T align_value) {
  return round_div(value, align_value) * align_value;
}

/**
 * @brief clip the input value between the boundary.
 *
 * @tparam T: data type.
 * @param x: input value.
 * @param low: lower boundary.
 * @param high: higher boundary.
 * @return a value belongs [low, high].
 */
template <class T>
inline constexpr T clip(
    const T x,
    const T low = (std::numeric_limits<T>::min)(),
    const T high = (std::numeric_limits<T>::max)()
) {
  return std::max(std::min(x, high), low);
}

/**
 * @brief convert degree angle to radian
 *
 * @tparam T: data type
 * @param deg: degree
 * @return radian
 */
template <class T>
inline constexpr T deg2rad(T deg) {
  constexpr T kDeg180 = static_cast<T>(180);
  return deg / kDeg180 * numbers::pi_v<T>;
}

/**
 * @brief convert radian angle to degree
 *
 * @tparam T: data type
 * @param rad: radian
 * @return degree
 */
template <class T>
inline constexpr T rad2deg(T rad) {
  constexpr T kDeg180 = static_cast<T>(180);
  return rad / numbers::pi_v<T> * kDeg180;
}

/// NOLINTEND(readability-identifier-naming)
#endif  // CLIM_NUMERICAL_H_
