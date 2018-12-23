LOCAL_PATH:= $(call my-dir)
CODEC_PATH:= $(LOCAL_PATH)

##################################################
###                  codec lib                 ###
##################################################

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(CODEC_PATH)/*.c*)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/../*.c*))

#JCrypto rely libbinder libmedia
LOCAL_SHARED_LIBRARIES := \
    libandroid_runtime \
    libnativehelper \
    liblog \
    libutils \
    libcutils \
    libbinder \
    libmedia \
    libgui \
    libstagefright \
    libstagefright_foundation


LOCAL_C_INCLUDES += \
    frameworks/base/core/jni \
    frameworks/av/media/libmedia \
	frameworks/av/media/libstagefright \
	$(LOCAL_PATH)/../../../Common \
	$(LOCAL_PATH)/.. \
	$(CODEC_PATH) \
    $(call include-path-for, libhardware)/hardware



LOCAL_MODULE:= libCodecBase

include $(BUILD_SHARED_LIBRARY)

include $(call all-makefiles-under,$(LOCAL_PATH))