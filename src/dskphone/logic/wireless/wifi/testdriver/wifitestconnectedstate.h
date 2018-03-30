#ifdef IF_WIFI_SIMULATION_DRIVER
#ifndef __WIFITESTCONNECTEDSTATE_H__
#define __WIFITESTCONNECTEDSTATE_H__

#include "wifitestdatadefine.h"
#include "../src/wifi_inc.h"

class CWIFITestConnectedState : public CWiFiState
{
public:
    CWIFITestConnectedState();
    virtual ~CWIFITestConnectedState();

    virtual bool AfterStateCreate(long ulDeviceHandle, void * pUserData);
    virtual bool BeforeStateTransfer(bool bEndConnect = false);
    //virtual bool ProcessOption(const WirelessSessionData& wsData);

protected:
    //bool ReplyMessage(unsigned long ulMsg);
};

#endif
#endif //IF_WIFI_SIMULATION_DRIVER

