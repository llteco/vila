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
 * Description: murmur3hasher
 ****************************************/
#ifndef CLIM_HASHER_MURMUR3_H_
#define CLIM_HASHER_MURMUR3_H_
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4505)
#endif
#include <string>

#include "clim/hasher/murmur3_internal.h"

static uint64_t MurmurHash3(const void* key, int len, uint32_t seed) {
  uint64_t t;
  murmur3::internal::MurmurHash3_x86_32(key, len, seed, &t);
  return t;
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

namespace murmur3 {
template <class T>
struct Hash {
  uint64_t operator()(const T& key) const noexcept {
    static_assert(std::is_trivial_v<T>);
    return MurmurHash3(reinterpret_cast<const char*>(&key), sizeof(T), 0);
  }
};

template <>
struct Hash<std::string> {
  uint64_t operator()(const std::string& key) const noexcept {
    return MurmurHash3(key.c_str(), static_cast<int>(key.size()), 0);
  }
};
}  // namespace murmur3
#endif  // CLIM_HASHER_MURMUR3_H_
