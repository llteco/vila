#
# Copyright (C) 2024-2026 The VILA Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

find_package(benchmark REQUIRED)

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

add_executable(crtp_benchmark crtp_benchmark.cpp)
target_link_libraries(
    crtp_benchmark
    PRIVATE
    benchmark::benchmark
    benchmark::benchmark_main)
