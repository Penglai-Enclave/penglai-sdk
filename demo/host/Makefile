PENGLAI_LIB ?= /opt/penglai
CC=$(CROSS_COMPILE)gcc

HOST=host

all:
	echo $(PENGLAI_LIB)
	$(CC) -o $(HOST) -static -I $(PENGLAI_LIB)/lib/host/include/  host.c  $(PENGLAI_LIB)/lib/libpenglai-enclave-host.a  -lpthread

clean:
	rm -f *.o $(HOST)
