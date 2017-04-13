#include <OpenHome/Types.h>
#include <OpenHome/Media/Pipeline/SpotifyReporter.h>
#include <OpenHome/Private/Ascii.h>
#include <OpenHome/Private/Converter.h>

#include <limits>

using namespace OpenHome;
using namespace OpenHome::Media;


// SpotifyDidlLiteWriter

SpotifyDidlLiteWriter::SpotifyDidlLiteWriter(const Brx& aUri, ISpotifyMetadata& aMetadata)
    : iUri(aUri)
    , iMetadata(aMetadata)
{
}

void SpotifyDidlLiteWriter::Write(IWriter& aWriter, TUint aBitDepth, TUint aChannels, TUint aSampleRate) const
{
    WriterAscii writer(aWriter);
    writer.Write(Brn("<DIDL-Lite xmlns:dc=\"http://purl.org/dc/elements/1.1/\" "));
    writer.Write(Brn("xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\" "));
    writer.Write(Brn("xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\">"));
    writer.Write(Brn("<item id=\"0\" parentID=\"0\" restricted=\"True\">"));

    writer.Write(Brn("<dc:title>"));
    Converter::ToXmlEscaped(writer, iMetadata.Track());
    writer.Write(Brn("</dc:title>"));

    writer.Write(Brn("<dc:creator>"));
    Converter::ToXmlEscaped(writer, iMetadata.Artist());
    writer.Write(Brn("</dc:creator>"));

    writer.Write(Brn("<upnp:artist role='AlbumArtist'>"));
    Converter::ToXmlEscaped(writer, iMetadata.Artist());
    writer.Write(Brn("</upnp:artist>"));

    writer.Write(Brn("<upnp:album>"));
    Converter::ToXmlEscaped(writer, iMetadata.Album());
    writer.Write(Brn("</upnp:album>"));

    writer.Write(Brn("<upnp:albumArtURI>"));
    Converter::ToXmlEscaped(writer, iMetadata.AlbumCoverUrl());
    writer.Write(Brn("</upnp:albumArtURI>"));

    WriteRes(writer, aBitDepth, aChannels, aSampleRate);

    writer.Write(Brn("<upnp:class>object.item.audioItem.musicTrack</upnp:class></item></DIDL-Lite>"));
}

void SpotifyDidlLiteWriter::SetDurationString(Bwx& aBuf) const
{
    // H+:MM:SS[.F0/F1]
    const TUint msPerSecond = 1000;
    const TUint msPerMinute = msPerSecond*60;
    const TUint msPerHour = msPerMinute*60;

    TUint timeRemaining = iMetadata.DurationMs();
    const TUint hours = iMetadata.DurationMs()/msPerHour;
    timeRemaining -= hours*msPerHour;

    const TUint minutes = timeRemaining/msPerMinute;
    timeRemaining -= minutes*msPerMinute;

    const TUint seconds = timeRemaining/msPerSecond;
    timeRemaining -= seconds*msPerSecond;

    const TUint milliseconds = timeRemaining;

    ASSERT(hours <= 99);
    if (hours < 10) {
        aBuf.Append('0');
    }
    Ascii::AppendDec(aBuf, hours);
    aBuf.Append(':');

    ASSERT(minutes <= 59);
    if (minutes < 10) {
        aBuf.Append('0');
    }
    Ascii::AppendDec(aBuf, minutes);
    aBuf.Append(':');

    ASSERT(seconds <= 60);
    if (seconds < 10) {
        aBuf.Append('0');
    }
    Ascii::AppendDec(aBuf, seconds);

    if (milliseconds > 0) {
        aBuf.Append('.');
        Ascii::AppendDec(aBuf, milliseconds);
        aBuf.Append('/');
        Ascii::AppendDec(aBuf, msPerSecond);
    }
}

