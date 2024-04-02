PENGLAI_LIB ?= /opt/penglai
CC=$(CROSS_COMPILE)gcc

HOST=penglai-host

all:
	$(CC) -o $(HOST) -static -I ../lib/host/include/  host.c  ../lib/libpenglai-enclave-host.a  -lpthread

clean:
	rm -f *.o $(HOST)
