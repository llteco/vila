"""
Copyright (c) 2025 Intel Corporation
Author: Wenyi Tang
E-mail: wenyi.tang@intel.com

"""

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
