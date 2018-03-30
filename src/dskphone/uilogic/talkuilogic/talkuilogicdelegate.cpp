#include "talkuilogicdelegate.h"
#include "configparser/modconfigparser.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "account/include/modaccount.h"

#if IF_TALKUI_BLUETOOTH_ENABLE
#include "wireless/bluetoothNew/include/modbluetooth.h"
#endif

#include "voice/include/modvoice.h"
#include "broadsoft/callcenter/include/modcallcenter.h"

//键盘锁
#include "keyguard/include/modkeyguard.h"
#include "commonunits/modcommonunits.h"
#include "hotplug/modhotplug.h"

// 两个swap的间隔时间
#define TALK_ACTION_SWAP_INTERVAL 600


CTalkUILogicDelegate & CTalkUILogicDelegate::GetInstance()
{
    static CTalkUILogicDelegate s_talkUILogicDelegate;

    return s_talkUILogicDelegate;
}

CTalkUILogicDelegate::CTalkUILogicDelegate(void)
    : m_bLockSwap(false)
{
}

CTalkUILogicDelegate::~CTalkUILogicDelegate(void)
{
    if (m_lockTimer.IsTimerRuning())
    {
        m_lockTimer.KillTimer();
    }
}

//是否显示联系人头像
int CTalkUILogicDelegate::IsDisplayContactPhoto()
{
#if IF_TALKUI_CONTACT_PHOTO
    return configParser_GetConfigInt(kszDisplayContactPhoto);
#else
    return 0;
#endif
}

//获取显示策略
int CTalkUILogicDelegate::GetDisplayMethod()
{
#if TALKUI_DEBUG_TEST
    return 0;
#endif

    return configParser_GetConfigInt(kszDisplayMethod);
}

//在有name和number的情况下 是否优先显示name  默认是优先显示name
bool CTalkUILogicDelegate::IsShowNameFirst()
{
    return true;
}

//是否显示Via 中转信息
bool CTalkUILogicDelegate::IsShowViaInfo()
{
#if TALKUI_DEBUG_TEST
    return true;
#endif

    return (configParser_GetConfigInt(kszDiversionEnable) != 0);
}

//是否显示TargetInfo 信息
int CTalkUILogicDelegate::IsShowTargetInfo()
{
#if TALKUI_DEBUG_TEST
    return true;
#endif
    return configParser_GetConfigInt(kszDisplayIncomingCallInfo);
}

//获取在拨号界面优先显示的 信息  1=Label 2=DisplayName 3=UserName 默认为3
int CTalkUILogicDelegate::GetDisplayMethodOnDialing()
{
    return configParser_GetConfigInt(kszDisplayMethodOnDialing);
}

//是否Joyce
bool CTalkUILogicDelegate::IsJoyce()
{
    return (configParser_GetConfigInt(kszIsJoyce) != 0);
}

//是否呼出按键 播放按键音
bool CTalkUILogicDelegate::IsVolumeSendKeySoundOn()
{
    return (configParser_GetConfigInt(kszVolumeSendKeySoundOn) != 0);
}

//是否隐藏 FeatureAccessCodes
bool CTalkUILogicDelegate::IsHideFeatureAccessCodes()
{
    return configParser_GetConfigInt(kszHideFeatureAccessCodes) != 0;
}

// 是否*、#呼出键
int CTalkUILogicDelegate::GetKeyAsSend()
{
    // 0--Disable，1--#，2--*
    return configParser_GetConfigInt(kszKeyAsSend);
}

//是否是配置中的呼出键
bool CTalkUILogicDelegate::IsDigitkeyAsSend(int iKey)
{
    int iKeyAsSendType = GetKeyAsSend();

    //类型1 为 #呼出
    if (1 == iKeyAsSendType)
    {
        if (PHONE_KEY_POUND == iKey)
        {
            return true;
        }
    }
    else if (2 == iKeyAsSendType)
    {
        if (PHONE_KEY_STAR == iKey)
        {
            return true;
        }
    }

    return false;
}

//这个配置的意思 应该是能不能呼出## 这么理解
bool CTalkUILogicDelegate::IsEnableSendPoundKey()
{
    return (configParser_GetConfigInt(kszSendPoundKey) != 0);
}

