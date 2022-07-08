APP = aes
APP_C_SRCS = src/aes.c
EXTRA_CLEAN = $(APP).dump
include $(PENGLAI_SDK)/app.mk
