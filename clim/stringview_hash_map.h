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
 * Description: hashmap using std::string_view as key
 ****************************************/
#ifndef CLIM_STRINGVIEW_HASH_MAP_H_
#define CLIM_STRINGVIEW_HASH_MAP_H_
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>

/// NOLINTBEGIN(readability-identifier-naming)

/**
 * @brief A helper class to use std::string as hash map keys but also
 * accept std::string_view as api argument.
 *
 * @tparam T: hash map value type.
 * @tparam H: hasher type.
 */
template <class T, typename H = std::hash<std::string_view>>
class StringHashmap : public std::unordered_map<std::string, T, H> {
  using hash_map_t = std::unordered_map<std::string, T, H>;

 public:
  StringHashmap() : hash_map_t() {}

  ~StringHashmap() = default;

  template <class... Args>
  std::pair<typename hash_map_t::iterator, bool> try_emplace(
      std::string_view key, Args&&... args
  ) {
    std::string k(key);
    return hash_map_t::try_emplace(k, std::forward<Args>(args)...);
  }

  typename hash_map_t::size_type erase(std::string_view key) noexcept {
    return hash_map_t::erase(std::string(key));
  }

  typename hash_map_t::size_type erase(const std::string& key) noexcept {
    return hash_map_t::erase(key);
  }

  typename hash_map_t::size_type erase(const char* key) noexcept {
    return hash_map_t::erase(key);
  }

  typename hash_map_t::size_type count(std::string_view key) const {
    return hash_map_t::count(std::string(key));
  }

  typename hash_map_t::size_type count(const std::string& key) const {
    return hash_map_t::count(key);
  }

  typename hash_map_t::size_type count(const char* key) const {
    return hash_map_t::count(key);
  }

  typename hash_map_t::mapped_type& at(std::string_view key) {
    return hash_map_t::at(std::string(key));
  }

  typename hash_map_t::mapped_type& at(const std::string& key) {
    return hash_map_t::at(key);
  }

  typename hash_map_t::mapped_type& at(const char* key) {
    return hash_map_t::at(key);
  }

  T& operator[](const std::string_view& key) {
    std::string k(key);
    return hash_map_t::operator[](k);
  }

  T& operator[](const std::string& key) { return hash_map_t::operator[](key); }

  T& operator[](const char* key) { return hash_map_t::operator[](key); }
};

/**
 * @brief A helper class to use std::string_view as hash map keys.
 *
 * @tparam T: hash map value type
 * @tparam H: hasher type
 */
template <class T, typename H = std::hash<std::string_view>>
class StringviewHashmap : public std::unordered_map<std::string_view, T, H> {
  using hash_map_t = std::unordered_map<std::string_view, T, H>;

 public:
  StringviewHashmap() : hash_map_t() {}

  ~StringviewHashmap() = default;

  void clear() noexcept {
    hash_map_t::clear();
    chars_.clear();
  }

  template <class... Args>
  std::pair<typename hash_map_t::iterator, bool> try_emplace(
      std::string_view key, Args&&... args
  ) {
    std::string_view k(key);
    if (hash_map_t::find(key) == hash_map_t::end()) {
      chars_.insert(std::string(key));
      k = *(chars_.find(std::string(key)));
    }
    return hash_map_t::try_emplace(k, std::forward<Args>(args)...);
  }

  typename hash_map_t::size_type erase(const std::string_view& key) noexcept {
    hash_map_t::erase(key);
    return chars_.erase(std::string(key));
  }

  T& operator[](const std::string_view& key) {
    std::string_view k(key);
    if (hash_map_t::find(key) == hash_map_t::end()) {
      chars_.insert(std::string(key));
      k = *(chars_.find(std::string(key)));
    }
    return hash_map_t::operator[](k);
  }

 private:
  std::unordered_set<std::string> chars_;
};

/// NOLINTEND(readability-identifier-naming)
#endif  // CLIM_STRINGVIEW_HASH_MAP_H_
