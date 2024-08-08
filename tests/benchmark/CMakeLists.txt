# INTEL CONFIDENTIAL
#
# Copyright (C) 2024 Intel Corporation. All Rights Reserved.
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

find_package(benchmark REQUIRED)
find_package(absl REQUIRED)

add_executable(container_benchmark container_benchmark.cpp)
target_link_libraries(
    container_benchmark
    PRIVATE
    benchmark::benchmark
    benchmark::benchmark_main
    vila)

add_executable(copy_benchmark copy_benchmark.cpp)
target_link_libraries(
    copy_benchmark
    PRIVATE
    benchmark::benchmark
    benchmark::benchmark_main
    vila)

add_executable(hash_benchmark hash_benchmark.cpp)
target_link_libraries(
    hash_benchmark
    PRIVATE
    benchmark::benchmark
    benchmark::benchmark_main
    vila)

add_executable(hashmap_benchmark hashmap_benchmark.cpp)
target_link_libraries(
    hashmap_benchmark
    PRIVATE
    benchmark::benchmark
    benchmark::benchmark_main
    absl::flat_hash_map
    absl::node_hash_map
    vila)

add_executable(pimpl_benchmark pimpl_benchmark.cpp)
target_link_libraries(
    pimpl_benchmark
    PRIVATE
    benchmark::benchmark
    benchmark::benchmark_main
    vila)

add_executable(ringbuffer_benchmark ringbuffer_benchmark.cpp)
target_link_libraries(
    ringbuffer_benchmark
    PRIVATE
    benchmark::benchmark
    benchmark::benchmark_main
    vila)

add_executable(string_split_benchmark string_split_benchmark.cpp)
target_link_libraries(
    string_split_benchmark
    PRIVATE
    benchmark::benchmark
    benchmark::benchmark_main
    vila)

add_executable(stringview_hashmap_benchmark stringview_hashmap_benchmark.cpp)
target_link_libraries(
    stringview_hashmap_benchmark
    PRIVATE
    benchmark::benchmark
    benchmark::benchmark_main
    vila)

add_executable(vt_speed_benchmark vt_speed_benchmark.cpp)
target_link_libraries(
    vt_speed_benchmark
    PRIVATE
    benchmark::benchmark
    benchmark::benchmark_main
    vila)
