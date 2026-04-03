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

**Legacy WORKSPACE setup (Windows)** requires loading three workspace files in sequence:
```bazel
load("@vila//vila:workspace0.bzl", vila_workspace0 = "workspace")
vila_workspace0()
load("@vila//vila:workspace1.bzl", vila_workspace1 = "workspace")
vila_workspace1()
load("@vila//vila:workspace2.bzl", vila_workspace2 = "workspace")
```

### CMake

```bash
mkdir build && cmake -Bbuild -S. -GNinja
cmake --build build --config Release

# With testing enabled
cmake -Bbuild -S. -GNinja -DVILA_ENABLE_TESTING=ON
```

## Code Style

- **Style**: Based on Google C++ Style (see `.clang-format`)
- **Clang-tidy**: Configured in `.clang-tidy`, targets `clim/` and `vila/` headers only (excludes generated files)
- **Pre-commit hooks**: Run via `pre-commit run -s HEAD^ -o HEAD` (see `.pre-commit-config.yaml`)
- **Spell checking**: Codespell configured with words bag at `.github/WORDS_BAG.txt`

## Architecture

```
clim/           # Header-only utility library (math, strings, containers, etc.)
  ├── argparse/        # Command-line argument parsing
  ├── container/       # Bounding boxes, ring buffers, etc.
  ├── filter/          # Kalman and alpha-beta filters
  ├── hash/            # CityHash, MurmurHash
  ├── math/            # Quaternion, numerical utilities
  ├── os/              # OS utilities (aligned malloc, barriers)
  ├── path/            # Cross-platform path handling
  ├── reflection/      # Reflection registry
  ├── string/          # String splitting, stripping, const_string
  ├── vt/              # Vector math (GEMM, neural network ops)
  └── zip/             # Zip utility functions

vila/           # Core library components
  ├── config/          # JSON-based configuration system
  ├── graph/            # Template header-only DAG (dag.h, graph.h, route.h, traversal.h)
  ├── hook/            # Windows DLL hooking (detours)
  ├── logging/         # Logger with WPP support (code_location, logger)
  ├── profiling/       # ITT, timer, trace utilities
  ├── status/          # Status and StatusOr error handling
  ├── widget/          # (UI components)
  └── bazel/           # Bazel-specific build rules and toolchains

python/         # Python bindings via nanobind/pybind11
tests/           # GoogleTest-based C++ tests
```

## Key Dependencies (via Bazel)

- `fmt` (12.1.0) - Formatting library
- `spdlog` - Logging library
- `googletest` - Testing framework
- `google_benchmark` - Benchmarking
- `rules_foreign_cc` - CMake/ ninja build support

## Testing

```bash
# C++ tests (Bazel)
bazelisk test //tests/clim/... //tests/vila/...

# Python tests
pip install -e python[test]
pytest --cov=python/vila python/tests
```

## Windows-Specific Notes

- Default C++ standard: C++17
- Windows WPP logging disabled by default (see commit ed44f79)
- Windows-specific configs use `select()` with `//conditions:default` since some build configs are Windows-only
- The `range_test` is Windows-only and requires C++20

## Editor Setup

The project includes `.vscode/` settings for convenience with bazelized projects.
