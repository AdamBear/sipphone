#include "voicechannel_include.h"
#include "modvoice.h"
#include "ehsheadset.h"
#ifdef BLUETOOTH_USED_OLD_VERSION
#include "wireless/bluetoothOld/include/btdefines.h"
#else
#include "wireless/bluetoothNew/include/btdefines.h"
#endif // IF_OLD_BLUETOOTH

#include "talk/talklogic/include/modtalklogic.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "configparser/modconfigparser.h"
#include "configiddefine.h"
#include "dsskey/include/dsskey_enumtypes.h"
#include "extended/bluetoothmessagedefine.h"
#include "extended/usbmessagedefine.h"
#include "adapterbox/include/modadapterbox.h"
#include "exp/include/exp_struct.h"

#define GET_ACTIVE_SESSION \
VoiceSessionPtr pSession = VoiceManager.GetActiveSession().lock();\
if (pSession.IsEmpty())\
{\
    return;\
}\
pSession

// 是否呼叫中心模式
bool voice_IsCallCenterMode()
{
#ifdef IF_FEATURE_DUALHEADSET
    return configParser_GetConfigInt(kszHeadsetTrainingSwitch) != 0;
#else
    return FALSE;
#endif
}

// 初始化
void voice_Init()
{
    VoiceManager.InitVoice();
}

// 摘挂机一定会更改摘挂状态
void voice_ChangeHandsetStatus(int iKey)
{
    VoiceManager.ChangeHandsetStatus(iKey);
}

// 手柄摘挂处理
bool voice_OnKeyPress(int iKey)
{
    if (voice_ProcessChannelKeyAccess(iKey))
    {
        return true;
    }

    return VoiceManager.OnKeyPress(iKey);
}

// 播放按键音
void voice_PlayKeyTone(int iKey, bool bPlayNow/* = false*/)
{
    if (!voice_IsChannelKey(iKey))
    {
        VoiceManager.PlayDTMF(iKey, bPlayNow);
    }
}

// 播放信号音
void voice_PlayTone(int iToneType, int iAutoStopTime/* = 0*/,
                    CHANNEL_MODE eChannel/* = CHANNEL_AUTO*/, int iStrategy/* = 1*/,
                    bool bVpmTimer /* = false */)
{
    GET_ACTIVE_SESSION->PlayTone(iToneType, iAutoStopTime, eChannel, iStrategy);
}

// 停止信号音
void voice_StopTone(int iToneType/* = -1*/)
{
    // 取消调试信息以免拨号界面快速拨号变慢
    GET_ACTIVE_SESSION->StopTone(iToneType);
}

// 设置是否静音
void voice_SetMute(int iCallID, bool bMute)
{
    GET_ACTIVE_SESSION->SetMute(iCallID, bMute);
}

// 播放DTMF音(local)
void voice_PlayLocalDTMF(int iKey)
{
    VoiceManager.PlayLocalDTMF(iKey);
}

// 发送remote DTMF
void voice_SendRemoteDTMF(int iCallID, int iKey, DTMF_DIR eDtmfDir/* = DTMF_DIR_TOBOTH*/)
{
    GET_ACTIVE_SESSION->SendRemoteDTMF(iCallID, iKey, eDtmfDir);
}

// 播放铃声
void voice_PlayRing(LPCSTR lpszRingFile, LPCSTR lpszDefaultRingFlieName/* = "Ring1.wav"*/)
{
    //VOICE_INFO("voice_PlayRing lpszRingFile=%s lpszRingPath=%s", lpszRingFile, lpszRingPath);
    GET_ACTIVE_SESSION->PlayRing(lpszRingFile, lpszDefaultRingFlieName);
}

// 通过绝对路径播放铃声
void voice_PlayRingFromFilePath(LPCSTR lpszRingPath)
{
    if (lpszRingPath == NULL)
    {
        return;
    }

    GET_ACTIVE_SESSION->PlayRingFromFilePath(lpszRingPath);
}

