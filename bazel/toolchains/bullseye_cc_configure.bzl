"""
Copyright (c) 2025-2026 Wenyi Tang
Author: Wenyi Tang
E-mail: wenyitang@outlook.com

config cc toolchain to use bullseye instruments
"""

load("@vila//bazel/toolchains:os.bzl", "is_windows")
load(
    "@vila//bazel/toolchains:windows_toolchain_configure.bzl",
    "get_clang_cl_vars",
    "get_msvc_vars",
    "get_path_env_var",
)

def find_bullseye_path(repository_ctx):
    """Find bullseye installation on local machine.

    HINT:
        BAZEL_BULLSEYE
        C:/Program Files/BullseyeCoverage

    Args:
        repository_ctx: _repository context object

    Returns:
        str: install root path of bullseye
    """
    bullseye_path = get_path_env_var(repository_ctx, "BAZEL_BULLSEYE")
    if bullseye_path:
        return bullseye_path

    default_install_path = "C:/Program Files/BullseyeCoverage"
    if repository_ctx.path(default_install_path).exists:
        return default_install_path

    return None

def _overwrite_bullseye_msvc(repository_ctx, msvc_vars, target_arch):
    bullseye_path = find_bullseye_path(repository_ctx)
    if bullseye_path:
        if target_arch == "x64":
            msvc_vars["%{msvc_cl_path_" + target_arch + "}"] = bullseye_path + "/bin/cl.exe"
            msvc_vars["%{msvc_link_path_" + target_arch + "}"] = bullseye_path + "/bin/link.exe"
        elif target_arch == "x86":
            msvc_vars["%{msvc_cl_path_" + target_arch + "}"] = bullseye_path + "/bin/x86/cl.exe"
            msvc_vars["%{msvc_link_path_" + target_arch + "}"] = bullseye_path + "/bin/x86/link.exe"
    else:
        print("bullseye may not be installed, please check the environment variable BAZEL_BULLSEYE")

def _overwrite_bullseye_clang_cl(repository_ctx, msvc_vars, target_arch):
    bullseye_path = find_bullseye_path(repository_ctx)
    if bullseye_path:
        if target_arch == "x64":
            msvc_vars["%{clang_cl_path_" + target_arch + "}"] = bullseye_path + "/bin/clang-cl.exe"
            msvc_vars["%{clang_cl_link_path_" + target_arch + "}"] = bullseye_path + "/bin/link.exe"
        elif target_arch == "x86":
            msvc_vars["%{clang_cl_path_" + target_arch + "}"] = bullseye_path + "/bin/x86/clang-cl.exe"
            msvc_vars["%{clang_cl_link_path_" + target_arch + "}"] = bullseye_path + "/bin/x86/link.exe"
    else:
        print("bullseye may not be installed, please check the environment variable BAZEL_BULLSEYE")

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

def _bullseye_configure(repository_ctx):
    paths = _resolve_labels(repository_ctx, [
        "@vila//bazel/toolchains:BUILD_bullseye.tpl",
        "@bazel_tools//tools/cpp:windows_cc_toolchain_config.bzl",
        "@bazel_tools//tools/cpp:vc_installation_error.bat.tpl",
        "@bazel_tools//tools/cpp:clang_installation_error.bat.tpl",
    ])

    if is_windows(repository_ctx):
        repo_name = repository_ctx.name.split("~")[-1]
        template_vars = dict({
            "%{constraint_values}": "\"@vila//bazel/toolchains:bullseye\"",
            "%{platform_name}": repo_name,
        })
        msvc_vars_x64 = get_msvc_vars(repository_ctx, paths, "x64")
        _overwrite_bullseye_msvc(repository_ctx, msvc_vars_x64, "x64")
        template_vars.update(msvc_vars_x64)
        clang_cl_vars_x64 = get_clang_cl_vars(repository_ctx, paths, msvc_vars_x64, "x64")
        _overwrite_bullseye_clang_cl(repository_ctx, clang_cl_vars_x64, "x64")
        template_vars.update(clang_cl_vars_x64)
        msvc_vars_x64_x86 = get_msvc_vars(repository_ctx, paths, "x86", msvc_vars_x64)
        _overwrite_bullseye_msvc(repository_ctx, msvc_vars_x64_x86, "x86")
        template_vars.update(msvc_vars_x64_x86)

        repository_ctx.template(
            "BUILD",
            paths["@vila//bazel/toolchains:BUILD_bullseye.tpl"],
            template_vars,
        )

bullseye_configure = repository_rule(
    implementation = _bullseye_configure,
    environ = ["BAZEL_BULLSEYE", "COVFILE"],
)
