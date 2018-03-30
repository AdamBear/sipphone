///////////////////////////////////////////////////////////
//  CBaseState.h
//  Implementation of the Class CBaseState
//  Created on:      15-五月-2014 9:18:16
//  Original author: songk
///////////////////////////////////////////////////////////
#ifndef CBASESTATE__INCLUDED_H
#define CBASESTATE__INCLUDED_H
#include "wirelessdefines.h"

class CSessionBase;

class CBaseState
{
public:
    CBaseState();
    //设置定时器
    bool SetTimer(bool bEnable, int iTimeOut);

    virtual ~CBaseState();

    //进入状态后处理函数
    virtual bool AfterStateCreate(long ulDeviceHandle, void * pUserData)
    {
        return false;
    }

    //状态被转换前处理函数
    virtual bool BeforeStateTransfer(bool bEndConnect = false)
    {
        return false;
    }

    //回调事件处理函数
    virtual bool ProcessCallbackEvent(msgObject & objMsg)
    {
        return false;
    }

    //用户事件处理函数
    virtual bool ProcessETLMsg(msgObject & objMsg)
    {
        return false;
    }

    //处理事件操作
    virtual bool ProcessOption(const WirelessSessionData & wsData)
    {
        return false;
    }

    //定时器处理函数
    virtual bool ProcessTimer(int  iTimerID)
    {
        return false;
    }

    //设置会话指针
    inline void SetSession(CSessionBase * pSession)
    {
        m_pSession = pSession;
    }

    //获取会话状态
    inline WIRELESS_SESSION_STATE  GetSessionState()
    {
        return m_eState;
    }

    //获取句柄
    inline long GetHandle()
    {
        return m_lDeviceHandle;
    }

protected:
    //处理退出状态
    virtual bool ExitState(bool bEndConnect = false);

protected:
    //device handle
    long m_lDeviceHandle;

    //会话状态
    WIRELESS_SESSION_STATE m_eState;

    //定时器ID
    const int m_iTimer;

    //会话指针
    CSessionBase * m_pSession;

private:
    CBaseState(const CBaseState &);
    CBaseState & operator = (const CBaseState &);
};

#endif // !defined(CBASESTATE__INCLUDED_H)
