#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "modhotplug.h"
#include "cameramanager.h"
#include "hdmimanager.h"
#include "hotplugthread.h"
#include "storagemanager.h"

void modhotplug_Init()
{
    bool bNeedCreateThread = false;

#ifdef IF_SUPPORT_HDMI
    bNeedCreateThread |= _HdmiManager.Init();
#endif

#ifdef IF_SUPPORT_VIDEO
    bNeedCreateThread |= _CameraManager.Init();
#endif

#ifdef IF_USB_SUPPORT
    bNeedCreateThread |= _StorageManager.Init();
#endif

    // 创建usb端口事件检测线程
    if (bNeedCreateThread
            && configParser_GetConfigInt(kszUsbPortEnable) != 0)
    {
        CreateHotplugThread();
    }
}

#ifdef IF_SUPPORT_VIDEO
CameraStatusType modhotplug_GetCameraStatus()
{
    return _CameraManager.GetStatus();
}

void modhotplug_CtrlCamera(bool bOpen, bool bSpecStream, bool bForce/* = false*/)
{
    _CameraManager.CtrlCamera(bOpen, bSpecStream, bForce);
}
#endif // #ifdef IF_SUPPORT_VIDEO

bool modhotplug_IsHdmiConnected()
{
#ifdef IF_SUPPORT_HDMI
    return _HdmiManager.IsConnected();
#else
    return false;
#endif
}

bool modhotplug_SetHdmiSwitch(bool bEnable)
{
#ifdef IF_SUPPORT_HDMI
    return _HdmiManager.SetHdmiSwitch(bEnable);
#else
    return false;
#endif
}

bool modhotplug_GetHdmiSwitch()
{
#ifdef IF_SUPPORT_HDMI
    return _HdmiManager.GetHdmiSwitch();
#else
    return false;
#endif
}

bool modhotplug_IsHdmiEnable()
{
#ifdef IF_SUPPORT_HDMI
    return _HdmiManager.IsHdmiEnable();
#else
    return false;
#endif
}

#ifdef IF_SUPPORT_HDMI
void modhotplug_GetHdmiInfo(DskDisplayDevInfo & info)
{
    _HdmiManager.GetDisplayDeviceInfo(info);
}
#endif //#ifdef IF_SUPPORT_HDMI


