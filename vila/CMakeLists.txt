#
# Copyright (C) 2023-2026 The VILA Authors.
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

add_library(vila STATIC)

file(GLOB_RECURSE
    VILA_SRC
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)
if(NOT WIN32)
    list(FILTER VILA_SRC EXCLUDE REGEX ".*detours.*")
endif()

target_sources(vila PRIVATE ${VILA_SRC})
target_link_libraries(vila PUBLIC fmt)
target_link_libraries(vila PRIVATE spdlog)
add_library(vila::vila ALIAS vila)
