#ifdef IF_SUPPORT_WIFI
///////////////////////////////////////////////////////////
//  CWifiSession.h
//  Implementation of the Class CWifiSession
//  Created on:      15-五月-2014 9:17:31
//  Original author: songk
///////////////////////////////////////////////////////////
#ifndef CWIFISESSION__INCLUDED_H
#define CWIFISESSION__INCLUDED_H

#include "wireless/common/sessionbase.h"
#include "wireless/common/basestate.h"

class CWiFiSession : public CSessionBase
{

public:
    CWiFiSession();
    virtual ~CWiFiSession();
    //注销一个会话
    virtual bool BeforeDestorySession(bool bEndConnect = false);
    //进入一个会话状态
    virtual bool EnterState(WIRELESS_SESSION_STATE eState, long lDeviceHandle, void * pUserData);
    //退出会话
    virtual bool ExitSession(bool bEndConnect = false);

    //获取当前话路net id
    int GetNetID();
};

//定义wifi状态工厂类
class CWifiStateFactory
{
public:
    static CBaseState * CreateWifiState(WIRELESS_SESSION_STATE eState);
private:
    CWifiStateFactory() {;}
    ~CWifiStateFactory() {;}
};

#endif // !defined(CWIFISESSION__INCLUDED_H)
#endif // IF_SUPPORT_WIFI
