#ifndef HEADER_PIPELINE_CONTENT_PLS
#define HEADER_PIPELINE_CONTENT_PLS

#include <OpenHome/Media/Protocol/Protocol.h>
#include <OpenHome/Buffer.h>
#include <OpenHome/OhNetTypes.h>
#include <OpenHome/Private/Stream.h>

namespace OpenHome {
namespace Media {

class ContentPls : public ContentProcessor
{
private: // from ContentProcessor
    TBool Recognise(const Brx& aUri, const Brx& aMimeType, const Brx& aData);
    ProtocolStreamResult Stream(Srx& aReader, TUint64 aTotalBytes, TUint64& aOffset);
    void Reset();
private:
    TBool iIsPlaylist;
};

} // namespace Media
} // namespace OpenHome

#endif  // HEADER_PIPELINE_CONTENT_PLS
