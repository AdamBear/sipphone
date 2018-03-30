#include "../include/modlinedevice.h"

#ifdef IF_SUPPORT_LINE_DEVICE
#include "linedevicemanager.h"

void lineDevice_Init()
{
    _LineDeviceManager.Init();
}

// 外接设备插入拔出事件
bool lineDevice_Event(int iKeyEvent)
{
    return _LineDeviceManager.ProcessDeviceEvent(iKeyEvent);
}

// 外接设备是否检测
bool lineDevice_IsConnect(bool bCheckMobile/* = true*/)
{
    return _LineDeviceManager.IsConnect(bCheckMobile);
}

#else

void lineDevice_Init()
{
}

// 外接设备插入拔出事件
bool lineDevice_Event(int iKeyEvent)
{
    return true;
}

// 外接设备是否检测
bool lineDevice_IsConnect(bool bCheckMobile/* = true*/)
{
    return false;
}

#endif // IF_SUPPORT_LINE_DEVICE