//在通话界面 有新来电的时候 是否改变softkey
bool CTalkUILogicDelegate::IsSoftkeyChangedCallIncome()
{
    //return false;
    return (configParser_GetConfigInt(kszSoftkeyChangedCallIncome) != 0);
}

//是否开启了 USB 录制的配置
bool CTalkUILogicDelegate::IsUSBRecordEnable()
{
#if IF_TALKUI_BLUETOOTH_ENABLE
    return (0 != configParser_GetConfigInt(kszUSBRecordEnable));
#endif

    return false;
}

//是否开启了 USB 截图的配置
bool CTalkUILogicDelegate::IsUSBScreenshotEnable()
{
#if IF_TALKUI_BLUETOOTH_ENABLE
    return (0 != configParser_GetConfigInt(kszUSBScreenshotEnable));
#endif

    return false;
}

//是否键盘锁锁上
bool CTalkUILogicDelegate::IsKeyGuardLock()
{
    return keyGuard_IsLock();
}

//获取通话、会议 新来电目标账号ID
int CTalkUILogicDelegate::GetTalkingIncomingCallTargetAccountId()
{
    //调用逻辑层 接口 获取当前来电的目标账号ID
    return talklogic_GetIncomingCallAccountId();
}

//从逻辑层获取hoteling 账号
yl::string CTalkUILogicDelegate::GetHotelingUserName(int iAccountID)
{
    return acc_GetCustomShowText(iAccountID);
}

//从逻辑层获取Label
yl::string CTalkUILogicDelegate::GetLabelFromLogic(int iAccountID)
{
    return acc_GetLabel(iAccountID);
}

//从逻辑层获取Displayname
yl::string CTalkUILogicDelegate::GetDisplayNameFromLogic(int iAccountID)
{
    return acc_GetDisplayName(iAccountID);
}

//从逻辑层获取UserName
yl::string CTalkUILogicDelegate::GetUserNameFromLogic(int iAccountID)
{
    return acc_GetUsername(iAccountID);
}

//音频静音状态
bool CTalkUILogicDelegate::GetAudioMuteStatus()
{
    return talklogic_GetMuteStatus();
}

//获取焦点话路sessionID
int CTalkUILogicDelegate::GetFocusSessionID()
{
    return talklogic_GetFocusedSessionID();
}

//切换音频mute 状态
void CTalkUILogicDelegate::SwapAudioMuteStatue()
{
    SendUIRequestAction(GetFocusSessionID(), TALK_ACTION_MUTE);
}

//是否小心中心模式
bool CTalkUILogicDelegate::IsCallCenterMode()
{
    return voice_IsCallCenterMode();
}

//获取通道类型
CHANNEL_MODE CTalkUILogicDelegate::GetCurrentChannel()
{
    return voice_GetCurrentChannel();
}

//蓝牙是否有开启
bool CTalkUILogicDelegate::IsBluetoothEnable()
{
#if IF_TALKUI_BLUETOOTH_ENABLE

#ifdef IF_SUPPORT_BLUETOOTH
    return Bluetooth_IsEnable();
#endif

#endif

    return false;
}

//蓝牙是否连接
bool CTalkUILogicDelegate::IsBluetoothConnected()
{
#if IF_TALKUI_BLUETOOTH_ENABLE

#ifdef IF_SUPPORT_BLUETOOTH
    return Bluetooth_IsConnected();
#endif

#endif

    return false;
}


