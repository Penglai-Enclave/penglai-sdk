APP = dhrystone
APP_C_SRCS = ./dhrystone.c
EXTRA_CLEAN = $(APP).dump
include $(PENGLAI_SDK)/app.mk
