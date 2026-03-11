/************************************************************************
 * Copyright (C) 2026 The VILA Authors.
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
 ************************************************************************/

/// @brief A sample API that does nothing but return 0.
#include "dll_to_hook.h"

#include <cstdio>

DLL_API int sample_api() {
  printf("%s+\n", __FUNCTION__);
  return 0;
}

EXTERN_C int WINAPI sample_api_stdcal() {
  printf("%s+\n", __FUNCTION__);
  return 0;
}

int sample_api_def() {
  printf("%s+\n", __FUNCTION__);
  return 0;
}
