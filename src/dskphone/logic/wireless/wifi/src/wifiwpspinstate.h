#ifdef IF_SUPPORT_WIFI
///////////////////////////////////////////////////////////
//  CWiFiStateWpspin.h
//  Implementation of the Class CWiFiStateWpspin
//  Created on:      15-五月-2014 9:18:38
//  Original author: songk
///////////////////////////////////////////////////////////
#ifndef CWIFISTATEWPSPIN__INCLUDED_
#define CWIFISTATEWPSPIN__INCLUDED_

class CWiFiWpspinState: public CWiFiWpsState
{
public:
    CWiFiWpspinState();
    ~CWiFiWpspinState();

protected:
    virtual bool StartWps();
    virtual bool StopWps();
};

#endif //#define(CWIFISTATEWPSPIN__INCLUDED_)
#endif // IF_SUPPORT_WIFI
