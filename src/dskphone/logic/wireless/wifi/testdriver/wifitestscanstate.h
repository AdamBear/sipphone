#ifdef IF_WIFI_SIMULATION_DRIVER
#ifndef __WIFITESTSCANSTATE_H__
#define __WIFITESTSCANSTATE_H__

#include "../src/wifi_inc.h"

class CWIFITestScanState : public CWiFiState
{
public:
    CWIFITestScanState();
    virtual ~CWIFITestScanState(void);
    //父类继承
    virtual bool AfterStateCreate(long ulDeviceHandle, void * pUserData);
    virtual bool BeforeStateTransfer(bool bEndConnect = false);
    //virtual bool ProcessTimer(int iTimerID);
};

#endif
#endif //IF_WIFI_SIMULATION_DRIVER
