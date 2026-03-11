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
cmake_minimum_required(VERSION 3.18)

project(vila CXX)
include(CheckIncludeFileCXX)

include_directories(${CMAKE_CURRENT_LIST_DIR})

option(VILA_ENABLE_TESTING "Enable testing" OFF)

if(MSVC)
    add_compile_options("/std:c++17")
    add_compile_options("/utf-8")
    add_compile_definitions(
        NOMINMAX
        SPDLOG_WCHAR_TO_UTF8_SUPPORT
        SPDLOG_WCHAR_FILENAMES
    )
else()
    add_compile_options(-std=c++17 -fPIC -frtti -fexceptions -mavx2)
    check_include_file_cxx(filesystem STD_FS)
    check_include_file_cxx(experimental/filesystem STD_EXP_FS)
    if(${STD_FS})
        add_compile_definitions(HAS_STD_FS)
    elseif(${STD_EXP_FS})
        add_compile_definitions(HAS_STD_EXP_FS)
    else()
        message(FATAL_ERROR "No c++17 feature filesystem support found")
    endif()
    link_libraries(stdc++fs)
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

if(VILA_ENABLE_TESTING)
    enable_testing()
    find_package(GTest REQUIRED)
    add_subdirectory(tests)
endif()
