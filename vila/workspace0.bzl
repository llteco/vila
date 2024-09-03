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
        sha256 = "bc283cdfcd526a52c3201279cda4bc298652efa898b10b4db0837dc51652756f",
        urls = [
            "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.7.1/bazel-skylib-1.7.1.tar.gz",
            "https://github.com/bazelbuild/bazel-skylib/releases/download/1.7.1/bazel-skylib-1.7.1.tar.gz",
        ],
    )

    # https://github.com/bazelbuild/rules_foreign_cc/releases
    foreign_cc_version = "0.12.0"
    http_archive(
        name = "rules_foreign_cc",
        sha256 = "a2e6fb56e649c1ee79703e99aa0c9d13c6cc53c8d7a0cbb8797ab2888bbc99a3",
        strip_prefix = "rules_foreign_cc-%s" % foreign_cc_version,
        url = "https://github.com/bazelbuild/rules_foreign_cc/archive/refs/tags/%s.tar.gz" % foreign_cc_version,
    )

    # https://github.com/pybind/pybind11_bazel/releases
    pybind11_bazel_version = "2.12.0"
    http_archive(
        name = "pybind11_bazel",
        sha256 = "dc14a960672babf6da2f283079a5b5c13e404a940ea7cdb8297b71f8f31643a5",
        strip_prefix = "pybind11_bazel-%s" % pybind11_bazel_version,
        url = "https://github.com/pybind/pybind11_bazel/archive/refs/tags/v%s.tar.gz" % pybind11_bazel_version,
    )

    # https://github.com/pybind/pybind11/releases
    pybind11_version = "2.13.5"
    http_archive(
        name = "pybind11",
        build_file = "@pybind11_bazel//:pybind11.BUILD",
        sha256 = "b1e209c42b3a9ed74da3e0b25a4f4cd478d89d5efbb48f04b277df427faf6252",
        strip_prefix = "pybind11-%s" % pybind11_version,
        url = "https://github.com/pybind/pybind11/archive/refs/tags/v%s.tar.gz" % pybind11_version,
    )
