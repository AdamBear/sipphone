#ifdef IF_WIFI_SIMULATION_DRIVER
#ifndef __WIFITESTCONNECTINGSTATE_H__
#define __WIFITESTCONNECTINGSTATE_H__
#include "wifitestdatadefine.h"
#include "../src/wifi_inc.h"

class CWIFITestConnectingState : public CWiFiState
{
public:
    CWIFITestConnectingState();
    virtual ~CWIFITestConnectingState();

    virtual bool AfterStateCreate(long ulDeviceHandle, void * pUserData);
    virtual bool BeforeStateTransfer(bool bEndConnect = false);
    //virtual bool ProcessTimer(int iTimerID);
    virtual bool ProcessOption(const WirelessSessionData & wsData);

protected:
    //bool ReplyMessage(unsigned long ulMsg);
};

#endif
#endif //IF_WIFI_SIMULATION_DRIVER
