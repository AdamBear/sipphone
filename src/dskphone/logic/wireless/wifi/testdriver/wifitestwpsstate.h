#ifdef IF_WIFI_SIMULATION_DRIVER
#ifndef _WIFITESTWPSSTATE_H_
#define _WIFITESTWPSSTATE_H_

#include "../src/wifi_inc.h"

class CWiFiTestWpsState : public CWiFiState
{
public:
    CWiFiTestWpsState();
    ~CWiFiTestWpsState();
    //重载
    virtual bool AfterStateCreate(long ulDeviceHandle, void * pUserData);
    virtual bool BeforeStateTransfer(bool bEndConnect = false);
    //virtual bool ProcessCallbackEvent(msgObject& objMsg);

protected:
    //开启WPS功能
    //virtual bool StartWps();
    //virtual bool StopWps();
};

#endif
#endif //IF_WIFI_SIMULATION_DRIVER
