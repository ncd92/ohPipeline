#ifndef HEADER_PIPELINE_PROTOCOL_FACTORY
#define HEADER_PIPELINE_PROTOCOL_FACTORY

namespace OpenHome {
class Environment;
namespace Media {

class Protocol;

class ProtocolFactory
{
public:
    static Protocol* NewHttp(Environment& aEnv);
    static Protocol* NewFile(Environment& aEnv);
    static Protocol* NewTone(Environment& aEnv);
};

class ContentProcessor;

class ContentProcessorFactory
{
public:
    static ContentProcessor* NewM3u();
    static ContentProcessor* NewPls();
    static ContentProcessor* NewOpml();
};

} // namespace Media
} // namespace OpenHome

#endif // HEADER_PIPELINE_PROTOCOL_FACTORY
