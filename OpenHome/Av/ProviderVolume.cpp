#include <OpenHome/Av/ProviderVolume.h>
#include <OpenHome/Buffer.h>
#include <OpenHome/Av/StringIds.h>
#include <OpenHome/Av/Product.h>
#include <OpenHome/Media/MuteManager.h>
#include <OpenHome/Av/VolumeManager.h>

using namespace OpenHome;
using namespace OpenHome::Av;
using namespace OpenHome::Configuration;
using namespace OpenHome::Media;
using namespace OpenHome::Net;

// from older .../Preamp/ServiceVolume.cpp
const TInt kActionNotSupportedCode = 801;
const Brn  kActionNotSupportedMsg("Action not supported");

const TInt kInvalidVolumeCode = 811;
const Brn  kInvalidVolumeMsg("Volume invalid");

const TInt kInvalidBalanceCode = 812;
const Brn  kInvalidBalanceMsg("Balance invalid");

const TInt kInvalidFadeCode = 813;
const Brn  kInvalidFadeMsg("Fade invalid");

const TInt kVolumeNotSupportedCode = 814;
const Brn  kVolumeNotSupportedMsg("Volume not supported");

const TInt kVolumeOffsetsNotSupportedCode = 815;
const Brn  kVolumeOffsetsNotSupportedMsg("Volume offsets not supported");

const TInt kInvalidChannelCode = 816;
const Brn  kInvalidChannelMsg("Channel invalid");

const TInt kVolumeOffsetOutOfRangeCode = 817;
const Brn  kVolumeOffsetOutOfRangeMsg("Volume offset out of range");


// OffsetsWriterJson

const TChar* OffsetsWriterJson::kKeyChannel = "channel";
const TChar* OffsetsWriterJson::kKeyOffset = "offset";

OffsetsWriterJson::OffsetsWriterJson(IWriter& aWriter)
    : iWriter(aWriter)
{
}

void OffsetsWriterJson::WriteStart()
{
    // WriterJsonArray automatically writes appropriate start.
}

void OffsetsWriterJson::WriteEnd()
{
    iWriter.WriteEnd();
}

void OffsetsWriterJson::Visit(const Brx& aChannel, TInt aOffsetBinaryMilliDb)
{
    WriterJsonObject obj = iWriter.CreateObject();
    obj.WriteString(kKeyChannel, aChannel);
    obj.WriteInt(kKeyOffset, aOffsetBinaryMilliDb);
    obj.WriteEnd();
}


