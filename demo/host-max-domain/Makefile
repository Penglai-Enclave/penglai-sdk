CC=riscv64-unknown-linux-gnu-gcc

HOST=host-max-domain

all:
	$(CC) -o $(HOST) -static -I $(PENGLAI_SDK)/lib/host/include/  $(HOST).c  $(PENGLAI_SDK)/lib/libpenglai-enclave-host.a  -lpthread

clean:
	rm -f *.o $(HOST)
