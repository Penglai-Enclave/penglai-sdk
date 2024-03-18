CC = riscv64-unknown-linux-gnu-gcc
CFLAGS = -Wall -shared -fPIC
LINK = riscv64-unknown-linux-gnu-ld
LINKFLAGS = -static
INCLUDE_DIR = include
GM_INCLUDE_DIR = ../gm/include
AR = riscv64-unknown-elf-ar
ARFLAGS = rcs

# 检测是否存在riscv64-unknown-linux-gnu-gcc工具链，如果不存在则使用gcc
ifeq ($(shell command -v riscv64-unknown-linux-gnu-gcc 2> /dev/null),)
    CC = gcc
	LINK = ld
	AS = as
	AR = ar
endif

ASM_SRCS = src/eret.S
C_SRCS = src/print.c src/seal.c

A_OBJS = $(patsubst %.S,%.o,$(ASM_SRCS))
C_OBJS = $(patsubst %.c,%.o,$(C_SRCS))

TARGET = ../libpenglai-enclave-eapp.a

all: $(TARGET)

$(A_OBJS): %.o : %.S
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -I$(GM_INCLUDE_DIR) -c $< -o $@
$(C_OBJS): %.o: %.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -I$(GM_INCLUDE_DIR) -c $< -o $@
$(TARGET): $(A_OBJS) $(C_OBJS)
	$(AR) $(ARFLAGS) $@ $^


clean:
	rm -f $(A_OBJS) $(C_OBJS) $(TARGET)
