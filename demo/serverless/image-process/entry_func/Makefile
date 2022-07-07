#CC=riscv64-unknown-linux-gnu-gcc
HOST=entry

all:
	$(CC) -static -o $(HOST) -I ../../sod-lib/  entry.c  ./sod.c -lpthread -lm

clean:
	rm -f *.o $(HOST)
