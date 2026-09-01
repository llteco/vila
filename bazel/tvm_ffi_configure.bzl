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

def _lookup_env_var(env, name, default = None):
    for key, value in env.items():
        if name.lower() == key.lower():
            return value
    return default

def _find_python(repository_ctx):
    """Locate a usable Python interpreter.

    Bazel 8's repository_ctx.which() rejects the 0-byte "app execution alias"
    stubs (python.exe/python3.exe) that Microsoft places on PATH, so fall back
    to the real interpreter install under %LOCALAPPDATA%\\Python.
    """
    if repository_ctx.attr.python:
        p = repository_ctx.path(repository_ctx.attr.python)
        if p.exists:
            return p

    for name in ["python3", "python"]:
        p = repository_ctx.which(name)
        if p and p.exists:
            return p

    if repository_ctx.os.name.lower().startswith("windows"):
        localappdata = _lookup_env_var(repository_ctx.os.environ, "LOCALAPPDATA")
        if localappdata:
            python_dir = repository_ctx.path(localappdata).get_child("Python")
            if python_dir.exists:
                # Prefer newest version directory, mirroring install layout:
                # <LOCALAPPDATA>\\Python\\pythoncore-<ver>\\python.exe
                versions = sorted(
                    [d for d in python_dir.readdir() if d.get_child("python.exe").exists],
                    key = lambda d: d.basename,
                    reverse = True,
                )
                if versions:
                    return versions[0].get_child("python.exe")

    return None

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
    python_path = _find_python(repository_ctx)

    # Verify Python was found
    if not python_path:
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

    # Step 6: Symlink library files and detect platform-specific binaries.
    # --libfiles returns: .lib (Windows import lib), .so (Linux), .dylib (macOS).
    # On Windows, the runtime .dll is a sibling of the .lib in the same directory.
    win_implib = None
    win_dll = None
    unix_shared = None  # .so or .dylib

    for lib_file in lib_files:
        lib_path = repository_ctx.path(lib_file)
        lib_name = lib_path.basename
        repository_ctx.symlink(lib_file, lib_name)

        if lib_name.endswith(".lib"):
            win_implib = lib_name
            dll_name = lib_name[:-4] + ".dll"
            dll_path = lib_path.dirname.get_child(dll_name)
            if dll_path.exists:
                repository_ctx.symlink(str(dll_path), dll_name)
                win_dll = dll_name
        elif lib_name.endswith(".so") or lib_name.endswith(".dylib"):
            unix_shared = lib_name


    # Step 7: Generate BUILD file with platform-specific cc_import targets.
    # Each cc_import pulls the shared library into dependents' runfiles for both
    # link-time and runtime. The aggregating cc_library propagates include paths
    # and selects the right import for the host platform via select().
    imports = ""
    select_branches = ""

    if win_implib and win_dll:
        imports += """
cc_import(
    name = "tvm_ffi_dll",
    interface_library = "{implib}",
    shared_library = "{dll}",
    target_compatible_with = ["@platforms//os:windows"],
    visibility = ["//visibility:private"],
)
""".format(implib = win_implib, dll = win_dll)
        select_branches += '\n        "@platforms//os:windows": [":tvm_ffi_dll"],'

    if unix_shared and unix_shared.endswith(".so"):
        imports += """
cc_import(
    name = "tvm_ffi_so",
    shared_library = "{so}",
    target_compatible_with = ["@platforms//os:linux"],
    visibility = ["//visibility:private"],
)
""".format(so = unix_shared)
        select_branches += '\n        "@platforms//os:linux": [":tvm_ffi_so"],'

    if unix_shared and unix_shared.endswith(".dylib"):
        imports += """
cc_import(
    name = "tvm_ffi_dylib",
    shared_library = "{dylib}",
    target_compatible_with = ["@platforms//os:osx"],
    visibility = ["//visibility:private"],
)
""".format(dylib = unix_shared)
        select_branches += '\n        "@platforms//os:osx": [":tvm_ffi_dylib"],'

    build_content = """
package(default_visibility = ["//visibility:public"])

load("@rules_cc//cc:defs.bzl", "cc_import", "cc_library")
{imports}
cc_library(
    name = "tvm_ffi",
    hdrs = glob([
        "include/**",
        "dlpack/**",
    ]),
    includes = [
        "include",
        "dlpack",
    ],
    deps = select({{{select}
        "//conditions:default": [],
    }}),
)
""".format(
        imports = imports,
        select = select_branches,
    )

    # Write the generated BUILD file to the external repository root
    repository_ctx.file("BUILD.bazel", build_content)

# Define the repository rule for Bazel
# Repository rules are used to fetch and configure external dependencies.
# This rule is called once per Bazel invocation to set up the TVM FFI dependency.
tvm_ffi_configure = repository_rule(
    implementation = _tvm_ffi_configure,
    attrs = {
        "python": attr.string(doc = "Explicit path to the Python interpreter. If unset, auto-discovered."),
    },
)
