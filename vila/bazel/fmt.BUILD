"""
Copyright (c) 2023 Intel Corporation
Author: Wenyi Tang
E-mail: wenyi.tang@intel.com

"""

load("@vila//vila/bazel/vila:vila.bzl", "vila_cc_library")

package(default_visibility = ["//visibility:public"])

licenses(["notice"])

exports_files(["LICENSE.rst"])

vila_cc_library(
    name = "fmt",
    srcs = [
        ":src/format.cc",
        ":src/os.cc",
    ],
    hdrs = glob(["include/fmt/*.h"]),
    defines = ["FMT_UNICODE=1"],
    includes = ["include"],
    strip_include_prefix = "include",
)
