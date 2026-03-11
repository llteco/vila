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
 * Description: city hash
 ****************************************/
#ifndef CLIM_HASHER_CITY_H_
#define CLIM_HASHER_CITY_H_
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4505)
#endif
#include <string>

#include "clim/hasher/city_internal.h"

static uint64_t CityHash64(const char* s, size_t len) {
  if (len <= 32) {
    if (len <= 16) {
      return city::internal::HashLen0to16(s, len);
    } else {
      return city::internal::HashLen17to32(s, len);
    }
  } else if (len <= 64) {
    return city::internal::HashLen33to64(s, len);
  }
  return city::internal::HashRest(s, len);
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

namespace city {
template <class T>
struct Hash {
  uint64_t operator()(const T& key) const noexcept {
    static_assert(std::is_trivial_v<T>);
    return CityHash64(reinterpret_cast<const char*>(&key), sizeof(T));
  }
};

template <>
struct Hash<std::string> {
  uint64_t operator()(const std::string& key) const noexcept {
    return CityHash64(key.c_str(), key.size());
  }
};
}  // namespace city
#endif  // CLIM_HASHER_CITY_H_
