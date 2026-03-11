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

add_executable(argparse_test argparse_test.cpp)
target_link_libraries(
    argparse_test PRIVATE GTest::gtest GTest::gtest_main vila)
gtest_add_tests(TARGET argparse_test)

add_executable(array_arithmetic_test array_arithmetic_test.cpp)
target_link_libraries(
    array_arithmetic_test PRIVATE GTest::gtest GTest::gtest_main vila)
gtest_add_tests(TARGET array_arithmetic_test)

add_executable(barrier_test barrier_test.cpp)
target_link_libraries(
    barrier_test PRIVATE GTest::gtest GTest::gtest_main vila)
gtest_add_tests(TARGET barrier_test)

add_executable(base64_test base64_test.cpp)
target_link_libraries(
    base64_test PRIVATE GTest::gtest GTest::gtest_main vila)
gtest_add_tests(TARGET base64_test)

add_executable(const_str_test const_str_test.cpp)
target_link_libraries(
    const_str_test PRIVATE GTest::gtest GTest::gtest_main vila)
gtest_add_tests(TARGET const_str_test)

add_executable(hash_test hash_test.cpp)
target_link_libraries(
    hash_test PRIVATE GTest::gtest GTest::gtest_main vila)
gtest_add_tests(TARGET hash_test)

add_executable(math_test math_test.cpp)
target_link_libraries(
    math_test PRIVATE GTest::gtest GTest::gtest_main vila)
gtest_add_tests(TARGET math_test)

add_executable(os_test os_test.cpp)
target_link_libraries(
    os_test PRIVATE GTest::gtest GTest::gtest_main vila)
gtest_add_tests(TARGET os_test)

add_executable(path_test path_test.cpp)
target_link_libraries(
    path_test PRIVATE GTest::gtest GTest::gtest_main vila)
gtest_add_tests(TARGET path_test)

add_executable(ringbuffer_test ringbuffer_test.cpp)
target_link_libraries(
    ringbuffer_test PRIVATE GTest::gtest GTest::gtest_main vila)
gtest_add_tests(TARGET ringbuffer_test)

add_executable(string_test string_test.cpp)
target_link_libraries(
    string_test PRIVATE GTest::gtest GTest::gtest_main vila)
gtest_add_tests(TARGET string_test)

add_executable(stringview_hashmap_test stringview_hashmap_test.cpp)
target_link_libraries(
    stringview_hashmap_test PRIVATE GTest::gtest GTest::gtest_main vila)
gtest_add_tests(TARGET stringview_hashmap_test)

add_executable(vt_nn_test vt_nn_test.cpp)
target_link_libraries(
    vt_nn_test PRIVATE GTest::gtest GTest::gtest_main vila)
gtest_add_tests(TARGET vt_nn_test)

add_executable(vt_test vt_test.cpp)
target_link_libraries(
    vt_test PRIVATE GTest::gtest GTest::gtest_main vila)
gtest_add_tests(TARGET vt_test)
