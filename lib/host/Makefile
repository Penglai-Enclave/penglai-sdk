CC = riscv64-unknown-linux-gnu-gcc
INCLUDE_DIR = include/
SRC_DIR = src
CCFLAGS = -Wall -Werror -shared -fPIC -I $(INCLUDE_DIR)
SRCS = param.c penglai-enclave.c elf.c
OBJS = $(patsubst %.c,%.o,$(SRCS))
AR = riscv64-unknown-elf-ar
ARFLAGS = rcs

# 检测是否存在riscv64-unknown-linux-gnu-gcc工具链，如果不存在则使用gcc
ifeq ($(shell command -v riscv64-unknown-linux-gnu-gcc 2> /dev/null),)
    CC = gcc
	AS = as
	AR = ar
endif

TARGET=../libpenglai-enclave-host.a

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

$(OBJS): %.o: $(addprefix $(SRC_DIR)/, %.c)
	$(CC) $(CCFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET) 