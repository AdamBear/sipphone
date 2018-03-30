#ifndef __AUDIO_DEVICES_H__
#define __AUDIO_DEVICES_H__

#include <ETLLib.hpp>
#include <yllist.h>

#include "voicecommon.h"
#include "ehsheadset.h"


/* 管理语音相关设备状态 */
class CAudioDevices
{
public:
    CAudioDevices();
    ~CAudioDevices();

    // 获取手柄状态，TRUE表示摘起，FALSE表示挂上
    bool GetHandsetStatus();
    // 设置手柄状态
    void SetHandsetStatus(bool bStatus);

    // 获取免提状态
    bool GetHandfreeStatus();
    // 设置免提通道标志
    void SetHandfreeStatus(bool bOpen);

    // 获取耳麦状态
    bool GetHeadsetStatus();
    //设置耳麦状态
    void SetHeadsetStatus(bool bStatus);

    // 当前最新插入的耳机类型
    HEADSET_TYPE GetHeadsetDeviceType();
#if defined(IF_SUPPORT_BLUETOOTH) || defined(IF_SUPPORT_USB_AUDIO)
    // 设置耳机设备连接状态
    void SetHeadsetDevice(HEADSET_TYPE eType, bool bConnect);
    // 加入一个耳机类型
    void PushDevice(HEADSET_TYPE eType);
    // 移除一个耳机类型
    bool PopDevice(HEADSET_TYPE eType);
#endif //#if defined(IF_SUPPORT_BLUETOOTH) || defined(IF_SUPPORT_USB_AUDIO)

    void EHSOpen(bool bForceToClose = false);
    void EHSClose(bool bForceToClose = false);
    void EHSSwitchStatus();
    void EHSPlayRing();
#if IF_BUG_27190
    void EHSPlayPhoneRing();
#endif
    void EHSStopRing(bool bForceStopEHS = true);
    int  EHSGetIndex();
    bool EHSOnTimer(unsigned int uTimeID);

private:
    bool m_bOffHook; // 手柄是否摘起
    bool m_bHeadset; // 耳麦是否打开
    bool m_bHandfree; // 免提是否打开
    CEHSHeadset * m_pEHS; // EHS无线耳麦管理
#if defined(IF_SUPPORT_BLUETOOTH) || defined(IF_SUPPORT_USB_AUDIO)
    YLList<HEADSET_TYPE> m_listHeadsetType;
#endif // IF_SUPPORT_BLUETOOTH

    // EHS消息
    bool OnEHSMessage(msgObject & objMsg);

#ifdef IF_SUPPORT_BLUETOOTH
    // 蓝牙耳机消息
    bool OnBTHeadsetMessage(msgObject & objMsg);
#endif // IF_SUPPORT_BLUETOOTH

#ifdef IF_SUPPORT_USB_AUDIO
    // USB耳机消息
    bool OnUSBHeadsetMessage(msgObject & objMsg);
#endif // IF_SUPPORT_USB_AUDIO

    bool IsHookOffByDriver() const;
};

#endif // !__AUDIO_DEVICES_H__
