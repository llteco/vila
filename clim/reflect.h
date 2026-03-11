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
 * Description: basic reflection definition
 ****************************************/
#ifndef CLIM_REFLECT_H_
#define CLIM_REFLECT_H_
#include <string_view>

#ifdef _WIN32
#define OS_PATH_SLASH '\\'
#else
#define OS_PATH_SLASH '/'
#endif

/**
 * Get file basename without suffix.
 * FILESTEM("a/b/c.xxx") -> "c"
 *
 * TODO: Can someone find a better way than macro?
 */
#ifndef FILESTEM
#define FILESTEM(x)                                    \
  std::string_view(x).substr(                          \
      std::string_view(x).rfind(OS_PATH_SLASH) + 1,    \
      std::string_view(x).rfind('.') -                 \
          std::string_view(x).rfind(OS_PATH_SLASH) - 1 \
  )
#endif

/**
 * Get file basename.
 * BASENAME("a/b/c/d/e.xxx") -> "e.xxx"
 */
#ifndef BASENAME
#define BASENAME(x) \
  std::string_view(x).substr(std::string_view(x).rfind(OS_PATH_SLASH) + 1)
#endif

#endif  // CLIM_REFLECT_H_
