LOCAL_PATH := $(call my-dir)

LOCAL_PROJECT_ROOT := $(LOCAL_PATH)#$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)))


FFMPEG_PATH  = ../common/ffmpeg
THREAD_PATH  = ../common/gthread
NAREFL_PATH  = ../common/NalBareflow

#include $(CLEAR_VARS)
#LOCAL_MODULE := avformat
#LOCAL_SRC_FILES := $(LOCAL_PROJECT_ROOT)/$(FFMPEG_PATH)/android/lib/libavformat.so
#include $(PREBUILT_SHARED_LIBRARY)
#
#include $(CLEAR_VARS)
#LOCAL_MODULE := avcodec
#LOCAL_SRC_FILES := $(LOCAL_PROJECT_ROOT)/$(FFMPEG_PATH)/android/lib/libavcodec.so
#include $(PREBUILT_SHARED_LIBRARY)
#
#include $(CLEAR_VARS)
#LOCAL_MODULE := avutil
#LOCAL_SRC_FILES := $(LOCAL_PROJECT_ROOT)/$(FFMPEG_PATH)/android/lib/libavutil.so
#include $(PREBUILT_SHARED_LIBRARY)
#
#include $(CLEAR_VARS)
#LOCAL_MODULE := swresample
#LOCAL_SRC_FILES := $(LOCAL_PROJECT_ROOT)/$(FFMPEG_PATH)/android/lib/libswresample.so
#include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_CFLAGS := -D__ANDROID__ 
LOCAL_CPPFLAGS 	+= -std=c++11
#LOCAL_CFLAGS += -std=c99

LOCAL_MODULE    := mcndk

LOCAL_C_INCLUDES += \
					$(LOCAL_PROJECT_ROOT)/../common \
					$(LOCAL_PROJECT_ROOT)/$(FFMPEG_PATH) \
					$(LOCAL_PROJECT_ROOT)/$(FFMPEG_PATH)/include \
					$(LOCAL_PROJECT_ROOT)/$(THREAD_PATH) \
					$(LOCAL_PROJECT_ROOT)/$(NAREFL_PATH)

#LOCAL_SRC_FILES := mediacodec_ndk.c FileDeCodecJni.cpp $(FFMPEG_PATH)/FfmpegContext.cpp $(THREAD_PATH)/gthreadpool.cpp
LOCAL_SRC_FILES := mediacodec_ndk.c \
				mediaextrator_ndk.c \
				McNative.cpp \
				GH264Decodec.cpp \
				GH264Extractor.cpp \
				UpperNdkEncodec.cpp \
				GMediaExtractor.cpp \
				$(THREAD_PATH)/gmutex.cpp \
				$(THREAD_PATH)/gthread.cpp \
				$(THREAD_PATH)/gthreadpool.cpp \
				$(NAREFL_PATH)/NALDecoder.cpp

#LOCAL_SHARED_LIBRARIES := avformat avcodec avutil swresample


#LOCAL_STATIC_LIBRARIES := libMediaStream
#LOCAL_SHARED_LIBRARIES := libandroid_runtime

LOCAL_LDLIBS := -llog -landroid

include $(BUILD_SHARED_LIBRARY)


