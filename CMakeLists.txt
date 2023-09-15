# INTEL CONFIDENTIAL
#
# Copyright (C) 2023 Intel Corporation. All Rights Reserved.
#
# The source code contained or described herein and all documents
# related to the source code ("Material") are owned by Intel Corporation
# or licensors. Title to the Material remains with Intel
# Corporation or its licensors. The Material contains trade
# secrets and proprietary and confidential information of Intel or its
# licensors. The Material is protected by worldwide copyright
# and trade secret laws and treaty provisions. No part of the Material may
# be used, copied, reproduced, modified, published, uploaded, posted,
# transmitted, distributed, or disclosed in any way without Intel's prior
# express written permission.
#
# No License under any patent, copyright, trade secret or other intellectual
# property right is granted to or conferred upon you by disclosure or
# delivery of the Materials, either expressly, by implication, inducement,
# estoppel or otherwise. Any license under such intellectual property rights
# must be express and approved by Intel in writing.
cmake_minimum_required(VERSION 3.18)

project(vila CXX)

include_directories(${CMAKE_SOURCE_DIR})
if(MSVC)
    add_compile_options("/std:c++17")
    add_compile_definitions(
        NOMINMAX
        SPDLOG_WCHAR_TO_UTF8_SUPPORT
        SPDLOG_WCHAR_FILENAMES
    )
else()
    add_compile_options("-std=c++17")
endif()
add_compile_definitions(SPDLOG_FMT_EXTERNAL)
set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "use external fmtlib" FORCE)

add_library(clim INTERFACE)
target_include_directories(clim INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(clim INTERFACE cxx_std_17)
add_library(vila::clim ALIAS clim)

add_subdirectory(fmt)
add_subdirectory(spdlog)
add_subdirectory(vila)
