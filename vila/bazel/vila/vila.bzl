"""
Copyright (c) 2022-2026 Wenyi Tang
Author: Wenyi Tang
E-mail: wenyitang@outlook.com

Convenient object library for vila source code
"""

load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_import", "cc_library", "cc_test")
load(
    "@vila//vila/bazel/vila:compile_options.bzl",
    "CXX_STD_17",
    "DEFAULT_COPTS",
    "DEFAULT_LINKOPTS",
    "UNICODE",
)

def vila_cc_library(name, **kwargs):
    """ Helper functions to add default copts and linkopts to cc library.

    Args:
        name: pass to the library name.
        **kwargs: pass other arguments.
    """
    kwargs["copts"] = DEFAULT_COPTS + kwargs.get("copts", [])
    cc_library(name = name, **kwargs)

def vila_cc_binary(name, **kwargs):
    """ Helper functions to add default copts and linkopts to cc binary.

    Args:
        name: pass to the library name.
        **kwargs: pass other arguments.
    """
    kwargs["copts"] = DEFAULT_COPTS + kwargs.get("copts", [])
    kwargs["linkopts"] = DEFAULT_LINKOPTS + kwargs.get("linkopts", [])
    linkstatic = kwargs.pop("linkstatic", True)
    cc_binary(name = name, linkstatic = linkstatic, **kwargs)

def vila_cc_test(name, **kwargs):
    """ Helper functions to add default copts and linkopts to cc test.

    Args:
        name: pass to the library name.
        **kwargs: pass other arguments.
    """
    kwargs["copts"] = CXX_STD_17 + UNICODE + kwargs.get("copts", [])
    kwargs["linkopts"] = DEFAULT_LINKOPTS + kwargs.get("linkopts", [])
    linkstatic = kwargs.pop("linkstatic", True)
    cc_test(name = name, linkstatic = linkstatic, **kwargs)

def vila_dll_library(name, srcs, deps = None, visibility = None):
    """ Helper functions to add a library that depends on shared libraries from `srcs`.

    This will also link output dll to target output directory.

    Args:
        name: pass to the library name.
        srcs: list of dependent shared libraries
        deps: additional dependencies
        visibility: target visibility
    """
    imports = [] if deps == None else list(deps)
    for i, lib in enumerate(srcs):
        native.filegroup(
            name = "__{}{}_intf".format(i, name),
            srcs = [lib],
            output_group = "interface_library",
            target_compatible_with = ["@platforms//os:windows"],
            visibility = ["//visibility:private"],
        )
        cc_import(
            name = "__{}{}_dll".format(i, name),
            interface_library = ":__{}{}_intf".format(i, name),
            shared_library = lib,
            visibility = ["//visibility:private"],
        )
        imports.append(":__{}{}_dll".format(i, name))
    cc_library(
        name = name,
        deps = imports,
        visibility = visibility,
    )

def vila_so_library(name, srcs, deps = None, visibility = None):
    """ Helper functions to add a library that depends on shared libraries from `srcs`.

    This will also link output dll to target output directory.

    Args:
        name: pass to the library name.
        srcs: list of dependent shared libraries
        deps: additional dependencies
        visibility: target visibility
    """
    imports = [] if deps == None else list(deps)
    for i, lib in enumerate(srcs):
        cc_import(
            name = "__{}{}_so".format(i, name),
            shared_library = lib,
            target_compatible_with = ["@platforms//os:linux"],
            visibility = ["//visibility:private"],
        )
        imports.append("__{}{}_so".format(i, name))
    cc_library(
        name = name,
        deps = imports,
        visibility = visibility,
    )
