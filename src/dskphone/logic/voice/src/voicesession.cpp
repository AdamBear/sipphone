#include "voicesession.h"

#include <dsklog/log.h>
#include <talk/talklogic/include/modtalklogic.h>
#ifdef IF_SUPPORT_USB_AUDIO
#include <usb_headset/include/modusbheadset.h>
#endif //IF_SUPPORT_USB_AUDIO
#ifdef IF_SUPPORT_BLUETOOTH
#include <wireless/bluetoothNew/include/modbluetooth.h>
#endif // IF_SUPPORT_BLUETOOTH

#include "modvoice.h"
#include "voicemanager.h"
#include "vpmmsghandler.h"
#include "cbasechannelmode.h"
#include "cautochannelmode.h"
#include "cselectchannelmode.h"
#include "cidlechannelmode.h"
#include "cheadsetchannelmode.h"
#include "cgroupchannelmode.h"
#include "cringchannelmode.h"
#include "channelforresume.h"

CVoiceSession::CVoiceSession(CHANNEL_MODE eChannel)
    : m_bPaused(true)
    , m_eCurrentChannel(CHANNEL_AUTO)
    , m_eGroupType(GROUP_HANDSET)
    , m_bPlayTone(false)
    , m_bPlayRing(0)
    , m_iPlayToneType(TONE_NONE)
    , m_pChannelMode(NULL)
{
    VOICE_INFO("Voice session [%p] created", this);
    SwitchChannel(eChannel);
}

CVoiceSession::~CVoiceSession()
{
    VOICE_INFO("Voice session [%p] release...", this);
    VoiceManager.FreeSession(this);
    // Session栈为空，则切换为NONE通道
    if (VoiceManager.IsSessionStackEmpty())
    {
        VOICE_INFO("Voice session stack is empty.");
        _VPMMSGHandler.OpenChannel(RC_INIT);
    }
    SAFE_DELETE(m_pChannelMode);
}

void CVoiceSession::Activate()
{
    VoiceManager.ActivateSession(this);
}

void CVoiceSession::Resume()
{
    if (!m_bPaused)
    {
        VOICE_NOTICE("Already resumed");
        return;
    }
    m_bPaused = false;
    SwitchChannel(GetCurrentChannel());
}

void CVoiceSession::Pause()
{
    if (m_bPaused)
    {
        VOICE_NOTICE("Already paused");
        return;
    }
    m_bPaused = true;
}

void CVoiceSession::PrintVoiceStatus() const
{
    VOICE_INFO("Current voice session[%p] channel mode[%d] play ring[%d] play tone[%d]",
               this, GetCurrentChannel(), IsPlayingRing(), IsPlayingTone());
    VOICE_INFO("Current real channel [%d]", _VPMMSGHandler.GetRealChannel());
    VOICE_INFO("HandFree[%d] HandSet[%d] HeadSet[%d]",
               VoiceManager.m_objDevices.GetHandfreeStatus(),
               VoiceManager.m_objDevices.GetHandsetStatus(),
               VoiceManager.m_objDevices.GetHeadsetStatus());
}

