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

add_subdirectory(pch)
