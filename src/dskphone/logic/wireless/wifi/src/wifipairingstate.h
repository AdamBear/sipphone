#ifdef IF_SUPPORT_WIFI
///////////////////////////////////////////////////////////
//  CWifiStatePairing.h
//  Implementation of the Class CWifiStateConnect
//  Created on:      15-五月-2014 9:18:38
//  Original author: songk
///////////////////////////////////////////////////////////

#ifndef  CWIFISTATEPAIRING__INCLUDED_
#define CWIFISTATEPAIRING__INCLUDED_

#include "wifidefines.h"

class CWiFiPairingState : public CWiFiState
{

public:
    CWiFiPairingState();
    virtual ~CWiFiPairingState();
    //父类重载
    virtual bool AfterStateCreate(long ulDeviceHandle, void * pUserData);
    virtual bool ProcessCallbackEvent(msgObject & objMsg);
    virtual bool ProcessETLMsg(msgObject & objMsg);
    virtual bool ProcessOption(const WirelessSessionData& wsData);

protected:
    //输入密码
    bool AuthNetwork(int iNetID, const AuthInfo & autoInfo);
};
#endif // !defined(CWIFISTATEPAIRING__INCLUDED_)
#endif // IF_SUPPORT_WIFI
