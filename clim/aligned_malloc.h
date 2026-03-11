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
 * Description: Allocates memory on a specified alignment boundary.
 ****************************************/
#ifndef CLIM_ALIGNED_MALLOC_H_
#define CLIM_ALIGNED_MALLOC_H_
#include <cstdint>

#ifdef _WIN32
#include <Windows.h>

/**
 * @brief Allocates memory on a specified alignment boundary.
 *
 * @tparam T: pointer type returned for convenience
 * @param size: Size of the requested memory allocation.
 * @param page_size: The alignment value, which must be an integer power of 2.
 * @return T: A pointer to the memory block that was allocated or NULL if the
 *            operation failed. The pointer is a multiple of alignment.
 */
template <class T = uint8_t*>
inline T aligned_malloc(size_t size, size_t page_size = 0x1000) {  // NOLINT
  return reinterpret_cast<T>(_aligned_malloc(size, page_size));
}

/**
 * @brief Frees a block of memory that was allocated with aligned_malloc.
 *
 * @param p: A pointer to the memory block that was returned to the
 *           aligned_malloc.
 */
inline void aligned_free(void* p) { _aligned_free(p); }  // NOLINT
#else
#include <malloc.h>
#include <stdlib.h>

/**
 * @brief Allocates memory on a specified alignment boundary.
 *
 * @tparam T: pointer type returned for convenience
 * @param size: Size of the requested memory allocation.
 * @param page_size: The alignment value, which must be an integer power of 2.
 * @return T: A pointer to the memory block that was allocated or NULL if the
 *            operation failed. The pointer is a multiple of alignment.
 */
template <class T = uint8_t*>
inline T aligned_malloc(size_t size, size_t page_size = 0x1000) {  // NOLINT
  return reinterpret_cast<T>(memalign(page_size, size));
}

/**
 * @brief Frees a block of memory that was allocated with aligned_malloc.
 *
 * @param p: A pointer to the memory block that was returned to the
 *           aligned_malloc.
 */
inline void aligned_free(void* p) { free(p); }  // NOLINT
#endif
#endif  // CLIM_ALIGNED_MALLOC_H_
