"""
Copyright (C) 2025-2026 The VILA Authors.

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
    "@vila//vila/bazel/bzlmod:extensions.bzl",
    "load_hedron",
    "load_ittapi",
    "load_rangev3",
    "load_spdlog",
)

def workspace(gtest = True, benchmark = True, protobuf = True, fmt = True, spdlog = True, rangev3 = True, hedron = True, ittapi = True):
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
            build_file = "@vila//vila/bazel:fmt.BUILD",
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