// 播放默认铃声
void voice_PlayDefaultRing()
{
    GET_ACTIVE_SESSION->PlayDefaultRing();
}

// 停止播放铃声
void voice_StopRing(bool bForceStopEHS /*= TRUE*/)
{
    GET_ACTIVE_SESSION->StopRing(FALSE, bForceStopEHS);
}

// 在菜单中停止播放铃声
void voice_StopRingInMenu()
{
    GET_ACTIVE_SESSION->StopRing(TRUE, TRUE);
}

// 开启通话
void voice_StartTalk(int iCallID, const ipvp_speech_t& talkParam)
{
    //VOICE_INFO("voice_StartTalk iCallID=%d", iCallID);

    GET_ACTIVE_SESSION->StartTalk(iCallID, &(const_cast<ipvp_speech_t&>(talkParam)));
}

// 停止通话
void voice_StopTalk(int iCallID)
{
    GET_ACTIVE_SESSION->StopTalk(iCallID);
}

#ifdef IF_BT_SUPPORT_PHONE
void voice_StartBtTalk()
{
    GET_ACTIVE_SESSION->StartBtTalk();
}

void voice_StopBtTalk()
{
    GET_ACTIVE_SESSION->StopBtTalk();
}

void voice_SetBtFarMute(int iCallID, bool bFarMute)
{
    GET_ACTIVE_SESSION->SetBtFarMute(iCallID, bFarMute);
}
#endif // IF_BT_SUPPORT_PHONE

// 重置通话参数
void voice_ResetTalk(int iCallID, const ipvp_speech_t& talkParam)
{
    //VOICE_INFO("voice_ResetTalk iCallID=%d", iCallID);
    GET_ACTIVE_SESSION->ResetTalk(iCallID, &(const_cast<ipvp_speech_t&>(talkParam)));
}

// 设置远端静音
void voice_SetFarMute(int iCallID, const ipvp_speech_t& talkParam, bool bFarMute)
{
    //VOICE_INFO("[voice_SetFarMute] iCallID=%d bFarMute=%d", iCallID, bFarMute);
    GET_ACTIVE_SESSION->SetFarMute(iCallID, &(const_cast<ipvp_speech_t&>(talkParam)), bFarMute);
}

// 更改通话中hold的状态
void voice_Hold(int iCallID, bool bHold/* = true*/)
{
    GET_ACTIVE_SESSION->SetCallHold(iCallID, bHold);
}

// 建立会议
void voice_StartConf(int iCallSum, int* pCallIDList, bool isVideoConf)
{
    //VOICE_INFO("voice_StartConf iCallSum=%d", iCallSum);
    GET_ACTIVE_SESSION->StartConf(iCallSum, pCallIDList, isVideoConf);
}

// 停止会议
void voice_StopConf(int iCallSum, int* pCallIDList, bool isVideoConf)
{
    //VOICE_INFO("voice_StopConf iCallSum=%d", iCallSum);
    GET_ACTIVE_SESSION->StopConf(iCallSum, pCallIDList, isVideoConf);
}

// 手动打开通道
void voice_SwitchChannel(CHANNEL_MODE eMode/* = CHANNEL_AUTO*/)
{
    //VOICE_INFO("voice_SwitchChannel eMode=%d", eMode);
    GET_ACTIVE_SESSION->SwitchChannel(eMode);
}

// 获取当前通道类型
CHANNEL_MODE voice_GetCurrentChannel()
{
    VoiceSessionPtr pSession = VoiceManager.GetActiveSession().lock();
    if (!pSession.IsEmpty())
    {
        return pSession->GetCurrentChannel();
    }
    return CHANNEL_NULL;
}

void voice_SetCurrentChannel(CHANNEL_MODE eMode, bool bNotify/* = false*/)
{
    GET_ACTIVE_SESSION->SetCurrentChannel(eMode, bNotify);
}

//  重置声音通道
void voice_ResetChannel()
{
    VoiceManager.ResetChannel();
}

