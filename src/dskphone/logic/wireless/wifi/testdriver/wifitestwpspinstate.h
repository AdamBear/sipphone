#ifdef IF_WIFI_SIMULATION_DRIVER
#ifndef _WIFITESTWPSPINSTATE_H_
#define _WIFITESTWPSPINSTATE_H_

#include "../src/wifi_inc.h"

class CWiFiTestWpsPinState : public CWiFiState
{
public:
    CWiFiTestWpsPinState();
    ~CWiFiTestWpsPinState();
    //重载
    virtual bool AfterStateCreate(long ulDeviceHandle, void * pUserData);
    virtual bool BeforeStateTransfer(bool bEndConnect = false);
    //virtual bool ProcessCallbackEvent(msgObject& objMsg);

protected:
    //virtual bool StartWps();
    //virtual bool StopWps();
};

#endif
#endif //IF_WIFI_SIMULATION_DRIVER
