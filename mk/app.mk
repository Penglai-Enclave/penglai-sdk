
CROSS_COMPILE ?= riscv64-unknown-elf-
RISCV_ARCH ?= rv64imac
RISCV_ABI ?= lp64

O ?= .

include $(PENGLAI_SDK)/mk/compile.mk

SDK_LIB_DIR := $(PENGLAI_SDK)/lib
SDK_INC_DIR := $(PENGLAI_SDK)/include

SDK_LIB := $(addprefix $(SDK_LIB_DIR)/, libpenglai-enclave-eapp.a libc.a)

GCC_LIB := $(shell $(CC_encl) -march=$(RISCV_ARCH) -mabi=$(RISCV_ABI) --print-libgcc-file-name)

CFLAGS += \
	-nostdinc \
	-I$(SDK_INC_DIR)/penglai-enclave-eapp/include \
	-I$(SDK_INC_DIR)/c/include \
	-I$(SDK_INC_DIR)/c/obj/include \
	-I$(SDK_INC_DIR)/c/arch/riscv64

APP_LDS ?= $(PENGLAI_SDK)/mk/app.lds

all: $(O)/$(APP)

sm := encl

objs :=

$(foreach f, $(APP_C_SRCS),\
	$(eval $(call process_c_src,$(strip $(f)),$($(f)_CFLAGS) $(DEFAULT_CFLAGS) $(CFLAGS),$(CURDIR),$(O),objs)))

$(foreach f, $(APP_A_SRCS),\
	$(eval $(call process_asm_src,$(strip $(f)),$($(f)_CFLAGS) $(DEFAULT_CFLAGS) $(CFLAGS),$(CURDIR),$(O),objs)))

$(O)/$(APP): $(objs) $(SDK_LIB)
	@echo '  LD    $(notdir $@)'
	$(q)mkdir -p $(dir $@)
	$(q)$(CC_encl) -T $(APP_LDS) -static -nostdlib $(DEFAULT_LDFLAGS) -o $@ -Wl,--start-group $^ $(LDFLAGS) $(GCC_LIB) -Wl,--end-group

clean:
	@echo '  CLEAN    .'
	$(q)rm -f $(O)/$(APP) $(objs) $(objs:.o=.d)
