APP = norx
APP_C_SRCS = ./norx.c
EXTRA_CLEAN = $(APP).dump
include $(PENGLAI_SDK)/app.mk
