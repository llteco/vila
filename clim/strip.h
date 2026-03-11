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
 * Description: strip strings
 ****************************************/
#ifndef CLIM_STRIP_H_
#define CLIM_STRIP_H_
#include <string>
#include <string_view>

inline std::string Strip(std::string_view str, char t = ' ') {
  size_t beg = str.find_first_not_of(t, 0);
  if (beg == std::string_view::npos) beg = 0;
  size_t end = str.find_last_not_of(t);
  if (end == std::string_view::npos) end = str.size() - 1;
  return std::string(str.substr(beg, end - beg + 1));
}
#endif  // CLIM_STRIP_H_
