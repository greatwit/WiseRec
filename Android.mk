LOCAL_PATH:= $(call my-dir)

#
# libmediaplayerservice
#

include $(CLEAR_VARS)

LOCAL_SRC_FILES:=               \
	MPEG4Writer.cpp				\
	CameraSource.cpp			\
    MediaRecorderClient.cpp     \
    StagefrightRecorder.cpp     \
	android_MediaRecorder.cpp   \

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

LOCAL_MODULE:= libwise_rec

include $(BUILD_SHARED_LIBRARY)

include $(call all-makefiles-under,$(LOCAL_PATH))
