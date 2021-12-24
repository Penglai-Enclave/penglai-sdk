outdir := $(OUT_DIR)/lib$(LIBNAME)
lib := $(outdir)/lib$(LIBNAME).a

CFLAGS += $(DEFAULT_CFLAGS)

#get_file_path = $(if $(patsubst /%,,$(1)),$(SDK_DIR)/$(lib_dir)/$(1),$(1))

$(foreach incdir,$(INCDIRS),\
	$(eval CFLAGS += -I$(call get_file_path,$(incdir),$(SDK_DIR)/$(lib_dir))))

ifeq ($(DEBUG),1)
CFLAGS += -g
endif

objs :=

$(foreach f, $(C_SRCS),\
	$(eval $(call process_c_src,$(strip $(f)),$($(f)_CFLAGS) $(CFLAGS),$(SDK_DIR)/$(lib_dir),$(outdir),objs)))

$(foreach f, $(ASM_SRCS),\
	$(eval $(call process_asm_src,$(strip $(f)),$($(f)_CFLAGS) $(CFLAGS),$(SDK_DIR)/$(lib_dir),$(outdir),objs)))

$(lib): $(objs)
	@echo '  AR     $(notdir $@)'
	$(q)$(AR) rcs $@ $^

TARGET_LIBS += $(lib)
CLEAN_OBJS += $(lib) $(objs) $(objs:.o=.d)

C_SRCS :=
ASM_SRCS :=
CFLAGS :=
INCDIRS :=

include $(LOCAL_PATH)/install_lib.mk
