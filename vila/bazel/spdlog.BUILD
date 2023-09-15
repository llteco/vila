"""
Copyright (c) 2022 Intel Corporation
Author: Wenyi Tang
E-mail: wenyi.tang@intel.com

"""

package(default_visibility = ["//visibility:public"])

licenses(["notice"])

exports_files(["LICENSE"])

cc_library(
    name = "spdlog",
    hdrs = glob(
        ["include/spdlog/**/*.h"],
        exclude = ["include/spdlog/fmt/bundled/*.h"],
    ),
    defines = ["SPDLOG_FMT_EXTERNAL"] + select({
        "@platforms//os:windows": [
            "SPDLOG_WCHAR_TO_UTF8_SUPPORT",
            "SPDLOG_WCHAR_FILENAMES",
        ],
        "//conditions:default": [],
    }),
    includes = ["include"],
    deps = ["@fmt"],
)
