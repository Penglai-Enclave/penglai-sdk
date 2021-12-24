LIBNAME := penglai-enclave-host

CFLAGS :=

C_SRCS := \
	src/param.c \
	src/elf.c \
	src/penglai-enclave.c

INCDIRS := include
EXPORT_INCDIRS := include

include $(SDK_DIR)/mk/lib_host.mk
