"""
Copyright (c) 2025 Intel Corporation
Author: Wenyi Tang
E-mail: wenyi.tang@intel.com

Follow fmt/support/bazel/BUILD.bazel
"""

load("@vila//vila/bazel/vila:vila.bzl", "vila_cc_library")

package(default_visibility = ["//visibility:public"])

licenses(["notice"])

exports_files(["LICENSE.rst"])

vila_cc_library(
    name = "fmt",
    srcs = [
        #"src/fmt.cc", # No C++ module support, yet in Bazel (https://github.com/bazelbuild/bazel/pull/19940)
        "src/format.cc",
        "src/os.cc",
    ],
    hdrs = glob([
        "include/fmt/*.h",
    ]),
    copts = select({
        "@platforms//os:windows": ["-utf-8"],
        "//conditions:default": [],
    }),
    includes = [
        "include",
    ],
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
)
