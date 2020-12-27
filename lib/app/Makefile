CC = riscv64-unknown-linux-gnu-gcc
CFLAGS = -Wall
LINK = riscv64-unknown-linux-gnu-ld
LINKFLAGS = -static
INCLUDE_DIR = include
AR = riscv64-unknown-elf-ar
ARFLAGS = rcs

ASM_SRCS = src/eret.S
C_SRCS = src/server_enclave.c

A_OBJS = $(patsubst %.S,%.o,$(ASM_SRCS))
C_OBJS = $(patsubst %.c,%.o,$(C_SRCS))

TARGET = ../libpenglai-enclave-eapp.a

all: $(TARGET)

$(A_OBJS): %.o : %.S
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@
$(C_OBJS): %.o: %.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@
$(TARGET): $(A_OBJS) $(C_OBJS)
	$(AR) $(ARFLAGS) $@ $^


clean:
	rm -f $(A_OBJS) $(C_OBJS) $(TARGET)
