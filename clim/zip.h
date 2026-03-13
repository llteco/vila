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
#ifndef CLIM_ZIP_H_
#define CLIM_ZIP_H_
#include <iterator>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace detail {

template <typename T1, typename T2>
auto make_pair_keep_ref(T1&& v1, T2&& v2) -> std::pair<T1, T2> {
  return std::pair<T1, T2>(std::forward<T1>(v1), std::forward<T2>(v2));
}

template <typename Tuple1, typename Tuple2, std::size_t... Is>
auto tuple_zip_impl(Tuple1&& t1, Tuple2&& t2, std::index_sequence<Is...>) {
  return std::make_tuple(make_pair_keep_ref(
      std::get<Is>(std::forward<Tuple1>(t1)),
      std::get<Is>(std::forward<Tuple2>(t2))
  )...);
}

template <typename ContainerRef, typename Iter>
decltype(auto) zip_deref(Iter& it) {
  if constexpr (std::is_lvalue_reference<ContainerRef>::value) {
    return *it;
  } else {
    return std::move(*it);
  }
}

template <typename ContainerRef, typename ElemRef>
using zip_item_t = std::conditional_t<
    std::is_lvalue_reference<ContainerRef>::value,
    ElemRef,
    std::remove_reference_t<ElemRef>>;

}  // namespace detail

// Zip two tuples element-wise. Both tuples must have the same number of
// elements.
template <typename Tuple1, typename Tuple2>
auto tuple_zip(Tuple1&& t1, Tuple2&& t2) {
  using BareTuple1 = std::remove_reference_t<Tuple1>;
  using BareTuple2 = std::remove_reference_t<Tuple2>;

  static_assert(
      std::tuple_size_v<BareTuple1> == std::tuple_size_v<BareTuple2>,
      "tuple_zip requires tuples with the same size"
  );
  constexpr auto size = std::tuple_size_v<BareTuple1>;
  return detail::tuple_zip_impl(
      std::forward<Tuple1>(t1), std::forward<Tuple2>(t2),
      std::make_index_sequence<size>{}
  );
}

// Zip two containers up to the shorter length.
// Lvalue containers produce reference pairs; rvalue containers produce value
// pairs.
template <typename Container1, typename Container2>
auto zip(Container1&& c1, Container2&& c2) {
  auto it1 = std::begin(c1);
  auto it2 = std::begin(c2);
  auto end1 = std::end(c1);
  auto end2 = std::end(c2);

  using PairType = std::pair<
      detail::zip_item_t<Container1&&, decltype(*it1)>,
      detail::zip_item_t<Container2&&, decltype(*it2)>>;

  std::vector<PairType> result;
  for (; it1 != end1 && it2 != end2; ++it1, ++it2) {
    result.emplace_back(
        detail::zip_deref<Container1&&>(it1),
        detail::zip_deref<Container2&&>(it2)
    );
  }

  return result;
}
#endif  // CLIM_ZIP_H_
