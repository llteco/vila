"""
Copyright (c) 2025 Intel Corporation
Author: Wenyi Tang
E-mail: wenyi.tang@intel.com

"""

def is_windows(ctx):
    """Returns True if the current platform is Windows."""

    return "windows" in ctx.os.name

def is_x86_64(ctx):
    """Returns True if the current platform is x86_64."""

    return ctx.os.arch == "x86_64"
