///////////////////////////////////////////////////////////
//  CBtSession.h
//  Implementation of the Class CBtSession
//  Created on:      2014年7月15日 11:14:36
//  Original author: songk
///////////////////////////////////////////////////////////
#ifdef IF_SUPPORT_BLUETOOTH
#ifndef __BTSESSION_H__
#define __BTSESSION_H__

class CBtSession: public CSessionBase
{
public:
    CBtSession();
    virtual ~CBtSession(void);
    //注销一个会话
    virtual bool BeforeDestorySession(bool bEndConnect = false);
    //进入一个会话状态
    virtual bool EnterState(WIRELESS_SESSION_STATE eState, long ulDeviceHandle, void * pUserData);
    virtual bool ExitSession(bool bEndConnect  = false);
    void * GetSessionInfo();

    //获取状态指针
    inline CBaseState * GetStatePtr()
    {
        return m_pState;
    }
};

class CBTStateFactory
{
public:
    static CBaseState * CreateState(WIRELESS_SESSION_STATE);

private:
    CBTStateFactory();
    ~CBTStateFactory();
};
#endif //__BTSESSION_H__
#endif // IF_SUPPORT_BLUETOOTH
