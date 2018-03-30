#ifdef IF_BT_SIMULATION_DRIVER
#ifndef __BTTESTSCANSTATE_H__
#define __BTTESTSCANSTATE_H__
#include "wireless/bluetoothNew/src/bluetooth_inc.h"

class CBTTestScanState : public CBtState
{
public:
    CBTTestScanState();
    virtual ~CBTTestScanState(void);
    //父类继承
    virtual bool AfterStateCreate(long ulDeviceHandle, void * pUserData);
    virtual bool BeforeStateTransfer(bool bEndConnect = false);
    virtual bool ProcessTimer(int iTimerID);
};

#endif // __BTTESTSCANSTATE_H__
#endif // IF_BT_SIMULATION_DRIVER