void CVoiceSession::SwitchChannel(CHANNEL_MODE eMode /*= CHANNEL_AUTO*/, bool bOpenEHS /*= false*/, bool bForeOpenGroup /*= false*/)
{
    VOICE_DBG("CVoiceManager::SwitchChannel mode[%d]", eMode);
#if 0
    CHANNEL_MODE eNewMode = eMode;

    if (NULL != VoiceManager.m_funUpdateChannelInfo)
    {
        VoiceManager.m_funUpdateChannelInfo(eNewMode);
    }

    //Step 1. 获取话机通道处理类
    CBaseChannelMode * pChannelMode = VoiceManager.GetChannelModePtr(eNewMode, bOpenEHS, bForeOpenGroup);
    if (NULL == pChannelMode)
    {
        return;
    }

    //Step 2. 获取物理通道
    REAL_CHANNEL eRealChannel = pChannelMode->GetRealChannelParam();

    //Step 3. 获取权限
    CHANNEL_MODE eNewChannelMode = GetChannelAccess(eNewMode, eRealChannel);

    VOICE_DBG("try mode[%d] access mode[%d] try realchannel[%d]", eNewMode, eNewChannelMode,
              eRealChannel);

    //Step 4. 切换通道
    if (eNewChannelMode == eNewMode
            || CHANNEL_RING == eNewMode) // 铃声通道不需要判断权限
    {
        pChannelMode->SwitchChannel(eRealChannel);

        ////Step 5. 释放资源,保存新通道
        SAFE_DELETE(m_pChannelMode);
        m_pChannelMode = pChannelMode;
    }
    else
    {
        SAFE_DELETE(pChannelMode);
        SwitchChannel(eNewChannelMode, bOpenEHS, bForeOpenGroup);
    }
    VOICE_INFO("after switch channel[%d]", eNewChannelMode);
#else
    OpenRealChannel(GetVPMChannel(eMode));
#endif
    PrintVoiceStatus();
}

CHANNEL_MODE CVoiceSession::GetChannelAccess(const CHANNEL_MODE eMode, REAL_CHANNEL eChannel)
{
    if (!VoiceManager.IsChannelFobidden(eChannel))
    {
        return eMode;
    }

    CHANNEL_MODE eNewMode = eMode;

    // 免提关闭
    if ((RC_HANDFREE == eChannel
            || RC_SPEAKER_ONLY == eChannel
            || RC_MIC_ONLY == eChannel
            || RC_GROUP_HANDSET == eChannel
            || RC_GROUP_HEADSET == eChannel)
            && VoiceManager.IsChannelFobidden(eChannel)
       )
    {
        if (VoiceManager.IsChannelFobidden(RC_HANDSET) && VoiceManager.m_objDevices.GetHandsetStatus())
        {
            eNewMode = CHANNEL_HANDSET;
        }
        else if (VoiceManager.IsChannelFobidden(RC_HEADSET))
        {
            // TODO 这里需要细分(蓝牙、EHS、耳麦、以及后续可能存在的USB HEADSET)
            eNewMode = CHANNEL_HEADSET;
        }
        else
        {
            eNewMode = CHANNEL_NULL;
        }
    }
    else if ((RC_2HEADSET == eChannel
              || RC_HEADSET == eChannel
              || RC_GROUP_HEADSET == eChannel)
             && VoiceManager.IsChannelFobidden(eChannel))
    {
        eNewMode = CHANNEL_NULL;
    }
    else if (RC_HANDSET == eChannel
             && VoiceManager.IsChannelFobidden(eChannel))
    {
        eNewMode = CHANNEL_NULL;
    }

    VOICE_INFO("Get Channel Access eChannel[%d] eMode[%d] eNewMode[%d]", eChannel, eMode, eNewMode);

    return eNewMode;
}

void CVoiceSession::OpenRealChannel(REAL_CHANNEL eChannel)
{
    VOICE_DBG("CVoiceManager::OpenRealChannel [%d]", eChannel);
    _VPMMSGHandler.OpenChannel(eChannel);
    // 通道切换后重新设置音量
    VoiceManager.m_objVolume.SetVolume();
}

CHANNEL_MODE CVoiceSession::GetCurrentChannel() const
{
    return m_eCurrentChannel;
}

void CVoiceSession::SetCurrentChannel(CHANNEL_MODE eChannel, bool bNotify /*= true*/)
{
    if (m_eCurrentChannel == eChannel)
    {
        return;
    }

    CHANNEL_MODE eOldChannel = m_eCurrentChannel;
    m_eCurrentChannel = eChannel;

    if (bNotify)
    {
        // 通知外部声音通道发生改变
        etl_NotifyApp(true, VOICE_CHANGE_CHANNEL, m_eCurrentChannel, eOldChannel);
    }
}

