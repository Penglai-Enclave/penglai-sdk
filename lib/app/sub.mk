LIBNAME := penglai-enclave-eapp

# use CFLAGS to define flags of
# c compiler for this library
#
# Please note that, include path
# shall not be included in CFLAGS,
# it shall be defined in INCDIRS
CFLAGS :=

ASM_SRCS := \
	src/eret.S

C_SRCS := \
	src/print.c

INCDIRS := include

# headers files under directories
# defined in EXPORT_INCDIRS
# would be included in the
# output of sdk
EXPORT_INCDIRS := include

# use build_enclave_lib.mk means
# this library would be link by
# enclave, instead of host library
include $(SDK_DIR)/mk/lib_enclave.mk
