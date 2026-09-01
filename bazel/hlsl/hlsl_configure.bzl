"""
Copyright (c) 2022-2026 Wenyi Tang
Author: Wenyi Tang
E-mail: wenyitang@outlook.com

Configure HLSL compilers
"""

load("@rules_cc//cc/private/toolchain:windows_cc_configure.bzl", "find_vc_path", "setup_vc_env_vars")

def _hlsl_configure(ctx):
    vc_path = find_vc_path(ctx)
    if not vc_path:
        # NOTFOUND is a keyword for detecting existence of FXC/DXC
        fxc = "VC is NOTFOUND on host, please specify BAZEL_VC."
        dxc = "VC is NOTFOUND on host, please specify BAZEL_VC."
    else:
        env = setup_vc_env_vars(ctx, vc_path, ["WINDOWSSDKDIR", "WINDOWSSDKVERSION"])
        sdk_bin_path = ctx.path("{}\\bin\\{}".format(env["WINDOWSSDKDIR"], env["WINDOWSSDKVERSION"]))

        # find in environment
        if "FXC" in ctx.os.environ:
            fxc = ctx.path(ctx.os.environ["FXC"])
        else:
            fxc = ctx.path("{}\\x64\\fxc.exe".format(sdk_bin_path))
        if "DXC" in ctx.os.environ:
            dxc = ctx.path(ctx.os.environ["DXC"])
        else:
            dxc = ctx.path("{}\\x64\\dxc.exe".format(sdk_bin_path))

    ctx.template("BUILD.bazel", Label("//bazel/hlsl:BUILD.tpl"))
    ctx.template("compiler.bzl", Label("//bazel/hlsl:compiler.bzl.tpl"), {
        "%{fxc}": str(fxc),
        "%{dxc}": str(dxc),
    })

hlsl_configure = repository_rule(
    implementation = _hlsl_configure,
    environ = ["BAZEL_VC"],
)
