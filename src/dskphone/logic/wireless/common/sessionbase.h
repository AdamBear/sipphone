///////////////////////////////////////////////////////////
//  CSessionBase.h
//  Implementation of the Class CSessionBase
//  Created on:      15-五月-2014 9:17:21
//  Original author: songk
///////////////////////////////////////////////////////////
#ifndef CSESSIONBASE__INCLUDED_H
#define CSESSIONBASE__INCLUDED_H

#include "wirelessdefines.h"
#include "basestate.h"
class CSessionBase
{
public:
    CSessionBase();
    virtual ~CSessionBase();

    //销毁会话
    virtual bool BeforeDestorySession(bool bEndConnect = false)
    {
        return false;
    }
    //进入一个会话状态
    virtual bool EnterState(WIRELESS_SESSION_STATE eState, long ulDeviceHandle, void * pUserData)
    {
        return false;
    }
    //回调事件处理函数
    virtual bool ProcessCallbackEvent(msgObject & objMsg);
    //用户事件处理函数
    virtual bool ProcessETLMsg(msgObject & objMsg);
    //处理事件操作
    virtual bool ProcessOption(const WirelessSessionData & wsData);
    //定时器处理
    virtual bool ProcessTimer(int iTimerID);
    //退出会话
    virtual bool ExitSession(bool bEndConnect)
    {
        return false;
    }
    //获取当前会话状态
    inline WIRELESS_SESSION_STATE GetSessionState()
    {
        return (m_pState == NULL) ? SESSION_UNINIT_STATE : m_pState->GetSessionState();
    }

    //获取session id
    inline long GetSessionID()
    {
        return m_lSessionID;
    }

    inline long GetHandle()
    {
        return (m_pState == NULL) ? -1 : m_pState->GetHandle();
    }

protected:
    // 获取SessionID
    static long GenerateSessionID();
    // 重置SessionID
    static void ResetSessionID()
    {
        m_lIncreasedID = 0;
    }

protected:
    //会话指针
    CBaseState * m_pState;
    //当前SessionID
    long m_lSessionID;
    //递增的SessionID
    static long m_lIncreasedID;
    static long m_lSessionCount;

private:
    CSessionBase(const CSessionBase &);
    CSessionBase & operator = (const CSessionBase &);
};


#endif // !defined(CSESSIONBASE__INCLUDED_H)
