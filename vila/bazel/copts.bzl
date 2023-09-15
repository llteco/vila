"""
Copyright (c) 2023 Intel Corporation
Author: Wenyi Tang
E-mail: wenyi.tang@intel.com

Compile options
"""

_COPTS_MSVC = [
    # Enables standard C++ stack unwinding. Catches only standard C++ exceptions
    # when you use catch(...) syntax. Unless /EHc is also specified, the
    # compiler assumes that functions declared as extern "C" may throw a C++
    # exception.
    "/EHsc",
    "/GR",  # Enable run-time type information (RTTI)
    # Specify standards conformance mode to the compiler. Use this option to help
    # you identify and fix conformance issues in your code, to make it both more
    # correct and more portable.
    "/permissive-",
    "/D_WIN32_WINNT=0xA00",  # overwrite built-in 0x0601 which is old win7
    # These are security flags required by Intel policy
    "/sdl",
    "/GS",
    "/ZH:SHA_256",
    "/guard:cf",
    # "/Qspectre",
    # Disable noisy warnings
    "/wd4127",  # condition can be if constexpr
    "/wd4324",  # struct is aligned
    "/wd4819",  # non-unicode characters
]

_COPTS_LLVM = [
    "-Wno-macro-redefined",
    "-D_WIN32_WINNT=0xA00",  # overwrite built-in 0x0601 which is old win7
    "-Wno-comment",
    "-Wno-inconsistent-missing-override",
    "-Wno-switch",
    "-Wno-unused-but-set-variable",
    "-Wno-unused-command-line-argument",
    "-Wno-unused-function",
    "-Wno-unused-value",
    "-Wno-unused-variable",
    "-Wno-unknown-warning-option",
]

_LINKOPTS_MSVC = [
    "/DEBUG:FULL",
    "/guard:cf",
    "/DYNAMICBASE",
]

COPTS = select({
    # If --compiler=clang-cl is specified on windows to use LLVM clang.
    "@//:windows-clang-cl": _COPTS_LLVM + [
        "-Werror",
        # Use wchar_t to support unicode.
        # The Windows API will use *W version under this flag (On the opposite
        # they are *A version).
        "-D_UNICODE",
        "-DUNICODE",
        "/std:c++17",
    ],
    "@platforms//os:windows": _COPTS_MSVC + [
        "/std:c++17",
        # Set warning level to very strict and treat them as errors
        "/W4",
        # "/WX",
        # Use wchar_t to support unicode.
        # The Windows API will use *W version under this flag (On the opposite
        # they are *A version).
        "/D_UNICODE",
        "/DUNICODE",
        # /Z7 compiles debug information to object file (.obj), see
        # https://docs.microsoft.com/en-us/cpp/build/reference/z7-zi-zi-debug-information-format
        # /Zi would lead to an known bug of cl.exe, unless we compile bazel
        # with only one thread (-j=1). So we use /Z7 here.
        "/Z7",
    ],
    "@//:llvm-clang": _COPTS_LLVM + [
        "-Wall",
        "-std=c++17",
    ],
    "//conditions:default": ["-std=c++17"],
})

LINKOPTS = select({
    "@platforms//os:windows": _LINKOPTS_MSVC,
    "//conditions:default": [],
})
