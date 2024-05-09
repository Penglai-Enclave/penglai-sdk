##
## Author: Shang Qingyu
## Description:
## 	 compile sign tool need openssl 3.0.0 installation. Now the docker environment
##   doesn't support openssl. So compilation is done in RISC-V qemu. Compile GM in
##   docker.

CUR_SOURCE = ${wildcard *.c}
OPENSSL_LIB=/usr/local/openssl/lib64/libcrypto.a -ldl -lpthread
SDK_LIB_DIR = $(PENGLAI_SDK)/lib
GM_LIBS = $(SDK_LIB_DIR)/gm/sm2.a $(SDK_LIB_DIR)/gm/miracl.a
GM_LIB_INCLUDE_DIR = $(SDK_LIB_DIR)/gm/include
HOST_LIB = $(SDK_LIB_DIR)/libpenglai-enclave-host.a
HOST_LIB_INCLUDE_DIR = $(SDK_LIB_DIR)/host/include

CFLAGS += -O3 -I $(HOST_LIB_INCLUDE_DIR) -I $(GM_LIB_INCLUDE_DIR) -L/usr/local -lcrypto -ldl -lpthread

all: penglai_sign

penglai_sign: $(CUR_SOURCE) $(HOST_LIB) $(GM_LIBS) $(OPENSSL_LIB)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o penglai_sign
