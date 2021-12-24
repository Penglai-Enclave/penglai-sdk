LOCAL_PATH := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
# rechieve lib directory by looking up last 2 items from makefile_list
lib_dir := $(strip $(patsubst %/,%,$(dir $(lastword $(filter-out $(lastword $(MAKEFILE_LIST)), $(MAKEFILE_LIST))))))

lib_outdir := $(OUT_DIR)/lib$(LIBNAME)
lib := $(lib_outdir)/lib$(LIBNAME).a

$(lib): $(call get_file_path,$(PREBUILT_LIB),$(SDK_DIR)/$(lib_dir))
	@echo '  COPY     $(notdir $@)'
	$(q)mkdir -p $(dir $@)
	$(q)cp $< $@

include $(LOCAL_PATH)/install_lib.mk
