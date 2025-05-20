"""
INTEL CONFIDENTIAL

Copyright (C) 2025 Intel Corporation. All Rights Reserved.

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

    # https://github.com/bazel-contrib/bazel_features
    bazel_features_version = "1.29.0"
    http_archive(
        name = "bazel_features",
        integrity = "sha256-2FLzic6NuLjC+YB6T68GWw0LowIWOJjNJCi2yn0IZoE=",
        strip_prefix = "bazel_features-%s" % bazel_features_version,
        url = "https://github.com/bazel-contrib/bazel_features/archive/refs/tags/v%s.tar.gz" % bazel_features_version,
    )

    # https://github.com/bazelbuild/rules_cc/releases
    rules_cc_version = "0.0.14"  # this is the last working version for legacy workspace
    http_archive(
        name = "rules_cc",
        integrity = "sha256-kG6JKGrMZ8IIGcPIizKD3g1YaK/aM2NdcKyuDel3e7c=",
        strip_prefix = "rules_cc-%s" % rules_cc_version,
        url = "https://github.com/bazelbuild/rules_cc/archive/refs/tags/%s.tar.gz" % rules_cc_version,
    )

    # https://github.com/bazelbuild/rules_foreign_cc/releases
    foreign_cc_version = "0.14.0"
    http_archive(
        name = "rules_foreign_cc",
        integrity = "sha256-4PDrsaIiPJmpBKVl5iqihb8dGortoi0Q6iEnWRYkhmw=",
        strip_prefix = "rules_foreign_cc-%s" % foreign_cc_version,
        url = "https://github.com/bazelbuild/rules_foreign_cc/archive/refs/tags/%s.tar.gz" % foreign_cc_version,
    )

    # https://github.com/bazelbuild/rules_python/releases
    rules_python_version = "1.4.1"
    http_archive(
        name = "rules_python",
        integrity = "sha256-n587MAqSZOTHeZkxLOZjvl3umlbjYaH2/n7GDhvu+aM=",
        strip_prefix = "rules_python-%s" % rules_python_version,
        url = "https://github.com/bazelbuild/rules_python/archive/refs/tags/%s.tar.gz" % rules_python_version,
    )

    # https://github.com/pybind/pybind11_bazel/releases
    pybind11_version = "2.13.6"
    http_archive(
        name = "pybind11_bazel",
        integrity = "sha256-yuaAZwv6boJwPAPyo8mVQIzcv0NhbXvdGY70XTwydzE=",
        strip_prefix = "pybind11_bazel-%s" % pybind11_version,
        url = "https://github.com/pybind/pybind11_bazel/archive/refs/tags/v%s.tar.gz" % pybind11_version,
    )

    # https://github.com/pybind/pybind11/releases
    pybind11_minor = int(pybind11_version.split(".")[1])
    http_archive(
        name = "pybind11",
        build_file = "@pybind11_bazel//:%s" % ("pybind11.BUILD" if pybind11_minor <= 11 else "pybind11-BUILD.bazel"),
        integrity = "sha256-4Iy4f0dz2pf6e18DXeh2OrxlbYfVdz5i9toFh9Hw7CA=",
        strip_prefix = "pybind11-%s" % pybind11_version,
        url = "https://github.com/pybind/pybind11/archive/refs/tags/v%s.tar.gz" % pybind11_version,
    )
