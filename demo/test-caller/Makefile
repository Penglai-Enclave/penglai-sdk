CC=riscv64-unknown-linux-gnu-gcc

HOST=test-caller

all:
	$(CC) -o $(HOST) -I $(PENGLAI_SDK)/lib/host/include/  host.c  $(PENGLAI_SDK)/lib/libpenglai-enclave-host.a  -lpthread

clean:
	rm -f *.o $(HOST)
