LOCAL_PATH := $(call my-dir)

#use the below statement if you intend to use luajit
#LUA_LIB :=luajit
LUA_LIB :=lua
$(call import-add-path,$(LOCAL_PATH)/../externalLib)
include $(CLEAR_VARS)

LOCAL_SRC_FILES  := java_type.cpp luadroid.cpp log_wrapper.cpp script_context.cpp utf8.cpp lfs.c dex/fake_dlfcn.c  dex/DexResolver.cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_STATIC_LIBRARIES :=$(LUA_LIB)
ifneq ($(APP_OPTIM),debug)
LOCAL_LDFLAGS += -Wl,--gc-sections
LOCAL_LDFLAGS += -Wl,--version-script=$(LOCAL_PATH)/sym.map
endif
LOCAL_LDLIBS := -ldl -llog -latomic
LOCAL_MODULE :=luadroid
$(info local c includes=$(LOCAL_C_INCLUDES))
$(info local path=$(LOCAL_PATH))
include $(BUILD_SHARED_LIBRARY)

$(call import-module, $(LUA_LIB))