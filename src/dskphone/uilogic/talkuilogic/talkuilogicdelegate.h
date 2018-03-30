#ifndef _TALK_UILOGIC_DELEGATE_H_
#define _TALK_UILOGIC_DELEGATE_H_

#include <ETLLib.hpp>
#include "talk/talklogic/include/uiandlogic_common.h"
#include "talkuilogiccommon.h"
#include "voice/include/voicecommon.h"

class CTalkUILogicDelegate : public CMKProcessor
{
private:
    CTalkUILogicDelegate(void);

public:
    virtual ~CTalkUILogicDelegate(void);

public:
    static CTalkUILogicDelegate & GetInstance();

    //判断
public:
    //显示联系人头像策略
    int IsDisplayContactPhoto();

    //获取显示策略
    int GetDisplayMethod();

    //在有name和number的情况下 是否优先显示name
    bool IsShowNameFirst();

    //是否显示Via 中转信息
    bool IsShowViaInfo();

    //是否显示TargetInfo 信息
    int IsShowTargetInfo();

    //获取在拨号界面优先显示的 信息  1=Label 2=DisplayName 3=UserName 默认为3
    int GetDisplayMethodOnDialing();

    //是否Joyce
    bool IsJoyce();

    //是否呼出按键 播放按键音
    bool IsVolumeSendKeySoundOn();

    //是否隐藏 FeatureAccessCodes
    bool IsHideFeatureAccessCodes();

    // 是否*、#呼出键
    int GetKeyAsSend();

    //是否是配置中的呼出键
    bool IsDigitkeyAsSend(int iKey);

    //这个配置的意思 应该是能不能呼出## 这么理解
    bool IsEnableSendPoundKey();

    //在通话界面 有新来电的时候 是否改变softkey
    bool IsSoftkeyChangedCallIncome();

    //是否开启了 USB 录制的配置
    bool IsUSBRecordEnable();

    //是否开启了 USB 截图的配置
    bool IsUSBScreenshotEnable();

    //是否键盘锁锁上
    bool IsKeyGuardLock();

public:
    //获取通话、会议 新来电目标账号ID
    int GetTalkingIncomingCallTargetAccountId();

    //从逻辑层获取hoteling 账号
    yl::string GetHotelingUserName(int iAccountID);

    //从逻辑层获取Label
    yl::string GetLabelFromLogic(int iAccountID);

    //从逻辑层获取Displayname
    yl::string GetDisplayNameFromLogic(int iAccountID);

    //从逻辑层获取UserName
    yl::string GetUserNameFromLogic(int iAccountID);

    //音频静音状态
    bool GetAudioMuteStatus();


    //获取焦点话路sessionID
    int GetFocusSessionID();

    //切换音频mute 状态
    void SwapAudioMuteStatue();

    //是否消息中心模式
    bool IsCallCenterMode();


public:
    //获取通道类型
    CHANNEL_MODE GetCurrentChannel();
    //蓝牙是否有开启
    bool IsBluetoothEnable();
    //蓝牙是否连接
    bool IsBluetoothConnected();


public:
    //
    //获取通道图标
    TALKUI_CHANNEL_ICON GetChannelIcon();
    ////获取蓝牙图标
    //TALKUI_BLUETOOTH_ICON GetBlueToothIcon();

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    //获取callcenter 队列信息
    yl::string GetQueueStatus();
#endif

public:
    //操作
    //播放按键音
    void PlayKeyTone(int iKey);


    //机型差异判断
public:
    //是否显示swap 的softkey
    bool IsShowSwapSoftkey();

    //是否x键转换为mute键
    bool IsMuteReplaceX();

    //是否conf界面显示 confmanager的softkey
    bool IsConfShowManagerSoftkey();

public:
    int GetScrollTimer();

    bool IsCustomDynamicSoftkeyOn();

public:
    bool SendUIRequestAction(int iSessionID, TALK_ACTION_TYPE eActionType, WPARAM wActionParam = 0,
                             LPARAM lActionParam = 0, void * pExtraData = NULL);

protected:
    // 是否允许给逻辑层发送Action  该函数配合 SendUIRequestAction 使用
    virtual bool IsAllowSendAction(TALK_ACTION_TYPE eActionType);

protected:
    // 对swap操作进行加锁操作
    void LockSwap();
    void UnlockSwap();      //解锁swap 操作
    bool IsSwapLock();      //询问swap是否已经加锁
    // 是否对swap操作进行上锁
    // 34227的容错方案,防止用户过快的进行swap操作
    bool     m_bLockSwap;

public:
    BOOL OnSwapLockTimer(msgObject & msg);      //swap 加锁的定时函数
protected:
    mkThreadTimer       m_lockTimer;        //解锁定时器


};

#define _TalkUILogicDelegate CTalkUILogicDelegate::GetInstance()

#endif

