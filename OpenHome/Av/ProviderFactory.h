#ifndef HEADER_PROVIDER_FACTORY
#define HEADER_PROVIDER_FACTORY

#include <OpenHome/Types.h>

namespace OpenHome {
    class IPowerManager;
namespace Configuration {
    class IConfigInitialiser;
    class IConfigManager;
}
namespace Media {
    class IMute;
}
namespace Net {
    class DvDevice;
    class DvProvider;
}
namespace Av {
    class Product;
    class IVolumeManager;
    class IVolume;
    class IBalance;
    class IFade;

/**
 * DvProvider (the base class of all providers) does not have a publicly
 * accessible destructor, so the ProviderFactory cannot return pointers to that
 * type, as they cannot be deleted.
 *
 * This interface provides a virtual destructor for provider classes, allowing
 * this anonymous IProvider type to be returned from the ProviderFactory but,
 * crucially, also provides a public (virtual) destructor.
 */
class IProvider
{
public:
    virtual ~IProvider() {}
};

class ProviderFactory
{
public:
    static IProvider* NewConfiguration(Product& aProduct, Net::DvDevice& aDevice, Configuration::IConfigManager& aConfigReader);
    static IProvider* NewVolume(Product& aProduct, Net::DvDevice& aDevice, Configuration::IConfigManager& aConfigReader,
                                IVolumeManager& aVolumeManager, IBalance* aBalance, IFade* aFade);
};

} // namespace Av
} // namespaceOpenHome

#endif // HEADER_PROVIDER_FACTORY
