#pragma once

#include <OpenHome/Types.h>
#include <OpenHome/Buffer.h>
#include <OpenHome/Media/InfoProvider.h>
#include <OpenHome/Configuration/ConfigManager.h>

namespace OpenHome {
namespace Av {

class IProduct;

class ISource
{
    friend class Product;
public:
    static const TUint kMaxSystemNameBytes = 20;
    static const TUint kMaxSourceNameBytes = 20;
    static const TUint kMaxSourceTypeBytes = 20;
    static const TUint kMaxAttributesBytes = 100;
public:
    virtual ~ISource() {}
    virtual const Brx& SystemName() const = 0;
    virtual const Brx& Type() const = 0;
    virtual void Name(Bwx& aBuf) const = 0;
    virtual TBool IsVisible() const = 0;
    virtual void Activate() = 0;
    virtual void Deactivate() = 0;
    virtual void SetVisible(TBool aVisible) = 0;
    virtual void StandbyEnabled() = 0;
    virtual void PipelineStopped() = 0;
private:
    virtual void Initialise(IProduct& aProduct, Configuration::IConfigInitialiser& aConfigInit, Configuration::IConfigManager& aConfigManagerReader, TUint aId) = 0;
};

class Source : public ISource/*, protected IInfoProvider*/
{
private:
    static const TUint kMaxSourceTypeBytes = 20;
    static const TUint kMaxSourceIndexDigits = 2; // assume a source count of 0..99 is reasonable
    static const OpenHome::Brn kKeySourceNamePrefix;
    static const OpenHome::Brn kKeySourceNameSuffix;
public:
    static const TUint kKeySourceNameMaxBytes = 32;
    static void GetSourceNameKey(const Brx& aName, Bwx& aBuf);
protected: // from ISource
    const Brx& SystemName() const override;
    const Brx& Type() const override;
    void Name(Bwx& aBuf) const override;
    TBool IsVisible() const override;
    void Deactivate() override;
    void SetVisible(TBool aVisible) override;
protected:
    Source(const TChar* aSystemName, const TChar* aType);
    ~Source();
    TBool IsActive() const;
    void DoActivate();
private: // from ISource
    void Initialise(IProduct& aProduct, Configuration::IConfigInitialiser& aConfigInit, Configuration::IConfigManager& aConfigManagerReader, TUint aId) override;
private:
    void NameChanged(Configuration::KeyValuePair<const Brx&>& aName);
protected:
    TBool iActive;
private:
    mutable Mutex iLock;
    Brn iSystemName;
    Brn iType;
    Bws<kMaxSourceNameBytes> iName;
    TBool iVisible;

    IProduct* iProduct;
    Configuration::ConfigText* iConfigName;
    TUint iConfigNameSubscriptionId;
    TBool iConfigNameCreated;
};

} // namespace Av
} // namespace OpenHome

