

#include "CameraListen.h"

#include "ComDefine.h"
#define TAG "JNICameraListen"

using namespace android;



JNICameraListen::JNICameraListen(IVideoCallback *callback):mCamera(NULL)
{
	mCallback = callback;
}

sp<Camera> JNICameraListen::getCamera()
{
	return mCamera;
}

void JNICameraListen::notify(int32_t msgType, int32_t ext1, int32_t ext2)
{
    GLOGD("function %s,line:%d notify",__FUNCTION__,__LINE__);
}

void JNICameraListen::postData(int32_t msgType, const sp<IMemory>& dataPtr, camera_frame_metadata_t *metadata)
{
	/*
    // VM pointer will be NULL if object is released
    JNIEnv *env = AndroidRuntime::getJNIEnv();


    int32_t dataMsgType = msgType & ~CAMERA_MSG_PREVIEW_METADATA;

    // return data based on callback type
    switch (dataMsgType) {
        case CAMERA_MSG_VIDEO_FRAME:
            // should never happen
            break;

        // For backward-compatibility purpose, if there is no callback
        // buffer for raw image, the callback returns null.
        case CAMERA_MSG_RAW_IMAGE:
            break;

        // There is no data.
        case 0:
            break;

        default:
            ALOGV("dataCallback(%d, %p)", dataMsgType, dataPtr.get());
            break;
    }

    // post frame metadata to Java
    if (metadata && (msgType & CAMERA_MSG_PREVIEW_METADATA)) {
        //postMetadata(env, CAMERA_MSG_PREVIEW_METADATA, metadata);
    }
	*/
	if (dataPtr != NULL) {
		ssize_t offset;
		size_t size;
		sp<IMemoryHeap> heap = dataPtr->getMemory(&offset, &size);
		
		//int fd = heap->getHeapID();
		//void* base = heap->getBase();
		//ALOGE("postData: off=%d, size=%d hid:%d base:%d", (int)heap->getOffset(), heap->getSize(), fd, (int)base);
		
		//void* hBase = mmap(0, size, PROT_READ, MAP_SHARED, fd, offset);  
		//void* base = heap->getBase();
		//uint_t f = heap->getFlags();
		//int os = heap->getOffset();
		//int sz = heap->getSize();
		//ALOGE("postData: off=%ld, size=%d heapid:%d base:%d flag:%d  offset:%d heapsize:%s", offset, size, id, base, f, os, sz);
		
		
		unsigned int heapBase = (unsigned int)heap->base();
		if (heapBase != 0)
		{
			//const jbyte* data = reinterpret_cast<const jbyte*>(heapBase + offset);
			if(mCallback)
			{
				V4L2BUF_t buff;
				buff.addrVirY = heapBase;
				buff.length   = size;
				mCallback->VideoSource(&buff);
			}
			
			//CodecBaseLib::getInstance()->AddBuffer((char*)data, size);
		}
		
		//ALOGE("postData: off=%ld, size=%d", offset, size);
		mCamera->setPreviewCallbackFlags(CAMERA_FRAME_CALLBACK_FLAG_BARCODE_SCANNER);
	}
}

void JNICameraListen::postDataTimestamp(nsecs_t timestamp, int32_t msgType, const sp<IMemory>& dataPtr)
{
    // TODO: plumb up to Java. For now, just drop the timestamp
}





// disconnect from camera service
// It's okay to call this when the native camera context is already null.
// This handles the case where the user has called release() and the
// finalizer is invoked later.
int JNICameraListen::CameraRelease()
{
    // TODO: Change to ALOGV
	GLOGD("function %s,line:%d release camera",__FUNCTION__,__LINE__);
	
	// clear callbacks
	if (mCamera != NULL) {
		mCamera->setPreviewCallbackFlags(CAMERA_FRAME_CALLBACK_FLAG_NOOP);
		mCamera->disconnect();
		mCamera.clear();
		mCamera = NULL;
	}

	// remove context to prevent further Java access
	//context->decStrong(thiz);
	
	return 0;
}

jstring JNICameraListen::getParameters()
{
	String8 params8 = mCamera->getParameters();
	if (params8.isEmpty()) {
		GLOGE("function %s,line:%d getParameters failed (empty parameters)",__FUNCTION__,__LINE__);
		return NULL;
    }
	JNIEnv *env = AndroidRuntime::getJNIEnv();
    return env->NewStringUTF(params8.string());
}

void JNICameraListen::stopPreview()
{
	GLOGV("function %s,line:%d stopPreview",__FUNCTION__,__LINE__);
	if (mCamera != NULL)
		mCamera->stopPreview();
}

void JNICameraListen::setDisplayOrientation(jint value)
{
	GLOGV("function %s,line:%d setDisplayOrientation",__FUNCTION__,__LINE__);
	if(mCamera != NULL)
		if (mCamera->sendCommand(CAMERA_CMD_SET_DISPLAY_ORIENTATION, value, 0) != NO_ERROR) {
		GLOGE("function %s,line:%d set display orientation failed",__FUNCTION__,__LINE__);
    }
}



