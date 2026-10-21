# ------------------------------------
# Copyright (c) 2026 Seweryn Plywaczyk
#
# SPDX-License-Identifier: Apache-2.0
# ------------------------------------

filegroup(
    name = "empty",
)

filegroup(
    name = "all_files",
    srcs = [
        "@qnx_sdp//:all_files",
    ],
)

toolchain(
    name = "cc_toolchain_for_qnx_x86_64",
    exec_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
    ],
    target_compatible_with = [
        "@platforms//os:qnx",
        "@platforms//cpu:x86_64",
    ],
    toolchain = "@qnx_sdp//:qnx_x86_64_toolchain",
    toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
)
