GCC ?= riscv64-unknown-linux-gnu-gcc
INCLUDE_DIR = include/
SRC_DIR = src
GCCFLAGS = -Wall -Werror -shared -fPIC -I $(INCLUDE_DIR)
SRCS = param.c penglai-enclave.c elf.c
OBJS = $(patsubst %.c,%.o,$(SRCS))
ARC ?= riscv64-unknown-elf-ar
ARCFLAGS = rcs

TARCGET=../libpenglai-enclave-host.a

all: $(TARCGET)

$(TARCGET): $(OBJS)
	$(ARC) $(ARCFLAGS) $@ $^

$(OBJS): %.o: $(addprefix $(SRC_DIR)/, %.c)
	$(GCC) $(GCCFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARCGET) 
