"""
Copyright (c) 2024 Intel Corporation
Author: Wenyi Tang
E-mail: wenyi.tang@intel.com

make symlink of the target
"""

def _depset_to_list(x):
    """Helper function to convert depset to list."""
    iter_list = x.to_list() if type(x) == "depset" else x
    return iter_list

def _symlink(ctx):
    out_files = []
    for t in ctx.attr.targets:
        for file in _depset_to_list(t.data_runfiles.files):
            if ctx.attr.keep_source_tree:
                out = ctx.actions.declare_file(file.dirname + "/" + file.basename)
            else:
                out = ctx.actions.declare_file(file.basename)
                if out in out_files:
                    continue
            ctx.actions.symlink(output = out, target_file = file)
            out_files.append(out)
    return DefaultInfo(data_runfiles = ctx.runfiles(files = out_files))

symlink = rule(
    implementation = _symlink,
    attrs = {
        "targets": attr.label_list(mandatory = True, doc = "binary targets to make the symlink"),
        "keep_source_tree": attr.bool(default = False, doc = "keep the source directory tree"),
    },
)

def _symlink_exe(ctx):
    ret = _symlink(ctx)
    for file in _depset_to_list(ret.data_runfiles.files):
        if file.extension == "exe" or file.extension == "":
            return DefaultInfo(
                files = ret.data_runfiles.files,
                executable = file,
            )
    fail("No executable found in the symlinked targets")

symlink_exe = rule(
    implementation = _symlink_exe,
    attrs = {
        "targets": attr.label_list(mandatory = True, doc = "binary targets to make the symlink"),
        "keep_source_tree": attr.bool(default = False, doc = "keep the source directory tree"),
    },
    executable = True,
)
