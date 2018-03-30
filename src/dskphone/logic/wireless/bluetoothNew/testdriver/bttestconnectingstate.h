#ifdef IF_BT_SIMULATION_DRIVER
#ifndef __BTTESTCONNECTINGSTATE_H__
#define __BTTESTCONNECTINGSTATE_H__
#include "bttestdatadefine.h"
class CBTTestConnectingState : public CBtState
{
public:
    CBTTestConnectingState();
    virtual ~CBTTestConnectingState();

    virtual bool AfterStateCreate(long ulDeviceHandle, void * pUserData);
    virtual bool BeforeStateTransfer(bool bEndConnect = false);
    virtual bool ProcessTimer(int iTimerID);

protected:
    bool ReplyMessage(unsigned long ulMsg);
};

#endif // __BTTESTCONNECTINGSTATE_H__
#endif // IF_BT_SIMULATION_DRIVER
