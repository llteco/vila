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
 * Description: Linux's memcpy_s
 ****************************************/
#ifndef CLIM_LINUX_MEMCPY_S_H_
#define CLIM_LINUX_MEMCPY_S_H_
#include <cstring>

#ifndef _WIN32
/// NOLINTNEXTLINE(readability-identifier-naming)
inline void memcpy_s(
    void* dst, size_t dst_size, const void* src, size_t src_size
) {
  if (src_size < dst_size) {
    std::memcpy(dst, src, src_size);
  } else {
    std::memcpy(dst, src, dst_size);
  }
}
#endif

/// NOLINTNEXTLINE(readability-identifier-naming)
inline void truncated_memcpy(
    void* dst, size_t dst_size, const void* src, size_t src_size
) {
  if (src_size < dst_size) {
    memcpy_s(dst, dst_size, src, src_size);
  } else {
    memcpy_s(dst, dst_size, src, dst_size);
  }
}
#endif  // CLIM_LINUX_MEMCPY_S_H_
