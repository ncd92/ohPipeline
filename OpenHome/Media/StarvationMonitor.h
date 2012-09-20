#ifndef HEADER_PIPELINE_STARVATION_MONITOR
#define HEADER_PIPELINE_STARVATION_MONITOR

#include <OpenHome/OhNetTypes.h>
#include <OpenHome/Private/Thread.h>
#include <OpenHome/Media/Msg.h>
#include <OpenHome/Media/Stopper.h> // FIXME - inappropriate location for IPipelineElement

namespace OpenHome {
namespace Media {

/*
Fixed buffer which implements a delay (poss ~100ms) to allow time for songcast sending

- Pulls audio, stopping pulling whenever it grows above a 'normal use' threshold (NormalSize).
- After emptying (seeing a halt msg or starting to ramp down), enters 'buffering' mode and 
  pulls until it reaches GorgeSize.  Doesn't deliver any audio to rhs while in buffering mode.
- If halt msg encountered, allows buffer to be exhausted without ramping down.
- If no halt msg, starts ramping down once less that StarvationThreshold of data remains.
- On exit from buffering mode, ramps up iff ramped down before buffering.
*/
    
class StarvationMonitor : private MsgQueueJiffies, public IPipelineElement
{
    friend class SuiteStarvationMonitor;
public:
    StarvationMonitor(MsgFactory& aMsgFactory, TUint aNormalSize, TUint aStarvationThreshold, TUint aGorgeSize, TUint aRampUpSize);
    void Enqueue(Msg* aMsg);
public: // from IPipelineElement
    Msg* Pull();
private:
    enum EStatus
    {
        ERunning
       ,ERampingDown
       ,EBuffering
       ,ERampingUp
    };
private:
    MsgAudio* DoProcessMsgOut(MsgAudio* aMsg);
    void Ramp(MsgAudio* aMsg, TUint aRampDuration, Ramp::EDirection aDirection);
private: // from MsgQueueJiffies
    void ProcessMsgIn(MsgHalt* aMsg);
    void ProcessMsgIn(MsgFlush* aMsg);
    Msg* ProcessMsgOut(MsgAudioPcm* aMsg);
    Msg* ProcessMsgOut(MsgSilence* aMsg);
private: // test helpers
    TBool EnqueueWouldBlock() const;
    TBool PullWouldBlock() const;
private:
    static const TUint kMaxSizeSilence = Jiffies::kJiffiesPerMs * 5;
    MsgFactory& iMsgFactory;
    TUint iNormalMax;
    TUint iStarvationThreshold;
    TUint iGorgeSize;
    TUint iRampUpSize;
    mutable Mutex iLock;
    Semaphore iSemIn;
    Semaphore iSemOut;
    EStatus iStatus;
    TUint iCurrentRampValue;
    TUint iRampDownDuration;
    TUint iRemainingRampSize;
    TBool iPlannedHalt;
};

} // namespace Media
} // namespace OpenHome

#endif // HEADER_PIPELINE_STARVATION_MONITOR
