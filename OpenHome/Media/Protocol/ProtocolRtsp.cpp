#include <OpenHome/Media/Protocol/ProtocolRtsp.h>
#include <OpenHome/Private/Debug.h>
#include <OpenHome/OsWrapper.h>

using namespace OpenHome;
using namespace OpenHome::Media;


// ProtocolRtsp

ProtocolRtsp::ProtocolRtsp(Environment& aEnv, const Brx& aGuid)
    : ProtocolNetwork(aEnv)
    , iEnv(aEnv)
    , iRtspClient(iEnv, iReaderBuf, iWriterBuf, aGuid)
{
}

void ProtocolRtsp::StartStream() {
    LOG(kMedia, "ProtocolRaop::StartStream\n");
    TUint streamId = iIdProvider->NextStreamId();
    iSupply->OutputStream(iUri.AbsoluteUri(), 0, false, true, *this, streamId);
}

ProtocolStreamResult ProtocolRtsp::Stream(const Brx& aUri)
{
    iUri.Replace(aUri);
    LOG(kMedia, "ProtocolRtsp::Stream ");
    LOG(kMedia, iUri.AbsoluteUri());
    LOG(kMedia, "\n");

    if (iUri.Scheme() != Brn("rtsp")) {
        if (iUri.Scheme() != Brn("mms")) {
            LOG(kMedia, "ProtocolRtsp::Stream Scheme not recognised\n");
            Close();
            return EProtocolErrorNotSupported;
        }
    }

    if (!Connect(iUri, kRtspPort))
    {
        LOG(kMedia, "ProtocolRtsp::Stream Connection failure\n");
        return EProtocolStreamErrorUnrecoverable;
    }

    iRtspClient.Initialise(iUri);

    try {
        // Options

        iRtspClient.WriteMethodOptions();
        iRtspClient.WriteHeaderUserAgent();
        iRtspClient.WriteHeaderSeq();
        iRtspClient.WriteFlush();

        if (iRtspClient.Read() != 200) {
            LOG(kMedia, "ProtocolRtsp::Stream OPTIONS failure\n");
            return EProtocolStreamErrorUnrecoverable;
        }

        // Describe

        iRtspClient.WriteMethodDescribe();
        iRtspClient.WriteHeaderAccept(Brn("application/sdp"));
        iRtspClient.WriteHeaderUserAgent();
        iRtspClient.WriteHeaderSupported(Brn("com.microsoft.wm.predstrm"));
        iRtspClient.WriteHeaderSeq();
        iRtspClient.WriteFlush();

        if (iRtspClient.Read() != 200) {       // DESCRIBE method also returns SDP info
            LOG(kMedia, "ProtocolRtsp::Stream DESCRIBE failure\n");
            return EProtocolStreamErrorUnrecoverable;
        }

        iRtspClient.ReadSdp(iSdpInfo);

        if (iSdpInfo.MediaAudioCount() == 0) {
            LOG(kMedia, "ProtocolRtsp::Stream SDP Audio count failure\n");
            return EProtocolStreamErrorUnrecoverable;
        }

        if (iSdpInfo.AudioPgmpu().Bytes() == 0) {
            LOG(kMedia, "ProtocolRtsp::Stream SDP Pgmpu bytes failure\n");
            return EProtocolStreamErrorUnrecoverable;
        }

        if (iSdpInfo.AudioControlUri().Bytes() == 0) {
            LOG(kMedia, "ProtocolRtsp::Stream SDP Audio control uri failure\n");
            return EProtocolStreamErrorUnrecoverable;
        }

        if (iSdpInfo.SessionControlUri().Bytes() == 0) {
            LOG(kMedia, "ProtocolRtsp::Stream SDP Session control uri failure\n");
            return EProtocolStreamErrorUnrecoverable;
        }

        // Establish Stream Uri

        Bws<RtspClient::kMaxUriBytes> stream;

        stream.Replace(iSdpInfo.SessionControlUri());
        //stream.Replace(stream.Split(0, stream.Bytes()-1));
        stream.Append(iSdpInfo.AudioControlUri());
        //stream.Replace(stream.Split(0, stream.Bytes()-1));

        // Setup

        iRtspClient.WriteMethodSetup(stream);
        iRtspClient.WriteHeaderTransport(Brn("RTP/AVP/TCP;unicast;interleaved=0-1"));
        iRtspClient.WriteHeaderUserAgent();
        iRtspClient.WriteHeaderSeq();
        iRtspClient.WriteFlush();

        if (iRtspClient.Read() != 200) {
            LOG(kMedia, "ProtocolRtsp::Stream SETUP failure\n");
            return EProtocolStreamErrorUnrecoverable;
        }

        if (iRtspClient.SessionId().Bytes() == 0) {
            LOG(kMedia, "ProtocolRtsp::Stream Session Id failure\n");
            return EProtocolStreamErrorUnrecoverable;
        }

        iKeepAlivePeriod = iRtspClient.Timeout() * 1000;
        iKeepAliveTime = Os::TimeInMs(iEnv.OsCtx()) + iKeepAlivePeriod;

        // Play

        iRtspClient.WriteMethodPlay(iSdpInfo.SessionControlUri());
        iRtspClient.WriteHeaderAccelerateStreaming(8000, 100000000);
        iRtspClient.WriteHeaderSession();
        iRtspClient.WriteHeaderUserAgent();
        iRtspClient.WriteHeaderSeq();
        iRtspClient.WriteFlush();

        if (iRtspClient.Read() != 200) {
            LOG(kMedia, "ProtocolRtsp::Stream PLAY failure\n");
            return EProtocolStreamErrorRecoverable;
        }
    }
    catch(HttpError&) {
        LOG(kMedia, "ProtocolRtsp::Stream HttpError (or RTSP error)\n");
        return EProtocolStreamErrorUnrecoverable;
    }
    catch(BufferOverflow&) {
        LOG(kMedia, "ProtocolRtsp::Stream BufferOverflow in RTSP data)\n");
        return EProtocolStreamErrorUnrecoverable;
    }
    catch(ReaderError&) {
        LOG(kMedia, "ProtocolRtsp::Stream ReaderError\n");
        return EProtocolStreamErrorUnrecoverable;
    }
    catch(WriterError&) {
        LOG(kMedia, "ProtocolRtsp::Stream WriterError\n");
        return EProtocolStreamErrorUnrecoverable;
    }

    StartStream();

    // Output pgmpu

    Log::PrintHex(iSdpInfo.AudioPgmpu());
    iSupply->OutputData(iSdpInfo.AudioPgmpu());

    // Output audio stream

    for (;;) {
        if(iKeepAliveTime < Os::TimeInMs(iEnv.OsCtx())) {
            try {      
                LOG(kMedia, "-ProtocolRtsp::Stream send alive\n");

                iRtspClient.WriteMethodGetParameter(iSdpInfo.SessionControlUri());
                iRtspClient.WriteHeaderSession();
                iRtspClient.WriteHeaderUserAgent();
                iRtspClient.WriteHeaderSeq();
                iRtspClient.WriteFlush();
            }
            catch(WriterError) {
                LOG(kMedia, "<ProtocolRtsp::Stream Writer error\n");
                return EProtocolStreamErrorUnrecoverable;
            }

            iKeepAliveTime = Os::TimeInMs(iEnv.OsCtx()) + iKeepAlivePeriod;
        }

        try {
            Brn data = iRtspClient.ReadRtsp(iSdpInfo);
            Log::PrintHex(data);
            iSupply->OutputData(data);
        }
        catch (ReaderError&) {
            LOG(kMedia, "<ProtocolRtsp::Stream Reader error\n");
            return EProtocolStreamErrorUnrecoverable;
        }
        catch (HttpError&) {
            LOG(kMedia, "<ProtocolRtsp::Stream Http/Rtsp error\n");
            return EProtocolStreamErrorUnrecoverable;
        }
    }
}

TUint ProtocolRtsp::TryStop(TUint /*aTrackId*/, TUint /*aStreamId*/)
{
    return 0;   // FIXME
}

