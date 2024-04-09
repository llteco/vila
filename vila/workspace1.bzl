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
    http_archive(
        name = "com_google_googletest",
        sha256 = "ad7fdba11ea011c1d925b3289cf4af2c66a352e18d4c7264392fead75e919363",
        strip_prefix = "googletest-1.13.0",
        urls = ["https://github.com/google/googletest/archive/refs/tags/v1.13.0.tar.gz"],
    )

    http_archive(
        name = "com_google_benchmark",
        sha256 = "2aab2980d0376137f969d92848fbb68216abb07633034534fc8c65cc4e7a0e93",
        strip_prefix = "benchmark-1.8.2",
        urls = ["https://github.com/google/benchmark/archive/refs/tags/v1.8.2.tar.gz"],
    )

    # https://github.com/fmtlib/fmt/releases
    http_archive(
        name = "fmt",
        build_file = "@vila//vila/bazel:fmt.BUILD",
        sha256 = "1250e4cc58bf06ee631567523f48848dc4596133e163f02615c97f78bab6c811",
        strip_prefix = "fmt-10.2.1",
        url = "https://github.com/fmtlib/fmt/archive/refs/tags/10.2.1.tar.gz",
    )

    # https://github.com/gabime/spdlog/releases
    http_archive(
        name = "spdlog",
        build_file = "@vila//vila/bazel:spdlog.BUILD",
        sha256 = "534f2ee1a4dcbeb22249856edfb2be76a1cf4f708a20b0ac2ed090ee24cfdbc9",
        strip_prefix = "spdlog-1.13.0",
        url = "https://github.com/gabime/spdlog/archive/refs/tags/v1.13.0.tar.gz",
    )

    # Hedron's Compile Commands Extractor for Bazel
    # https://github.com/hedronvision/bazel-compile-commands-extractor
    http_archive(
        name = "hedron_compile_commands",
        sha256 = "9c4ce757e0e53f8d69283968263cdee98d8f711191da3bdaa94875ea3ad8b601",
        strip_prefix = "bazel-compile-commands-extractor-f56c9e944474fc3a6aade106ff44a372ab8c84d2",
        url = "https://github.com/hedronvision/bazel-compile-commands-extractor/archive/f56c9e944474fc3a6aade106ff44a372ab8c84d2.zip",
    )
