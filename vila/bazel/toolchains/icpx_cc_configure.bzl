"""
Copyright (c) 2024 Intel Corporation
Author: Wenyi Tang
E-mail: wenyi.tang@intel.com

config cc toolchain to use oneAPI icx and icpx instruments
"""

load(
    "@vila//vila/bazel/toolchains:windows_toolchain_configure.bzl",
    "get_clang_cl_vars",
    "get_msvc_vars",
    "get_path_env_var",
)

def find_oneapi_path(repository_ctx):
    r"""Find oneAPI installation on local machine.

    HINT:
        BAZEL_ICPX
        ONEAPI_ROOT
        c:\Program Files (x86)\Intel\oneAPI\compiler\latest\bin\

    Args:
        repository_ctx: _repository context object

    Returns:
        str: install root path of icpx
    """
    icpx_path = get_path_env_var(repository_ctx, "BAZEL_ICPX")
    if icpx_path:
        return icpx_path
    oneapi_path = get_path_env_var(repository_ctx, "ONEAPI_ROOT")
    if oneapi_path:
        return oneapi_path + "/compiler/latest"

    default_install_path = "C:/Program Files (x86)/Intel/oneAPI/compiler/latest"
    if repository_ctx.path(default_install_path).exists:
        return default_install_path

    return None

def _overwrite_icpx_msvc(repository_ctx, msvc_vars, target_arch):
    icpx_path = find_oneapi_path(repository_ctx)

    # convert symbolic link to real path
    icpx_path = str(repository_ctx.path(icpx_path).realpath)
    if icpx_path:
        if target_arch == "x64":
            msvc_vars["%{msvc_cl_path_" + target_arch + "}"] = icpx_path + "/bin/icx.exe"
            msvc_vars["%{msvc_env_lib_" + target_arch + "}"] += ";" + icpx_path + "/lib"
        elif target_arch == "x86":
            msvc_vars["%{msvc_cl_path_" + target_arch + "}"] = icpx_path + "/bin32/icx.exe"
            msvc_vars["%{msvc_env_lib_" + target_arch + "}"] += ";" + icpx_path + "/lib32"
        msvc_vars["%{msvc_cxx_builtin_include_directories_" + target_arch + "}"] += ",\n        " + ",\n        ".join([
            "\"%s\"" % (icpx_path + "/lib/clang/19/include"),
            "\"%s\"" % (icpx_path + "/opt/compiler/include"),
        ])

def _overwrite_icpx_clang_cl(repository_ctx, msvc_vars, target_arch):
    icpx_path = find_oneapi_path(repository_ctx)

    # convert symbolic link to real path
    icpx_path = str(repository_ctx.path(icpx_path).realpath)
    if icpx_path:
        if target_arch == "x64":
            msvc_vars["%{clang_cl_path_" + target_arch + "}"] = icpx_path + "/bin/icx.exe"
        elif target_arch == "x86":
            msvc_vars["%{clang_cl_path_" + target_arch + "}"] = icpx_path + "/bin32/icx.exe"

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

def _icpx_configure(repository_ctx):
    paths = _resolve_labels(repository_ctx, [
        "@vila//vila/bazel/toolchains:BUILD.tpl",
        "@bazel_tools//tools/cpp:windows_cc_toolchain_config.bzl",
        "@bazel_tools//tools/cpp:vc_installation_error.bat.tpl",
        "@bazel_tools//tools/cpp:clang_installation_error.bat.tpl",
    ])

    template_vars = dict()
    msvc_vars_x64 = get_msvc_vars(repository_ctx, paths, "x64")
    _overwrite_icpx_msvc(repository_ctx, msvc_vars_x64, "x64")
    template_vars.update(msvc_vars_x64)
    clang_cl_vars_x64 = get_clang_cl_vars(repository_ctx, paths, msvc_vars_x64, "x64")
    _overwrite_icpx_clang_cl(repository_ctx, clang_cl_vars_x64, "x64")
    template_vars.update(clang_cl_vars_x64)
    msvc_vars_x64_x86 = get_msvc_vars(repository_ctx, paths, "x86", msvc_vars_x64)
    _overwrite_icpx_msvc(repository_ctx, msvc_vars_x64_x86, "x86")
    template_vars.update(msvc_vars_x64_x86)

    repository_ctx.template(
        "BUILD",
        paths["@vila//vila/bazel/toolchains:BUILD.tpl"],
        template_vars,
    )

icpx_configure = repository_rule(
    implementation = _icpx_configure,
    environ = ["BAZEL_ICPX", "ONEAPI_ROOT"],
)