CVoiceSession::GROUP_TYPE CVoiceSession::GetGroupType() const
{
    return m_eGroupType;
}

void CVoiceSession::SetGroupType(GROUP_TYPE eGroupType)
{
    m_eGroupType = eGroupType;
}

bool CVoiceSession::IsPlayingTone() const
{
    return m_bPlayTone;
}

bool CVoiceSession::IsPlayingRing() const
{
    return m_bPlayRing;
}

yl::string CVoiceSession::GetLastRingFile() const
{
    return m_strLastRingFile;
}

void CVoiceSession::PlayTone(int iToneType, int iAutoStopTime /*= 0*/,
                             CHANNEL_MODE eChannel /*= CHANNEL_AUTO*/, int iStrategy /*= 1*/)
{
    // 播放TONE音之前，先停止dtmf 播放定时器
    VoiceManager.StopPlayLocalDTMF();

    // 如果已经在播放tone音，则先停止
    bool bNeedReplayTone = TRUE;
    if (m_bPlayTone)
    {
        if (m_iPlayToneType == iToneType)
        {
            if (eChannel == CHANNEL_AUTO
                    || eChannel == m_eCurrentChannel)
            {
                // 通道不变则不再重复播放
                return;
            }
            else
            {
                // 和当前播放的tone相同，则只更改通道和音量，不再重新播放
                bNeedReplayTone = FALSE;
            }
        }
        else
        {
            StopTone();
        }
    }

    bool bSwitchChannel = TRUE;
    bool bSwitchChannelToToneOnIlde = FALSE;
    // 通话中播放信号音使用通话音量
    if (m_eCurrentChannel == eChannel)
    {
        bSwitchChannel = FALSE;
    }
    else
    {
        if (eChannel == CHANNEL_AUTO)
        {
            if (!talklogic_SessionExist())
            {
                bSwitchChannelToToneOnIlde = TRUE;
            }
            else
            {
                bSwitchChannel = FALSE;
            }
        }
    }

    if (bSwitchChannel)
    {
        // 记录播放tone之前的通道
        _ChannelResume.SetChannelBeforeTone(m_eCurrentChannel);

        if (bSwitchChannelToToneOnIlde)
        {
            // Idle状态用Auto通道播放，则优先用耳麦，否则用免提
            SwitchChannel(CHANNEL_TONE_IDLE);
        }
        else
        {
            // 打开对应的通道
            SwitchChannel(eChannel);
        }
    }
    else
    {
        // 清空播放tone之前的通道记录
        _ChannelResume.SetChannelBeforeTone(CHANNEL_AUTO);
    }

    m_bPlayTone = TRUE;

    // 设置音量
    VoiceManager.m_objVolume.SetVolume();

    if (bNeedReplayTone)
    {
        // 记录当前tone类型
        m_iPlayToneType = iToneType;
        _VPMMSGHandler.PlayTone(iToneType, iStrategy, iAutoStopTime);
        if (iAutoStopTime > 0)
        {
            StopTone(iToneType); // 需要马上调用停止接口，播放时长由VPM控制
        }
    }
}

void CVoiceSession::StopTone(int iToneType /*= -1*/)
{
    if (!m_bPlayTone)
    {
        return;
    }

    // 只停止类型为iToneType的信号音
    if (iToneType != -1 && m_iPlayToneType != iToneType)
    {
        return;
    }

    // 发消息给VPM停止
    _VPMMSGHandler.StopTone();

    m_bPlayTone = FALSE;
    m_iPlayToneType = -1;

    // 停止tone之后重置通道
    _ChannelResume.ResumeAfterTone();
}