ProviderVolume::ProviderVolume(DvDevice& aDevice, IConfigManager& aConfigReader, IVolumeManager& aVolumeManager, IBalance* aBalance, IFade* aFade, IVolumeOffsetter* aVolumeOffsetter)
    : DvProviderAvOpenhomeOrgVolume3(aDevice)
    , iLock("PVOL")
    , iVolume(aVolumeManager)
    , iBalance(aBalance)
    , iFade(aFade)
    , iVolumeOffsetter(aVolumeOffsetter)
    , iUserMute(aVolumeManager)
    , iVolumeMax(aVolumeManager.VolumeMax())
{
    EnablePropertyVolume();
    EnablePropertyMute();
    EnablePropertyBalance();
    EnablePropertyFade();
    EnablePropertyVolumeLimit();
    EnablePropertyVolumeMax();
    EnablePropertyVolumeUnity();
    EnablePropertyVolumeSteps();
    EnablePropertyVolumeMilliDbPerStep();
    EnablePropertyBalanceMax();
    EnablePropertyFadeMax();
    EnablePropertyUnityGain();
    EnablePropertyVolumeOffsets();
    EnablePropertyVolumeOffsetMax();

    EnableActionCharacteristics();
    EnableActionSetVolume();
    EnableActionVolumeInc();
    EnableActionVolumeDec();
    EnableActionVolume();
    EnableActionSetBalance();
    EnableActionBalanceInc();
    EnableActionBalanceDec();
    EnableActionBalance();
    EnableActionSetFade();
    EnableActionFadeInc();
    EnableActionFadeDec();
    EnableActionFade();
    EnableActionSetMute();
    EnableActionMute();
    EnableActionVolumeLimit();
    EnableActionUnityGain();
    EnableActionVolumeOffset();
    EnableActionSetVolumeOffset();

    aVolumeManager.AddVolumeObserver(*this);
    aVolumeManager.AddMuteObserver(*this);
    aVolumeManager.AddUnityGainObserver(*this);
    iConfigVolumeLimit = &aConfigReader.GetNum(VolumeConfig::kKeyLimit);
    iSubscriberIdVolumeLimit = iConfigVolumeLimit->Subscribe(MakeFunctorConfigNum(*this, &ProviderVolume::VolumeLimitChanged));
    if (iBalance == nullptr) {
        iConfigBalance = nullptr;
        SetPropertyBalance(0);
    }
    else {
        iConfigBalance = &aConfigReader.GetNum(VolumeConfig::kKeyBalance);
        iSubscriberIdBalance = iConfigBalance->Subscribe(MakeFunctorConfigNum(*this, &ProviderVolume::BalanceChanged));
    }
    if (iFade == nullptr) {
        iConfigFade = nullptr;
        SetPropertyFade(0);
    }
    else {
        iConfigFade = &aConfigReader.GetNum(VolumeConfig::kKeyFade);
        iSubscriberIdFade = iConfigFade->Subscribe(MakeFunctorConfigNum(*this, &ProviderVolume::FadeChanged));
    }

    if (iVolumeOffsetter == nullptr) {
        SetPropertyVolumeOffsets(Brn("[]")); // Empty JSON array.
    }
    else {
        // Registering with the IVolumeOffsetterObservable will result in
        // initial callback, which will call SetPropertyVolumeOffsets().
        iVolumeOffsetter->AddVolumeOffsetterObserver(*this);
    }

    SetPropertyVolumeMax(aVolumeManager.VolumeMax());
    SetPropertyVolumeUnity(aVolumeManager.VolumeUnity());
    SetPropertyVolumeSteps(aVolumeManager.VolumeStep());
    SetPropertyVolumeMilliDbPerStep(aVolumeManager.VolumeMilliDbPerStep());
    SetPropertyBalanceMax(aVolumeManager.BalanceMax());
    SetPropertyFadeMax(aVolumeManager.FadeMax());
    SetPropertyVolumeOffsetMax(aVolumeManager.OffsetMax());
}

ProviderVolume::~ProviderVolume()
{
    iConfigVolumeLimit->Unsubscribe(iSubscriberIdVolumeLimit);
    if (iConfigBalance != nullptr) {
        iConfigBalance->Unsubscribe(iSubscriberIdBalance);
    }
    if (iConfigFade != nullptr) {
        iConfigFade->Unsubscribe(iSubscriberIdFade);
    }
}

void ProviderVolume::Characteristics(IDvInvocation& aInvocation, IDvInvocationResponseUint& aVolumeMax, IDvInvocationResponseUint& aVolumeUnity, IDvInvocationResponseUint& aVolumeSteps, IDvInvocationResponseUint& aVolumeMilliDbPerStep, IDvInvocationResponseUint& aBalanceMax, IDvInvocationResponseUint& aFadeMax)
{
    TUint maxVol = 0;
    TUint unityVol = 0;
    TUint volSteps = 0;
    TUint milliDbPerVolStep = 0;
    TUint maxBalance = 0;
    TUint maxFade = 0;

    GetPropertyVolumeMax(maxVol);
    GetPropertyVolumeUnity(unityVol);
    GetPropertyVolumeSteps(volSteps);
    GetPropertyVolumeMilliDbPerStep(milliDbPerVolStep);
    GetPropertyBalanceMax(maxBalance);
    GetPropertyFadeMax(maxFade);

    aInvocation.StartResponse();
    aVolumeMax.Write(maxVol);
    aVolumeUnity.Write(unityVol);
    aVolumeSteps.Write(volSteps);
    aVolumeMilliDbPerStep.Write(milliDbPerVolStep);
    aBalanceMax.Write(maxBalance);
    aFadeMax.Write(maxFade);
    aInvocation.EndResponse();
}

void ProviderVolume::SetVolume(IDvInvocation& aInvocation, TUint aValue)
{
    HelperSetVolume(aInvocation, aValue, ErrorOutOfRange::Report);
}

void ProviderVolume::VolumeInc(IDvInvocation& aInvocation)
{
    TUint volume = 0;
    GetPropertyVolume(volume);
    HelperSetVolume(aInvocation, volume+1, ErrorOutOfRange::Ignore);
}

