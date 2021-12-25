
CROSS_COMPILE_HOST ?= riscv64-unknown-linux-gnu-
RISCV_ARCH ?= rv64imac
RISCV_ABI ?= lp64

O ?= .

include $(PENGLAI_SDK)/mk/compile.mk

SDK_LIB_DIR := $(PENGLAI_SDK)/lib
SDK_INC_DIR := $(PENGLAI_SDK)/include

SDK_LIB := $(SDK_LIB_DIR)/libpenglai-enclave-host.a

CFLAGS += \
	-I$(SDK_INC_DIR)/penglai-enclave-host/include

all: $(O)/$(APP)

sm := host

objs :=

$(foreach f, $(APP_C_SRCS),\
	$(eval $(call process_c_src,$(strip $(f)),$($(f)_CFLAGS) $(DEFAULT_CFLAGS) $(CFLAGS),$(CURDIR),$(O),objs)))

$(foreach f, $(APP_A_SRCS),\
	$(eval $(call process_asm_src,$(strip $(f)),$($(f)_CFLAGS) $(DEFAULT_CFLAGS) $(CFLAGS),$(CURDIR),$(O),objs)))

$(O)/$(APP): $(objs) $(SDK_LIB)
	@echo '  LD    $(notdir $@)'
	$(q)mkdir -p $(dir $@)
	$(q)$(CC_host) -o $@ $(DEFAULT_LDFLAGS) -Wl,--start-group $^ $(LDFLAGS) -Wl,--end-group

clean:
	@echo '  CLEAN    .'
	$(q)rm -f $(O)/$(APP) $(objs) $(objs:.o=.d)