void CVoiceSession::PlayRing(LPCSTR lpszRingFile, LPCSTR lpszDefaultRingFlieName)
{
    if (lpszRingFile == NULL)
    {
        return;
    }

    if (pathFileExist(lpszRingFile))
    {
        // 传过来是绝对路径，则直接播放
        PlayRingFromFilePath(lpszRingFile);
    }
    else
    {
        yl::string strRingFile = commonUnit_SearchRingPath(lpszRingFile, lpszDefaultRingFlieName);
        VOICE_INFO("Ring file:%s", strRingFile.c_str());
        PlayRingFromFilePath(strRingFile.c_str());
    }
}

void CVoiceSession::PlayRingFromFilePath(LPCSTR lpszRingPath)
{
    if (lpszRingPath == NULL)
    {
        return;
    }

    // 静音铃声不需要给耳麦和蓝牙耳机播放
    bool bSilenceRing = (commonAPI_StringMatch(lpszRingPath, "Silent.wav", false, CONTAIN_MATCH)
                         || commonAPI_StringMatch(lpszRingPath, "Silence.wav", false, CONTAIN_MATCH));

    // 播放铃声时也要通知EHS播放
    if (!bSilenceRing)
    {
        // 当前界面是响铃界面，且播放铃声时才通知EHS响铃
        if (talklogic_IsAllSessionRinging())
        {
#if IF_BUG_27190
            if (IsRingHeadset() && configParser_GetConfigInt(kszEHSPlayPhoneRingSwitch) != 0)
            {
                VoiceManager.m_objDevices.EHSPlayPhoneRing();
            }
            else
            {
                //耳机响自带铃声前，先进行close操作，确保此时耳机是关闭的
                //否则耳机开启的时候，进行响自带铃多款耳机会响铃错误，有的是既响话机铃又响耳机铃，有的只响话机铃；
                VoiceManager.m_objDevices.EHSClose();
                VoiceManager.m_objDevices.EHSPlayRing();
            }
#else
            VoiceManager.m_objDevices.EHSPlayRing();
#endif
        }
    }

    if (m_bPlayRing)
    {
        // 如果和上次播放的铃声相同，则不再重新播放
        if (m_strLastRingFile == lpszRingPath)
        {
            return;
        }
        else
        {
            StopRing();
        }
    }

    // 记录播放Ring之前的通道
    _ChannelResume.SetChannelBeforeRing(m_eCurrentChannel);

    //// 打开相应通道
    SwitchChannel(CHANNEL_RING);

    yl::string strRingPath = lpszRingPath;

    // 记录最近一次播放铃声的类型
    m_strLastRingFile = strRingPath;

    // 应VPM要求，在播放铃声前需要mute解决铃声忽大忽小的问题
    if (!talklogic_GetMuteStatus())
    {
        SetMute(-1, TRUE);
    }

    bool bIsBtHsPlaying = false;
    if (talklogic_IsAllSessionRinging())
    {
#ifdef IF_SUPPORT_USB_AUDIO
        if (HEADSET_USB == VoiceManager.m_objDevices.GetHeadsetDeviceType())
        {
            // 播放USB耳机铃声
            USBHeadset_PlayRing();
        }
#endif //IF_SUPPORT_USB_AUDIO

#ifdef IF_SUPPORT_BLUETOOTH
        if (HEADSET_BT == VoiceManager.m_objDevices.GetHeadsetDeviceType() && !bSilenceRing)
        {
            // 播放蓝牙耳机铃声
            bIsBtHsPlaying = Bluetooth_HSPlayRing();
        }
#endif // IF_SUPPORT_BLUETOOTH
    }

    // 响一声铃声，传给VMP参数为0
    bool bSplash = commonAPI_StringMatch(lpszRingPath, "splash.wav", false);

    m_bPlayRing = TRUE;

    bool bNeedPlay = true;
#ifdef IF_SUPPORT_BLUETOOTH
    // RING_DEVICE 为RD_HEADSET时, 不需要播放话机铃声
    // PlayRing()之后,话机铃声数据会播放到耳机, 造成干扰 http://10.2.1.199/Bug.php?BugID=92770
    if (RD_HEADSET == (RING_DEVICE)configParser_GetConfigInt(kszRingerDevice)
            && VoiceManager.IsBluetoothMode() && bIsBtHsPlaying)
    {
        bNeedPlay = false;
    }
#endif // IF_SUPPORT_BLUETOOTH

#ifdef IF_SUPPORT_USB_AUDIO
    // 耳麦不响话机铃声
    if (voice_IsUSBHeadsetMode()
            && RD_HEADSET == (RING_DEVICE)configParser_GetConfigInt(kszRingerDevice))
    {
        bNeedPlay = false;
    }
#endif

    //VOICE_INFO("CVoiceManager::PlayRing: strRingFile=[%s]", strRingPath.c_str());
    if (strRingPath.size() > 99)
    {
        // 铃声路径太长会导致vpm出错
        PlayDefaultRing();
    }
    else
    {
        // 1代表只响1遍，-1代表循环响
        //http://bugfree.yealink.com/Bug.php?BugID=70634
        // todo t49
        PlayRingStart(strRingPath, bSplash, bNeedPlay);
    }

    // 播放铃声之后再设置一次铃声音量（CVolumeManager::ProcessZeroVolumeOfRing）
    VoiceManager.m_objVolume.SetVolume();
}

