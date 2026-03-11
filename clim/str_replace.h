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
 * Description: replace string
 ****************************************/
#ifndef CLIM_STR_REPLACE_H_
#define CLIM_STR_REPLACE_H_
#include <string>
#include <string_view>

/**
 * @brief Replace all matched parts of a string with another string.
 *
 * The replacement will change the original string object.
 *
 * @param [inout] str: the string to be replaced.
 * @param [in] what: the part to replace.
 * @param [in] with: the replacement string.
 * @return std::string_view of the replaced string.
 */
inline std::string_view StrReplaceInplace(
    std::string& str, std::string_view what, std::string_view with
) {
  for (size_t pos = 0; pos < str.size();) {
    if (str.substr(pos, what.size()) == what) {
      str.replace(pos, what.size(), with);
      pos += with.size();
    } else {
      ++pos;
    }
  }
  return str;
}

/**
 * @brief Replace all matched parts of a string with another string.
 *
 * The replacement won't change the original string object.
 *
 * @param [in] str: the string to be replaced.
 * @param [in] what: the part to replace.
 * @param [in] with: the replacement string.
 * @return The replaced string.
 */
inline std::string StrReplace(
    std::string_view str, std::string_view what, std::string_view with
) {
  std::string ret(str);
  StrReplaceInplace(ret, what, with);
  return ret;
}
#endif  // CLIM_STR_REPLACE_H_
