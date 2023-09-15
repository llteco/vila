"""
INTEL CONFIDENTIAL

Copyright (C) 2023 Intel Corporation. All Rights Reserved.

The source code contained or described herein and all documents
related to the source code ("Material") are owned by Intel Corporation
or licensors. Title to the Material remains with Intel
Corporation or its licensors. The Material contains trade
secrets and proprietary and confidential information of Intel or its
licensors. The Material is protected by worldwide copyright
and trade secret laws and treaty provisions. No part of the Material may
be used, copied, reproduced, modified, published, uploaded, posted,
transmitted, distributed, or disclosed in any way without Intel's prior
express written permission.

No License under any patent, copyright, trade secret or other intellectual
property right is granted to or conferred upon you by disclosure or
delivery of the Materials, either expressly, by implication, inducement,
estoppel or otherwise. Any license under such intellectual property rights
must be express and approved by Intel in writing.
"""

CXX_STD_14 = select({
    "@platforms//os:windows": ["/std:c++14"],
    "//conditions:default": ["-std=c++14"],
})

CXX_STD_17 = select({
    "@platforms//os:windows": ["/std:c++17"],
    "//conditions:default": ["-std=c++17"],
})

MSVC_COPTS = [
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
    # "/Qspectre",  # spectre mitigation
    # Disable noisy warnings
    "/wd4127",  # condition can be if constexpr
    "/wd4324",  # struct is aligned
    "/wd4819",  # non-unicode characters
]

CLANG_COPTS = [
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

MSVC_LINKOPTS = [
    # "/DEBUG:FULL",  # integrate debug symbol into binaries, will increase footprint
    "/guard:cf",
    "/DYNAMICBASE",
    "/DEFAULTLIB:onecore.lib",
]

DEFAULT_COPTS = select({
    # If --compiler=clang-cl is specified on windows to use LLVM clang.
    "@vila//vila:windows-clang-cl": CLANG_COPTS + [
        "-Werror",
        # Use wchar_t to support unicode.
        # The Windows API will use *W version under this flag (On the opposite
        # they are *A version).
        "-D_UNICODE",
        "-DUNICODE",
    ],
    "@platforms//os:windows": MSVC_COPTS + [
        # Set warning level to very strict and treat them as errors
        "/W4",
        "/WX",
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
    "@vila//vila:llvm-clang": CLANG_COPTS + [
        "-Wall",
    ],
    "//conditions:default": [],
}) + CXX_STD_17

DEFAULT_LINKOPTS = select({
    "@platforms//os:windows": MSVC_LINKOPTS,
    "//conditions:default": [],
})
