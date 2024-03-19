CC = $(CROSS_COMPILE)gcc
INCLUDE_DIR = include/
SRC_DIR = src
CCFLAGS = -Wall -Werror -shared -fPIC -I $(INCLUDE_DIR)
SRCS = param.c penglai-enclave.c elf.c
OBJS = $(patsubst %.c,%.o,$(SRCS))
AR = $(CROSS_COMPILE)ar
ARFLAGS = rcs

TARGET=../libpenglai-enclave-host.a

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

$(OBJS): %.o: $(addprefix $(SRC_DIR)/, %.c)
	$(CC) $(CCFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET) 