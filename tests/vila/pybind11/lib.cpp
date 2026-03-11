/*
 * Copyright (C) 2023-2026 The VILA Authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace py::literals;  // NOLINT(*-using-namespace)

PYBIND11_MODULE(pybind_test, m) {  // NOLINT
  m.doc() = "PYBIND11 TEST";
  m.def("hello", []() { return "Hello, World!"; });
  m.def("add", [](int a, int b) { return a + b; }, "a"_a, "b"_a);
}
