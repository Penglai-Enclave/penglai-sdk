APP = prime
APP_C_SRCS = prime.c
EXTRA_CLEAN = $(APP).dump
# include $(PENGLAI_LIB)/app.mk
PENGLAI_LIB ?= /opt/penglai

# below code is app.mk add with "CFLAGS += -I .. "
CC = $(CROSS_COMPILE)gcc
CFLAGS = -Wall 
CFLAGS += -I ..
LINK = $(CROSS_COMPILE)ld
AS = $(CROSS_COMPILE)as
endif

SDK_LIB_DIR = $(PENGLAI_LIB)/lib
RUNTIME_LIB_DIR = $(PENGLAI_LIB)/runtime/lib
MUSL_LIBC = $(RUNTIME_LIB_DIR)/libc.a
SDK_APP_LIB = $(SDK_LIB_DIR)/libpenglai-enclave-eapp.a
GCC_LIB = $(SDK_LIB_DIR)/libgcc.a
SDK_INCLUDE_DIR = $(SDK_LIB_DIR)/app/include

LDFLAGS = -static -L$(SDK_LIB_DIR) -L$(RUNTIME_LIB_DIR) -lpenglai-enclave-eapp -lc
#LDFLAGS = -static -L$(SDK_LIB_DIR) -lpenglai-enclave-eapp
CFLAGS += -I$(SDK_INCLUDE_DIR)

APP_C_OBJS = $(patsubst %.c,%.o, $(APP_C_SRCS))
APP_A_OBJS = $(patsubst %.s,%.o, $(APP_A_SRCS))
APP_LDS ?= $(PENGLAI_LIB)/app.lds

APP_BIN = $(patsubst %,%,$(APP))

all: $(APP_BIN)

$(APP_C_OBJS): %.o: %.c
	echo $(PENGLAI_LIB)
	$(CC) $(CFLAGS) -c $<

$(APP_BIN): % : $(APP_C_OBJS) $(APP_A_OBJS) $(SDK_APP_LIB) $(MUSL_LIBC) $(GCC_LIB)
	$(LINK) $(LDFLAGS) -o $@ $^ -T $(APP_LDS)
	chmod -x $@

clean:
	rm -f *.o $(APP_BIN) $(EXTRA_CLEAN)
