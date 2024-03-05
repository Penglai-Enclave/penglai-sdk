ISA ?= rv64imafdc
ABI ?= lp64d
CFLAGS := -mabi=$(ABI) -march=$(ISA)
CXXFLAGS := $(CFLAGS)  $(EXTRA_CXXFLAGS)
CXXFLAGS += -DTFLITE_WITHOUT_XNNPACK
# LDFLAGS := --allow-multiple-definition
# For now, compile the evm using the legacy compilation chain
TOOLCHAINPATH := /home/penglai/toolchain-720/bin
CXX := $(TOOLCHAINPATH)/riscv64-unknown-linux-gnu-g++
CC := $(TOOLCHAINPATH)/riscv64-unknown-linux-gnu-gcc
LINK = $(TOOLCHAINPATH)/riscv64-unknown-linux-gnu-ld

LIBS := \
-lstdc++ \
-lgcc_eh \
-lc \
-lgcc \
-lc_add

SDK_LIB_DIR = $(PENGLAI_SDK)/lib
MUSL_LIB_DIR = $(PENGLAI_SDK)/musl/lib
GM_LIB_DIR = $(SDK_LIB_DIR)/gm

APP=evm
APP_LDS ?= $(PENGLAI_SDK)/app.lds
SDK_INCLUDE_DIR = $(SDK_LIB_DIR)/app/include
GM_LIB_INCLUDE_DIR = $(SDK_LIB_DIR)/gm/include

SDK_APP_LIB = $(SDK_LIB_DIR)/libpenglai-enclave-eapp.a
MUSL_LIBC = $(MUSL_LIB_DIR)/libc.a
GCC_LIB = $(SDK_LIB_DIR)/libgcc.a
GM_LIBS = $(SDK_LIB_DIR)/gm/sm2.a $(SDK_LIB_DIR)/gm/miracl.a

all:
	$(CC) -I$(SDK_INCLUDE_DIR) -c src/evm_entry.c -g -o evm_entry.o
	$(CXX) -c src/uint256/uint256_t.cpp $(CXXFLAGS) -g -static -o uint256_t.o
	$(CXX) -c src/uint256/uint128_t.cpp $(CXXFLAGS) -g -static -o uint128_t.o
	$(CXX) -c src/evm_main.cpp $(CXXFLAGS) -g -static -o evm_main.o
	$(CXX) -c src/processor.cpp $(CXXFLAGS) -g -static -o processor.o
	$(CXX) -c src/stack.cpp $(CXXFLAGS) -g -static -o stack.o
	$(CXX) -c src/util.cpp $(CXXFLAGS) -g -static -o util.o
	$(CXX) -c src/stub.cpp $(CXXFLAGS) -g -static -o stub.o
	$(LINK) -L./lib -L$(SDK_LIB_DIR) -L$(MUSL_LIB_DIR) -I$(GM_LIB_INCLUDE_DIR) -g -static -o $(APP) evm_entry.o uint256_t.o uint128_t.o evm_main.o processor.o stack.o util.o stub.o $(GM_LIBS) $(SDK_APP_LIB) $(MUSL_LIBC) $(GCC_LIB) $(LIBS) -T $(APP_LDS)
	chmod -x $(APP)

clean:
	rm -f *.o $(APP)
