# Penglai SDK

This is the SDK for Penglai TEE (sPMP and PMP version).

If you are seeking for SDK for Penglai-TVM,
please refer [Penglai-SDK-TVM](https://github.com/Penglai-Enclave/Penglai-sdk-TVM)

This repo contains SDK to build host-side apps and enclave apps, as well as some demos.

## Build SDK

SDK needs to be built from sources firstly, with the following commands.

```
# Specify cross compiler toolchain for enclave app and host app.
# riscv64-unknown-elf- is used for enclave app, and
# riscv64-unknown-linux-gnu- for host-side app, by default
export CROSS_COMPILE=<prefix_of_enclave_app_cross_compiler_toolchain>
export CROSS_COMPILE_HOST=<prefix_of_host_app_cross_compiler_toolchain>

# Specifiy architecture of target board,
# 'rv64imac' by default
export RISCV_ARCH=<arch_of_target_board>

# Specifiy abi of target board,
# 'lp64' by default
export RISCV_ABI=<abi_of_target_board>

# Pre-build all the required components for this SDK, i.e, musl.
# prebuild only needs to run once
make prebuild

# SDK is built under <repo_path>/build, by default
export OUT_DIR=/path/to/build

# SDK is installed under <repo_path>/build/output, by default
export PENGLAI_SDK=/path/to/to/penglai_sdk

make install
```

If needs to clean/rebuild SDK,

```
# clean build output of SDK
make clean

# clean installed files/headers
# under PENGLAI_SDK
make cleanclean

```

## Demos

The demo/ directory contains example enclave/host apps.
You need to build and install SDK first before building demo apps.

```
# Build demo with SDK specified with PENGLAI_SDK
make demo
```

After Penglai is deployed on the target device,
you can run the demo with the following commands,
where the `enclave_app_file_name` is the file name
of enclave app, i.e, prime.

```
./host <enclave_app_file_name>
```

The `host` is a host app also included in the demo.
It's an enclave invoker that starts an enclave with the
corresponding enclave app ELF.

## How to build an enclave/a host app?

The SDK includes build scripts that could
help with building enclave app and host app.

To build an enclave/host app, the following variables
need to be defined,
```

# APP: name of the generated ELF
APP :=

# CFLAGS: compile flags for all the
# sources of this enclave
CFLAGS :=

# If needs to specify compile flags only for
# certain sources, defined variable <file_name>_CFLAGS,
# .i.e, main.c_CFLAGS
<file_name>_CFLAGS :=

# LDFLAGS: linking flags for
# this enclave app
LDFLAGS :=

# APP_C_SRCS  .c sources of this enclave
APP_C_SRCS :=

# APP_A_SRCS  .S sources of this enclave
APP_A_SRCS :=

# linker script for this app
# If not specified, use <sdk>/mk/app.lds by default
#
# This variable is not used for the
# building of host app
APP_LDS :=

```

`<sdk_path>/mk/app.mk` needs to be included
after defining the variables above when building
an enclave app, and similarly,
`<sdk_path>/mk/host_app.mk>` for a host app.

You could refer to build scripts in demo as examples.

Just as building the SDK, one should also define environment variables,
`PENGLAI_SDK`, `CROSS_COMPILE`, `CROSS_COMPILE_HOST`,
`RISCV_ARCH`, `RISCV_ABI` when building host apps and enclave apps.

`PENGLAI_SDK` must be specified.

The default values of `CROSS_COMPILE`, `CROSS_COMPILE_HOST`,
`RISCV_ARCH` and `RISCV_ABI` are the same with that when
building the SDK.

Developers could define 'O' as the output directory.
The default value of output directory is
current directory.

## Licenses

Mulan Permissive Software License，Version 1 (Mulan PSL v1)

## Code Contributions

Please use Merge Request/Pull Requests on to pose and contribute your work.

Please fell free to post your concerns, ideas, code or anything others to issues.

## Wiki and more documents

Refer our [doc website](https://penglai-doc.readthedocs.io/en/latest/).
