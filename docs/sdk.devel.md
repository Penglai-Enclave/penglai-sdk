## Developement of SDK

This document covers how to improve the current SDK.
Developers could refer to this document when they
need to integrate new libraries/scripts into the SDK.

### Integrate a library in SDK

Developers could follow the steps below if
they want to add a library that
could be used by the SDK users.

1. Create a folder under ./lib/, and move
the correponding source code and headers
in this folder.

2. Create a file `sub.mk` in this folder, and define
following variables

```
# LIBNAME: name of the library
# the filename of the output
# would be lib<LIBNAME>.a
LIBNAME :=

# CFLAGS: compile flags for all the
# sources of this library
CFLAGS :=

# If needs to specify compile flags only for
# certain sources, defined variable <file_name>_CFLAGS,
# .i.e, main.c_CFLAGS
<file_name>_CFLAGS :=

# C_SRCS  .c sources of this library
C_SRCS :=

# ASM_SRCS  .S sources of this library
ASM_SRCS :=

# include path for library building
INCDIRS :=

# include path for the users of this library,
# the directories specified in EXPORT_INCDIRS
# would be installed in PENGLAI_SDK
EXPORT_INCDIRS :=

```

The developer should include $(SDK_DIR)/mk/lib_enclave.mk
or $(SDK_DIR)/mk/lib_host.mk for enclave lib and host lib
respectively, after defining variables above.
SDK_DIR is the variable that points to root path of SDK.

### How to add a prebuilt library?

The prebuilt library is the kind of library whose building
is indepdendent from that of SDK.
We need to add a library as a *prebuilt library*, usually
because building of it is relatively complicated.

In the current version of SDK, lib musl is integrated
as a prebuilt library.

To add a library as prebuilt library, the developer
should create a build script `prebuild.mk` under the
lib folder.
In the `prebuild.mk`, the build command of this library
should be defined as the first valid building rule.

Another `sub.mk` should also be added under the library
folder and following variables shall be defined,
```
# LIBNAME: name of the library
# the filename of the output
# would be lib<LIBNAME>.a
LIBNAME :=

# filename of the object
# that is generated during the prebuild
PREBUILT_LIB :=

# include path for the users of this library,
# the directories specified in EXPORT_INCDIRS
# would be installed in PENGLAI_SDK
EXPORT_INCDIRS :=

```

The developer should also include
$(SDK_DIR)/mk/lib_prebuilt.mk

Check function `add_prebuild_target` in mk/compile.mk
for detail.

### Installation of libraries and build scripts

All library files would be copied to $(PENGLAI_SDK)/lib
with the name lib$(LIBNAME).a.

All directories defined in EXPORT_INCDIRS
would be copied to $(PENGLAI_SDK)/include/$(LIBNAME)

Scripts defined in SDK_BUILD_SCRIPTS would
be copied to $(PENGLAI_SDK)/mk.

Check function `install_build_script` in mk/compile.mk
for detail.
