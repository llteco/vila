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
#ifndef TESTS_VILA_HOOK_DLL_TO_HOOK_H_
#define TESTS_VILA_HOOK_DLL_TO_HOOK_H_
#include <Windows.h>
#include <winnt.h>

#ifdef _WIN32
#ifdef DLL_EXPORT
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif
#else
#define DLL_API __attribute__((visibility("default")))
#endif

/// extern "C", WINAPI, and DLL_API all could be optional
DLL_API int sample_api();

EXTERN_C int WINAPI sample_api_stdcal();

int sample_api_def();

#endif  // TESTS_VILA_HOOK_DLL_TO_HOOK_H_
