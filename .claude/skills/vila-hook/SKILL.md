---
name: vila-hook
description: Guidance for using the vila/hook component - Windows DLL hooking via Microsoft Detours library. Use this skill when hooking Windows API functions, intercepting DLL calls, or using Detours for function interception on Windows only.
---

# VILA Hook - Windows DLL Hooking

Windows-only component for function hooking using Microsoft Detours.

## Headers

```cpp
#include "vila/hook/detours/detours.h"  // Detours library wrapper
```

## Bazel Target

```bazel
deps = ["//vila/hook"]
# Note: Only builds on Windows (@platforms//os:windows)
```

## Dependencies

- `vila/hook/detours/` contains Microsoft Detours library (MIT license)
- Windows-only: uses `target_compatible_with = ["@platforms//os:windows"]`

## Usage

Detours is Microsoft's library for binary function interception:

```cpp
#include "vila/hook/detours/detours.h"

// Target function to hook
int OriginalFunction(int arg);

// Hook function (replacement)
int HookedFunction(int arg) {
    // Do something before original
    int result = OriginalFunction(arg);  // call original
    // Do something after
    return result;
}

// Install hook
DetourTransactionBegin();
DetourUpdateThread(GetCurrentThread());
DetourAttach(&(PVOID&)OriginalFunction, HookedFunction);
DetourTransactionCommit();

// Remove hook
DetourTransactionBegin();
DetourUpdateThread(GetCurrentThread());
DetourDetach(&(PVOID&)OriginalFunction, HookedFunction);
DetourTransactionCommit();
```

## Hooking Windows API

```cpp
#include <Windows.h>
#include "vila/hook/detours/detours.h"

// Original function pointer
static BOOL (WINAPI* OriginalCreateFileW)(
    LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE
) = CreateFileW;

// Hook function
BOOL WINAPI HookedCreateFileW(
    LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes, HANDLE hTemplateFile
) {
    LOGI("CreateFileW called: {}", lpFileName);
    return OriginalCreateFileW(
        lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
        dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile
    );
}

// Install
void InstallHooks() {
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)OriginalCreateFileW, HookedCreateFileW);
    DetourTransactionCommit();
}
```

## Transaction Pattern

Detours uses transaction-based hook installation for safety:

```cpp
// Multiple hooks in single transaction
DetourTransactionBegin();
DetourUpdateThread(GetCurrentThread());

DetourAttach(&(PVOID&)Func1, HookFunc1);
DetourAttach(&(PVOID&)Func2, HookFunc2);
DetourAttach(&(PVOID&)Func3, HookFunc3);

LONG error = DetourTransactionCommit();
if (error != NO_ERROR) {
    // Handle error - hooks not installed
}
```

## Notes

- Detours can hook any function with known signature
- Works on both x86 and x64 Windows
- Original function pointer must be preserved for calling through
- Hook transactions are atomic - either all succeed or none
- Thread-safe: `DetourUpdateThread` ensures correct thread state
