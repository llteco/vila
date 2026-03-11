"""
Copyright (c) 2024-2026 Wenyi Tang
Author: Wenyi Tang
E-mail: wenyitang@outlook.com

Use precompiled headers (PCH) on Windows MSVC to speed up build time.

Usage:

    load("@vila//vila/bazel:pre_compile.bzl", "pch_library")

    flags = pch_library(
        name = "mypch",
        pch_hdr = "//lib:pch.h",  # must be absolute label for now
        deps = ["//other:dep"],
    )

    cc_library(
        name = "mylib",
        srcs = ["mylib.cpp"],
        hdrs = ["mylib.h"],
        deps = [":mypch"],
        copts = flags,
    )

Known Limitations:

    1. Only MSVC on Windows is supported.
    2. PCH generation has a multi-thread conflict when using Bazel's parallel build.

          To overcome this problem, we recommend to build PCH headers first, and then
          build the dependent cc_library targets.

          ```
          bazel build //:mypch
          bazel build //:mylib
          ```

          Or you may build with `--jobs=1` to disable parallel build.


See reference for more details.

Ref:

1. https://learn.microsoft.com/en-us/cpp/build/creating-precompiled-header-files
2. https://learn.microsoft.com/en-us/cpp/build/reference/yc-create-precompiled-header-file
3. https://learn.microsoft.com/en-us/cpp/build/reference/yu-use-precompiled-header-file
"""

def _make_pch_impl(ctx):
    ctx.actions.write(
        output = ctx.outputs.pch_cxx,
        content = "// BAZEL AUTO GENERATED",
        is_executable = False,
    )
    ctx.actions.symlink(output = ctx.outputs.pch_hxx, target_file = ctx.file.pch_hdr)

make_pch = rule(
    implementation = _make_pch_impl,
    attrs = {
        "pch_hdr": attr.label(mandatory = True, allow_single_file = True, doc = "PCH header file"),
        "pch_cxx": attr.output(mandatory = True, doc = "PCH output file"),
        "pch_hxx": attr.output(mandatory = True, doc = "PCH output file"),
    },
)

def pch_library(name, pch_hdr, deps = [], cc_library_cb = None):
    """Make a cc library that creates PCH file.

    Args:
        name: name to the cc library
        pch_hdr: a label of the PCH header file
        deps: deps of the cc library
        cc_library_cb: callback to the cc_library rule. Defaults to native.cc_library.

    Returns:
        A list of copts to use the PCH file.
    """
    if not pch_hdr.startswith("//"):
        fail("pch_hdr must be an absolute label, eg. //lib:pch.h")
    pch_header = Label(pch_hdr).name + ".hxx"
    make_pch(
        name = name + "_pch",
        pch_hdr = pch_hdr,
        pch_cxx = name + ".pch.cxx",
        pch_hxx = pch_header,
    )
    pch_lbl = Label(pch_hdr)
    pch_path = "$(BINDIR)/" + pch_lbl.package.replace("/", "_") + "_" + pch_lbl.name + ".pch"
    cc_library = native.cc_library if cc_library_cb == None else cc_library_cb
    cc_library(
        name = name,
        srcs = [":" + name + "_pch"],
        hdrs = [pch_header],
        copts = [
            "/Yc" + pch_header,
            "/FI" + pch_header,
            "/Fp" + pch_path,
        ],
        includes = ["."],
        target_compatible_with = ["@platforms//os:windows"],
        visibility = ["//visibility:public"],
        deps = deps,
    )
    return [
        "/Yu" + pch_header,
        "/FI" + pch_header,
        "/Fp" + pch_path,
    ]
