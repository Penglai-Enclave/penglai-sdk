# Penglai SDK

This is the SDK for Penglai TEE (sPMP and PMP version).

If you are seeking for SDK for Penglai-TVM,
please refer [Penglai-SDK-TVM](https://github.com/Penglai-Enclave/Penglai-sdk-TVM)

This repo contains SDK to write host-side applications and enclave-apps, as well as some demos.

## Quick Start

To compile the sdk in openEuler, run the script `./replace_compiler_prefix.sh` in the `./sdk` directory

If you are using the `riscv64-unknown-linux-gnu-` cross-compilation toolchain or the [docker environment](https://github.com/Penglai-Enclave/Penglai-Enclave-sPMP?tab=readme-ov-file#build-penglai-sdk) we provide, you do not need to execute the above script.

[Penglai (PMP)](https://github.com/Penglai-Enclave/Penglai-Enclave-sPMP) provides the instructions and scripts to build and run SDK demos.


## Demos

The demo/ directory provides several example enclave applications, e.g., prime.

## Building your own demo

As explained in [Penglai (PMP)](https://github.com/Penglai-Enclave/Penglai-Enclave-sPMP), running an enclave demo requires two executables, host and enclave.

If you just want to write some simple demos, such as `hello_world`, then you can directly use the host executable we provide.

First you should create a new directory in the demo directory to place your enclave related files, such as `hello_world`, and then create a new file for writing your code, such as `hello_world.c`.

Before you write your own enclave code, you can refer to the writing of `demo/prime/prime.c`, and you should note the following requirements:

- `EAPP_ENTRY` is used to declare that this is the entry function of an enclave;
- `EAPP_RESERVE_REG` is is used to save the value of the register;
- `EAPP_RETURN` is used to provide the return value after exiting the enclave;
- If you need to output some information, you can use the `eapp_print` function which is also used in `prime.c`, and you can also see its implementation in `lib/app/src/print.c`;
- Make sure you include the appropriate header files: `#include "eapp.h"` and `#include "print.h"`;

After you have written the logic of your enclave, you also need to write a corresponding Makefile for your enclave. If your enclave is simple, you can directly imitate the structure of `demo/prime/Makefile`.

Finally, you need to modify the `demo/Makefile` to add the name of the newly created directory to the corresponding location. For example, you need to add `make -C hello_world` under the `all` target and `make -C hello_world clean` under the `clean` target.

Now, you can re-execute `PENGLAI_SDK=$(pwd) make -j8` command. If everything is successful, you will see the corresponding executable file appear in your demo directory. Then you just need to refer to [Penglai (PMP)](https://github.com/Penglai-Enclave/Penglai-Enclave-sPMP) to copy the executable file to the VM using *scp*, and run your own demo, e.g., a hello_world enclave, using `./host hello_world`.

Congratulations! You have successfully built and run a demo written by yourself on Penglai.

## Quickly build your own demo with secGear

SecGear is a confidential computing security application development kit for the computing industry, which aims to provide a unified development framework for developers on different hardware devices. Currently, RISCV-V penglai TEE is supported.

You can refer to the following documentation to get started with secGear for Penglai enclave.

[secGear for RISC-V TEE](https://github.com/Penglai-Enclave/Penglai-secGear/blob/riscv-penglai-zx-dev/docs/riscv_tee.md#secgear-for-risc-v-tee)

## Licenses

Mulan Permissive Software License，Version 1 (Mulan PSL v1)

## Code Contributions

Please use Merge Request/Pull Requests on to pose and contribute your work.

Please fell free to post your concerns, ideas, code or anything others to issues.

## Wiki and more documents

Refer our [doc website](https://penglai-doc.readthedocs.io/en/latest/).
