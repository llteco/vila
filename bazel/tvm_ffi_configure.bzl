"""
Copyright (C) 2026 The VILA Authors.

Locate TVM FFI from pip-installed apache-tvm-ffi package.

TVM FFI (apache-tvm-ffi) is a pre-built binary package distributed via PyPI.
It provides C++ headers and a pre-built DLL/SO for calling TVM runtime from C++.

This repository rule discovers the TVM FFI installation paths by running:
    python -m tvm_ffi.config --includedir    # Include directory
    python -m tvm_ffi.config --dlpack-includedir  # DLPack include
    python -m tvm_ffi.config --libfiles     # Library files (.lib on Windows)

The rule then creates symlinks within the external repository to allow Bazel
to use these paths with relative includes (Bazel doesn't support absolute paths).

Usage:
    1. Ensure apache-tvm-ffi is installed: pip install apache-tvm-ffi
    2. Add this repository rule to your WORKSPACE or MODULE.bazel
    3. Depend on @tvm_ffi//:tvm_ffi in your targets

Example in MODULE.bazel:
    tvm_ffi_ext = use_extension("@vila//bazel/bzlmod:extensions.bzl", "tvm_ffi_extension")
    use_repo(tvm_ffi_ext, "tvm_ffi")

Example in WORKSPACE:
    load("@vila//bazel:tvm_ffi_configure.bzl", "tvm_ffi_configure")
    tvm_ffi_configure(name = "tvm_ffi")
"""

def _tvm_ffi_configure(repository_ctx):
    """Repository rule implementation to locate TVM FFI from pip installation.

    This function is called by Bazel during the repository fetch phase.
    It uses 'python -m tvm_ffi.config' to discover the TVM FFI installation
    paths and creates a BUILD file that can be used by other targets.

    Args:
        repository_ctx: The repository context provided by Bazel

    The function performs the following steps:
        1. Find Python executable (from attribute or Bazel toolchain)
        2. Query TVM FFI for include directories
        3. Query TVM FFI for library files
        4. Create symlinks in the external repository
        5. Generate a BUILD file with cc_library target
    """

    # Step 1: Find Python executable
    # This is the Python that rules_python registers via python_register_toolchains
    python_path = repository_ctx.which("python3")
    if not python_path:
        python_path = repository_ctx.which("python")

    # Verify Python was found
    if not python_path or not python_path.exists:
        fail(
            "Python not found. Please either:\n" +
            "1. Install apache-tvm-ffi: pip install apache-tvm-ffi\n" +
            "2. Ensure python3 or python is available on PATH\n" +
            "3. Use python_register_toolchains in your WORKSPACE/MODULE.bazel",
        )

    # Step 2: Get TVM FFI include directory
    # The --includedir flag returns the path to tvm/ffi/include
    result = repository_ctx.execute(
        [str(python_path), "-m", "tvm_ffi.config", "--includedir"],
        quiet = False,
    )

    # If TVM FFI is not installed, the command will fail
    if result.return_code != 0:
        fail("TVM FFI not found. Install with: pip install apache-tvm-ffi")

    # Store the include directory path (strip whitespace)
    include_dir = result.stdout.strip()

    # Step 3: Get DLPack include directory
    # DLPack is a standardization of in-memory tensor data structures
    result = repository_ctx.execute(
        [str(python_path), "-m", "tvm_ffi.config", "--dlpack-includedir"],
        quiet = False,
    )
    dlpack_dir = result.stdout.strip()

    # Step 4: Get library files for linking
    # On Windows, this returns the .lib import library (not the .dll)
    # On Linux, this would return .so files if available
    result = repository_ctx.execute(
        [str(python_path), "-m", "tvm_ffi.config", "--libfiles"],
        quiet = False,
    )

    # Parse the comma-separated list of library files
    lib_files = [f.strip() for f in result.stdout.strip().split(",") if f.strip()]

    # Step 5: Create symlinks in the external repository
    # Bazel requires relative paths for includes, but TVM FFI uses absolute paths.
    # We create symlinks from the external repo to the actual TVM FFI installation.
    # This approach works across platforms and doesn't require copying files.
    repository_ctx.symlink(include_dir, "include")
    repository_ctx.symlink(dlpack_dir, "dlpack")

    # Step 6: Symlink the import library for linking
    # On Windows, we need the .lib file to link against TVM FFI at compile time.
    # The .dll is loaded at runtime via tvm_ffi.load_module() in Python.
    lib_files_srcs = ""
    if lib_files:
        # Take the first library file (typically the import library)
        for lib_file in lib_files:
            # Create symlink to the .lib file
            lib_name = repository_ctx.path(lib_file).basename
            repository_ctx.symlink(lib_file, lib_name)

            # Format for BUILD file - use filegroup for proper handling
            lib_files_srcs += '"%s",' % lib_name


    # Step 7: Generate BUILD file
    # The BUILD file defines a cc_library that propagates include paths.
    # Using includes (not copts) ensures paths propagate to dependent targets.
    # Note: TVM FFI is a pre-built binary, so we don't compile any source files.
    # The library is loaded at runtime via dlopen/LoadLibrary from Python.
    build_content = """
package(default_visibility = ["//visibility:public"])

load("@rules_cc//cc:cc_library.bzl", "cc_library")

cc_library(
    name = "tvm_ffi",
    # Relative paths via symlinked directories - Bazel requires this
    includes = [
        "include",
        "dlpack",
    ],
    # Link against TVM FFI import library (.lib on Windows)
    # {lib_files_srcs} is intentionally left without leading comma if empty
    srcs = [{lib_files_srcs}],
    visibility = ["//visibility:public"],
)
""".format(
        lib_files_srcs = lib_files_srcs,
    )

    # Write the generated BUILD file to the external repository root
    repository_ctx.file("BUILD.bazel", build_content)

# Define the repository rule for Bazel
# Repository rules are used to fetch and configure external dependencies.
# This rule is called once per Bazel invocation to set up the TVM FFI dependency.
tvm_ffi_configure = repository_rule(implementation = _tvm_ffi_configure)
