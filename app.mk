CC = riscv64-unknown-linux-gnu-gcc
CFLAGS = -Wall
LINK = riscv64-unknown-linux-gnu-ld
AS = riscv64-unknown-linux-gnu-as

SDK_LIB_DIR = $(PENGLAI_SDK)/lib
MUSL_LIB_DIR = $(PENGLAI_SDK)/musl/lib
MUSL_LIBC = $(MUSL_LIB_DIR)/libc.a
SDK_APP_LIB = $(SDK_LIB_DIR)/libpenglai-enclave-eapp.a
GCC_LIB = $(SDK_LIB_DIR)/libgcc.a
SDK_INCLUDE_DIR = $(SDK_LIB_DIR)/app/include

LDFLAGS = -static -L$(SDK_LIB_DIR) -L$(MUSL_LIB_DIR) -lpenglai-enclave-eapp -lc
#LDFLAGS = -static -L$(SDK_LIB_DIR) -lpenglai-enclave-eapp
CFLAGS += -I$(SDK_INCLUDE_DIR)

APP_C_OBJS = $(patsubst %.c,%.o, $(APP_C_SRCS))
APP_A_OBJS = $(patsubst %.s,%.o, $(APP_A_SRCS))
APP_LDS ?= $(PENGLAI_SDK)/app.lds

APP_BIN = $(patsubst %,%,$(APP))

all: $(APP_BIN)

$(APP_C_OBJS): %.o: %.c
	echo $(PENGLAI_SDK)
	$(CC) $(CFLAGS) -c $<

$(APP_BIN): % : $(APP_C_OBJS) $(APP_A_OBJS) $(SDK_APP_LIB) $(MUSL_LIBC) $(GCC_LIB)
	$(LINK) $(LDFLAGS) -o $@ $^ $(SDK_LIB_DIR)/app/src/metadata_sec.o -T $(APP_LDS)
	chmod -x $@

clean:
	rm -f *.o $(APP_BIN) $(EXTRA_CLEAN)
