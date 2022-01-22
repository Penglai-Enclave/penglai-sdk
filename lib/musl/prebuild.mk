.PHONY: all

all:
	./configure --disable-shared --target=riscv64 CROSS_COMPILE=$(CROSS_COMPILE) CFLAGS='-march=$(RISCV_ARCH) -mabi=$(RISCV_ABI)'
	make
