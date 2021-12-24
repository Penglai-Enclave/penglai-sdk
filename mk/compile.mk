ifneq ($V,1)
q := @
cmd-echo := true
cmd-echo-silent := echo
else
q :=
cmd-echo := echo
cmd-echo-silent := true
endif

CC_encl := $(CROSS_COMPILE)gcc
CC_host := $(CROSS_COMPILE_HOST)gcc

OBJDUMP := $(CROSS_COMPILE)objdump
OBJCOPY := $(CROSS_COMPILE)objcopy
AR      := $(CROSS_COMPILE)ar

DEFAULT_CFLAGS := -Wall -MD -MP -march=$(RISCV_ARCH) -mabi=$(RISCV_ABI)
DEFAULT_LDFLAGS := -march=$(RISCV_ARCH) -mabi=$(RISCV_ABI)

# process_src(src_file, build_flags, indir, outdir, suffix, accumulate_variable)
define process_src
obj := $(4)/$$(patsubst %.$(5),%.o,$(1))
dep := $$(obj:.o=.d)
$(6) += $$(obj)

-include $$(dep)

$$(obj): $(call get_file_path,$(1),$(3))
	@echo '  COMPILE       $$(notdir $$@)'
	$(q)mkdir -p $$(dir $$@)
	$(q)$(CC_$(sm)) $(2) -c $$< -o $$@
endef

# process_header(relative_path_of_file_to_indir,indir,outdir,accumulate_variable)
define process_header
inc := $(3)/$(1)
$(4) += $$(inc)

$$(inc): $(2)/$1
	@echo '  INSTALL    $$(notdir $$@)'
	$(q)mkdir -p $$(dir $$@)
	$(q)cp $$< $$@
endef

define add_prebuild_target
.PHONY: prebuild_$(1)
$(2) += prebuild_$(1)

prebuild_$(1):
	make CROSS_COMPILE=$(CROSS_COMPILE) RISCV_ARCH=$(RISCV_ARCH) RISCV_ABI=$(RISCV_ABI) -C lib/$(1) -f prebuild.mk

endef

# install_build_script(script_file,path_to_install,accumulate_variable)
define install_build_script
outfile := $(2)/$$(notdir $1)
$(3) += $$(outfile)

$$(outfile): $1
	@echo '  INSTALL    $$(notdir $$@)'
	$(q)mkdir -p $$(dir $$@)
	$(q)cp $$< $$@
endef

# get path of file according to
# whether input path is an absolute one
# get_file_path(file_path, abs_path_prefix)
get_file_path = $(if $(patsubst /%,,$(1)),$(2)/$(1),$(1))

# process_c_src(src_file, build_flags, in_dir, out_dir, accumu_var)
process_c_src = $(call process_src,$(1),$(2),$(3),$(4),c,$(5))

# process_asm_src(src_file, build_flags, in_dir, out_dir, accumu_var)
process_asm_src = $(call process_src,$(1),$(2),$(3),$(4),S,$(5))
