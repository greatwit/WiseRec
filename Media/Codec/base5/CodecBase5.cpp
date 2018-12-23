
#include "CodecBase.h"

#include "media_MediaCodec.h"

#include "Configure.h"

#include "ComDefine.h"
#define TAG "CodecBase5"

namespace android 
{

status_t CodecBase::CreateCodec(const sp<AMessage> &format, const sp<Surface> &surface, const sp<ICrypto> &crypto, int flags)
{
	status_t err;

	GLOGD("function %s,line:%d configure...1", __FUNCTION__, __LINE__);
	
	sp<IGraphicBufferProducer> bufferProducer;
	if(surface!=NULL)
		bufferProducer = surface->getIGraphicBufferProducer();
	err = mCodec->configure(format, bufferProducer, crypto, flags);
	GLOGD("function %s,line:%d configure...2 err:%d", __FUNCTION__, __LINE__, err);

	return err;
}

}  // namespace android

////////////////////////////////////////////////////////////////////////////////
