CC=riscv64-unknown-linux-gnu-gcc
CXX := riscv64-unknown-linux-gnu-g++

# 检测是否存在riscv64-unknown-linux-gnu-gcc工具链，如果不存在则使用gcc
ifeq ($(shell command -v riscv64-unknown-linux-gnu-gcc 2> /dev/null),)
    CC = gcc
	CXX = g++
endif

HOST=evm_host

all:
	$(CXX) -o $(HOST) -static -I $(PENGLAI_LIB)/lib/host/include/ host.cpp $(PENGLAI_LIB)/lib/libpenglai-enclave-host.a  -lpthread

clean:
	rm -f *.o $(HOST)
