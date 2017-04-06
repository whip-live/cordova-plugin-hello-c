LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := helloc

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../common

SRC_LIST := $(wildcard $(LOCAL_PATH)/*.c)
SRC_LIST += $(wildcard $(LOCAL_PATH)/../../common/*.c)

LOCAL_SRC_FILES := $(SRC_LIST:$(LOCAL_PATH)/%=%)

include $(BUILD_SHARED_LIBRARY)
