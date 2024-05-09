CC = riscv64-unknown-linux-gnu-gcc
CCFLAGS = -Wall -Werror -shared -fPIC
SRCS = big.c ecc.c random.c sm2.c sm3.c
OBJS = $(patsubst %.c,%.o,$(SRCS))
AR = riscv64-unknown-elf-ar
ARFLAGS = rcs

TARGET=../libpenglai-signtool_gm.a

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

$(OBJS): %.o: %.c
	$(CC) $(CCFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET) 