//获取通道图标
TALKUI_CHANNEL_ICON CTalkUILogicDelegate::GetChannelIcon()
{
    //通道图标类型
    TALKUI_CHANNEL_ICON objChannelIcon = TALKUI_CHANNEL_ICON_UNKNOWN;

    //获取通道类型
    CHANNEL_MODE eChannelMode = GetCurrentChannel();

    TALKUI_INFO("CTalkUILogicDelegate::GetChannelIcon %d", eChannelMode);

    switch (eChannelMode)
    {
    case CHANNEL_HANDFREE:
    case CHANNEL_GROUP:
        {
            //免提
            objChannelIcon = TALKUI_CHANNEL_ICON_HANDFREE;
        }
        break;
    case CHANNEL_HANDSET:
        {
            //手柄
            objChannelIcon = TALKUI_CHANNEL_ICON_HANDSET;
        }
        break;
    case CHANNEL_HEADSET:
    case CHANNEL_2HEADSET:
        {
            //耳麦
            objChannelIcon = TALKUI_CHANNEL_ICON_HEADSET;
        }
        break;
    case CHANNEL_RING:
        {
            if (voice_IsRingHeadset())
            {
                // 耳麦响铃
                objChannelIcon = TALKUI_CHANNEL_ICON_HEADSET;
            }
            else
            {
                // 免提响铃
                objChannelIcon = TALKUI_CHANNEL_ICON_HANDFREE;
            }
        }
        break;
    default:
        {
            //未知
            TALKUI_INFO("CTalkUILogicDelegate::GetChannelIcon Invalid Type %d", eChannelMode);
#warning unknown icon
            objChannelIcon = TALKUI_CHANNEL_ICON_HANDFREE;
        }
        break;
    }

    return objChannelIcon;
}
//
////获取蓝牙图标
//TALKUI_BLUETOOTH_ICON CTalkUILogicDelegate::GetBlueToothIcon()
//{
//  //蓝牙没有连接 那么直接返回蓝牙无效
//  if (!IsBluetoothEnable())
//  {
//      return TALKUI_BLUETOOTH_ICON_DISABLE;
//  }
//
//  //蓝牙有连接
//  if(IsBluetoothConnected())
//  {
//      return TALKUI_BLUETOOTH_ICON_CONNECTED;
//  }
//
//  //返回蓝牙无连接
//  return TALKUI_BLUETOOTH_ICON_UNCONNECTED;
//
//}

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
//获取callcenter 队列信息
yl::string CTalkUILogicDelegate::GetQueueStatus()
{
    yl::string strDisplay;
    if (CallCenter_IsEnableQueueStatus()
            && AS_IDLE != ACD_GetAcdStatus()
            && configParser_GetConfigInt(kszBroadsoftActive) != 0
            && ACD_BROADSOFT == Get_ACDType())
    {
        CallCenterQueueStatus ccQueueStatus = ACD_GetQueueStatus();
        strDisplay = ccQueueStatus.m_strName;
        strDisplay += ":";
        if (QUEUE_EMPTY == ccQueueStatus.m_eStatus)
        {
            strDisplay += _UILOGIC_LANG_TRAN(TRID_EMPTY);
        }
        else if (QUEUE_NORMAL == ccQueueStatus.m_eStatus)
        {
            strDisplay += _UILOGIC_LANG_TRAN(TRID_QUEUEING);
        }
        else if (QUEUE_THRESHOLD_EXCEEDED == ccQueueStatus.m_eStatus)
        {
            strDisplay += _UILOGIC_LANG_TRAN(TRID_ALERT);
        }
        else
        {
            strDisplay = "";
        }
    }
    else
    {
        strDisplay = "";
    }

    return strDisplay;
}
#endif

//播放按键音
void CTalkUILogicDelegate::PlayKeyTone(int iKey)
{
    // 调用voic 接口 播放按键音
    voice_PlayKeyTone(iKey);
}

//机型差异判断
//是否显示swap 的softkey
bool CTalkUILogicDelegate::IsShowSwapSoftkey()
{
    return true;
}

//是否x键转换为mute键
bool CTalkUILogicDelegate::IsMuteReplaceX()
{
    if (configParser_GetConfigInt(kszMuteMode) == 0)
    {
        return false;
    }

    // T23和T21没有Mute键，只有用X键复用
    if (PT_T23 == devicelib_GetPhoneType())
    {
        return true;
    }

    if (PT_T21 == devicelib_GetPhoneType())
    {
        return true;
    }

    return false;
}

//是否conf界面显示 confmanager的softkey
bool CTalkUILogicDelegate::IsConfShowManagerSoftkey()
{
    return true;
}