void SpotifyDidlLiteWriter::WriteRes(IWriter& aWriter, TUint aBitDepth, TUint aChannels, TUint aSampleRate) const
{
    WriterAscii writer(aWriter);

    Bws<kMaxDurationBytes> duration;
    SetDurationString(duration);
    writer.Write(Brn("<res"));
    writer.Write(Brn(" duration=\""));
    writer.Write(duration);
    writer.Write(Brn("\""));

    writer.Write(Brn(" protocolInfo=\""));
    writer.Write(Brn("spotify:*:audio/L16:*"));
    writer.Write(Brn("\""));

    WriteOptionalAttributes(writer, aBitDepth, aChannels, aSampleRate);

    writer.Write(Brn(">"));
    writer.Write(iUri);
    writer.Write(Brn("</res>"));
}

void SpotifyDidlLiteWriter::WriteOptionalAttributes(IWriter& aWriter, TUint aBitDepth, TUint aChannels, TUint aSampleRate) const
{
    WriterAscii writer(aWriter);

    if (aBitDepth != 0) {
        writer.Write(Brn(" bitsPerSample=\""));
        writer.WriteUint(aBitDepth);
        writer.Write(Brn("\""));
    }

    if (aSampleRate != 0) {
        writer.Write(Brn(" sampleFrequency=\""));
        writer.WriteUint(aSampleRate);
        writer.Write(Brn("\""));
    }

    if (aChannels != 0) {
        writer.Write(Brn(" nrAudioChannels=\""));
        writer.WriteUint(aChannels);
        writer.Write(Brn("\""));
    }

    if (aBitDepth != 0 && aChannels != 0 && aSampleRate != 0) {
        const TUint byteDepth = aBitDepth/8;
        const TUint bytesPerSec = byteDepth*aSampleRate*aChannels;
        const TUint bytesPerMs = bytesPerSec / 1000;
        const TUint totalBytes = iMetadata.DurationMs() * bytesPerMs;
        writer.Write(Brn(" size=\""));
        writer.WriteUint(totalBytes);
        writer.Write(Brn("\""));
    }
}


// StartOffset

StartOffset::StartOffset()
    : iOffsetMs(0)
{
}

void StartOffset::SetMs(TUint aOffsetMs)
{
    iOffsetMs = aOffsetMs;
}

TUint64 StartOffset::OffsetSample(TUint aSampleRate) const
{
    return (static_cast<TUint64>(iOffsetMs)*aSampleRate)/1000;
}


// SpotifyReporter

const TUint SpotifyReporter::kSupportedMsgTypes =   eMode
                                                  | eTrack
                                                  | eDrain
                                                  | eDelay
                                                  | eMetatext
                                                  | eStreamInterrupted
                                                  | eHalt
                                                  | eWait
                                                  | eDecodedStream
                                                  | eBitRate
                                                  | eAudioPcm
                                                  | eSilence
                                                  | eQuit;

const Brn SpotifyReporter::kInterceptMode("Spotify");

SpotifyReporter::SpotifyReporter(IPipelineElementUpstream& aUpstreamElement, MsgFactory& aMsgFactory, TrackFactory& aTrackFactory)
    : PipelineElement(kSupportedMsgTypes)
    , iUpstreamElement(aUpstreamElement)
    , iMsgFactory(aMsgFactory)
    , iTrackFactory(aTrackFactory)
    , iTrackDurationMs(0)
    , iMetadata(nullptr)
    , iMsgTrackPending(false)
    , iMsgDecodedStreamPending(false)
    , iDecodedStream(nullptr)
    , iSubSamples(0)
    , iInterceptMode(false)
    , iPipelineTrackSeen(false)
    , iLock("SARL")
{
}

SpotifyReporter::~SpotifyReporter()
{
    AutoMutex _(iLock);
    if (iMetadata != nullptr) {
        iMetadata->Destroy();
    }
    if (iDecodedStream != nullptr) {
        iDecodedStream->RemoveRef();
    }
}

