///////////////////////////////////////////////////////////
//  CHdmiManager.h
//  Implementation of the Class CHdmiManager
//  Created on:      06-二月-2015 14:51:58
//  Original author: huangwz
///////////////////////////////////////////////////////////
#ifndef _HDMI_MANAGER_H_
#define _HDMI_MANAGER_H_

#include "hotplug/commondefine.h"
#include "singletonclassdefine.h"
#include "service_dsk_define.h"
#include <ETLLib.hpp>

#ifdef IF_SUPPORT_HDMI
class CHdmiManager
{
    DECLARE_SINGLETON_CLASS(CHdmiManager)
public:
    bool Init();
    void Uninit();

    static LRESULT OnMessage(msgObject & objMsg);

    bool IsConnected()
    {
        return m_eStatus == HST_CONNECTED;
    }

    void SetConnectState(bool bConnected, const DskDisplayDevInfo * devResInfo);

    void SetDisplayDevice(bool bForce);

    void GetDisplayDeviceInfo(DskDisplayDevInfo & stInfo)
    {
        stInfo = m_stDevInfo;
    }

    bool SetHdmiSwitch(bool bEnable);

    bool GetHdmiSwitch();

    bool IsHdmiEnable();

private:
    int GetScreenInfo(DskScreenInfo & info);

private:
    // 连接状态
    HDMIStatusType m_eStatus;

    // 设备信息
    DskDisplayDevInfo m_stDevInfo;
};

#define _HdmiManager GET_SINGLETON_INSTANCE(CHdmiManager)

#endif
#endif // _HDMI_MANAGER_H_
