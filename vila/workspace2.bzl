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

load("@vila//vila/bazel:tvm_ffi_configure.bzl", "tvm_ffi_configure")
load("@vila//vila/bazel/toolchains:bullseye_cc_configure.bzl", "bullseye_configure")
load("@vila//vila/bazel/toolchains:sycl_cc_configure.bzl", "sycl_configure")
load("@vila//vila/bazel/wdk:wdk_configure.bzl", "wdk_configure")

def workspace(bullseye = False, sycl = False, tvm_ffi = False):
    """Loads a set of vila dependencies. To be used in a WORKSPACE file.

    Args:
        bullseye: Whether to include Bullseye Coverage.
        sycl: Whether to include oneAPI DPC++ compiler.
        tvm_ffi: Whether to include TVM FFI (requires: pip install apache-tvm-ffi).
    """

    # Get Bullseye Coverage tool
    if bullseye:
        bullseye_configure(name = "local_config_bullseye")

    # Get WDK package
    wdk_configure(name = "local_config_wdk")

    # Get oneAPI DPC++ compiler
    if sycl:
        sycl_configure(name = "local_config_sycl")

        # After bazel 6.4:
        # For extra copt needed for sycl, it is convenient to specify '--config=sycl'
        native.register_toolchains(
            "@local_config_sycl//:cc-toolchain-x64_sycl",
            # Comment out the following line to use the bullseye.
            # "@local_config_bullseye//:cc-toolchain-x64_windows",
        )

    # Get TVM FFI (optional, requires pip install apache-tvm-ffi)
    if tvm_ffi:
        tvm_ffi_configure(name = "tvm_ffi")