void ProviderVolume::VolumeDec(IDvInvocation& aInvocation)
{
    TUint volume = 0;
    GetPropertyVolume(volume);
    HelperSetVolume(aInvocation, volume-1, ErrorOutOfRange::Ignore);
}

void ProviderVolume::Volume(IDvInvocation& aInvocation, IDvInvocationResponseUint& aValue)
{
    TUint vol = 0;
    GetPropertyVolume(vol);
    aInvocation.StartResponse();
    aValue.Write(vol);
    aInvocation.EndResponse();
}

void ProviderVolume::SetBalance(IDvInvocation& aInvocation, TInt aValue)
{
    HelperSetBalance(aInvocation, aValue, ErrorOutOfRange::Report);
}

void ProviderVolume::BalanceInc(IDvInvocation& aInvocation)
{
    TInt balance = 0;
    GetPropertyBalance(balance);
    HelperSetBalance(aInvocation, balance+1, ErrorOutOfRange::Ignore);
}

void ProviderVolume::BalanceDec(IDvInvocation& aInvocation)
{
    TInt balance = 0;
    GetPropertyBalance(balance);
    HelperSetBalance(aInvocation, balance-1, ErrorOutOfRange::Ignore);
}

void ProviderVolume::Balance(IDvInvocation& aInvocation, IDvInvocationResponseInt& aValue)
{
    TInt balance = 0;
    GetPropertyBalance(balance);
    aInvocation.StartResponse();
    aValue.Write(balance);
    aInvocation.EndResponse();
}

void ProviderVolume::SetFade(IDvInvocation& aInvocation, TInt aValue)
{
    HelperSetFade(aInvocation, aValue, ErrorOutOfRange::Report);
}

void ProviderVolume::FadeInc(IDvInvocation& aInvocation)
{
    TInt fade = 0;
    GetPropertyFade(fade);
    HelperSetFade(aInvocation, fade+1, ErrorOutOfRange::Ignore);
}

void ProviderVolume::FadeDec(IDvInvocation& aInvocation)
{
    TInt fade = 0;
    GetPropertyFade(fade);
    HelperSetFade(aInvocation, fade-1, ErrorOutOfRange::Ignore);
}

void ProviderVolume::Fade(IDvInvocation& aInvocation, IDvInvocationResponseInt& aValue)
{
    TInt fade = 0;
    GetPropertyFade(fade);
    aInvocation.StartResponse();
    aValue.Write(fade);
    aInvocation.EndResponse();
}

void ProviderVolume::SetMute(IDvInvocation& aInvocation, TBool aValue)
{
    try {
        if (aValue) {
            iUserMute.Mute();
        }
        else {
            iUserMute.Unmute();
        }
    }
    catch (MuteNotSupported&) {
        aInvocation.Error(kActionNotSupportedCode, kActionNotSupportedMsg);
    }
    aInvocation.StartResponse();
    aInvocation.EndResponse();
}

void ProviderVolume::Mute(IDvInvocation& aInvocation, IDvInvocationResponseBool& aValue)
{
    TBool muted = false;
    GetPropertyMute(muted);
    aInvocation.StartResponse();
    aValue.Write(muted);
    aInvocation.EndResponse();
}

void ProviderVolume::VolumeLimit(IDvInvocation& aInvocation, IDvInvocationResponseUint& aValue)
{
    TUint volLimit = 0;
    GetPropertyVolumeLimit(volLimit);
    aInvocation.StartResponse();
    aValue.Write(volLimit);
    aInvocation.EndResponse();
}

void ProviderVolume::UnityGain(IDvInvocation& aInvocation, IDvInvocationResponseBool& aValue)
{
    TBool unityGain = false;
    GetPropertyUnityGain(unityGain);
    aInvocation.StartResponse();
    aValue.Write(unityGain);
    aInvocation.EndResponse();
}

void ProviderVolume::VolumeOffset(IDvInvocation& aInvocation, const Brx& aChannel, IDvInvocationResponseInt& aVolumeOffsetBinaryMilliDb)
{
    if (iVolumeOffsetter == nullptr) {
        aInvocation.Error(kVolumeOffsetsNotSupportedCode, kVolumeOffsetsNotSupportedMsg);
        return;
    }

    try {
        const TInt offset = iVolumeOffsetter->GetVolumeOffset(aChannel);
        aInvocation.StartResponse();
        aVolumeOffsetBinaryMilliDb.Write(offset);
        aInvocation.EndResponse();
    }
    catch (ChannelInvalid&) {
        aInvocation.Error(kInvalidChannelCode, kInvalidChannelMsg);
    }
}

