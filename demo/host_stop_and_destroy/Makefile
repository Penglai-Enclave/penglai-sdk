CC = $(CROSS_COMPILE)gcc

HOST=host

all:
	$(CC) -march=rv64imac -mabi=lp64 -o $(HOST) -static -I $(PENGLAI_SDK)/lib/host/include/  host.c  $(PENGLAI_SDK)/lib/libpenglai-enclave-host.a  -lpthread

clean:
	rm -f *.o $(HOST)