Msg* SpotifyReporter::Pull()
{
    Msg* msg = nullptr;
    while (msg == nullptr) {
        if (!iInterceptMode) {
            msg = iUpstreamElement.Pull();
            msg = msg->Process(*this);

            if (iInterceptMode) {
                // Mode changed. Need to set up some variables that are
                // accessed from different threads, so need to acquire iLock.
                AutoMutex _(iLock);
                iMsgTrackPending = true;
                iMsgDecodedStreamPending = true;
                iSubSamples = 0;
            }
        }
        else {
            // iLock needs to be held for a subset of the checks below, and in
            // certain msg->Process() calls.
            // However, doing it that way results in acquiring the mutex twice
            // when processing MsgAudioPcm during a Spotify stream. Instead,
            // acquire iLock here and keep it held while calling msg->Process().
            AutoMutex _(iLock);

            // Don't output any generated MsgTrack or modified MsgDecodedStream
            // unless in Spotify mode, and seen a MsgTrack and MsgDecodedStream
            // arrive via pipeline.
            if (iPipelineTrackSeen && iDecodedStream != nullptr) {
                // Only want to output generated MsgTrack if it's pending and if stream format is known to get certain elements for track metadata.
                if (iMsgTrackPending) {
                    ASSERT(iMetadata != nullptr);
                    const DecodedStreamInfo& info = iDecodedStream->StreamInfo();
                    const TUint bitDepth = info.BitDepth();
                    const TUint channels = info.NumChannels();
                    const TUint sampleRate = info.SampleRate();

                    BwsTrackMetaData metadata;
                    WriterBuffer writerBuffer(metadata);
                    SpotifyDidlLiteWriter metadataWriter(iTrackUri, *iMetadata);
                    metadataWriter.Write(writerBuffer, bitDepth, channels, sampleRate);
                    iMetadata->Destroy();
                    iMetadata = nullptr;
                    Track* track = iTrackFactory.CreateTrack(iTrackUri, metadata);

                    const TBool startOfStream = false;  // Report false as don't want downstream elements to re-enter any stream detection mode.
                    MsgTrack* msg = iMsgFactory.CreateMsgTrack(*track, startOfStream);
                    track->RemoveRef();
                    iMsgTrackPending = false;
                    return msg;
                }
                if (!iMsgTrackPending && iMsgDecodedStreamPending) {
                    /*
                     * If iDecodedStream == nullptr, means still need to pull first
                     * MsgDecodedStream of Spotify stream from upstream element.
                     * Return control to keep pulling from upstream until first
                     * MsgDecodedStream is eventually pulled.
                     */
                    if (iDecodedStream != nullptr) {
                        iMsgDecodedStreamPending = false;
                        MsgDecodedStream* msg = CreateMsgDecodedStreamLocked();
                        UpdateDecodedStream(*msg);
                        return iDecodedStream;
                    }
                }
            }

            msg = iUpstreamElement.Pull();
            msg = msg->Process(*this);
        }
    }
    return msg;
}

TUint64 SpotifyReporter::SubSamples() const
{
    AutoMutex _(iLock);
    return iSubSamples;
}

void SpotifyReporter::TrackChanged(const Brx& aUri, ISpotifyMetadata* aMetadata, TUint aStartMs)
{
    AutoMutex _(iLock);
    iMsgTrackPending = true;
    if (iMetadata != nullptr) {
        iMetadata->Destroy();
        iMetadata = nullptr;
    }
    iTrackUri.Replace(aUri);
    iMetadata = aMetadata;
    iStartOffset.SetMs(aStartMs);
    iTrackDurationMs = iMetadata->DurationMs();
    iMsgDecodedStreamPending = true;
}

void SpotifyReporter::NotifySeek(TUint aOffsetMs)
{
    AutoMutex _(iLock);
    iStartOffset.SetMs(aOffsetMs);
}

