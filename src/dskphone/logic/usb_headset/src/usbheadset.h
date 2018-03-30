#ifdef IF_SUPPORT_USB_AUDIO
#ifndef __USBHEADSET_H__
#define __USBHEADSET_H__

#include "ETLLib.hpp"
#include "interfacedefine.h"
#include "singletonclassdefine.h"
#include "usbheadsetaction.h"

class CUSBHeadset
{
    DECLARE_SINGLETON_CLASS(CUSBHeadset)

public:
    // 初始化
    bool Init();
    // usb headset插入拔出
    bool Plug(bool bIn);
    // usb headset key
    bool ProcessKey(int iKey, bool bPress, bool bRelative);
    // 处理hook按键
    bool ProcessHooKey(bool & bUsbChannelClose);
    // 同步状态到耳机
    bool SyncState(bool bForce = false);
    // 定时器处理
    bool OnTimer(UINT uTimer);
    // 定时器到,同步usb设备状态
    bool SyncDeviceState(bool bSyncHook = true, bool bSyncMute = true, bool bSyncHold = true,
                         bool bSyncRing = true);
    // 同步usb设备的音量
    bool SyncVolume(int iVolume);
    // 异步线程执行结果处理
    void ProcessActionResult(USBHEADSET_CTRL_TYPE eActionType, bool bResult);
    // 设置响铃状态
    void SetRingState(int iPlay);

    bool IsConnect()
    {
        return m_bUSBPlugIn;
    }

    static void SetUsbHeadsetLog(int iModuleID, int iLogLevel);

private:
    // usb 耳机是否可用
    bool IsUSBHeadsetAvaliable();
    // 将本地的音量映射在耳机的音量
    int GetUSBVolume();
    // 将设备的音量映射成话机的音量
    int GetPhoneVolume();
    // 添加命令
    bool AddCmd(USBHEADSET_CTRL_TYPE eType, int iValue, bool bRun = true);
    // 异步线程执行
    bool RunCmd();
    // 设置状态同步定时器
    bool SetSyncTimer(bool bEnable);
    // 设置是否强制同步状态
    void SetForceSync(bool bForce)
    {
        m_bForceSync = bForce;
    }
    bool GetForceSync()
    {
        return m_bForceSync;
    }

    // 同步的定时器
    int m_iTimerID;

    int m_iHoldState;               // hold状态
    int m_iHookState;               // 手柄状态
    int m_iMuteState;               // mute状态
    int m_iRingerState;             // 响铃状态
    bool m_bForceSync;              // 强制同步标识
    yl::string m_strDialNum;        // 呼出号码
    bool m_bUSBPlugIn;              // usb耳机开关
};

// 获取全局唯一的对象
#define _USBHeadset GET_SINGLETON_INSTANCE(CUSBHeadset)

#endif // __USBHEADSET_H__
#endif // IF_SUPPORT_USB_AUDIO

