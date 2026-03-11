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
 * Description: defines a rect
 ****************************************/
#ifndef CLIM_RECT_H_
#define CLIM_RECT_H_
#include <array>
#include <cstdlib>

#include "clim/array_arithmetic.h"

template <typename T>
struct Rect : std::array<T, 4> {
  Rect() { this->fill((T)0); }

  Rect(T a, T b, T c, T d) {
    (*this)[0] = a;
    (*this)[1] = b;
    (*this)[2] = c;
    (*this)[3] = d;
  }

  explicit Rect(T (&x)[4]) {
    (*this)[0] = x[0];
    (*this)[1] = x[1];
    (*this)[2] = x[2];
    (*this)[3] = x[3];
  }

  template <typename U>
  Rect<T>& operator=(const std::array<U, 4>& array) {
    (*this)[0] = static_cast<T>(array[0]);
    (*this)[1] = static_cast<T>(array[1]);
    (*this)[2] = static_cast<T>(array[2]);
    (*this)[3] = static_cast<T>(array[3]);
    return *this;
  }
};

template <typename T>
struct Point : std::array<T, 2> {
  Point() { this->fill((T)0); }

  Point(T a, T b) {
    (*this)[0] = a;
    (*this)[1] = b;
  }

  explicit Point(T (&x)[2]) {
    (*this)[0] = x[0];
    (*this)[1] = x[1];
  }

  template <typename U>
  Point<T>& operator=(const std::array<U, 2>& array) {
    (*this)[0] = static_cast<T>(array[0]);
    (*this)[1] = static_cast<T>(array[1]);
    return *this;
  }

  T X() const { return (*this)[0]; }

  T Y() const { return (*this)[1]; }

  T& X() { return (*this)[0]; }

  T& Y() { return (*this)[1]; }
};

using Rectd = Rect<double>;
using Rectf = Rect<float>;
using Recti = Rect<int32_t>;

using Pointd = Point<double>;
using Pointf = Point<float>;
using Pointi = Point<int32_t>;
#endif  // CLIM_RECT_H_
