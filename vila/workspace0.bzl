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
    # https://github.com/bazelbuild/bazel-skylib/releases
    http_archive(
        name = "bazel_skylib",
        sha256 = "cd55a062e763b9349921f0f5db8c3933288dc8ba4f76dd9416aac68acee3cb94",
        urls = [
            "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.5.2/bazel-skylib-1.5.2.tar.gz",
            "https://github.com/bazelbuild/bazel-skylib/releases/download/1.5.0/bazel-skylib-1.5.0.tar.gz",
        ],
    )

    # https://github.com/bazelbuild/rules_foreign_cc/releases
    http_archive(
        name = "rules_foreign_cc",
        sha256 = "476303bd0f1b04cc311fc258f1708a5f6ef82d3091e53fd1977fa20383425a6a",
        strip_prefix = "rules_foreign_cc-0.10.1",
        url = "https://github.com/bazelbuild/rules_foreign_cc/releases/download/0.10.1/rules_foreign_cc-0.10.1.tar.gz",
    )

    http_archive(
        name = "pybind11_bazel",
        sha256 = "2d3316d89b581966fc11eab9aa9320276baee95c8233c7a8efc7158623a48de0",
        strip_prefix = "pybind11_bazel-ff261d2e9190955d0830040b20ea59ab9dbe66c8",
        urls = ["https://github.com/pybind/pybind11_bazel/archive/ff261d2e9190955d0830040b20ea59ab9dbe66c8.zip"],
    )

    # https://github.com/pybind/pybind11/releases
    http_archive(
        name = "pybind11",
        build_file = "@pybind11_bazel//:pybind11.BUILD",
        sha256 = "d475978da0cdc2d43b73f30910786759d593a9d8ee05b1b6846d1eb16c6d2e0c",
        strip_prefix = "pybind11-2.11.1",
        url = "https://github.com/pybind/pybind11/archive/refs/tags/v2.11.1.tar.gz",
    )
