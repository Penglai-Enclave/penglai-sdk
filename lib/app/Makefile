CC = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -shared -fPIC
LINK = $(CROSS_COMPILE)ld
LINKFLAGS = -static
INCLUDE_DIR = include
GM_INCLUDE_DIR = ../gm/include
AR = $(CROSS_COMPILE)ar
ARFLAGS = rcs

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
