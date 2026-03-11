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
 * Description: hash functions
 ****************************************/
#ifndef CLIM_HASHER_HASH_H_
#define CLIM_HASHER_HASH_H_
#include <string_view>
#include <type_traits>
#include <utility>

#include "clim/hasher/city.h"
#include "clim/hasher/murmur3.h"

/**
 * @brief A helper class implement string hash using several hash functions.
 *
 * For string length >= 16, it is city hasher 64bit, for other cases it is
 * original std::hash.
 */
struct MixStringHash {
  uint64_t operator()(std::string_view key) const noexcept {
    if (key.size() >= 16) {
      return CityHash64(key.data(), key.size());
    } else {
      const std::hash<std::string_view> hasher;
      return hasher(key);
    }
  }
};
#endif  // CLIM_HASHER_HASH_H_
