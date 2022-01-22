CC=riscv64-unknown-linux-gnu-gcc

HOST=host

all:
	$(CC) -o $(HOST) -static -I .. -I $(PENGLAI_SDK)/lib/host/include/  host.c  $(PENGLAI_SDK)/lib/libpenglai-enclave-host.a  -lpthread

clean:
	rm -f *.o $(HOST)
