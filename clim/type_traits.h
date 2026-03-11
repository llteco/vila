/*
 * Copyright (C) 2023-2026 The VILA Authors.
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
 *
 */
#ifndef CLIM_TYPE_TRAITS_H_
#define CLIM_TYPE_TRAITS_H_
#include <type_traits>

template <class T, class Ty = int>
using must_be_int = std::enable_if_t<std::is_integral<T>::value, Ty>;
template <class T, class Ty = int>
using must_be_float = std::enable_if_t<std::is_floating_point<T>::value, Ty>;
template <class T, class Ty = int>
using must_be_number = std::enable_if_t<std::is_arithmetic<T>::value, Ty>;
template <class T, class Ty = int>
using must_be_pointer = std::enable_if_t<std::is_pointer<T>::value, Ty>;
template <class T, class Ty = int>
using must_not_be_pointer = std::enable_if_t<!std::is_pointer<T>::value, Ty>;
#endif  // CLIM_TYPE_TRAITS_H_