void CVoiceSession::PlayDefaultRing()
{
    yl::string strRingPath;
    commonUnits_GetDefaultRing(strRingPath);
    PlayRingFromFilePath(strRingPath.c_str());
}

void CVoiceSession::PlayRingStart(yl::string strRingPath, bool bLoop /*= false*/, bool bNeedPlay /*= true*/)
{
    if (strRingPath.empty())
    {
        return;
    }

    _VPMMSGHandler.SetRealPlayRing(true);

    if (bNeedPlay)
    {
        _VPMMSGHandler.PlayRing(strRingPath, bLoop ? 1 : -1);
    }
}

void CVoiceSession::StopRing(bool bInMenu /*= FALSE*/, bool bForceStopEHS /*= TRUE*/)
{
    if (!m_bPlayRing)
    {
        return;
    }

    if (bInMenu)
    {
        // 如果在菜单中停止播放铃声，则需要判断当前是否存在来电响铃界面
        if (talklogic_IsAllSessionRinging())
        {
            return;
        }
    }

    // 停止铃声时也要通知EHS停止
    // 强制关闭EHS,需要根据当前EHS状态发送开关命令,如果为当前为开则发送打开命令,否则发送关闭
    // 耳麦关闭的情况下需要强制关闭EHS响铃
    VoiceManager.m_objDevices.EHSStopRing();
    //m_objDevices.EHSStopRing(!m_bHeadset);

    // 应VPM要求，在停止铃声前需要unmute解决铃声忽大忽小的问题
    if (!talklogic_GetMuteStatus())
    {
        SetMute(-1, FALSE);
    }

#ifdef IF_SUPPORT_BLUETOOTH
    if (HEADSET_BT == VoiceManager.m_objDevices.GetHeadsetDeviceType())
    {
        // 蓝牙耳机停止铃声
        Bluetooth_HSStopRing();
    }
#endif // IF_SUPPORT_BLUETOOTH

#ifdef IF_SUPPORT_USB_AUDIO
    if (HEADSET_USB == VoiceManager.m_objDevices.GetHeadsetDeviceType())
    {
        // USB耳机停止响铃
        USBHeadset_StopRing();
    }
#endif // IF_SUPPORT_USB_AUDIO

    m_bPlayRing = FALSE;
    _VPMMSGHandler.StopRing();
    // 将上次播放的文件置空
    m_strLastRingFile = "";

    // 停止铃声之后重置通道
    _ChannelResume.ResumeAfterRing();
}

