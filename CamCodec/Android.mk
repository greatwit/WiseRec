LOCAL_PATH:= $(call my-dir)

#
# libmediaplayerservice
#

include $(CLEAR_VARS)

LOCAL_CPPFLAGS 	+= -std=c++11

LOCAL_SRC_FILES:=               \
	mediaextrator_ndk.c \
	CameraFrame.cpp		\
	CamCodecNative.cpp   \

LOCAL_SHARED_LIBRARIES :=       \
    libandroid_runtime 			\
    libnativehelper 			\
    libbinder                   \
    liblog                      \
    libgui                      \
    libmedia                    \
    libstagefright              \
    libstagefright_foundation   \
    libstagefright_omx          \
	libcutils 					\
    libutils                    \
	libcamera_client 			\
	libmediaplayerservice		\

LOCAL_STATIC_LIBRARIES :=       \
    libstagefright_nuplayer     \
    libstagefright_rtsp         \

LOCAL_C_INCLUDES :=                                                 \
    $(call include-path-for, graphics corecg)                       \
    $(TOP)/frameworks/av/media/libstagefright/include               \
    $(TOP)/frameworks/av/media/libstagefright/rtsp                  \
    $(TOP)/frameworks/av/media/libstagefright/wifi-display          \
    $(TOP)/frameworks/native/include/media/openmax                  \
    $(TOP)/external/tremolo/Tremolo                                 \

LOCAL_MODULE:= libCamCodec

include $(BUILD_SHARED_LIBRARY)

include $(call all-makefiles-under,$(LOCAL_PATH))
