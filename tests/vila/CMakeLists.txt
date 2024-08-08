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

add_executable(code_location_test code_location_test.cpp)
target_link_libraries(
    code_location_test PRIVATE GTest::gtest GTest::gtest_main vila)
gtest_add_tests(TARGET code_location_test)

add_executable(config_test config_test.cpp)
target_link_libraries(
    config_test PRIVATE GTest::gtest GTest::gtest_main vila)
gtest_add_tests(TARGET config_test)

add_executable(logger_test logger_test.cpp)
target_link_libraries(
    logger_test PRIVATE GTest::gtest GTest::gtest_main vila)
gtest_add_tests(TARGET logger_test)

add_executable(meta_dag_test meta_dag_test.cpp)
target_link_libraries(
    meta_dag_test PRIVATE GTest::gtest GTest::gtest_main vila)
gtest_add_tests(TARGET meta_dag_test)

add_executable(meta_graph_test meta_graph_test.cpp)
target_link_libraries(
    meta_graph_test PRIVATE GTest::gtest GTest::gtest_main vila)
gtest_add_tests(TARGET meta_graph_test)

add_executable(meta_route_test meta_route_test.cpp)
target_link_libraries(
    meta_route_test PRIVATE GTest::gtest GTest::gtest_main vila)
gtest_add_tests(TARGET meta_route_test)

add_executable(meta_traversal_test meta_traversal_test.cpp)
target_link_libraries(
    meta_traversal_test PRIVATE GTest::gtest GTest::gtest_main vila)
gtest_add_tests(TARGET meta_traversal_test)
