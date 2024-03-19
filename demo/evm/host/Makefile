PENGLAI_LIB ?= /opt/penglai
CC=$(CROSS_COMPILE)gcc
CXX := $(CROSS_COMPILE)g++



HOST=evm_host

all:
	$(CXX) -o $(HOST) -static -I $(PENGLAI_LIB)/lib/host/include/ host.cpp $(PENGLAI_LIB)/lib/libpenglai-enclave-host.a  -lpthread

clean:
	rm -f *.o $(HOST)
