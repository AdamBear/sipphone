#ifdef IF_SUPPORT_WIFI
///////////////////////////////////////////////////////////
//  CWifiStateScannor.h
//  Implementation of the Class CWifiStateScannor
//  Created on:      15-五月-2014 9:18:32
//  Original author: songk
///////////////////////////////////////////////////////////

#ifndef CWIFISTATESCANNOR__INCLUDED_H
#define CWIFISTATESCANNOR__INCLUDED_H

#include "wifidefines.h"

class CWiFiScanState : public CWiFiState
{

public:
    CWiFiScanState();
    virtual ~CWiFiScanState();
    //父类继承
    virtual bool AfterStateCreate(long ulDeviceHandle, void * pUserData);
    virtual bool BeforeStateTransfer(bool bEndConnect = false);
    virtual bool ProcessCallbackEvent(msgObject & objMsg);
    virtual bool ProcessETLMsg(msgObject & objMsg)
    {
        return false;
    }
    virtual bool ProcessTimer(int iTimerID);
    //处理事件操作
    virtual bool ProcessOption(const WirelessSessionData & wsData);

protected:
    //开始搜索
    bool StartScan(int iLoopTime = 0, bool bRset = false);
    bool StopScan();
};
#endif // !defined(CWIFISTATESCANNOR__INCLUDED_H)
#endif // IF_SUPPORT_WIFI
