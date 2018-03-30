#ifdef IF_BT_SIMULATION_DRIVER
#ifndef __BTTESTPAIRSTATE_H__
#define __BTTESTPAIRSTATE_H__
#include "bttestdatadefine.h"

class CBTTestPairState : public CBtState
{
public:
    CBTTestPairState();
    virtual ~CBTTestPairState(void);

    virtual bool AfterStateCreate(long ulDeviceHandle, void * pUserData);
    virtual bool BeforeStateTransfer(bool bEndConnect = false);
    virtual bool ProcessTimer(int iTimerID);
    virtual bool ProcessOption(const WirelessSessionData & wsData);

protected:
    bool ProcessAnswerPin(const WirelessSessionData & wsData);

protected:
    bool ReplyMessage(unsigned long lMsg, bool bPaired = false);
};

#endif // __BTTESTPAIRSTATE_H__
#endif // IF_BT_SIMULATION_DRIVER
