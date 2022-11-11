# all: TestRandom TestSM2 TestSM4_CBC TestSM4_OCB

# LIBS = ../gm/sm2.a ../gm/miracl.a 
# CFLAGS = -I ../gm/include

# TestSM2: test_sm2.c $(LIBS)
# 	$(CC) -static $(CFLAGS) -o $@ $^ 

# TestRandom: test_random.c $(LIBS)
# 	$(CC) -static $(CFLAGS) -o $@ $^ 

# TestSM4_CBC: test_sm4_cbc.c $(LIBS)
# 	$(CC) -static $(CFLAGS) -o $@ $^

# TestSM4_OCB: test_sm4_ocb.c $(LIBS)
# 	$(CC) -static $(CFLAGS) -o $@ $^

# clean:
# 	-rm -f TestSM2 TestRandom TestSM4_CBC TestSM4_OCB


APP = test_sm2 test_random test_sm4_cbc test_sm4_ocb
APP_C_SRCS = test_sm2.c test_random.c test_sm4_cbc.c test_sm4_ocb.c

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
GM_LIBS = $(SDK_LIB_DIR)/gm/sm2.a $(SDK_LIB_DIR)/gm/miracl.a
GM_LIB_DIR = $(SDK_LIB_DIR)/gm
GM_LIB_INCLUDE_DIR = $(SDK_LIB_DIR)/gm/include

LDFLAGS = -static -L$(SDK_LIB_DIR) -L$(MUSL_LIB_DIR) -L$(GM_LIB_DIR) -lpenglai-enclave-eapp -lc
CFLAGS += -I$(SDK_INCLUDE_DIR) -I$(GM_LIB_INCLUDE_DIR) -I$(GM_LIB_INCLUDE_DIR)/miracl

APP_C_OBJS = $(patsubst %.c,%.o, $(APP_C_SRCS))
APP_LDS ?= $(PENGLAI_SDK)/app.lds

APP_BIN = $(patsubst %,%,$(APP))

all: $(APP_BIN)

$(APP_C_OBJS): %.o: %.c
	echo $(PENGLAI_SDK)
	$(CC) $(CFLAGS) -c $<

$(APP_BIN): % : %.o $(SDK_APP_LIB) $(GM_LIBS) $(MUSL_LIBC) $(GCC_LIB)
	$(LINK) $(LDFLAGS) -o $@ $^ -T $(APP_LDS)
	chmod -x $@

clean:
	rm -f *.o $(APP_BIN)
