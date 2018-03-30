#ifndef __WIFITESTPAIRSTATE_H__
#define __WIFITESTPAIRSTATE_H__
#include "../src/wifi_inc.h"

class CWIFITestPairState : public CWiFiState
{
public:
    CWIFITestPairState();
    virtual ~CWIFITestPairState(void);

    virtual bool AfterStateCreate(long ulDeviceHandle, void * pUserData);
    virtual bool BeforeStateTransfer(bool bEndConnect = false);
    virtual bool ProcessTimer(int iTimerID);
    virtual bool ProcessOption(const WirelessSessionData & wsData);

protected:
    bool AnswerPwd(long ulDeviceHandle, const yl::string & strPin);
};

#endif
