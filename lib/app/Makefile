GCC ?= riscv64-unknown-linux-gnu-gcc
CFLAGS = -Wall -shared -fPIC
LINK ?= riscv64-unknown-linux-gnu-ld
LINKFLAGS = -static
INCLUDE_DIR = include
GM_INCLUDE_DIR = ../gm/include
ARC ?= riscv64-unknown-elf-ar
ARCFLAGS = rcs

ASM_SRCS = src/eret.S
C_SRCS = src/print.c src/seal.c

A_OBJS = $(patsubst %.S,%.o,$(ASM_SRCS))
C_OBJS = $(patsubst %.c,%.o,$(C_SRCS))

TARCGET = ../libpenglai-enclave-eapp.a

all: $(TARCGET) METADATA

METADATA:
	$(GCC) $(CFLAGS) -c src/metadata_sec.S -o src/metadata_sec.o

$(A_OBJS): %.o : %.S
	$(GCC) $(CFLAGS) -I$(INCLUDE_DIR) -I$(GM_INCLUDE_DIR) -c $< -o $@
$(C_OBJS): %.o: %.c
	$(GCC) $(CFLAGS) -I$(INCLUDE_DIR) -I$(GM_INCLUDE_DIR) -c $< -o $@
$(TARCGET): $(A_OBJS) $(C_OBJS)
	$(ARC) $(ARCFLAGS) $@ $^


clean:
	rm -f $(A_OBJS) $(C_OBJS) $(TARCGET)
	rm -f src/metadata_sec.o
