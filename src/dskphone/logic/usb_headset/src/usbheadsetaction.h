#ifdef IF_SUPPORT_USB_AUDIO
#ifndef _USB_HEADSET_ACTION_H_
#define _USB_HEADSET_ACTION_H_

#include "singletonclassdefine.h"
#include "ETLLib.hpp"
#define VP_USB_THREAD_NAME       "USBThread"

// 命令类型
enum USBHEADSET_CTRL_TYPE
{
    USBHEADSET_NONE = 0,
    USBHEADSET_INIT,
    USBHEADSET_UNINIT,
    USBHEADSET_PLUGIN,
    USBHEADSET_PLUGOUT,
    USBHEADSET_RING,
    USBHEADSET_HOOK,
    USBHEADSET_MUTE,
    USBHEADSET_HOLD,
    USBHEADSET_VOLUME,
    USBHEADSET_VOLUME_INC,
    USBHEADSET_VOLUME_DEC
};

// 命令信息
struct UsbHeadsetCtrl
{
    USBHEADSET_CTRL_TYPE eCmd;
    int iValue;
    bool bFinished;

    UsbHeadsetCtrl()
    {
        eCmd = USBHEADSET_NONE;
        iValue = 0;
        bFinished = false;
    }
};

// 按键信息
struct UsbHeadsetKeyInfo
{
    int iKey;
    int iButtonData;
    bool bRelative;

    UsbHeadsetKeyInfo()
    {
        iKey = 0;
        iButtonData = -1;
        bRelative = false;
    }
};

class CUsbHeadsetAction
{
    DECLARE_SINGLETON_CLASS(CUsbHeadsetAction)

public:
    // 设置命令列表, 添加命令
    bool AddCmd(USBHEADSET_CTRL_TYPE eType, int iValue);
    // 命令处理
    bool ProcessCmd();
    // 执行执行
    bool RunCmd(YLList<UsbHeadsetCtrl> & listCmd);

    //消息处理函数
    static LRESULT OnMessage(msgObject & objMessage);

private:
    YLList<UsbHeadsetCtrl>      m_listCmd;          // 执行链表
};

#define _UsbHeadsetAction GET_SINGLETON_INSTANCE(CUsbHeadsetAction)

#endif // _USB_HEADSET_ACTION_H_
#endif // IF_SUPPORT_USB_AUDIO
