"""
Copyright (c) 2022-2026 Wenyi Tang
Author: Wenyi Tang
E-mail: wenyitang@outlook.com
"""

load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

licenses(["notice"])

exports_files(["LICENSE"])

cc_library(
    name = "spdlog_wchar_support",
    hdrs = glob(
        ["include/spdlog/**/*.h"],
        exclude = ["include/spdlog/fmt/bundled/*.h"],
    ),
    defines = ["SPDLOG_FMT_EXTERNAL"] + [
        "SPDLOG_WCHAR_TO_UTF8_SUPPORT",
        "SPDLOG_WCHAR_FILENAMES",
    ],
    includes = ["include"],
    target_compatible_with = ["@platforms//os:windows"],
    deps = ["@fmt"],
)

cc_library(
    name = "spdlog_no_wchar",
    hdrs = glob(
        ["include/spdlog/**/*.h"],
        exclude = ["include/spdlog/fmt/bundled/*.h"],
    ),
    defines = ["SPDLOG_FMT_EXTERNAL"],
    includes = ["include"],
    deps = ["@fmt"],
)

cc_library(
    name = "spdlog",
    deps = select({
        "@platforms//os:windows": [":spdlog_wchar_support"],
        "//conditions:default": [":spdlog_no_wchar"],
    }),
)
