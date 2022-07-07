APP = miniz
APP_C_SRCS = ./miniz.c
EXTRA_CLEAN = $(APP).dump
include $(PENGLAI_SDK)/app.mk
