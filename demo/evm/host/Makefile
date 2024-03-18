CC=riscv64-unknown-linux-gnu-gcc
CXX := riscv64-unknown-linux-gnu-g++

HOST=evm_host

all:
	$(CXX) -o $(HOST) -static -I $(PENGLAI_LIB)/lib/host/include/ host.cpp $(PENGLAI_LIB)/lib/libpenglai-enclave-host.a  -lpthread

clean:
	rm -f *.o $(HOST)
