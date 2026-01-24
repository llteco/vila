"""
Copyright (C) 2026 The VILA Authors.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
"""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def workspace(skylib = True, rules_cc = True, rules_foreign_cc = True, rules_python = True, pybind11 = True, nanobind = True):
    """Declare basic workspace dependency

    Args:
        skylib: Whether to include Bazel Skylib.
        rules_cc: Whether to include Bazel Rules for C/C++.
        rules_foreign_cc: Whether to include Bazel Rules for Foreign C/C++.
        rules_python: Whether to include Bazel Rules for Python.
        pybind11: Whether to include Pybind11.
        nanobind: Whether to include nanobind.
    """

    # https://github.com/bazelbuild/bazel-skylib/releases
    skylib_version = "1.9.0"
    if skylib:
        http_archive(
            name = "bazel_skylib",
            sha256 = "3b5b49006181f5f8ff626ef8ddceaa95e9bb8ad294f7b5d7b11ea9f7ddaf8c59",
            urls = [
                "https://github.com/bazelbuild/bazel-skylib/releases/download/{0}/bazel-skylib-{0}.tar.gz".format(skylib_version),
            ],
        )

    # https://github.com/bazel-contrib/bazel_features
    bazel_features_version = "1.36.0"
    http_archive(
        name = "bazel_features",
        integrity = "sha256-2FLzic6NuLjC+YB6T68GWw0LowIWOJjNJCi2yn0IZoE=",
        strip_prefix = "bazel_features-%s" % bazel_features_version,
        url = "https://github.com/bazel-contrib/bazel_features/archive/refs/tags/v%s.tar.gz" % bazel_features_version,
    )

    # https://github.com/bazelbuild/rules_cc/releases
    rules_cc_version = "0.0.14"  # this is the last working version for legacy workspace
    if rules_cc:
        http_archive(
            name = "rules_cc",
            integrity = "sha256-kG6JKGrMZ8IIGcPIizKD3g1YaK/aM2NdcKyuDel3e7c=",
            strip_prefix = "rules_cc-%s" % rules_cc_version,
            url = "https://github.com/bazelbuild/rules_cc/archive/refs/tags/%s.tar.gz" % rules_cc_version,
        )

    # https://github.com/bazelbuild/rules_foreign_cc/releases
    foreign_cc_version = "0.14.0"
    if rules_foreign_cc:
        http_archive(
            name = "rules_foreign_cc",
            integrity = "sha256-4PDrsaIiPJmpBKVl5iqihb8dGortoi0Q6iEnWRYkhmw=",
            strip_prefix = "rules_foreign_cc-%s" % foreign_cc_version,
            url = "https://github.com/bazelbuild/rules_foreign_cc/archive/refs/tags/%s.tar.gz" % foreign_cc_version,
        )

    # https://github.com/bazelbuild/rules_python/releases
    rules_python_version = "1.8.1"
    if rules_python:
        http_archive(
            name = "rules_python",
            integrity = "sha256-euJcDTtSEk//4ZmjRSD0PklvQCfVlFLfcBhOztI7lu8=",
            strip_prefix = "rules_python-%s" % rules_python_version,
            url = "https://github.com/bazelbuild/rules_python/archive/refs/tags/%s.tar.gz" % rules_python_version,
        )

    # https://github.com/pybind/pybind11_bazel/releases
    pybind11_version = "3.0.0"
    if pybind11:
        http_archive(
            name = "pybind11_bazel",
            integrity = "sha256-DS8PvRhMzZS4UpQ/kSw96H5f9jJbqcN+r+f0eTAmP9w=",
            strip_prefix = "pybind11_bazel-%s" % pybind11_version,
            url = "https://github.com/pybind/pybind11_bazel/archive/refs/tags/v%s.tar.gz" % pybind11_version,
        )

        # https://github.com/pybind/pybind11/releases
        pybind11_major = int(pybind11_version.split(".")[0])
        pybind11_minor = int(pybind11_version.split(".")[1])
        http_archive(
            name = "pybind11",
            build_file = "@pybind11_bazel//:%s" % ("pybind11.BUILD" if (pybind11_major <= 2 and pybind11_minor <= 11) else "pybind11-BUILD.bazel"),
            integrity = "sha256-RTsaPismbDrp2ockEcrbbWk6wYBjvXMibZbPtwFaIAw=",
            strip_prefix = "pybind11-%s" % pybind11_version,
            url = "https://github.com/pybind/pybind11/archive/refs/tags/v%s.tar.gz" % pybind11_version,
        )

    if nanobind:
        nanobind_version = "2.10.2"
        http_archive(
            name = "nanobind_bazel",
            integrity = "sha256-QJeB39Bx/m2i79HP+z9okTWEN5t4CcIRwFzJapnJmoY=",
            strip_prefix = "nanobind-bazel-%s" % nanobind_version,
            url = "https://github.com/nicholasjng/nanobind-bazel/archive/refs/tags/v%s.tar.gz" % nanobind_version,
        )

        robin_map_version = "1.4.0"
        http_archive(
            name = "robin-map",
            integrity = "sha256-eTDb+WNKz8Amhth/YVwPTzMTWUgTC4kiMxwW2QoDJQw=",
            strip_prefix = "robin-map-%s" % robin_map_version,
            url = "https://github.com/Tessil/robin-map/archive/refs/tags/v%s.tar.gz" % robin_map_version,
            build_file_content = """load("@rules_cc//cc:cc_library.bzl", "cc_library")

config_setting(
    name = "msvc_compiler",
    flag_values = {"@bazel_tools//tools/cpp:compiler": "msvc-cl"},
)

cc_library(
    name = "robin-map",
    hdrs = glob(["include/tsl/*.h"]),
    copts = select({
        ":msvc_compiler": ["/std:c++17"],
        "//conditions:default": ["--std=c++17"],
    }),
    strip_include_prefix = "include/",
    visibility = ["//visibility:public"],
)
"""
        )

        http_archive(
            name = "nanobind",
            build_file = "@nanobind_bazel//:nanobind.BUILD",
            integrity = "sha256-W7f4ZvbJxkQFMItp3n52gdj3eTI+NFvXGgAZnB6uwHM=",
            strip_prefix = "nanobind-%s" % nanobind_version,
            url = "https://github.com/wjakob/nanobind/archive/refs/tags/v%s.tar.gz" % nanobind_version,
        )
