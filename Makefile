SDK_DIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

# Prefix for compiler to
# build enclave applications
CROSS_COMPILE ?= riscv64-unknown-elf-

# prefix for compiler to
# build host applications
CROSS_COMPILE_HOST ?= riscv64-unknown-linux-gnu-

# Now we assume that host applications
# and enclave applications share
# the same ARCH and ABI
RISCV_ARCH ?= rv64imac
RISCV_ABI ?= lp64

# All intermeidate files will
# be placed under OUT_DIR
OUT_DIR ?= $(SDK_DIR)/build

PENGLAI_SDK ?= $(OUT_DIR)/output

# All generated SDK files will
# be placed under INSTALL_DIR
INSTALL_DIR := $(PENGLAI_SDK)

# define CROSS_COMPILE toolchains and HOST toolchains
include mk/compile.mk

LIBS := app host
PREBUILD_LIBS := musl

# Define build scripts in SDK_BUILD_SCRIPTS that are
# required in the generated SDK
SDK_BUILD_SCRIPTS := mk/app.mk mk/host_app.mk mk/compile.mk mk/app.lds

.PHONY: all prebuild build install demo clean

all: build

# Updated when processing sub.mk
# defined under each library
TARGET_LIBS :=

# Updated by $(call add_prebuild_target,...)
PREBUILD_TARGETS :=

# Updated when processing sub.mk
# defined under each library,
# by including install_lib.mk
INSTALL_LIBS :=
INSTALL_HEADERS :=

# Updated by $(call install_buid_script,...)
INSTALL_BUILD_SCRIPTS :=

CLEAN_OBJS :=

# define prebuild recipe for each
# prebuild library
$(foreach lib,$(PREBUILD_LIBS),\
	$(eval $(call add_prebuild_target,$(lib),PREBUILD_TARGETS)))

# include all lib/sub.mk, include prebuild libraries
include $(addprefix lib/,$(addsuffix /sub.mk, $(LIBS) $(PREBUILD_LIBS)))

$(foreach f,$(SDK_BUILD_SCRIPTS),\
	$(eval $(call install_build_script,$(SDK_DIR)/$(f),$(INSTALL_DIR)/mk,INSTALL_BUILD_SCRIPTS)))

# Pre-building all libraries before
# generating SDK, shall be
# called at least once before calling
# `make` / `make install`
prebuild: $(PREBUILD_TARGETS)

build: $(TARGET_LIBS)

install: $(INSTALL_LIBS) $(INSTALL_HEADERS) $(INSTALL_BUILD_SCRIPTS)

demo: install
	$(q)make PENGLAI_SDK=$(PENGLAI_SDK) -C demo

clean:
	@echo '  CLEAN   .'
	$(q)rm -f $(CLEAN_OBJS)

# remove installed libs/headers
cleanclean: clean
	@echo '  CLEAN INSTALL    .'
	$(q)rm -f $(INSTALL_LIBS) $(INSTALL_HEADERS) $(INSTALL_BUILD_SCRIPTS)
