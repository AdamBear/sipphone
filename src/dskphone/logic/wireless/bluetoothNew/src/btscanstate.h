///////////////////////////////////////////////////////////
//  CBTScanState.h
//  Implementation of the Class CBTScanState
//  Created on:      2014年7月15日 11:14:36
//  Original author: songk
///////////////////////////////////////////////////////////
#ifdef IF_SUPPORT_BLUETOOTH
#ifndef __BTSCANSTATE_H__
#define __BTSCANSTATE_H__
#include "../include/btdefines.h"

#define BT_DELAYSCAN_TIME       500     // 延迟处理开启扫描, 单位:ms

class CBTScanState : public CBtState
{
public:
    CBTScanState(void);
    virtual ~CBTScanState(void);
    //父类继承
    virtual bool AfterStateCreate(long ulDeviceHandle, void * pUserData);
    virtual bool BeforeStateTransfer(bool bEndConnect = false);
    virtual bool ProcessCallbackEvent(msgObject & objMsg);
    virtual bool ProcessTimer(int iTimerID);
    virtual bool ProcessOption(const WirelessSessionData & wsData);

protected:
    //扫描到设备
    void ProcessDeviceFound(unsigned long ulDeviceHandle);
    //开始扫描
    bool StartScan();
    //停止扫描
    bool StopScan();
    //子线程处理扫描操作 true:start,false:stop
    bool HandleScanOption(bool bStartOrStop);
    //设置延迟处理定时器
    void SetDelayTimer(bool bEnable, int iDelayTime = 0);
    // 处理停止扫描返回结果
    bool ProcessStopScanResult(bool bResult);

private:
    int         m_iScanIndex;       // 底层上报扫描到设备的顺序
    bool        m_bUserStop;        // 用户操作触发的停止, 包括:stop、connect
    int         m_iDelayTimer;      // 做延时用的定时器
};

#endif //__BTSCANSTATE_H__
#endif // IF_SUPPORT_BLUETOOTH
