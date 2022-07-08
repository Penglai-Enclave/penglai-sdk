APP = sha512
APP_C_SRCS = ./sha512.c
EXTRA_CLEAN = $(APP).dump
include $(PENGLAI_SDK)/app.mk
