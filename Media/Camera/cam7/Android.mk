LOCAL_PATH:= $(call my-dir)

##################################################
###                  codec lib                 ###
##################################################

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/Camera7.cpp)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/../*.c*))

#JCrypto rely libbinder libmedia
LOCAL_SHARED_LIBRARIES := \
    libandroid_runtime \
    liblog \
    libutils \
    libcamera_client \
    libgui


LOCAL_C_INCLUDES += \
    frameworks/av/media/libmedia \
	$(LOCAL_PATH)/../../../common \
	$(LOCAL_PATH)/.. \
	$(CODEC_PATH) \
    $(call include-path-for, libhardware)/hardware



LOCAL_MODULE:= libCamera

include $(BUILD_SHARED_LIBRARY)

include $(call all-makefiles-under,$(LOCAL_PATH))