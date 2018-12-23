LOCAL_PATH:= $(call my-dir)

#
# libmediaplayerservice
#

include $(CLEAR_VARS)

LOCAL_CPPFLAGS 	+= -std=c++11

LOCAL_SRC_FILES:=               \
	CameraContext.cpp \
	CodecContext.cpp		\
	FileEnCodec.cpp   \
	CameraEncodec.cpp \
	CodecMedia.cpp

LOCAL_SHARED_LIBRARIES := \
						libandroid_runtime \
						libnativehelper \
						libgui \
						liblog \
						libbinder \
						libutils \
						libcutils \
						libdl

LOCAL_STATIC_LIBRARIES :=       \
    libstagefright_nuplayer     \
    libstagefright_rtsp         \

LOCAL_C_INCLUDES += \
    frameworks/base/core/jni \
    frameworks/av/media/libmedia \
    frameworks/av/media/libstagefright \
    frameworks/native/include/media/openmax \
    $(LOCAL_PATH)/Camera \
	$(LOCAL_PATH)/../common \

LOCAL_C_INCLUDES += external/stlport/stlport bionic

LOCAL_MODULE:= libCameraSrc

include $(BUILD_SHARED_LIBRARY)

include $(call all-makefiles-under,$(LOCAL_PATH))
