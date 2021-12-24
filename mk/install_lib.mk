headers :=

$(foreach incdir,$(EXPORT_INCDIRS),\
	$(foreach f,$(addprefix $(incdir)/,$(shell find $(SDK_DIR)/$(lib_dir)/$(incdir) -type f -name '*.h' -printf '%P\n')),\
		$(eval $(call process_header,$(f),$(SDK_DIR)/$(lib_dir),$(INSTALL_DIR)/include/$(LIBNAME),headers))))

$(INSTALL_DIR)/lib/lib$(LIBNAME).a: $(lib)
	@echo '  INSTALL    $(notdir $@)'
	$(q)mkdir -p $(dir $@)
	$(q)cp $< $@

INSTALL_LIBS += $(INSTALL_DIR)/lib/lib$(LIBNAME).a
INSTALL_HEADERS += $(headers)

EXPORT_INCDIRS :=
