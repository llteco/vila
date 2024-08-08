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
