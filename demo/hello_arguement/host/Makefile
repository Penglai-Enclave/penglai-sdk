PENGLAI_LIB ?= /opt/penglai
CC=riscv64-unknown-linux-gnu-gcc
# 检测是否存在riscv64-unknown-linux-gnu-gcc工具链，如果不存在则使用gcc
ifeq ($(shell command -v riscv64-unknown-linux-gnu-gcc 2> /dev/null),)
    CC = gcc
endif

HOST=host

all:
	$(CC) -o $(HOST) -static -I .. -I $(PENGLAI_LIB)/lib/host/include/  host.c  $(PENGLAI_LIB)/lib/libpenglai-enclave-host.a  -lpthread

clean:
	rm -f *.o $(HOST)
