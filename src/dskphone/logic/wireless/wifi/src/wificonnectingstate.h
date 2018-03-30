#ifdef IF_SUPPORT_WIFI
///////////////////////////////////////////////////////////
//  CWifiStateConnect.h
//  Implementation of the Class CWifiStateConnect
//  Created on:      15-五月-2014 9:18:38
//  Original author: songk
///////////////////////////////////////////////////////////

#ifndef CWIFISTATECONNECTOR__INCLUDED_
#define CWIFISTATECONNECTOR__INCLUDED_

class CWiFiConnectingState : public CWiFiState
{

public:
    CWiFiConnectingState();
    virtual ~CWiFiConnectingState();
    //父类重载
    virtual bool AfterStateCreate(long lDeviceHandle, void * pUserData);
    virtual bool BeforeStateTransfer(bool bEndConnect = false);
    virtual bool ProcessCallbackEvent(msgObject & objMsg);

};
#endif // !defined(CWIFISTATECONNECTOR__INCLUDED_)
#endif // IF_SUPPORT_WIFI
