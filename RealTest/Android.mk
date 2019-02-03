LOCAL_PATH:= $(call my-dir)

Media_PATH	 = ../Media
CAMERA_PATH  = ../Media/Camera
CODEC_PATH   = ../Media/Codec
MCNDK_PATH	 = ../Mcndk
COMMON_PATH  = ../common
NALDECODER	 = ../common/NalBareflow

##################################################
###                 camera lib                 ###
##################################################

include $(CLEAR_VARS)

LOCAL_CPPFLAGS 	+= -std=c++11

LOCAL_SRC_FILES:= \
	$(Media_PATH)/CameraContext.cpp \
	$(MCNDK_PATH)/mediacodec_ndk.c \
	$(MCNDK_PATH)/mediaextrator_ndk.c \
	CameraNdkEncodec.cpp \
	CameraNative.cpp \
	CameraStub.cpp
	

#JCrypto rely libbinder libmedia
LOCAL_SHARED_LIBRARIES := \
    libandroid_runtime \
    libutils \
    liblog \
    libandroid \
    libdl	#libopen

LOCAL_LDLIBS := -llog -landroid

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/$(COMMON_PATH) \
    $(LOCAL_PATH)/$(Media_PATH) \
    $(LOCAL_PATH)/$(CAMERA_PATH) \
    $(LOCAL_PATH)/$(MCNDK_PATH) \
    $(call include-path-for, libhardware)/hardware

#for test
#$(warning $(LOCAL_PATH))

LOCAL_MODULE:= libNativeCamera

include $(BUILD_SHARED_LIBRARY)

##################################################
###                 codec lib                  ###
##################################################

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	CodecNative.cpp \
	CodecStub.cpp \
	$(COMMON_PATH)/gthreadpool.cpp \
	$(Media_PATH)/CodecContext.cpp \
	$(NALDECODER)/NALDecoder.cpp
	

#JCrypto rely libbinder libmedia
LOCAL_SHARED_LIBRARIES := \
    libandroid_runtime \
    libutils \
    liblog \
    libandroid \
    libdl	#libopen

LOCAL_LDLIBS := -llog -landroid

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/$(COMMON_PATH) \
    $(LOCAL_PATH)/$(Media_PATH) \
    $(LOCAL_PATH)/$(CAMERA_PATH) \
    $(LOCAL_PATH)/$(NALDECODER) \
    $(call include-path-for, libhardware)/hardware

#for test
#$(warning $(LOCAL_PATH))

LOCAL_MODULE:= libNativeCodec

include $(BUILD_SHARED_LIBRARY)



