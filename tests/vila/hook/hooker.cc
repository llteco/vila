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
#include <stdio.h>
#include <windows.h>

#include "detours.h"
#include "dll_to_hook.h"

static int(WINAPI* trueSampleApi)(void) = sample_api;

int WINAPI hookedSampleApi(void) {
  printf("hookedSampleApi Starting.\n");
  fflush(stdout);

  trueSampleApi();

  printf("hookedSampleApi done.\n");
  fflush(stdout);
  return 1;
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved) {
  LONG error;
  (void)hinst;
  (void)reserved;

  if (DetourIsHelperProcess()) {
    return TRUE;
  }

  if (dwReason == DLL_PROCESS_ATTACH) {
    DetourRestoreAfterWith();

    printf("hook starting!\n");
    fflush(stdout);

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&trueSampleApi, hookedSampleApi);
    error = DetourTransactionCommit();

    if (error == NO_ERROR) {
      printf("Detoured sample_api().\n");
    } else {
      printf("Error detouring sample_api(): %d\n", error);
    }
  } else if (dwReason == DLL_PROCESS_DETACH) {
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&(PVOID&)trueSampleApi, hookedSampleApi);
    error = DetourTransactionCommit();

    printf("Removed detour sample_api() (result=%d)\n", error);
    fflush(stdout);
  }

  return TRUE;
}