void CVoiceSession::StartTalk(int iCallID, ipvp_speech_t * pTalkParam)
{
    if (pTalkParam == NULL)
    {
        return;
    }

    VOICE_INFO("StartTalk iCallID=[0x%x]", iCallID);
#ifdef IF_BT_SUPPORT_PHONE
    if (talklogic_IsBluetoothSessionByCallID(iCallID))
    {
        _VPMMSGHandler.StartBtTalk(iCallID);
    }
    else
#endif // IF_BT_SUPPORT_PHONE
    {
        _VPMMSGHandler.StartTalk(iCallID, pTalkParam);
    }
    // 调整音量
    VoiceManager.m_objVolume.SetVolume(TRUE, iCallID);
}

void CVoiceSession::StopTalk(int iCallID)
{
#ifdef IF_BT_SUPPORT_PHONE
    if (talklogic_IsBluetoothSessionByCallID(iCallID))
    {
        _VPMMSGHandler.StopBtTalk(iCallID);
    }
    else
#endif // IF_BT_SUPPORT_PHONE
    {
        _VPMMSGHandler.StopTalk(iCallID);
    }
}

void CVoiceSession::ResetTalk(int iCallID, void * pTalkParam)
{
#ifdef IF_BT_SUPPORT_PHONE
    if (talklogic_IsBluetoothSessionByCallID(iCallID))
    {
        _VPMMSGHandler.ResetBtTalk(iCallID);
    }
    else
#endif // IF_BT_SUPPORT_PHONE
    {
        _VPMMSGHandler.ResetTalk(iCallID, pTalkParam);

    }
    // 调整音量
    VoiceManager.m_objVolume.SetVolume(TRUE, iCallID);
}

#ifdef IF_BT_SUPPORT_PHONE
void CVoiceSession::StartBtTalk()
{
    _VPMMSGHandler.StartBtTalk(-1);
}

void CVoiceSession::StopBtTalk()
{
    _VPMMSGHandler.StopBtTalk(-1);
}

void CVoiceSession::SetBtFarMute(int iCallID, bool bFarMute)
{
    _VPMMSGHandler.SetBtFarMute(iCallID, bFarMute);
}
#endif // IF_BT_SUPPORT_PHONE

void CVoiceSession::SetMute(int iCallID, bool bMute)
{
    VOICE_INFO("SetMute iCallID=%d bMute=%d", iCallID, bMute);
    _VPMMSGHandler.SetMute(iCallID, bMute);
}

void CVoiceSession::SetFarMute(int iCallID, ipvp_speech_t * pTalkParam, bool bFarMute)
{
    _VPMMSGHandler.SetFarMute(iCallID, pTalkParam, bFarMute);
}

void CVoiceSession::StartConf(int iCallSum, int * pCallIDList, bool isVideoConf)
{
    if (iCallSum < 1 || pCallIDList == NULL)
    {
        return;
    }

    VOICE_INFO("StartConf callid:[%d][%d]", (int)pCallIDList[0], (int)pCallIDList[1]);
    _VPMMSGHandler.StartConf(iCallSum, pCallIDList, isVideoConf);
}

void CVoiceSession::StopConf(int iCallSum, int * pCallIDList, bool isVideoConf)
{
    if (iCallSum < 2 || pCallIDList == NULL)
    {
        return;
    }

    VOICE_INFO("StopConf");
    _VPMMSGHandler.StopConf(iCallSum, pCallIDList, isVideoConf);
}

void CVoiceSession::SetCallHold(int iCallID, bool bHold /*= true*/)
{
#ifdef IF_BT_SUPPORT_PHONE
    if (talklogic_IsBluetoothSessionByCallID(iCallID))
    {
        _VPMMSGHandler.SetBtCallHold(iCallID, bHold);
    }
    else
#endif // IF_BT_SUPPORT_PHONE
    {
        _VPMMSGHandler.SetCallHold(iCallID, bHold);
    }

    if (!bHold)
    {
        // 调整音量
        VoiceManager.m_objVolume.SetVolume(TRUE, iCallID);
    }
}

