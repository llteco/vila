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

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def workspace():
    """Loads a set of vila dependencies. To be used in a WORKSPACE file.
    """

    # https://github.com/google/googletest/releases
    gtest_version = "1.15.2"
    http_archive(
        name = "com_google_googletest",
        sha256 = "7b42b4d6ed48810c5362c265a17faebe90dc2373c885e5216439d37927f02926",
        strip_prefix = "googletest-%s" % gtest_version,
        url = "https://github.com/google/googletest/archive/refs/tags/v%s.tar.gz" % gtest_version,
    )

    # https://github.com/google/benchmark/releases
    benchmark_version = "1.9.0"
    http_archive(
        name = "com_google_benchmark",
        sha256 = "35a77f46cc782b16fac8d3b107fbfbb37dcd645f7c28eee19f3b8e0758b48994",
        strip_prefix = "benchmark-%s" % benchmark_version,
        url = "https://github.com/google/benchmark/archive/refs/tags/v%s.tar.gz" % benchmark_version,
    )

    # https://github.com/fmtlib/fmt/releases
    fmt_version = "11.0.2"
    http_archive(
        name = "fmt",
        build_file = "@vila//vila/bazel:fmt.BUILD",
        sha256 = "6cb1e6d37bdcb756dbbe59be438790db409cdb4868c66e888d5df9f13f7c027f",
        strip_prefix = "fmt-%s" % fmt_version,
        url = "https://github.com/fmtlib/fmt/archive/refs/tags/%s.tar.gz" % fmt_version,
    )

    # https://github.com/gabime/spdlog/releases
    spdlog_version = "1.14.1"
    http_archive(
        name = "spdlog",
        build_file = "@vila//vila/bazel:spdlog.BUILD",
        sha256 = "1586508029a7d0670dfcb2d97575dcdc242d3868a259742b69f100801ab4e16b",
        strip_prefix = "spdlog-%s" % spdlog_version,
        url = "https://github.com/gabime/spdlog/archive/refs/tags/v%s.tar.gz" % spdlog_version,
    )

    # # https://github.com/abseil/abseil-cpp/releases
    # http_archive(
    #     name = "abseil-cpp",
    #     integrity = "sha256-czcmuMOm05pBINfkXqi0GkNM2s3kAculAPFCNsSbOdw=",
    #     strip_prefix = "abseil-cpp-20240116.2",
    #     url = "https://github.com/abseil/abseil-cpp/archive/refs/tags/20240116.2.tar.gz",
    # )

    # https://github.com/ericniebler/range-v3
    rangev3_version = "53c40dd628450c977ee1558285ff43e0613fa7a9"
    http_archive(
        name = "rangev3",
        integrity = "sha256-3uvT1HqOcMb2maFCPul05Ku/AWqCAZEdYFQe4eRWFFQ=",
        strip_prefix = "range-v3-%s" % rangev3_version,
        url = "https://github.com/ericniebler/range-v3/archive/%s.zip" % rangev3_version,
    )

    # Hedron's Compile Commands Extractor for Bazel
    # https://github.com/hedronvision/bazel-compile-commands-extractor
    http_archive(
        name = "hedron_compile_commands",
        sha256 = "9c4ce757e0e53f8d69283968263cdee98d8f711191da3bdaa94875ea3ad8b601",
        strip_prefix = "bazel-compile-commands-extractor-f56c9e944474fc3a6aade106ff44a372ab8c84d2",
        url = "https://github.com/hedronvision/bazel-compile-commands-extractor/archive/f56c9e944474fc3a6aade106ff44a372ab8c84d2.zip",
    )
