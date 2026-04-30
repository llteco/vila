# Bazel Structure Refactoring Mitigation Plan

## Summary

Refactor Bazel workspace structure by moving workspace files and bazel rules to more conventional locations:
- Move `vila/workspace0.bzl`, `vila/workspace1.bzl`, `vila/workspace2.bzl` to root folder
- Move `vila/bazel/` to `bazel/`

## Status: COMPLETED ✓

## Changes

### File Movements

| Source | Destination |
|--------|-------------|
| `vila/workspace0.bzl` | `workspace0.bzl` |
| `vila/workspace1.bzl` | `workspace1.bzl` |
| `vila/workspace2.bzl` | `workspace2.bzl` |
| `vila/bazel/` (entire directory) | `bazel/` |

### Path Updates

Replace `@vila//vila/bazel` with `@vila//bazel` in all files.
Replace `@vila//vila:workspace*.bzl` with `//:workspace*.bzl` in WORKSPACE and documentation.

### Files Affected

#### Root Level
- `WORKSPACE` - workspace load paths
- `MODULE.bazel` - bzlmod extension paths
- `README.md` - documentation examples
- `.claude/CLAUDE.md` - build instructions

#### Moved Files (internal references)
- `workspace1.bzl` - references `@vila//vila/bazel/`
- `workspace2.bzl` - references `@vila//vila/bazel/`
- `bazel/bzlmod/extensions.bzl` - references `@vila//vila/bazel/`
- `bazel/tvm_ffi_configure.bzl` - references `@vila//vila/bazel/`
- `bazel/vila/vila.bzl` - references `@vila//vila/bazel/vila/`
- `bazel/vila/compile_options.bzl` - references `@vila//vila:` (keeps same, vila constraints)
- `bazel/toolchains/*.bzl` - various references
- `bazel/wdk/wdk_configure.bzl` - template paths
- `bazel/pre_compile.bzl` - self-reference
- `bazel/toolchains/BUILD_sycl.tpl` - load statement
- `bazel/fmt.BUILD` - load statement

#### vila Library BUILD Files
- `vila/BUILD.bazel` - sycl constraint reference
- `vila/config/BUILD.bazel` - vila_cc_library load
- `vila/graph/BUILD.bazel` - vila_cc_library load
- `vila/logging/BUILD.bazel` - vila_cc_library load
- `vila/profiling/BUILD.bazel` - vila_cc_library load
- `vila/status/BUILD.bazel` - vila_cc_library load
- `vila/widget/BUILD.bazel` - vila_cc_library load

## Execution Steps

1. ✓ Move workspace files to root
2. ✓ Move bazel directory to root
3. ✓ Update WORKSPACE load paths
4. ✓ Update MODULE.bazel extension paths
5. ✓ Update all .bzl file references
6. ✓ Update all BUILD file load statements
7. ✓ Update documentation (README.md, CLAUDE.md)
8. ✓ Remove empty `vila/bazel/` directory structure

## Post-Refactoring Validation

```bash
# Test legacy WORKSPACE build (PASSED)
bazelisk build --noenable_bzlmod //vila/status
# Result: Build completed successfully, 22 total actions

# Test bzlmod build (PASSED)
bazelisk build //vila/status
# Result: Build completed successfully, 22 total actions
```

## Backward Compatibility

This is a breaking change for downstream consumers using legacy WORKSPACE. They must update their load paths:
- `@vila//vila:workspace*.bzl` → `//:workspace*.bzl`

Bzlmod users are unaffected since they use `bazel_dep()` only.