void CVoiceSession::SendRemoteDTMF(int iCallID, int iKey, DTMF_DIR eDtmfDir)
{
    VOICE_INFO("SendRemoteDTMF iKey=%d eDtmfDir=%d", iKey, eDtmfDir);
    _VPMMSGHandler.SendRemoteDTMF(iCallID, iKey, eDtmfDir);
}

CHANNEL_MODE CVoiceSession::AmendChannel(CHANNEL_MODE eChannel) const
{
    if (eChannel == CHANNEL_AUTO)
    {
        if (voice_IsCallCenterMode())
        {
            // 呼叫中心模式
            if (VoiceManager.m_objDevices.GetHeadsetStatus())
            {
                // 耳麦可用
                if (VoiceManager.m_objDevices.GetHandsetStatus())
                {
                    // 手柄摘起，则使用双耳麦通道
                    return CHANNEL_2HEADSET;
                }
                else
                {
                    // 手柄挂着，则使用耳麦通道
                    return CHANNEL_HEADSET;
                }
            }
            else
            {
                // 耳麦不可用，则使用免提
                return CHANNEL_HANDFREE;
            }
        }
        else
        {
            // 非呼叫中心模式
            if (VoiceManager.m_objDevices.GetHeadsetStatus())
            {
                // 耳麦可用，则使用耳麦
                return CHANNEL_HEADSET;
            }
            else if (VoiceManager.m_objDevices.GetHandfreeStatus())
            {
                // 免提开启，则使用免提
                return CHANNEL_HANDFREE;
            }
            else if (VoiceManager.m_objDevices.GetHandsetStatus())
            {
                // 手柄摘起，则使用手柄
                return CHANNEL_HANDSET;
            }
            else
            {
                // 容错：使用免提
                VOICE_WARN("All channel inactive, use handfree");
                return CHANNEL_HANDFREE;
            }
        }
    }
    if (eChannel == CHANNEL_RING)
    {
        RING_DEVICE eRingerDevice = (RING_DEVICE)configParser_GetConfigInt(kszRingerDevice);
        if (VoiceManager.m_objDevices.GetHeadsetStatus())
        {
            if (RD_HEADSET == eRingerDevice)
            {
                return CHANNEL_HEADSET;
            }
            else if (RD_HEADSET_GROUP == eRingerDevice)
            {
                return CHANNEL_GROUP;
            }
        }
        return CHANNEL_HANDFREE;
    }
    if (eChannel == CHANNEL_IDLE)
    {
        if (VoiceManager.m_objDevices.GetHeadsetStatus())
        {
            // 耳麦可用，则使用耳麦
            return CHANNEL_HEADSET;
        }
        else
        {
            // 否则统一使用手柄
            return CHANNEL_HANDSET;
        }
    }
    return eChannel;
}

