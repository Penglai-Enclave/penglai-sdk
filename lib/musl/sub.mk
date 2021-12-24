LIBNAME := c

PREBUILT_LIB := lib/libc.a

EXPORT_INCDIRS := \
	include \
	obj/include \
	arch/riscv64

include $(SDK_DIR)/mk/lib_prebuilt.mk
