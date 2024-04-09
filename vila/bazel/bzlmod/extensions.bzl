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

load("@pybind11_bazel//:python_configure.bzl", "python_configure")
load("@vila//vila/bazel/wdk:wdk_configure.bzl", "wdk_configure")
load("@vila//vila/bazel/toolchains:bullseye_cc_configure.bzl", "bullseye_configure")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def load_spdlog(ctx):
    spdlog_version = "1.13.0"

    # https://github.com/gabime/spdlog/releases
    http_archive(
        name = "spdlog",
        build_file = "@vila//vila/bazel:spdlog.BUILD",
        sha256 = "534f2ee1a4dcbeb22249856edfb2be76a1cf4f708a20b0ac2ed090ee24cfdbc9",
        strip_prefix = "spdlog-%s" % spdlog_version,
        url = "https://github.com/gabime/spdlog/archive/refs/tags/v%s.tar.gz" % spdlog_version,
    )

def load_hedron(ctx):
    # Hedron's Compile Commands Extractor for Bazel
    # https://github.com/hedronvision/bazel-compile-commands-extractor
    http_archive(
        name = "hedron_compile_commands",
        sha256 = "9c4ce757e0e53f8d69283968263cdee98d8f711191da3bdaa94875ea3ad8b601",
        strip_prefix = "bazel-compile-commands-extractor-f56c9e944474fc3a6aade106ff44a372ab8c84d2",
        url = "https://github.com/hedronvision/bazel-compile-commands-extractor/archive/f56c9e944474fc3a6aade106ff44a372ab8c84d2.zip",
    )

spdlog_extension = module_extension(
    implementation = load_spdlog,
)

hedron_extension = module_extension(
    implementation = load_hedron,
)

python_configure_extension = module_extension(
    implementation = lambda ctx: python_configure(name = "local_config_python"),
)

wdk_configure_extension = module_extension(
    implementation = lambda ctx: wdk_configure(name = "local_config_wdk"),
)

bullseye_configure_extension = module_extension(
    implementation = lambda ctx: bullseye_configure(name = "local_config_bullseye"),
)