// 是否正在播放铃声
bool voice_IsPlayingRing()
{
    VoiceSessionPtr pSession = VoiceManager.GetActiveSession().lock();
    if (!pSession.IsEmpty())
    {
        return pSession->IsPlayingRing();
    }
    return false;
}

// 是否耳麦响铃
bool voice_IsRingHeadset()
{
    // 是否耳麦响铃
    return VoiceManager.IsRingHeadset();
}

// 获取手柄的状态
bool voice_GetHandsetStatus()
{
    return VoiceManager.m_objDevices.GetHandsetStatus();
}

// 获取免提
bool voice_GetHandfreeStatus()
{
    return VoiceManager.m_objDevices.GetHandfreeStatus();
}

// 获取耳麦状态
bool voice_GetHeadsetStatus()
{
    return VoiceManager.m_objDevices.GetHeadsetStatus();
}

// 判断输入按键是否通道键
bool voice_IsChannelKey(int iKey)
{
    return (iKey == PHONE_KEY_HANDSET_ON_HOOK || iKey == PHONE_KEY_HANDSET_OFF_HOOK
            || iKey == PHONE_KEY_HANDFREE || iKey == PHONE_KEY_HEADSET_CTRL
            || iKey == PHONE_KEY_EHS_OPEN || iKey == PHONE_KEY_EHS_CLOSE
            || iKey == PHONE_KEY_LINEDEVICE_ON_HOOK || iKey == PHONE_KEY_LINEDEVICE_OFF_HOOK);
}

// 设置指定类型及大小的音量
void voice_SetVolume(int iVolume, VOLUME_TYPE eType/* = VT_AUTO*/, bool bWriteToFile/* = FALSE*/)
{
    VoiceManager.m_objVolume.SetVolume(iVolume, eType, bWriteToFile);
}

// 获取指定类型的音量
int voice_GetVolume(VOLUME_TYPE eType/* = VT_AUTO*/)
{
    return VoiceManager.m_objVolume.GetVolume(eType);
}

// 查询EHS是否接上
bool voice_IsEHSInserted()
{
    return VoiceManager.m_objDevices.EHSGetIndex() != -1;
}

// 获取EHS在EXP模块中的序号
int voice_GetEHSIndex()
{
    return VoiceManager.m_objDevices.EHSGetIndex();
}

// 调整EHS的状态，如果耳麦打开则打开，否则关闭
void voice_SwitchEHSStatus()
{
    VoiceManager.m_objDevices.EHSSwitchStatus();
}

// 关闭无线耳麦
void voice_CloseEHS()
{
    VoiceManager.m_objDevices.EHSClose();
}

// 开启无线耳麦
void voice_OpenEHS()
{
    if (VoiceManager.m_objDevices.GetHeadsetStatus())
    {
        VoiceManager.m_objDevices.EHSOpen();
    }
}

// 在menu菜单中播放dtmf
bool voice_PlayDTMFInMenu(int iKeyCode, bool bPlayNow/* = false*/)
{
    // 存在话路时从其他模块不允许播放按键音
    if (talklogic_SessionExist())
    {
        return FALSE;
    }

    VoiceManager.PlayDTMF(iKeyCode, bPlayNow);
    return TRUE;
}

bool voice_ProcessChannelKeyAccess(int iKey)
{
    if (!voice_IsChannelKey(iKey))
    {
        return false;
    }

    // 测试模式通道键要放开
    if (AdapterBox_IsTestMode())
    {
        return false;
    }

    bool bForbidden = false;
    CHANNEL_MODE eMode = CHANNEL_NULL;

    if (iKey == PHONE_KEY_HANDSET_ON_HOOK
            || iKey == PHONE_KEY_HANDSET_OFF_HOOK)
    {
        bForbidden = !VoiceManager.m_bHandSetEnable;
        eMode = CHANNEL_HANDSET;
    }
    else if (iKey == PHONE_KEY_HANDFREE)
    {
        bForbidden = !VoiceManager.m_bHandFreeEnable;
        eMode = CHANNEL_HANDFREE;
    }
    else if (iKey == PHONE_KEY_HEADSET_CTRL
             || iKey == PHONE_KEY_EHS_OPEN
             || iKey == PHONE_KEY_EHS_CLOSE)
    {
        bForbidden = !VoiceManager.m_bHeadSetEnable;
        eMode = CHANNEL_HEADSET;
    }

    //消息通知出去
    if (bForbidden
            && (iKey != PHONE_KEY_HANDSET_ON_HOOK)) //挂手柄时不需要通知
    {
        etl_NotifyApp(FALSE, VOICE_CHANNEL_FORBDDEN, eMode, 0);
    }

    return bForbidden;
}

