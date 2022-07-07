CC=riscv64-unknown-linux-gnu-gcc

HOST=dhrystone

all:
	$(CC) -o $(HOST) -static -I $(PENGLAI_SDK)/lib/host/include/  -I. $(HOST).c  $(PENGLAI_SDK)/lib/libpenglai-enclave-host.a  -lpthread

clean:
	rm -f *.o $(HOST)
