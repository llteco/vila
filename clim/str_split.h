/*
 * INTEL CONFIDENTIAL
 *
 * Copyright (C) 2021-2023 Intel Corporation
 *
 * This software and the related documents are Intel copyrighted materials,
 * and your use of them is governed by the express license under which they
 * were provided to you ("License"). Unless the License provides otherwise,
 * you may not use, modify, copy, publish, distribute, disclose or transmit
 * this software or the related documents without Intel's prior written
 * permission. This software and the related documents are provided as is, with
 * no express or implied warranties, other than those that are expressly stated
 * in the License.
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
 * @brief splitting string with a separate character
 *
 * @param s: input string
 * @param sep: a separate character
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
