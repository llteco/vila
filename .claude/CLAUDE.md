# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

VILA (Vision Infrastructure Library Assembler) is a C++ infrastructure library for computer vision applications. It provides JSON-based configuration, logging with WPP support, reflection registry, error handling (Status/StatusOr), profiling tools, and a template DAG library.

## Build Systems

### Bazel (Primary)

```bash
# Build all tests
bazelisk test //tests/clim/... //tests/vila/...

# Build a specific test
bazelisk test //tests/clim:argparse_test

# Build with specific config (e.g., SYCL)
bazelisk build --config=sycl //...

# Run using legacy WORKSPACE (on Windows)
bazelisk --output_base="C:/temp/_vila_workspace" build --noenable_bzlmod //...
```

**Legacy WORKSPACE setup** requires loading three workspace files in sequence:
```bazel
load("//:workspace0.bzl", vila_workspace0 = "workspace")
vila_workspace0()
load("//:workspace1.bzl", vila_workspace1 = "workspace")
vila_workspace1()
load("//:workspace2.bzl", vila_workspace2 = "workspace")
vila_workspace2()
```

### CMake

```bash
mkdir build && cmake -Bbuild -S. -GNinja
cmake --build build --config Release

# With testing enabled
cmake -Bbuild -S. -GNinja -DVILA_ENABLE_TESTING=ON
```

## Code Style

- **Style**: Google C++ Style with 80-column limit (see `.clang-format`)
- **Clang-tidy**: Targets `clim/*.h` and `vila/*.h` headers only (excludes generated files). Warnings treated as errors for readability, bugprone, and concurrency checks.
- **Pre-commit hooks**: `pre-commit run -s HEAD^ -o HEAD` (see `.pre-commit-config.yaml`)
- **Spell checking**: Codespell with custom words bag at `.github/WORDS_BAG.txt`

## Architecture

```
clim/           # Header-only utility library
  ├── argparse         # Command-line argument parsing (depends on //clim:string)
  ├── container        # Bounding boxes, ring buffers, array arithmetic
  ├── filter           # Kalman and alpha-beta filters (depends on //clim:container)
  ├── hash             # CityHash, MurmurHash implementations
  ├── math             # Quaternion, numerical utilities
  ├── os               # Aligned malloc, barriers, OS utilities
  ├── path             # Cross-platform path handling (defines HAS_STD_FS on non-Windows)
  ├── reflection       # Reflection registry (depends on //clim:string)
  ├── string           # String splitting, stripping, const_string
  ├── vt               # Vector math (GEMM, neural network ops; depends on //clim:traits)
  └── zip              # Zip utility functions

vila/           # Core library components
  ├── config           # JSON-based configuration system
  ├── graph            # Template header-only DAG library (dag.h, digraph.h, graph.h, node.h, route.h, traversal.h)
  ├── hook             # Windows DLL hooking via Detours library
  ├── logging          # Logger with WPP support (code_location, logger)
  ├── profiling        # ITT instrumentation, timer, trace utilities
  ├── status           # Status and StatusOr error handling
  └── widget           # Registration tokens

bazel/          # Bazel-specific build rules and toolchains
python/         # Python utilities (vila.core, vila.arith.random)
tests/          # GoogleTest-based C++ tests
```

## Key Dependencies (via Bazel)

- `fmt` (12.1.0) - Formatting library
- `spdlog` - Logging library (loaded via extension)
- `googletest` (1.17.0) - Testing framework
- `google_benchmark` (1.9.2) - Benchmarking
- `rangev3` - Range library (for C++20 `range_test`)
- `ittapi` - Intel ITT instrumentation
- `tvm_ffi` - TVM FFI support (optional, requires `pip install apache-tvm-ffi`)

## Custom Bazel Rules

The project provides custom build rules in `bazel/vila/vila.bzl`:

- `vila_cc_library` - Adds default copts
- `vila_cc_binary` - Adds default copts/linkopts, defaults to `linkstatic=True`
- `vila_cc_test` - Adds C++17 + Unicode copts, defaults to `linkstatic=True`
- `vila_dll_library` - Imports Windows DLL dependencies
- `vila_so_library` - Imports Linux shared library dependencies

## Testing

```bash
# C++ tests (Bazel)
bazelisk test //tests/clim/... //tests/vila/...

# Python tests
pip install -e python[test]
pytest --cov=python/vila python/tests
```

## Windows-Specific Notes

- Default C++ standard: C++17 (`/std:c++17` in `.bazelrc`)
- Windows WPP logging disabled by default (controlled by `--enable_wpp` flag)
- Windows-specific configs use `select()` with `//conditions:default`
- `range_test` is Windows-only and requires C++20
- Use `/FC` flag for `__FILE__` to expand to full Windows path (see `const_string_test`)
- `.bazelrc` uses `--spawn_strategy=local` to work around sandbox issues
