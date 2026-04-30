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
load(
    "@vila//bazel/bzlmod:extensions.bzl",
    "load_hedron",
    "load_ittapi",
    "load_rangev3",
    "load_spdlog",
)

def workspace(
        gtest = True,
        benchmark = True,
        protobuf = True,
        fmt = True,
        spdlog = True,
        rangev3 = True,
        hedron = True,
        ittapi = True,
        pybind11 = True,
        nanobind = True):
    """Loads a set of vila dependencies. To be used in a WORKSPACE file.

    Args:
        gtest: Whether to include Google Test.
        benchmark: Whether to include Google Benchmark.
        protobuf: Whether to include Protocol Buffers.
        fmt: Whether to include {fmt}.
        spdlog: Whether to include spdlog.
        rangev3: Whether to include range-v3.
        hedron: Whether to include Hedron.
        ittapi: Whether to include Intel ITT API.
        pybind11: Whether to include Pybind11.
        nanobind: Whether to include nanobind.
    """

    # https://github.com/google/googletest/releases
    gtest_version = "1.17.0"
    if gtest:
        http_archive(
            name = "com_google_googletest",
            integrity = "sha256-Zfq3AdmCnTjLd8FKzcQx0hCL/b+JeeQOuK5Wft8Qsnw=",
            strip_prefix = "googletest-%s" % gtest_version,
            url = "https://github.com/google/googletest/archive/refs/tags/v%s.tar.gz" % gtest_version,
        )

    # https://github.com/google/benchmark/releases
    benchmark_version = "1.9.2"
    if benchmark:
        http_archive(
            name = "com_google_benchmark",
            integrity = "sha256-QJB1F2Fo3Ea7uBt0wbS2kAOFtdFr/BgdZ4r7Bg2Si9M=",
            strip_prefix = "benchmark-%s" % benchmark_version,
            url = "https://github.com/google/benchmark/archive/refs/tags/v%s.tar.gz" % benchmark_version,
        )

    # https://github.com/protocolbuffers/protobuf/releases
    protobuf_version = "29.4"
    if protobuf:
        http_archive(
            name = "com_google_protobuf",
            integrity = "sha256-a9ncyRsX7yXCat+G23HGfsAkMdyS6Vier4LiKIkjBJY=",
            strip_prefix = "protobuf-%s" % protobuf_version,
            url = "https://github.com/google/protobuf/archive/refs/tags/v%s.tar.gz" % protobuf_version,
        )

    # https://github.com/fmtlib/fmt/releases
    fmt_version = "12.0.0"
    if fmt:
        http_archive(
            name = "fmt",
            build_file = "@vila//bazel:fmt.BUILD",
            integrity = "sha256-qj6Pu2oAZsA0VENK3R8fwjKZ6FdYzuwNfS2XRDFIHkA=",
            strip_prefix = "fmt-%s" % fmt_version,
            url = "https://github.com/fmtlib/fmt/archive/refs/tags/%s.tar.gz" % fmt_version,
        )

    # https://github.com/gabime/spdlog/releases
    if spdlog:
        load_spdlog(None)

    # # https://github.com/abseil/abseil-cpp/releases
    # http_archive(
    #     name = "abseil-cpp",
    #     integrity = "sha256-czcmuMOm05pBINfkXqi0GkNM2s3kAculAPFCNsSbOdw=",
    #     strip_prefix = "abseil-cpp-20240116.2",
    #     url = "https://github.com/abseil/abseil-cpp/archive/refs/tags/20240116.2.tar.gz",
    # )

    # https://github.com/ericniebler/range-v3
    if rangev3:
        load_rangev3(None)

    # Hedron's Compile Commands Extractor for Bazel
    # https://github.com/hedronvision/bazel-compile-commands-extractor
    if hedron:
        load_hedron(None)

    # Intel® Instrumentation and Tracing Technology (ITT) and Just-In-Time (JIT) API
    # https://github.com/intel/ittapi
    if ittapi:
        load_ittapi(None)

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
""",
        )

        http_archive(
            name = "nanobind",
            build_file = "@nanobind_bazel//:nanobind.BUILD",
            integrity = "sha256-W7f4ZvbJxkQFMItp3n52gdj3eTI+NFvXGgAZnB6uwHM=",
            strip_prefix = "nanobind-%s" % nanobind_version,
            url = "https://github.com/wjakob/nanobind/archive/refs/tags/v%s.tar.gz" % nanobind_version,
        )
