#ifdef IF_SUPPORT_WIFI
///////////////////////////////////////////////////////////
//  CWifiStateScannor.h
//  Implementation of the Class CWifiSycleScanState
//  Created on:      07-十二月-2015 21:36:32
//  Original author: sub
///////////////////////////////////////////////////////////

#ifndef CWIFICYCLESCANSTATE__INCLUDED_H
#define CWIFICYCLESCANSTATE__INCLUDED_H

#include "wifidefines.h"

class CWiFiCycleScanState : public CWiFiState
{

public:
    CWiFiCycleScanState();
    virtual ~CWiFiCycleScanState();
    //父类继承
    virtual bool AfterStateCreate(long ulDeviceHandle, void * pUserData);
    virtual bool BeforeStateTransfer(bool bEndConnect = false);
    virtual bool ProcessCallbackEvent(msgObject & objMsg);
    virtual bool ProcessETLMsg(msgObject & objMsg)
    {
        return false;
    }
    virtual bool ProcessTimer(int iTimerID);
    //处理用户的操作
    virtual bool ProcessOption(const WirelessSessionData & wsData);

protected:
    //开始搜索
    bool StartScan(bool bFirstScan);
    //停止搜索
    bool StopScan(bool bFirstScan);
private:
    bool m_bIsFirstScan;
};
#endif // !defined(CWIFISTATESCANNOR__INCLUDED_H)
#endif // IF_SUPPORT_WIFI
