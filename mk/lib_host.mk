LOCAL_PATH := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
# rechieve lib directory by looking up last 2 items from makefile_list
lib_dir := $(strip $(patsubst %/,%,$(dir $(lastword $(filter-out $(lastword $(MAKEFILE_LIST)), $(MAKEFILE_LIST))))))

sm := host
CFLAGS += -Werror -shared -fPIC

include $(LOCAL_PATH)/lib_static.mk
