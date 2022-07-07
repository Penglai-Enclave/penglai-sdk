#CC=riscv64-unknown-linux-gnu-gcc
HOST=imageErosion

all:
	$(CC) -static -o $(HOST) -I ../../sod-lib/  $(HOST).c  ./sod.c -lpthread -lm

clean:
	rm -f *.o $(HOST)
