CC=riscv64-unknown-linux-gnu-gcc

HOST=sha512

all:
	$(CC) -o $(HOST) -static -I $(PENGLAI_SDK)/lib/host/include/ -I.  $(HOST).c  $(PENGLAI_SDK)/lib/libpenglai-enclave-host.a  -lpthread

clean:
	rm -f *.o $(HOST)
