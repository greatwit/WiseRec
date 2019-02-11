LOCAL_PATH:= $(call my-dir)

API_PATH 	= ..
COMMON_PATH	= ../../../common


##################################################
###                  camera so lib             ###
##################################################

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/Camera7.cpp)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/$(API_PATH)/*.c*))

#JCrypto rely libbinder libmedia
LOCAL_SHARED_LIBRARIES := \
    libandroid_runtime \
    liblog \
    libutils \
    libcamera_client \
    libgui


LOCAL_C_INCLUDES += \
    frameworks/av/media/libmedia \
	$(LOCAL_PATH)/$(COMMON_PATH) \
	$(LOCAL_PATH)/$(API_PATH) \
    $(call include-path-for, libhardware)/hardware

LOCAL_MODULE := libCamera

include $(BUILD_SHARED_LIBRARY)

#include $(call all-makefiles-under,$(LOCAL_PATH))

##################################################
###                  camera test               ###
##################################################

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/Camera7.cpp)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/CameraTest.cpp)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/$(API_PATH)/CameraListen.cpp))

#JCrypto rely libbinder libmedia
LOCAL_SHARED_LIBRARIES := \
    libandroid_runtime \
    liblog \
    libutils \
    libcamera_client \
    libgui


LOCAL_C_INCLUDES += \
    frameworks/av/media/libmedia \
	$(LOCAL_PATH)/$(COMMON_PATH) \
	$(LOCAL_PATH)/$(API_PATH) \
    $(call include-path-for, libhardware)/hardware

LOCAL_LDLIBS := -landroid

LOCAL_MODULE := libCameraTest

include $(BUILD_SHARED_LIBRARY)