Msg* SpotifyReporter::ProcessMsg(MsgMode* aMsg)
{
    if (aMsg->Mode() == kInterceptMode) {
        iInterceptMode = true;

        ClearDecodedStream();
        iPipelineTrackSeen = false;
    }
    else {
        iInterceptMode = false;
    }

    return aMsg;
}

Msg* SpotifyReporter::ProcessMsg(MsgTrack* aMsg)
{
    if (!iInterceptMode) {
        return aMsg;
    }
    iPipelineTrackSeen = true;  // Only matters when in iInterceptMode. Ensures in-band MsgTrack is output before any are generated from out-of-band notifications.
    return aMsg;
}

Msg* SpotifyReporter::ProcessMsg(MsgDecodedStream* aMsg)
{
    if (!iInterceptMode) {
        return aMsg;
    }
    const DecodedStreamInfo& info = aMsg->StreamInfo();
    ASSERT(info.SampleRate() != 0);
    ASSERT(info.NumChannels() != 0);

    // Clear any previous cached MsgDecodedStream and cache the one received.
    UpdateDecodedStream(*aMsg);

    aMsg->RemoveRef();  // UpdateDecodedStream() adds its own reference.
    iMsgDecodedStreamPending = true;    // Set flag so that a MsgDecodedStream with updated attributes is output in place of this.
    return nullptr;
}

Msg* SpotifyReporter::ProcessMsg(MsgAudioPcm* aMsg)
{
    if (!iInterceptMode) {
        return aMsg;
    }

    ASSERT(iDecodedStream != nullptr);  // Can't receive audio until MsgDecodedStream seen.
    const DecodedStreamInfo& info = iDecodedStream->StreamInfo();
    TUint samples = aMsg->Jiffies()/Jiffies::PerSample(info.SampleRate());

    // iLock held in ::Pull() method to protect iSubSamples.
    TUint64 subSamplesPrev = iSubSamples;
    iSubSamples += samples*info.NumChannels();
    ASSERT(iSubSamples >= subSamplesPrev); // Overflow not handled.
    return aMsg;
}

void SpotifyReporter::ClearDecodedStream()
{
    if (iDecodedStream != nullptr) {
        iDecodedStream->RemoveRef();
        iDecodedStream = nullptr;
    }
}

void SpotifyReporter::UpdateDecodedStream(MsgDecodedStream& aMsg)
{
    ClearDecodedStream();
    iDecodedStream = &aMsg;
    iDecodedStream->AddRef();
}

TUint64 SpotifyReporter::TrackLengthJiffiesLocked() const
{
    ASSERT(iDecodedStream != nullptr);
    const DecodedStreamInfo& info = iDecodedStream->StreamInfo();
    const TUint64 trackLengthJiffies = (static_cast<TUint64>(iTrackDurationMs)*info.SampleRate()*Jiffies::PerSample(info.SampleRate()))/1000;
    return trackLengthJiffies;
}

MsgDecodedStream* SpotifyReporter::CreateMsgDecodedStreamLocked() const
{
    ASSERT(iDecodedStream != nullptr);
    const DecodedStreamInfo& info = iDecodedStream->StreamInfo();
    // Due to out-of-band track notification from Spotify, audio for current track was probably pushed into pipeline before track offset/duration was known, so use updated values here.
    const TUint64 trackLengthJiffies = TrackLengthJiffiesLocked();
    MsgDecodedStream* msg =
        iMsgFactory.CreateMsgDecodedStream(info.StreamId(), info.BitRate(), info.BitDepth(),
                                           info.SampleRate(), info.NumChannels(), info.CodecName(),
                                           trackLengthJiffies, iStartOffset.OffsetSample(info.SampleRate()),
                                           info.Lossless(), info.Seekable(), info.Live(), info.AnalogBypass(),
                                           info.Multiroom(), info.Profile(), info.StreamHandler());
    return msg;
}
