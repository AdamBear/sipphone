#ifndef __VOICE_MANAGER_H__
#define __VOICE_MANAGER_H__

#include "service_ipvp.h"

#include "audiodevices.h"
#include "volumemanager.h"
#include "voicesession.h"

class CBaseChannelMode;

class CVoiceManager
{
    friend class CVolumeManager;
    friend class CChannelForResume;

public:
    static CVoiceManager & GetInstance();
    static void ReleaseInstance();

    static bool IsSupportHandset();
    static bool IsSupportHeadset();

private:
    static CVoiceManager * s_pInstance;

    CVoiceManager();
    ~CVoiceManager();

public:
    // 初始化
    void InitVoice();

    // 创建Session
    virtual VoiceSessionPtr AllocSession(CHANNEL_MODE eChannel);
    // 释放Session：由CVoiceSession析构函数调用
    void FreeSession(const CVoiceSession * session);
    // 激活Session：把Session移动到顶部
    void ActivateSession(CVoiceSession * session);
    // Session栈是否为空
    bool IsSessionStackEmpty() const;
    // 获取顶部Session
    virtual VoiceSessionWeakPtr GetActiveSession() const;

    // 摘挂机一定会更改摘挂状态
    void ChangeHandsetStatus(int iKey);
    // 通道按键处理
    bool OnKeyPress(int iKey);
    // 播放按键音
    void PlayDTMF(int iKey, bool bPlayNow = false);
    // 播放本地DTMF音
    void PlayLocalDTMF(int iKey, bool bPlayNow = false);
    void StopPlayLocalDTMF();

    // 是否耳麦响铃
    bool IsRingHeadset();

    //  重置声音通道
    void ResetChannel();

    //获取通道处理类
    CBaseChannelMode * GetChannelModePtr(CHANNEL_MODE eMode, bool bEHS = false,
                                         bool bForceOpenGroup = false);
    // 获取通道权限
    bool IsChannelFobidden(REAL_CHANNEL eChannel);

#ifdef IF_SUPPORT_BLUETOOTH
    // 是否蓝牙模式
    bool IsBluetoothMode();
#endif //IF_SUPPORT_BLUETOOTH

#if defined(IF_SUPPORT_BLUETOOTH) || defined(IF_SUPPORT_USB_AUDIO)
    // 设置耳机设备连接状态
    void SetHeadsetDevice(HEADSET_TYPE eType, bool bConnect);
    // 是否是耳机模式
    bool IsHeadsetDeviceMode();
    // 设置耳机通道
    bool SetHeadsetDeviceChannel(bool bEnable);
    // 获取当前正在使用耳机类型
    HEADSET_TYPE GetHeadsetChannelAvailableType(bool bAvalibaleInTalk = true);
#endif //#if defined(IF_SUPPORT_BLUETOOTH) || defined(IF_SUPPORT_USB_AUDIO)

    void PlayA2dp(bool bPlay);

    // 注册/注销回调函数
    void InitUpdateChanelInfo(FunUpdateChannelInfo pCallBack, bool bRegister);

#ifdef IF_SUPPORT_LINE_DEVICE
    bool SetLineDeviceChannel(int iSlaveMode);
#endif // IF_SUPPORT_LINE_DEVICE

    // 清空各通道模式相关的状态
    void ResetChannelParam(bool bEnable);
    void ResetChannelStatus(bool bEnable);

    void UpdateAccessStatus();

    bool m_bHandSetEnable;
    bool m_bHandFreeEnable;
    bool m_bHeadSetEnable;
private:


#ifdef IF_FEATURE_DUALHEADSET
    // CallCenter模式下的通道按键处理
    bool OnKeyPressForCallCenter(int iKey);
#endif

    // Idle界面下按耳麦键关闭耳麦
    void CloseHeadsetOnIdle();

public:
    CVolumeManager  m_objVolume; // 音量管理
    CAudioDevices   m_objDevices; // 语音设备管理
    FunUpdateChannelInfo m_funUpdateChannelInfo;

private:
    typedef yl::pair<CVoiceSession *, VoiceSessionWeakPtr> SessionStackItem;
    // front为栈底，back为栈顶
    typedef YLList<SessionStackItem> SessionStack;

    SessionStack m_sessions;
    VoiceSessionPtr m_pIdleSession;

    int          m_iLocalDtmfOffTimer;

    YLList<int> m_listDtmfKey;

    void PauseActiveSession();
    void ResumeActiveSession();

    // 定时器消息处理
    bool OnTimer(msgObject & objMsg);
    // 处理重置声道消息
    bool OnSelectChannel(msgObject & objMsg);
    // 处理VPM启动消息
    bool OnVPMRegistMsg(msgObject & objMsg);
    // 处理配置改变消息
    bool OnConfigChange(msgObject & objMsg);
};

// 获取全局唯一的对象
#define VoiceManager (CVoiceManager::GetInstance())

#endif // __VOICE_MANAGER_H__
