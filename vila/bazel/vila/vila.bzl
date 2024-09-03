"""
Copyright (c) 2022 Intel Corporation
Author: Wenyi Tang
E-mail: wenyi.tang@intel.com

Convenient object library for vila source code
"""

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
    native.cc_library(name = name, **kwargs)

def vila_cc_binary(name, **kwargs):
    """ Helper functions to add default copts and linkopts to cc binary.

    Args:
        name: pass to the library name.
        **kwargs: pass other arguments.
    """
    kwargs["copts"] = DEFAULT_COPTS + kwargs.get("copts", [])
    kwargs["linkopts"] = DEFAULT_LINKOPTS + kwargs.get("linkopts", [])
    linkstatic = kwargs.pop("linkstatic", True)
    native.cc_binary(name = name, linkstatic = linkstatic, **kwargs)

def vila_cc_test(name, **kwargs):
    """ Helper functions to add default copts and linkopts to cc test.

    Args:
        name: pass to the library name.
        **kwargs: pass other arguments.
    """
    kwargs["copts"] = CXX_STD_17 + UNICODE + kwargs.get("copts", [])
    kwargs["linkopts"] = DEFAULT_LINKOPTS + kwargs.get("linkopts", [])
    linkstatic = kwargs.pop("linkstatic", True)
    native.cc_test(name = name, linkstatic = linkstatic, **kwargs)
