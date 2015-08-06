LOCAL_PATH:= $(call my-dir)

#include $(CLEAR_VARS)
#LOCAL_SRC_FILES:= ping.c ping_common.c
#LOCAL_MODULE := ping
#LOCAL_CFLAGS := -DWITHOUT_IFADDRS -Wno-sign-compare
#include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_CFLAGS := -Wno-sign-compare
LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog
LOCAL_SRC_FILES := tracepath.c miniTelnet.c Provider.c 
LOCAL_MODULE := tracepath
LOCAL_MODULE_TAGS := debug
#include $(BUILD_EXECUTABLE)
include $(BUILD_SHARED_LIBRARY)


