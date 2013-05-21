#include <OpenHome/Media/Protocol/ProtocolFactory.h>
#include <OpenHome/Media/Protocol/Protocol.h>
#include <OpenHome/Buffer.h>
#include <OpenHome/Private/Stream.h>
#include <OpenHome/Private/Ascii.h>
#include <OpenHome/Private/Parser.h>
#include <OpenHome/Private/Converter.h>
#include <OpenHome/Private/Debug.h>

/* Example OPML file

<?xml version="1.0" encoding="UTF-8"?>
<opml version="1">
        <head>
        <title>Listening Options</title>
        <status>200</status>
        </head>
        <body>
            <outline type="audio" text="stuff" URL="url" bitrate="num" reliability="num_percentage" guide_id="id" station_id="num" title="str" now_playing_id="id" media_type="type"/>
            <outline type="audio" text="stuff" URL="url" bitrate="num" reliability="num_percentage" guide_id="id" station_id="num" title="str" now_playing_id="id" media_type="type"/>
            ...
        </body>
</opml>
*/

namespace OpenHome {
namespace Media {

class ContentOpml : public ContentProcessor
{
private: // from ContentProcessor
    TBool Recognise(const Brx& aUri, const Brx& aMimeType, const Brx& aData);
    ProtocolStreamResult Stream(IProtocolReader& aReader, TUint64 aTotalBytes);
private:
    Brn EntityReadTag(IReader& aReader);
private:
    Bws<1024> iUri;
};

} // namespace Media
} // namespace OpenHome

using namespace OpenHome;
using namespace OpenHome::Media;


ContentProcessor* ContentProcessorFactory::NewOpml()
{ // static
    return new ContentOpml();
}


// ContentOpml

TBool ContentOpml::Recognise(const Brx& /*aUri*/, const Brx& /*aMimeType*/, const Brx& aData)
{
    /* Ignore
            Ascii::CaseInsensitiveEquals(aMimeType, Brn("text/xml")
       test.  A content match is a far better indicator of success than knowing we're dealing with some sort of xml doc. */
    if (Ascii::Contains(aData, Brn("<opml version"))) {
        return true;
    }
    return false;
}

ProtocolStreamResult ContentOpml::Stream(IProtocolReader& aReader, TUint64 aTotalBytes)
{
    LOG(kMedia, "ContentOpml::Stream\n");

    TUint64 bytesRemaining = aTotalBytes;
    
    try {
        for (;;) {
            Brn line(ReadLine(aReader, bytesRemaining));
            ReaderBuffer rb(line);
            Parser parser(EntityReadTag(rb));
            Brn name = parser.Next();
            if (!Ascii::CaseInsensitiveEquals(name, Brn("outline"))) {
                continue;
            }
            Brn att = parser.Next('=');            
            if (!Ascii::CaseInsensitiveEquals(att, Brn("type"))) {
                continue;
            }
            parser.Next('"');
            Brn type = parser.Next('"');
            att.Set(parser.Next('='));            
            if (!Ascii::CaseInsensitiveEquals(att, Brn("text"))) {
                continue;
            }
            parser.Next('"');
            Brn text = parser.Next('"'); // metadata - station name
            att.Set(parser.Next('='));            
            if (!Ascii::CaseInsensitiveEquals(att, Brn("url"))) {
                continue;
            }
            parser.Next('"');
            Brn uri = parser.Next('"');
            TUint bytes = uri.Bytes();
            if (bytes == 0) {
                continue;
            }

            // could maybe skip the copy into another buffer if we know that this function won't be called again for the same underlying buffer
            iUri.Replace(uri);
            Converter::FromXmlEscaped(iUri);
            ProtocolStreamResult res = iProtocolSet->Stream(iUri);
            if (res == EProtocolStreamStopped || res == EProtocolStreamSuccess) {
                return res;
            }

        }
    }
    catch (ReaderError&) {
    }
    if (bytesRemaining > 0) {
        return EProtocolStreamErrorRecoverable;
    }
    return EProtocolStreamErrorUnrecoverable;
}

Brn ContentOpml::EntityReadTag(IReader& aReader)
{
    aReader.ReadUntil('<');
    return aReader.ReadUntil('>');
}
