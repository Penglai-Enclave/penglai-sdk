CC = riscv64-unknown-linux-gnu-gcc
C++ = riscv64-unknown-linux-gnu-g++
CFLAGS := -O3 -static -DRISCV
#
#CC=riscv64-linux-gnu-gcc

#CC = gcc
#C++ = g++
#CFLAGS := -O3 -std=gnu99

all: entry_func/*.c imageErosion/*.c imageResize/*.c imageRotate/*.c serverless-user/*.c
	CC=$(CC)  make -C serverless-user
	CC=$(CC)  make -C entry_func
	CC=$(CC)  make -C imageResize
	CC=$(CC)  make -C imageRotate
	CC=$(CC)  make -C imageErosion


clean:
	CC=$(CC)  make -C serverless-user clean
	CC=$(CC)  make -C entry_func clean
	CC=$(CC)  make -C imageResize clean
	CC=$(CC)  make -C imageRotate clean
	CC=$(CC)  make -C imageErosion clean
