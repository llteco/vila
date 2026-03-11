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
 * Description: split string widh sep char
 ****************************************/
#ifndef CLIM_STR_SPLIT_H_
#define CLIM_STR_SPLIT_H_
#include <string>
#include <string_view>
#include <vector>

/**
 * @brief splitting string with a separate string
 *
 * @param s: input string
 * @param sep: a separate string
 * @return a list of separated string
 */
inline std::vector<std::string_view> StrSplitStringView(
    std::string_view s, std::string_view sep
) {
  // output has to allocate real buffer because input string view
  // may be an rvalue.
  std::vector<std::string_view> ret;
  std::string_view substr = s;
  if (s.empty()) return ret;
  if (sep.empty()) return {s};
  ret.reserve(2);
  while (!substr.empty()) {
    auto pos = substr.find(sep, 0);
    if (pos > substr.size()) {
      ret.push_back(substr);
      break;
    }
    ret.emplace_back(substr.data(), pos);
    substr = substr.substr(pos + sep.size());
  }
  return ret;
}

/**
 * @brief splitting string with a separate character, faster version
 *
 * @param s: input string
 * @param sep: a separate character
 * @return a list of separated string
 */
inline std::vector<std::string_view> StrSplitStringView(
    std::string_view s, char sep
) {
  // output has to allocate real buffer because input string view
  // may be an rvalue.
  std::vector<std::string_view> ret;
  std::string_view substr = s;
  if (s.empty()) return ret;
  ret.reserve(2);
  while (!substr.empty()) {
    auto pos = substr.find(sep, 0);
    if (pos > substr.size()) {
      ret.push_back(substr);
      break;
    }
    ret.emplace_back(substr.data(), pos);
    substr = substr.substr(pos + 1);
  }
  return ret;
}

/**
 * @brief splitting string with a separate character
 *
 * Same as StrSplit but returns a vector of std::string.
 * @param s: input string
 * @param sep: a separate character
 * @return a list of separated string
 */
inline std::vector<std::string> StrSplit(
    std::string_view s, std::string_view sep
) {
  std::vector<std::string_view> v = StrSplitStringView(s, sep);
  return std::vector<std::string>(v.begin(), v.end());
}
#endif  // CLIM_STR_SPLIT_H_
