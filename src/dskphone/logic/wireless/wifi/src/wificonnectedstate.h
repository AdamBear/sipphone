#ifdef IF_SUPPORT_WIFI
///////////////////////////////////////////////////////////
//  CWifiStateConnector.h
//  Implementation of the Class CWifiStateConnector
//  Created on:      15-五月-2014 9:18:38
//  Original author: songk
///////////////////////////////////////////////////////////

#ifndef CWIFISTATECONNECTED__INCLUDED_
#define CWIFISTATECONNECTED__INCLUDED_

class CWiFiConnectedState: public CWiFiState
{
public:
    CWiFiConnectedState();
    ~CWiFiConnectedState();

    //重载
    virtual bool AfterStateCreate(long lDeviceHandle, void * pUserData);
    virtual bool BeforeStateTransfer(bool bEndConnect = false);
    virtual bool ProcessCallbackEvent(msgObject & objMsg);
};

#endif  // !defined(CWIFISTATECONNECTED__INCLUDED_)
#endif // IF_SUPPORT_WIFI
