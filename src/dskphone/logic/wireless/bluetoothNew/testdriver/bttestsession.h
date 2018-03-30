#ifdef IF_BT_SIMULATION_DRIVER
#ifndef __BTTESTSESSION_H__
#define __BTTESTSESSION_H__
#include "wireless/bluetoothNew/src/bluetooth_inc.h"

class CBTTestSession : public CSessionBase
{
public:
    CBTTestSession();
    virtual ~CBTTestSession(void);

    //注销一个会话
    virtual bool BeforeDestorySession(bool bEndConnect = false);
    //进入一个会话状态
    virtual bool EnterState(WIRELESS_SESSION_STATE eState, long ulDeviceHandle, void * pUserData);
    virtual bool ExitSession(bool bEndConnect = false);

    inline CBaseState * GetStatePtr()
    {
        return m_pState;
    }
};

class CBTTestStateFactory
{
public:
    static CBaseState * CreateState(WIRELESS_SESSION_STATE eState);

private:
    CBTTestStateFactory();
    ~CBTTestStateFactory();
};

#endif // __BTTESTSESSION_H__
#endif // IF_BT_SIMULATION_DRIVER
