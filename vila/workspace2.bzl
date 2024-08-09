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

load("@vila//vila/bazel/toolchains:bullseye_cc_configure.bzl", "bullseye_configure")
load("@vila//vila/bazel/toolchains:icpx_cc_configure.bzl", "icpx_configure")
load("@vila//vila/bazel/wdk:wdk_configure.bzl", "wdk_configure")

def workspace():
    """Loads a set of vila dependencies. To be used in a WORKSPACE file.
    """

    # Get Bullseye Coverage tool
    bullseye_configure(name = "local_config_bullseye")

    # Get WDK package
    wdk_configure(name = "local_config_wdk")

    # Get oneAPI DPC++ compiler
    icpx_configure(name = "local_config_icpx")

    # After bazel 6.4:
    # Comment out the following line to use the icpx toolchain.
    # For extra copt needed for icpx, it is convenient to specify '--config=icpx'
    # native.register_toolchains("@local_config_icpx//:cc-toolchain-x64_windows")

    # native.register_toolchains("@local_config_bullseye//:cc-toolchain-x64_windows")
