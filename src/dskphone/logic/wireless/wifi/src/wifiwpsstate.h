#ifdef IF_SUPPORT_WIFI
///////////////////////////////////////////////////////////
//  CWiFiStateWps.h
//  Implementation of the Class CWiFiStateWps
//  Created on:      15-五月-2014 9:18:38
//  Original author: songk
///////////////////////////////////////////////////////////
#ifndef CWIFISTATEWPS__INCLUDED_
#define CWIFISTATEWPS__INCLUDED_

class CWiFiWpsState: public CWiFiState
{
public:
    CWiFiWpsState();
    ~CWiFiWpsState();
    //����
    virtual bool AfterStateCreate(long ulDeviceHandle, void * pUserData);
    virtual bool BeforeStateTransfer(bool bEndConnect = false);
    virtual bool ProcessCallbackEvent(msgObject & objMsg);

protected:
    //����WPS����
    virtual bool StartWps();
    virtual bool StopWps();
};

#endif //CWIFISTATEWPS__INCLUDED_
#endif // IF_SUPPORT_WIFI
