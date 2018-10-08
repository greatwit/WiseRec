LOCAL_PATH := $(call my-dir)

LOCAL_PROJECT_ROOT := $(LOCAL_PATH)#$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)))




include $(CLEAR_VARS)

LOCAL_CFLAGS := -D__ANDROID__ 
LOCAL_CFLAGS += -std=c99

LOCAL_MODULE    := mcndk

LOCAL_C_INCLUDES += \

LOCAL_SRC_FILES := mediacodec_ndk.c

#LOCAL_STATIC_LIBRARIES := libMediaStream
#LOCAL_SHARED_LIBRARIES := libnativehelper

LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)

