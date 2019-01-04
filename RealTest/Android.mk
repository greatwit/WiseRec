LOCAL_PATH:= $(call my-dir)

CAMERA_PATH  = ../Media
CODEC_PATH   = ../Media

##################################################
###                 camera lib                 ###
##################################################

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/CameraNative.cpp)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/CameraDL.cpp)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/$(CAMERA_PATH)/CameraContext.cpp))
	

#JCrypto rely libbinder libmedia
LOCAL_SHARED_LIBRARIES := \
    libandroid_runtime \
    libutils \
    libdl	#libopen

LOCAL_LDLIBS := -llog

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/../common \
    $(LOCAL_PATH)/../Media \
    $(LOCAL_PATH)/../Media/Camera \
    $(call include-path-for, libhardware)/hardware

#for test
#$(warning $(LOCAL_PATH))

LOCAL_MODULE:= libNativeCamera

include $(BUILD_SHARED_LIBRARY)