#ifdef IF_SUPPORT_LINE_DEVICE
// 设置多模通道
bool voice_SetLineDeviceChannel(int iSlaveMode)
{
    return VoiceManager.SetLineDeviceChannel(iSlaveMode);
}
#endif // IF_SUPPORT_LINE_DEVICE

bool voice_IsVolumeCanChange()
{
    return VoiceManager.m_objVolume.IsVolumeCanChange();
}

void voice_ResetTalkVolume()
{
    VoiceManager.m_objVolume.ResetTalkVolume();
}

#ifdef IF_SUPPORT_BLUETOOTH
//蓝牙模式是否可用
bool voice_IsBTChannelAvaliable()
{
    return HEADSET_BT == VoiceManager.GetHeadsetChannelAvailableType();
}

// 是否是蓝牙模式
bool voice_IsBluetoothMode()
{
    return VoiceManager.IsBluetoothMode();
}

// 开启蓝牙通道
void voice_OpenBluetoothChannel()
{
    VoiceManager.SetHeadsetDeviceChannel(true);
}

// 关闭蓝牙通道
void voice_CloseBluetoothChannel()
{
    VoiceManager.SetHeadsetDeviceChannel(false);
}
#endif // IF_SUPPORT_BLUETOOTH

#ifdef IF_SUPPORT_USB_AUDIO
// 是否是usb耳机通道
bool voice_IsUSBHeadsetMode()
{
    return HEADSET_USB == VoiceManager.m_objDevices.GetHeadsetDeviceType()
           && (RC_HEADSET == _VPMMSGHandler.GetRealChannel()
               || RC_2HEADSET == _VPMMSGHandler.GetRealChannel());
}

bool voice_IsUSBHeadsetChannelAvaliable()
{
    return HEADSET_USB == VoiceManager.GetHeadsetChannelAvailableType(false);
}
#endif // IF_SUPPORT_USB_AUDIO

#if defined(IF_SUPPORT_BLUETOOTH) || defined(IF_SUPPORT_USB_AUDIO)
// 是否是耳机模式
bool voice_IsHeadsetDeviceMode()
{
    return VoiceManager.IsHeadsetDeviceMode();
}

// 开启耳机通道
void voice_OpenHeadsetDeviceChannel()
{
    VoiceManager.SetHeadsetDeviceChannel(true);
}

// 设置耳机连接状态
bool voice_SetHeadsetDevice(int iType, bool bOpen)
{
    HEADSET_TYPE eType = (HEADSET_TYPE)iType;
    if (HEADSET_NORMAL == eType)
    {
        return false;
    }

    VoiceManager.m_objDevices.PopDevice(eType);
    VoiceManager.SetHeadsetDevice(eType, bOpen);
    return true;
}
#endif //#if defined(IF_SUPPORT_BLUETOOTH) || defined(IF_SUPPORT_USB_AUDIO)

// 播放远端声音(手机音乐)
void voice_PlayA2dp(bool bPlay)
{
    VoiceManager.PlayA2dp(bPlay);
}


bool voice_IsTheMinVolume()
{
    return VoiceManager.m_objVolume.GetVolume(VT_RING) == MIN_LOGIC_VOLUME;
}

// 注册/注销回调函数
void voice_InitUpdateChannelInfoCallBack(FunUpdateChannelInfo pCallBack, bool bRegister)
{
    VoiceManager.InitUpdateChanelInfo(pCallBack, bRegister);
}
