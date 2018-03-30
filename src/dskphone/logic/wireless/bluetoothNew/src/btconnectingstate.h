///////////////////////////////////////////////////////////
//  CBTConnectingState.h
//  Implementation of the Class CBTConnectingState
//  Created on:      2014年7月15日 11:14:36
//  Original author: songk
///////////////////////////////////////////////////////////
#ifdef IF_SUPPORT_BLUETOOTH
#ifndef __BTCONNECTINGSTATE_H__
#define __BTCONNECTINGSTATE_H__
#include "btstate.h"

class CBTConnectingState : public CBtState
{
public:
    CBTConnectingState(void);
    virtual ~CBTConnectingState(void);
    //父类继承
    virtual bool AfterStateCreate(long ulDeviceHandle, void * pUserData);
    //退出前处理
    virtual bool BeforeStateTransfer(bool bEndConnect = false);
    //回调消息处理
    virtual bool ProcessCallbackEvent(msgObject & objMsg);
    //操作处理
    virtual bool ProcessOption(const WirelessSessionData & wsData);

#if IF_BT_CONNECTING_OVERTIME
    //定时器处理
    virtual bool ProcessTimer(int iTimerID);
#endif // IF_BT_CONNECTING_OVERTIME

    bool ExitState(bool bEndConnect  = false, bool bConnectWattingDevice = true);

protected:
    //连接已匹配设备
    bool ConnectPairedDevice(unsigned long ulDeviceHandle);
    //连接接口
    bool StartConnectDevice(unsigned long ulDeviceHandle);
    bool DisconnectDevice(unsigned long ulDeviceHandle);
#if IF_SUPPORT_REPAIR
    bool UnpairPhoneDevice();
    bool RepairPhoneDevice();
#endif
};

#endif //__BTCONNECTINGSTATE_H__
#endif // IF_SUPPORT_BLUETOOTH
