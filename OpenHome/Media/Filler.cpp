#include <OpenHome/Media/Filler.h>
#include <OpenHome/OhNetTypes.h>
#include <OpenHome/Buffer.h>
#include <OpenHome/Private/Thread.h>
#include <OpenHome/Private/Printer.h>
#include <OpenHome/Media/Msg.h>
#include <OpenHome/Media/IdManager.h>
#include <OpenHome/Media/Protocol/Protocol.h>

using namespace OpenHome;
using namespace OpenHome::Media;

// UriProvider

const Brx& UriProvider::Mode() const
{
    return iMode;
}

UriProvider::UriProvider(const TChar* aMode)
    : iMode(aMode)
{
}

UriProvider::~UriProvider()
{
}


// Filler

Filler::Filler(ISupply& aSupply, IPipelineIdTracker& aIdTracker)
    : Thread("Filler")
    , iLock("FILL")
    , iSupply(aSupply)
    , iPipelineIdTracker(aIdTracker)
    , iActiveUriProvider(NULL)
    , iUriStreamer(NULL)
    , iTrack(NULL)
    , iStopped(true)
    , iSendHalt(false)
    , iQuit(false)
    , iNextHaltId(MsgHalt::kIdNone + 1)
{
}

Filler::~Filler()
{
    Kill();
    Join();
    if (iTrack != NULL) {
        iTrack->RemoveRef();
    }
}

void Filler::Add(UriProvider& aUriProvider)
{
    iUriProviders.push_back(&aUriProvider);
}

void Filler::Start(IUriStreamer& aUriStreamer)
{
    iUriStreamer = &aUriStreamer;
    Thread::Start();
}

void Filler::Play(const Brx& aMode, TUint aTrackId)
{
    AutoMutex a(iLock);
    UpdateActiveUriProvider(aMode);
    iActiveUriProvider->Begin(aTrackId);
    iStopped = false;
    Signal();
}

void Filler::PlayLater(const Brx& aMode, TUint aTrackId)
{
    AutoMutex a(iLock);
    UpdateActiveUriProvider(aMode);
    iActiveUriProvider->BeginLater(aTrackId);
    iStopped = false;
    Signal();
}

TUint Filler::Stop()
{
    iLock.Wait();
    iStopped = true;
    const TUint id = ++iNextHaltId;
    iSendHalt = true;
    Signal();
    iLock.Signal();
    return id;
}

void Filler::StopNoHalt()
{
    iLock.Wait();
    iStopped = true;
    iLock.Signal();
}

TBool Filler::Next(const Brx& aMode)
{
    TBool ret = false;
    iLock.Wait();
    if (iActiveUriProvider != NULL && iActiveUriProvider->Mode() == aMode) {
        ret = iActiveUriProvider->MoveNext();
        iStopped = false;
        Signal();
    }
    iLock.Signal();
    return ret;
}

TBool Filler::Prev(const Brx& aMode)
{
    TBool ret = false;
    iLock.Wait();
    if (iActiveUriProvider != NULL && iActiveUriProvider->Mode() == aMode) {
        ret = iActiveUriProvider->MovePrevious();
        iStopped = false;
        Signal();
    }
    iLock.Signal();
    return ret;
}

TBool Filler::IsStopped() const
{
    iLock.Wait();
    const TBool stopped = iStopped;
    iLock.Signal();
    return stopped;
}

void Filler::UpdateActiveUriProvider(const Brx& aMode)
{
    iActiveUriProvider = NULL;
    for (TUint i=0; i<iUriProviders.size(); i++) {
        UriProvider* uriProvider = iUriProviders[i];
        if (uriProvider->Mode() == aMode) {
            iActiveUriProvider = uriProvider;
            break;
        }
    }
    if (iActiveUriProvider == NULL) {
        iStopped = true;
        THROW(FillerInvalidMode);
    }
}

void Filler::Run()
{
    BwsMode mode;
    Wait();
    while (!iQuit) {
        for (;;) {
            iLock.Wait();
            const TBool wait = iStopped;
            const TBool sendHalt = iSendHalt;
            iSendHalt = false;
            iLock.Signal();
            if (!wait) {
                break;
            }
            if (sendHalt) {
                iSupply.OutputHalt(iNextHaltId);
            }
            Wait();
        }
        iLock.Wait();
        if (iActiveUriProvider == NULL) {
            iLock.Signal();
            continue;
        }
        if (iTrack != NULL) {
            iTrack->RemoveRef();
            iTrack = NULL;
        }
        iTrackPlayStatus = iActiveUriProvider->GetNext(iTrack);
        mode.Replace(iActiveUriProvider->Mode());
        if (iTrackPlayStatus == ePlayNo) {
            iSupply.OutputHalt(iStopped? iNextHaltId : MsgHalt::kIdNone);
            iSendHalt = false;
            iLock.Signal();
            iStopped = true;
        }
        else {
            iLock.Signal();
            ASSERT(iTrack != NULL);
            (void)iUriStreamer->DoStream(*iTrack, mode);
        }
    }
}

void Filler::OutputTrack(Track& aTrack, TUint aTrackId, const Brx& aMode)
{
    iTrackId = aTrackId;
    iSupply.OutputTrack(aTrack, aTrackId, aMode);
}

void Filler::OutputStream(const Brx& aUri, TUint64 aTotalBytes, TBool aSeekable, TBool aLive, IStreamHandler& aStreamHandler, TUint aStreamId)
{
    iPipelineIdTracker.AddStream(iTrack->Id(), iTrackId, aStreamId, (iTrackPlayStatus==ePlayYes));
    iSupply.OutputStream(aUri, aTotalBytes, aSeekable, aLive, aStreamHandler, aStreamId);
}

void Filler::OutputData(const Brx& aData)
{
    iSupply.OutputData(aData);
}

void Filler::OutputMetadata(const Brx& aMetadata)
{
    iSupply.OutputMetadata(aMetadata);
}

void Filler::OutputFlush(TUint aFlushId)
{
    iSupply.OutputFlush(aFlushId);
}

void Filler::OutputHalt(TUint aHaltId)
{
    iSupply.OutputHalt(aHaltId);
}

void Filler::OutputQuit()
{
    iQuit = true;
    iSupply.OutputQuit();
    Signal();
}