REAL_CHANNEL CVoiceSession::GetVPMChannel(CHANNEL_MODE eChannel)
{
    // Step 1: 修正通道类型：AUTO等类型自动选择
    CHANNEL_MODE eAmendChannel = AmendChannel(eChannel);
    // Step 2: 处理通道权限
    if (eAmendChannel == CHANNEL_HANDFREE && !VoiceManager.m_bHandFreeEnable)
    {
        if (VoiceManager.m_bHandSetEnable && VoiceManager.m_objDevices.GetHandsetStatus())
        {
            eAmendChannel = CHANNEL_HANDSET;
        }
        else if (VoiceManager.m_bHeadSetEnable)
        {
            eAmendChannel = CHANNEL_HEADSET;
        }
        else
        {
            eAmendChannel = CHANNEL_NULL;
        }
    }
    else if (eAmendChannel == CHANNEL_HANDSET && !VoiceManager.m_bHandSetEnable)
    {
        eAmendChannel = CHANNEL_NULL;
    }
    else if (eAmendChannel == CHANNEL_HEADSET && !VoiceManager.m_bHeadSetEnable)
    {
        eAmendChannel = CHANNEL_NULL;
    }

    // 记录修正后生效的逻辑通道
    SetCurrentChannel(eAmendChannel);

    // Step 3: 处理声道（灯）激活状态
    switch (eAmendChannel)
    {
    case CHANNEL_HANDSET:
        VoiceManager.m_objDevices.SetHandfreeStatus(false);
        VoiceManager.m_objDevices.SetHeadsetStatus(false);
        break;
    case CHANNEL_HANDFREE:
    case CHANNEL_SPEAKER_ONLY:
    case CHANNEL_MIC_ONLY:
        VoiceManager.m_objDevices.SetHandfreeStatus(true);
        VoiceManager.m_objDevices.SetHeadsetStatus(false);
        break;
    case CHANNEL_HEADSET:
    case CHANNEL_2HEADSET:
    case CHANNEL_BTHEADSET:
        VoiceManager.m_objDevices.SetHandfreeStatus(false);
        VoiceManager.m_objDevices.SetHeadsetStatus(true);
        break;
    case CHANNEL_GROUP:
        VoiceManager.m_objDevices.SetHandfreeStatus(true);
        break;
    default:
        break;
    }
    if (eChannel == CHANNEL_RING && talklogic_IsAllSessionRinging())
    {
        VoiceManager.m_objDevices.SetHandfreeStatus(false);
    }

    // Step 4: 转换为VPM通道
    REAL_CHANNEL eRealChannel = _VPMMSGHandler.GetRealChannel();
    switch (eAmendChannel)
    {
    case CHANNEL_HANDFREE:
        // CP机型的免提通道应该修正为VPM_CHNN_CP
        if (devicelib_GetPhoneType() == PT_CP860 || devicelib_GetPhoneType() == PT_CP920)
        {
            eRealChannel = RC_HANDFREE; // VPM_CHNN_CP;
        }
        else
        {
            eRealChannel = RC_HANDFREE;
        }
        break;
    case CHANNEL_HANDSET:
        eRealChannel = RC_HANDSET;
        break;
    case CHANNEL_HEADSET:
        // 耳麦通道根据当前耳麦设备进行切换
        switch (VoiceManager.m_objDevices.GetHeadsetDeviceType())
        {
        case HEADSET_NORMAL:
            eRealChannel = RC_HEADSET;
            break;
        case HEADSET_BT:
            eRealChannel = /*talklogic_SessionExist() ? RC_BT_HEADSET :*/ RC_HEADSET;
            break;
        case HEADSET_USB:
            eRealChannel = /*talklogic_SessionExist() ? RC_USB_HEADSET :*/ RC_HEADSET;
            break;
        }
        break;
    case CHANNEL_GROUP:
        {
            if (IsPlayingRing())
            {
                // 响铃中的Group只有Headset Group(RingerDevice=Headset&Speaker)
                eRealChannel = RC_GROUP_HEADSET;
                break;
            }
            if (eRealChannel == RC_GROUP_HANDSET || eRealChannel == RC_GROUP_HEADSET)
            {
                break;
            }
            else if (eRealChannel == RC_HANDSET)
            {
                eRealChannel = RC_GROUP_HANDSET;
                break;
            }
            else if (eRealChannel == RC_HEADSET)
            {
                eRealChannel = RC_GROUP_HEADSET;
                break;
            }
            else if (VoiceManager.m_objDevices.GetHandsetStatus())
            {
                eRealChannel = RC_GROUP_HANDSET;
                break;
            }
        }
        break;
    case CHANNEL_2HEADSET:
        eRealChannel = RC_2HEADSET;
        break;
    }
    if (eChannel == CHANNEL_RING || eChannel == CHANNEL_IDLE)
    {
        SetCurrentChannel(eChannel, false);
    }
    return eRealChannel;
}