void ProviderVolume::SetVolumeOffset(IDvInvocation& aInvocation, const Brx& aChannel, TInt aVolumeOffsetBinaryMilliDb)
{
    if (iVolumeOffsetter == nullptr) {
        aInvocation.Error(kVolumeOffsetsNotSupportedCode, kVolumeOffsetsNotSupportedMsg);
        return;
    }

    try {
        iVolumeOffsetter->SetVolumeOffset(aChannel, aVolumeOffsetBinaryMilliDb);
        aInvocation.StartResponse();
        aInvocation.EndResponse();
    }
    catch (ChannelInvalid&) {
        aInvocation.Error(kInvalidChannelCode, kInvalidChannelMsg);
    }
    catch (VolumeOffsetOutOfRange&) {
        aInvocation.Error(kVolumeOffsetOutOfRangeCode, kVolumeOffsetOutOfRangeMsg);
    }
}

void ProviderVolume::VolumeChanged(const IVolumeValue& aVolume)
{
    SetPropertyVolume(aVolume.VolumeUser());
}

void ProviderVolume::MuteChanged(TBool aValue)
{
    SetPropertyMute(aValue);
}

void ProviderVolume::UnityGainChanged(TBool aValue)
{
    SetPropertyUnityGain(aValue);
}

void ProviderVolume::VolumeOffsetsChanged(IVolumeOffsetterVisitable& aVisitable)
{
    Bws<512> offsets;
    WriterBuffer writerBuffer(offsets);
    OffsetsWriterJson writerJson(writerBuffer);

    writerJson.WriteStart();
    aVisitable.AcceptVolumeOffsetterVisitor(writerJson);
    writerJson.WriteEnd();

    SetPropertyVolumeOffsets(offsets);
}

void ProviderVolume::HelperSetVolume(IDvInvocation& aInvocation, TUint aVolume, ErrorOutOfRange aReportOutOfRange)
{
    try {
        iVolume.SetVolume(aVolume);
    }
    catch (VolumeOutOfRange&) {
        if (aVolume > iVolumeMax && aReportOutOfRange == ErrorOutOfRange::Report) {
            aInvocation.Error(kInvalidVolumeCode, kInvalidVolumeMsg);
        }
    }
    catch (VolumeNotSupported&) {
        aInvocation.Error(kVolumeNotSupportedCode, kVolumeNotSupportedMsg);
    }
    aInvocation.StartResponse();
    aInvocation.EndResponse();
}

void ProviderVolume::HelperSetBalance(IDvInvocation& aInvocation, TInt aBalance, ErrorOutOfRange aReportOutOfRange)
{
    if (iBalance == nullptr) {
        aInvocation.Error(kActionNotSupportedCode, kActionNotSupportedMsg);
    }
    try {
        iBalance->SetBalance(aBalance);
    }
    catch (BalanceOutOfRange&) {
        if (aReportOutOfRange == ErrorOutOfRange::Report) {
            aInvocation.Error(kInvalidBalanceCode, kInvalidBalanceMsg);
        }
    }
    aInvocation.StartResponse();
    aInvocation.EndResponse();
}

void ProviderVolume::HelperSetFade(IDvInvocation& aInvocation, TInt aFade, ErrorOutOfRange aReportOutOfRange)
{
    if (iFade == nullptr) {
        aInvocation.Error(kActionNotSupportedCode, kActionNotSupportedMsg);
    }
    try {
        iFade->SetFade(aFade);
    }
    catch (FadeOutOfRange&) {
        if (aReportOutOfRange == ErrorOutOfRange::Report) {
            aInvocation.Error(kInvalidFadeCode, kInvalidFadeMsg);
        }
    }
    aInvocation.StartResponse();
    aInvocation.EndResponse();
}

void ProviderVolume::VolumeLimitChanged(ConfigNum::KvpNum& aKvp)
{
    SetPropertyVolumeLimit(aKvp.Value());
}

void ProviderVolume::BalanceChanged(ConfigNum::KvpNum& aKvp)
{
    const TInt balance = aKvp.Value();
    SetPropertyBalance(balance);
}

void ProviderVolume::FadeChanged(ConfigNum::KvpNum& aKvp)
{
    const TInt fade = aKvp.Value();
    SetPropertyFade(fade);
}
