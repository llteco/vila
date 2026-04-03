"""
Copyright (c) 2025-2026 Wenyi Tang
Author: Wenyi Tang
E-mail: wenyitang@outlook.com

config cc toolchain to use oneAPI SYCL compiler (icp a.k.a dpc++)
"""

load("@vila//vila/bazel/toolchains:os.bzl", "is_windows")
load("@vila//vila/bazel/toolchains:unix_toolchain_configure.bzl", "configure_unix_toolchain")
load(
    "@vila//vila/bazel/toolchains:windows_toolchain_configure.bzl",
    "get_msvc_vars",
    "get_path_env_var",
)

def find_oneapi_path(repository_ctx):
    r"""Find oneAPI installation on local machine.

    HINT:
        CMPLR_ROOT
        ONEAPI_ROOT
        c:\Program Files (x86)\Intel\oneAPI\compiler\latest\

    Args:
        repository_ctx: _repository context object

    Returns:
        str: install root path of icpx
    """
    icpx_path = get_path_env_var(repository_ctx, "CMPLR_ROOT")
    if icpx_path:
        return icpx_path
    oneapi_path = get_path_env_var(repository_ctx, "ONEAPI_ROOT")
    if oneapi_path:
        return oneapi_path + "/compiler/latest"

    if is_windows(repository_ctx):
        default_install_path = "C:/Program Files (x86)/Intel/oneAPI/compiler/latest"
    else:
        default_install_path = "/opt/intel/oneapi/compiler/latest"
    if repository_ctx.path(default_install_path).exists:
        return default_install_path

    return None

def get_icx_version(repository_ctx):
    r"""Get icx version from oneAPI installation.

    Args:
        repository_ctx: The context with which to find paths.

    Returns:
        str: icx version
    """
    oneapi_path = find_oneapi_path(repository_ctx)
    icx_path = repository_ctx.path(oneapi_path).get_child("bin", "icx-cl.exe")
    ret = repository_ctx.execute([icx_path, "--version", "-nologo"])
    if ret.return_code != 0:
        fail("Failed to get icx version (%s): %s" % (ret.return_code, ret.stderr))
    return ret.stdout.split("\n")[0].split(" ")[-1].strip("()")

def get_llvm_version(repository_ctx):
    r"""Get llvm version from oneAPI installation.

    Args:
        repository_ctx: The context with which to find paths.

    Returns:
        str: llvm version
    """
    oneapi_path = find_oneapi_path(repository_ctx)
    if not oneapi_path:
        return "llvm-unknown"

    llvm_path = repository_ctx.path(oneapi_path).get_child("lib", "clang")
    for v in llvm_path.readdir():
        return v.basename
    return "llvm-unknown"

def _append_template_list_entries(existing_entries, new_entries):
    r"""Append rendered list entries to a template field without leading commas.

    Args:
        existing_entries: Existing rendered entries string (without surrounding []).
        new_entries: List of quoted entries to append.

    Returns:
        str: Rendered entries string safe for use inside [] in BUILD templates.
    """
    existing = existing_entries.strip()
    new_rendered = ",\n        ".join(new_entries)
    if not existing:
        return "        " + new_rendered
    return existing + ",\n        " + new_rendered

def _overwrite_sycl_msvc(repository_ctx, msvc_vars, target_arch):
    oneapi_path = find_oneapi_path(repository_ctx)
    llvm_version = get_llvm_version(repository_ctx)

    # convert symbolic link to real path
    if oneapi_path:
        oneapi_path = str(repository_ctx.path(oneapi_path).realpath)
        if target_arch == "x64":
            msvc_vars["%{msvc_cl_path_" + target_arch + "}"] = oneapi_path + "/bin/icx-cl.exe"
            msvc_vars["%{msvc_link_path_" + target_arch + "}"] = oneapi_path + "/bin/icx-cl.exe"
            msvc_vars["%{msvc_env_lib_" + target_arch + "}"] += ";" + oneapi_path + "/lib"
        elif target_arch == "x86":
            fail("32-bit support is deprecated!")
        msvc_vars["%{msvc_env_include_x64}"] += ";" + ";".join([
            oneapi_path + "/include",
            oneapi_path + "/lib/clang/%s/include" % llvm_version,
            oneapi_path + "/opt/compiler/include",
        ])
        msvc_vars["%{msvc_cxx_builtin_include_directories_" + target_arch + "}"] = _append_template_list_entries(
            msvc_vars.get("%{msvc_cxx_builtin_include_directories_" + target_arch + "}", ""),
            [
                "\"%s\"" % (oneapi_path + "/include"),
                "\"%s\"" % (oneapi_path + "/lib/clang/%s/include" % llvm_version),
                "\"%s\"" % (oneapi_path + "/opt/compiler/include"),
            ],
        )
    else:
        print("oneAPI DPC++ may not be installed, please check the environment variable ONEAPI_ROOT")

