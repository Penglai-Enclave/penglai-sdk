APP = bigint
APP_C_SRCS = ./bigint.c
EXTRA_CLEAN = $(APP).dump
include $(PENGLAI_SDK)/app.mk
