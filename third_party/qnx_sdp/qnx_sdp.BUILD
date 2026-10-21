# ------------------------------------
# Copyright (c) 2026 Seweryn Plywaczyk
#
# SPDX-License-Identifier: Apache-2.0
# ------------------------------------

load("@open_qnx_ecu//bazel/toolchains/qnx/qcc:cc_toolchain_config.bzl", "cc_toolchain_config")
load("@rules_cc//cc/toolchains:cc_toolchain.bzl", "cc_toolchain")

package(default_visibility = ["//visibility:public"])

# Toolchain config
cc_toolchain_config(
    name = "qnx_x86_64_toolchain_config",
)

cc_toolchain(
    name = "qnx_x86_64_toolchain",
    all_files = ":all_files",
    ar_files = ":all_files",
    as_files = ":all_files",
    compiler_files = ":all_files",
    dwp_files = ":empty",
    linker_files = ":all_files",
    objcopy_files = ":empty",
    strip_files = ":all_files",
    supports_param_files = 0,
    toolchain_config = ":qnx_x86_64_toolchain_config",
)

filegroup(
    name = "all_files",
    srcs = glob(["**/*"]),
)

filegroup(
    name = "qcc",
    srcs = ["host/linux.x86_64/usr/bin/qcc"],
)

filegroup(
    name = "cxx_builtin_include_directories",
    srcs = [
        "host/linux/x86_64/usr/lib/gcc/x86_64-pc-nto-qnx8.0.0/12.2.0/include",
        "target/qnx/usr/include",
        "target/qnx/usr/include/c++/v1",
    ],
)