def _resolve_labels(repository_ctx, labels):
    """Resolves a collection of labels to their paths.

    Label resolution can cause the evaluation of Starlark functions to restart.
    For functions with side-effects (like the auto-configuration functions, which
    inspect the system and touch the file system), such restarts are costly.
    We cannot avoid the restarts, but we can minimize their penalty by resolving
    all labels upfront.

    Among other things, doing less work on restarts can cut analysis times by
    several seconds and may also prevent tickling kernel conditions that cause
    build failures.  See https://github.com/bazelbuild/bazel/issues/5196 for
    more details.

    Args:
      repository_ctx: The context with which to resolve the labels.
      labels: Labels to be resolved expressed as a list of strings.

    Returns:
      A dictionary with the labels as keys and their paths as values.
    """
    return dict([(label, repository_ctx.path(Label(label))) for label in labels])

def _sycl_configure(repository_ctx):
    paths = _resolve_labels(repository_ctx, [
        "@vila//vila/bazel/toolchains:BUILD_sycl.tpl",
        "@vila//vila/bazel/toolchains:BUILD_sycl_unix.tpl",
        # required by msvc
        "@bazel_tools//tools/cpp:windows_cc_toolchain_config.bzl",
        "@bazel_tools//tools/cpp:vc_installation_error.bat.tpl",
        "@bazel_tools//tools/cpp:clang_installation_error.bat.tpl",
        # required by unix
        "@bazel_tools//tools/cpp:generate_system_module_map.sh",
        "@bazel_tools//tools/cpp:armeabi_cc_toolchain_config.bzl",
        "@bazel_tools//tools/cpp:unix_cc_toolchain_config.bzl",
        "@bazel_tools//tools/cpp:linux_cc_wrapper.sh.tpl",
        "@bazel_tools//tools/cpp:osx_cc_wrapper.sh.tpl",
    ])

    repo_name = repository_ctx.name.split("~")[-1]
    template_vars = dict({
        "%{constraint_values}": "\"@vila//vila/bazel/toolchains:sycl\"",
        "%{platform_name}": repo_name,
    })
    if is_windows(repository_ctx):
        msvc_vars_x64 = get_msvc_vars(repository_ctx, paths, "x64")
        _overwrite_sycl_msvc(repository_ctx, msvc_vars_x64, "x64")
        template_vars.update(msvc_vars_x64)

        repository_ctx.template(
            "BUILD",
            paths["@vila//vila/bazel/toolchains:BUILD_sycl.tpl"],
            template_vars,
        )
    else:
        oneapi_path = find_oneapi_path(repository_ctx)
        if oneapi_path:
            template_vars["gcc"] = oneapi_path + "/bin/icx"
            template_vars["llvm-cov"] = oneapi_path + "/bin/compiler/llvm-cov"
            template_vars["llvm-profdata"] = oneapi_path + "/bin/compiler/llvm-profdata"
            template_vars["ar"] = oneapi_path + "/bin/compiler/llvm-ar"
            template_vars["builtin_include_directories"] = [
                oneapi_path + "/include",
            ]
        configure_unix_toolchain(repository_ctx, paths, "x86_64", template_vars)

sycl_configure = repository_rule(
    implementation = _sycl_configure,
    environ = ["CMPLR_ROOT", "ONEAPI_ROOT"],
)
