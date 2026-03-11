"""
Copyright (C) 2025-2026 The VILA Authors.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
"""

workspace(name = "vila")

load("@vila//vila:workspace0.bzl", vila_workspace0 = "workspace")

vila_workspace0()

load("@vila//vila:workspace1.bzl", vila_workspace1 = "workspace")

vila_workspace1()

load("@vila//vila:workspace2.bzl", vila_workspace2 = "workspace")

vila_workspace2(sycl = True)

# load("@bazel_features//:deps.bzl", "bazel_features_deps")
# load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")
# load("@rules_foreign_cc//foreign_cc:repositories.bzl", "rules_foreign_cc_dependencies")
load("@rules_python//python:repositories.bzl", "py_repositories", "python_register_toolchains")

# bazel_features_deps()

# bazel_skylib_workspace()

# # This sets up some common toolchains for building targets. For more details, please see
# # https://bazelbuild.github.io/rules_foreign_cc/0.9.0/flatten.html#rules_foreign_cc_dependencies
# rules_foreign_cc_dependencies()

py_repositories()

python_register_toolchains(
    name = "local_config_python",
    python_version = "3.13",
)
