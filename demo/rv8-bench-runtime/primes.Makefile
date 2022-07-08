APP = primes
APP_C_SRCS = src/primes.c
EXTRA_CLEAN = $(APP).dump
include $(PENGLAI_SDK)/app.mk