//给逻辑层发送action
bool CTalkUILogicDelegate::SendUIRequestAction(int iSessionID, TALK_ACTION_TYPE eActionType,
        WPARAM wActionParam,
        LPARAM lActionParam, void * pExtraData)
{
    //是否允许该action 通过
    //if (!IsAllowSendAction(eActionType))
    //{
    //  //不允许通过后需要弹出解锁框
    //  keyGuard_IsEnableModuld(KGM_MODULE_DSSKEY);
    //  return true;
    //}

    //34227的容错方案,防止用户过快的进行swap操作
    if (TALK_ACTION_SWAP == eActionType)
    {
        //判断如果已经锁住了 那么直接返回
        if (IsSwapLock())
        {
            return true;
        }

        //关闭锁
        LockSwap();

        //启动定时器
        //启动定时器前 先关闭定时器
        if (m_lockTimer.IsTimerRuning())
        {
            m_lockTimer.KillTimer();
        }

        //启动定时器
        m_lockTimer.SetTimer(TALK_ACTION_SWAP_INTERVAL, NULL, MK_MSG_ENTRY(this,
                             CTalkUILogicDelegate::OnSwapLockTimer));
    }

    TALKUI_INFO("_TalklogicManager.ProcessAction eActionType is %d. wActionParam is %d ", eActionType,
                wActionParam);
    return talklogic_UIEvent(iSessionID, eActionType, wActionParam, lActionParam, pExtraData);
}

// 是否允许给逻辑层发送Action  该函数配合 SendUIRequestAction 使用
bool CTalkUILogicDelegate::IsAllowSendAction(TALK_ACTION_TYPE eActionType)
{
    // 键盘锁未上锁,或者处于MenuKey的状态
    // 允许所有Action通过
    if (!keyGuard_IsLock()
            || keyGuard_GetKeyGuardType() == KG_MENU_KEY)
    {
        return true;
    }

    // 在Allkeys、Funkey开启的时候只允许接听呼出和接听通话
    return (eActionType == TALK_ACTION_SEND
            || eActionType == TALK_ACTION_CANCEL
            || eActionType == TALK_ACTION_ANSWER
            || eActionType == TALK_ACTION_KEYPRESS
            || eActionType == TALK_ACTION_TEXT_CHANGE
            || eActionType == TALK_ACTION_NEWCALL
            || eActionType == TALK_ACTION_REJECT
            || eActionType == TALK_ACTION_FOCUSE_CHANGE
            || eActionType == TALK_ACTION_SCREEN_CLICK
            || eActionType == TALK_ACTION_VOLUME_CHANGE
            || eActionType == TALK_ACTION_VOLUMEBAR_SHOW
            || eActionType == TALK_ACTION_WND_FOCUSE_CHANGE
            || eActionType == TALK_ACTION_EXIT_FAILD_INFO
            //在通话过程中按数字键属于DTMF
            || eActionType == TALK_ACTION_DTMF
            //不允许hold但运行resume，因为进入line界面后会自动hold
            || eActionType == TALK_ACTION_RESUME
            || eActionType == TALK_ACTION_ACCOUNT_CHANGE
           );
}

void CTalkUILogicDelegate::LockSwap()
{
    m_bLockSwap = true;
}

void CTalkUILogicDelegate::UnlockSwap()     //解锁swap 操作
{
    m_bLockSwap = false;
}

bool CTalkUILogicDelegate::IsSwapLock()
{
    return m_bLockSwap;
}

BOOL CTalkUILogicDelegate::OnSwapLockTimer(msgObject & msg)     //swap 加锁的定时函数
{
    if (msg.wParam == (UINT)&m_lockTimer)
    {
        m_lockTimer.KillTimer();

        UnlockSwap();       //解锁swap

    }
    return TRUE;
}

int CTalkUILogicDelegate::GetScrollTimer()
{
    int iScrollTimeout = configParser_GetConfigInt(kszRemoteDispScrollTimer);
    if (0 != iScrollTimeout)
    {
        if (iScrollTimeout < 100)
        {
            iScrollTimeout = 100;
        }
        else if (iScrollTimeout > 2000)
        {
            iScrollTimeout = 2000;
        }
    }
    else
    {
        iScrollTimeout = 500;
    }

    return iScrollTimeout;
}

bool CTalkUILogicDelegate::IsCustomDynamicSoftkeyOn()
{
    return configParser_GetConfigInt(kszCustomDynamicSoftkey) != 0;
}
