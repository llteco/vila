# Vision Infrastructure Library Assembler (VILA)

VILA is an infrastructure for C++ programming in computer vision field. It contains components below:

- Built-in JSON based configuration system.
- High performance logging system, which supports Windows logging events (WPP).
- A reflection registry.
- An easy-to-use error handling system (Status and StatusOr).
- A simple timer and profiler.
- A template header-only DAG class.
- Natively support both CMake and Bazel.

## Integration Guide

### Bazel Integration

With the incoming bazel 6 and bazel 7, we support both legacy WORKSPACE and the new [Bzlmod](https://bazel.build/external/module).

Integrate into bazel with bzlmod is an easy and recommended way:

```Bazel
## MODULE.bazel

module(name = "your-project")

bazel_dep(name = "vila")
# currently vila is not registered in BCR
git_override(
    module_name = "vila",
    remote = "https://github.com/intel-innersource/frameworks.camera.cpp.vila",
    commit = "main",
)
```

If you'd like to stick to using WORKSPACE, add following lines to your WORKSPACE file:

```Bazel
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
http_archive(
    name = "vila",
    sha256 = "932db4e10fd8a377626d8c92040658104367d064978213e788aab8ee23482002",
    strip_prefix = "frameworks.camera.cpp.vila-d68a3ae01e149a30e75232da1e4a53950bb31542",
    url = "https://github.com/intel-innersource/frameworks.camera.cpp.vila/archive/d68a3ae01e149a30e75232da1e4a53950bb31542.zip",
)

load("@vila//vila:workspace0.bzl", vila_workspace0 = "workspace")

vila_workspace0()

load("@vila//vila:workspace1.bzl", vila_workspace1 = "workspace")

vila_workspace1()

load("@vila//vila:workspace2.bzl", vila_workspace2 = "workspace")

vila_workspace2()

load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")
load("@rules_foreign_cc//foreign_cc:repositories.bzl", "rules_foreign_cc_dependencies")
load("@pybind11_bazel//:python_configure.bzl", "python_configure")

bazel_skylib_workspace()

# This sets up some common toolchains for building targets. For more details, please see
# https://bazelbuild.github.io/rules_foreign_cc/0.9.0/flatten.html#rules_foreign_cc_dependencies
rules_foreign_cc_dependencies()

python_configure(name = "local_config_python")

```

### CMake Integration

1. clone vila to your repository as a submodule
2. update vila's submodule
    ```
    git submodule add https://github.com/intel-innersource/frameworks.camera.cpp.vila vila
    ```
3. `add_subdirectory(vila)`
