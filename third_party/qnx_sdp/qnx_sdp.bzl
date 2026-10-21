# ------------------------------------
# Copyright (c) 2026 Seweryn Plywaczyk
#
# SPDX-License-Identifier: Apache-2.0
# ------------------------------------

QNX_SDP_PACKAGE = "QNX_SDP_PACKAGE"

def _qnx_sdp_repo_impl(ctx):
    if QNX_SDP_PACKAGE not in ctx.os.environ:
        fail("ERROR: Variable {} not set".format(QNX_SDP_PACKAGE))

    qnx_package = ctx.os.environ[QNX_SDP_PACKAGE]
    ctx.extract(archive = qnx_package)
    ctx.symlink(ctx.attr.build_file, "BUILD")

qnx_sdp_repo = repository_rule(
    implementation = _qnx_sdp_repo_impl,
    environ = [QNX_SDP_PACKAGE],
    attrs = {
        "build_file": attr.label(allow_single_file = True, mandatory = True),
    },
)

def _qnx_sdp_extension_impl(ctx):
    qnx_sdp_repo(
        name = "qnx_sdp",
        build_file = ":qnx_sdp.BUILD",
    )

qnx_sdp_extension = module_extension(
    implementation = _qnx_sdp_extension_impl,
)
