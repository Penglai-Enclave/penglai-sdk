# Penglai-EVM

This demo implements a relatively complete Ethereum Virtual Machine(EVM) in the Penglai system.

## Project file introduction

- `host` directory: Place the corresponding host file. In the host file, the bytecode and parameters that need to be executed will be passed to the enclave.
- `include` directory: Place header files used by the project.
- `lib` directory: Place the static libraries needed to import C++.
- `src` directory: 
  - `uint256` directory: Implement the uint256 type that needs to be used in the project.
  - `evm_entry.c`: Entry file of enclave.
  - `evm_main.cpp`: Complete the system initialization and create the corresponding processor to execute the bytecode.
  - `process.cpp`: The core part, executing bytecode, will execute corresponding logic according to different instructions.
  - `stack.cpp`: The stack structure used in EVM.
  - `stub.cpp`: Some stub functions involved in the static library.
  - `util.cpp`: Provide some basic methods, such as data type conversion, encryption and decryption, etc.
