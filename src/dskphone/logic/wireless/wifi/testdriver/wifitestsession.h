#ifdef IF_WIFI_SIMULATION_DRIVER
#ifndef _WIFITESTSESSION_H_
#define _WIFITESTSESSION_H_

#include "../src/wifi_inc.h"

class CWIFITestSession : public CSessionBase
{
public:
    CWIFITestSession();
    virtual ~CWIFITestSession(void);

    //注销一个会话
    virtual bool BeforeDestorySession(bool bEndConnect = false);
    //进入一个会话状态
    virtual bool EnterState(WIRELESS_SESSION_STATE eState, long ulDeviceHandle, void * pUserData);
//  virtual bool ExitSession(bool bEndConnect = false);

    inline CBaseState * GetStatePtr()
    {
        return m_pState;
    }
};

class CWIFITestStateFactory
{
public:
    static CBaseState * CreateState(WIRELESS_SESSION_STATE eState);

private:
    CWIFITestStateFactory();
    ~CWIFITestStateFactory();
};

#endif
#endif //IF_WIFI_SIMULATION_DRIVER
