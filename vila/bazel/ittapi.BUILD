"""
Copyright (c) 2025-2026 Wenyi Tang
Author: Wenyi Tang
E-mail: wenyitang@outlook.com
"""

load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "ittnotify",
    srcs = glob([
        "src/ittnotify/*.c",
        "src/ittnotify/*.h",
    ]),
    hdrs = [
        "include/ittnotify.h",
        "include/jitprofiling.h",
        "include/libittnotify.h",
    ],
    defines = ["VILA_ITT_ENABLED=1"],
    includes = ["include"],
)
