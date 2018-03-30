#include "voicechannel_include.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "commonunits/modcommonunits.h"
#include "configparser/modconfigparser.h"
#include "configiddefine.h"
#include "usb_headset/include/modusbheadset.h"

CVolumeManager::CVolumeManager()
{
    m_eCurrentType = VT_AUTO;
    m_iCurrentVolume = -1;
}

CVolumeManager::~CVolumeManager()
{

}

// 获取指定类型的音量大小，VT_AUTO表示选择当前通道
int CVolumeManager::GetVolume(VOLUME_TYPE eType/* = VT_AUTO*/)
{
    if (eType == VT_AUTO)
    {
        eType = GetVolumeType();
    }

    int iVolume = GetSpkVolume(eType);

    // 纠正logic音量值
    ValidLogicVolume(iVolume);

    return iVolume;
}

// 设置当前音量
void CVolumeManager::SetVolume(bool bStartTalk /*= FALSE*/, int iCallID/* = -1*/)
{
    // 先获取当前音量类型
    VOLUME_TYPE eType = GetVolumeType(bStartTalk, iCallID);
    // 根据当前音量类型从配置中读取音量大小
    int iVolume = GetSpkVolume(eType);

    // 纠正logic音量值
    ValidLogicVolume(iVolume);

#if IF_FEATURE_PAGING
    //加强音量,范围改为在16~30
    if (talklogic_IsMulticastPage(iCallID)
            && configParser_GetConfigInt(kszMulticastEnhanceVolume) == 1
            && VoiceManager.m_objDevices.GetHandfreeStatus())
    {
        iVolume += ENHANCE_VOLUME;
    }
#endif
    VOICE_INFO("SetVolume iVolume=%d eType=%d", iVolume, eType);

    // 通知VPM更新
    SetVolumeToVPM(eType, iVolume);
}

// 更改音量(bWriteToFile表示是否写入配置中)
void CVolumeManager::SetVolume(int iVolume, VOLUME_TYPE eType, bool bWriteToFile/* = FALSE*/)
{
    if (eType == VT_AUTO)
    {
        eType = GetVolumeType();
    }
#if IF_FEATURE_PAGING
    if (eType == VT_TALK_PAGING)
    {
        return;
    }
#endif
    // 纠正logic音量值
    ValidLogicVolume(iVolume);

    if (bWriteToFile)
    {
        // 将调节之后的音量值写入配置中
        SetSpkVolume(eType, iVolume);
    }
#if IF_FEATURE_PAGING
    //加强音量,范围改为在16~30
    if (talklogic_IsMulticastPage()
            && configParser_GetConfigInt(kszMulticastEnhanceVolume) == 1
            && VoiceManager.m_objDevices.GetHandfreeStatus())
    {
        iVolume += ENHANCE_VOLUME;
    }
#endif
    VOICE_INFO("Set volume value[%d] type[%d]", iVolume, eType);

    // 通知VPM更新
    SetVolumeToVPM(eType, iVolume);
}

// 获取当前音量类型
VOLUME_TYPE CVolumeManager::GetVolumeType(bool bStartTalk /*= FALSE*/, int iCallID/* = -1*/)
{
    bool bTalkVolume = FALSE;
    bool bColorRingback = FALSE;
    bool bPaging = FALSE;
    // 获取Talk和ColorRingback状态
    talklogic_GetTalkAndColorRingbackStatus(bTalkVolume, bColorRingback, bPaging);

#if IF_FEATURE_PAGING
    if (iCallID != -1)
    {
        bPaging = talklogic_IsMulticastPage(iCallID);
    }

    if (bPaging)
    {
        if (!IsVolumeCanChange())
        {
            return VT_TALK_PAGING;
        }
    }
#endif

    // 如果是开启通话，则使用通话的音量
    if (!bTalkVolume && bStartTalk)
    {
        bTalkVolume = TRUE;
    }

    return GetVolumeTypeByChannel(bTalkVolume, bColorRingback);
}

// 根据通道判断音量类型
VOLUME_TYPE CVolumeManager::GetVolumeTypeByChannel(bool bTalk, bool bColorRingback)
{
#if 0 // T2x彩铃不单独处理
    if (bColorRingback)
    {
        // 彩铃的音量单独处理
        return VT_COLORRINGBACK;
    }
    else
#endif
    {
        bTalk |= bColorRingback;

        // http://10.2.1.199/Bug.php?BugID=39248
        // 如果全是响铃话路且正在播放铃声则返回铃声音量类型
        if (talklogic_IsAllSessionRinging()
                && voice_IsPlayingRing())
        {
            return VT_RING;
        }

        switch (voice_GetCurrentChannel())
        {
        case CHANNEL_RING:
            {
                return VT_RING;
            }
            break;
        case CHANNEL_GROUP:
            {
                return VT_GROUP;
            }
            break;
        case CHANNEL_HANDFREE:
        case CHANNEL_SPEAKER_ONLY:
        case CHANNEL_MIC_ONLY:
            {
                if (bTalk)
                {
                    return VT_TALK_HANDFREE;
                }
                else
                {
                    return VT_TONE_HANDFREE;
                }
            }
            break;
        case CHANNEL_HANDSET:
            {
                if (bTalk)
                {
                    return VT_TALK_HANDSET;
                }
                else
                {
                    return VT_TONE_HANDSET;
                }
            }
            break;
        case CHANNEL_HEADSET:
        case CHANNEL_2HEADSET:
            {
                if (bTalk)
                {
                    return VT_TALK_HEADSET;
                }
                else
                {
                    return VT_TONE_HEADSET;
                }
            }
            break;
        case CHANNEL_IDLE:
            {
                if (VoiceManager.m_objDevices.GetHeadsetStatus())
                {
                    return VT_TONE_HEADSET;
                }
                else
                {
                    VoiceSessionPtr pSession = VoiceManager.GetActiveSession().lock();
                    // idle播放信号音用免提tone音量
                    if (!pSession.IsEmpty() && pSession->IsPlayingTone())
                    {
                        return VT_TONE_HANDFREE;
                    }
                    else
                    {
                        return VT_TONE_HANDSET;
                    }
                }
            }
            break;
        case CHANNEL_AUTO:
            {
                if (VoiceManager.m_objDevices.GetHeadsetStatus())
                {
                    if (bTalk)
                    {
                        return VT_TALK_HEADSET;
                    }
                    else
                    {
                        return VT_TONE_HEADSET;
                    }
                }
                else if (VoiceManager.m_objDevices.GetHandsetStatus())
                {
                    if (bTalk)
                    {
                        return VT_TALK_HANDSET;
                    }
                    else
                    {
                        return VT_TONE_HANDSET;
                    }
                }
                else
                {
                    if (bTalk)
                    {
                        return VT_TALK_HANDFREE;
                    }
                    else
                    {
                        return VT_TONE_HANDFREE;
                    }
                }
            }
            break;
        default:
            break;
        }
    }

    VOICE_WARN("[VOICE] GetVolumeTypeByChannel, can't get correct channel!");
    return VT_TONE_HANDSET;
}

// 纠正逻辑音量值
void CVolumeManager::ValidLogicVolume(int & iVolume)
{
    if (iVolume < MIN_LOGIC_VOLUME)
    {
        iVolume = MIN_LOGIC_VOLUME;
    }
    else if (iVolume > MAX_LOGIC_VOLUME)
    {
        iVolume = MAX_LOGIC_VOLUME;
    }
}

// 纠正发送给VPM的音量值
void CVolumeManager::ValidVolumeToVPM(int & iVolume)
{
    if (iVolume < MIN_REAL_VOLUME)
    {
        iVolume = MIN_REAL_VOLUME;
    }
    else if (iVolume > MAX_REAL_VOLUME)
    {
        iVolume = MAX_REAL_VOLUME;
    }
}

// 调用VPM接口设置音量
void CVolumeManager::SetVolumeToVPM(VOLUME_TYPE eType, int iVolume)
{
    // 对铃声音量特殊处理
    if (eType == VT_RING)
    {
        ProcessZeroVolumeOfRing(iVolume);
    }

    // 如果设置新的音量类型及大小与当前类型及大小没有变化则不通知vpm
    if (m_eCurrentType == eType && m_iCurrentVolume == iVolume)
    {
        // 双耳麦<==>耳麦，需要重新设置音量
        if (m_eCurrentType != VT_TALK_HEADSET
                && m_eCurrentType != VT_TONE_HEADSET
#if IF_FEATURE_PAGING
                && m_eCurrentType != VT_TALK_PAGING
#endif
           )
        {
            VOICE_WARN("Volume[%d] type[%d] is same as before!", m_eCurrentType, m_iCurrentVolume);
            return;
        }
    }

    // 记录下最新的音量类型和大小
    m_eCurrentType = eType;
    m_iCurrentVolume = iVolume;

    if (iVolume <= 0 && eType != VT_RING)
    {
        iVolume = 0;
    }

    // 给vpm发消息
    _VPMMSGHandler.SetVolume(iVolume);
}

// 对播放铃声，铃声音量为0时的特殊处理
void CVolumeManager::ProcessZeroVolumeOfRing(int iLogicVolume)
{
    VoiceSessionPtr pSession = VoiceManager.GetActiveSession().lock();
    if (pSession.IsEmpty())
    {
        return;
    }
    // 音量为0，且正在播放铃声，则将铃声停止
    // （VPM上停止，逻辑上还在播放，为了不影响应用层逻辑判断）
    if (iLogicVolume == 0)
    {
        if (pSession->IsPlayingRing())
        {
            VOICE_INFO("ProcessZeroVolumeOfRing StopRing");
            _VPMMSGHandler.StopRing();
#if 0
#ifdef IF_SUPPORT_USB_AUDIO
            // http://10.2.1.199/Bug.php?BugID=100171
            if (voice_IsUSBHeadsetChannelAvaliable())
            {
                // USB耳机停止响铃
                USBHeadset_StopRing();
            }
#endif // IF_SUPPORT_USB_AUDIO
#endif
        }
    }
    else
    {
        // 物理上未播放铃声，当前铃声音量又大于0，则重新播放铃声
        if (pSession->IsPlayingRing() && !_VPMMSGHandler.IsRealPlayRing())
        {
            VOICE_INFO("ProcessZeroVolumeOfRing PlayRing");
            yl::string strRing = pSession->GetLastRingFile();
            if (strRing.size() > 99)
            {
                commonUnits_GetDefaultRing(strRing);
            }

#ifdef IF_SUPPORT_USB_AUDIO
            // USB耳麦时,不允许话机铃声播放到耳机
            if (voice_IsUSBHeadsetMode()
                    && RD_HEADSET == (RING_DEVICE)configParser_GetConfigInt(kszRingerDevice))
            {
                return;
            }
#endif

            pSession->PlayRingStart(strRing);
#if 0
#ifdef IF_SUPPORT_USB_AUDIO
            if (voice_IsUSBHeadsetChannelAvaliable())
            {
                // USB耳机响铃
                USBHeadset_PlayRing();
            }
#endif // IF_SUPPORT_USB_AUDIO
#endif
        }
    }
}

// 获取spk音量
int CVolumeManager::GetSpkVolume(VOLUME_TYPE eType, int iDefault/* = DEFALUT_VOL*/)
{
    yl::string strCfgName = "";
    switch (eType)
    {
    case VT_TALK_HANDSET:
        strCfgName = kszVolumeTalkOutputHandset;
        break;
    case VT_TALK_HEADSET:
        strCfgName = kszVolumeTalkOutputHeadset;
        break;
    case VT_TALK_HANDFREE:
        strCfgName = kszVolumeTalkOutputHandfree;
        break;
    case VT_TONE_HANDSET:
        strCfgName = kszVolumeToneHandset;
        break;
    case VT_TONE_HEADSET:
        strCfgName = kszVolumeToneHeadset;
        break;
    case VT_TONE_HANDFREE:
        strCfgName = kszVolumeToneHandfree;
        break;
    case VT_RING:
        {
            yl::string strForceRing = configParser_GetConfigString(kszForceVoiceRing);
            int nVoiceRing = ChangeForceVoice(strForceRing);
            if (nVoiceRing >= MIN_LOGIC_VOLUME)
            {
                return nVoiceRing;
            }
            strCfgName = kszVolumeRing;
        }
        break;
    case VT_GROUP:
        strCfgName = kszVolumeOutputGroup;
        break;
#if IF_FEATURE_PAGING
    case VT_TALK_PAGING:
        {
            return talklogic_GetPagingVolume();
        }
        break;
#endif
    default:
        break;
    }

    return configParser_GetConfigInt(strCfgName.c_str());
}

// 设置spk音量
void CVolumeManager::SetSpkVolume(VOLUME_TYPE eType, int iVolume)
{
    yl::string strCfgName = "";
    switch (eType)
    {
    case VT_TALK_HANDSET:
        strCfgName = kszVolumeTalkOutputHandset;
        break;
    case VT_TALK_HEADSET:
        strCfgName = kszVolumeTalkOutputHeadset;
        break;
    case VT_TALK_HANDFREE:
        strCfgName = kszVolumeTalkOutputHandfree;
        break;
    case VT_TONE_HANDSET:
        strCfgName = kszVolumeToneHandset;
        break;
    case VT_TONE_HEADSET:
        strCfgName = kszVolumeToneHeadset;
        break;
    case VT_TONE_HANDFREE:
        strCfgName = kszVolumeToneHandfree;
        break;
    case VT_RING:
        strCfgName = kszVolumeRing;
        break;
    case VT_GROUP:
        strCfgName = kszVolumeOutputGroup;
        break;
    default:
        break;
    }

    configParser_SetConfigInt(strCfgName.c_str(), iVolume, CONFIG_LEVEL_PHONE);
}

// 音量是否可以改变
bool CVolumeManager::IsVolumeCanChange()
{
#if IF_FEATURE_PAGING
    int iVolume = talklogic_GetPagingVolume();
    if (iVolume != INVALID_PAGING_VOLUME
            && VoiceManager.m_objDevices.GetHandfreeStatus())
    {
        return false;
    }
#endif

    CHANNEL_MODE eCurrentChannel = voice_GetCurrentChannel();
    yl::string strVoiceRing = configParser_GetConfigString(kszForceVoiceRing);
    if (ChangeForceVoice(strVoiceRing) >= MIN_LOGIC_VOLUME
            && (CHANNEL_IDLE == eCurrentChannel
                || CHANNEL_RING == eCurrentChannel
                || talklogic_IsFoucsInRingSession()))
    {
        VOICE_INFO("Current channel [%d] not allow adjust volume", eCurrentChannel);
        return false;
    }

    return true;
}

int CVolumeManager::ChangeForceVoice(yl::string & strVoice)
{
    if (!strVoice.empty())
    {
        int iForceRing = atoi(strVoice.c_str());
        if (iForceRing >= MIN_LOGIC_VOLUME && iForceRing <= MAX_LOGIC_VOLUME)
        {
            VOICE_INFO("CVolumeManager::ChangeForceVoice %d", iForceRing);
            return iForceRing;
        }
    }
    return -1;
}

void CVolumeManager::ResetTalkVolume()
{
    int iResetHandfreeVolume = configParser_GetConfigInt(kszVolumeTalkResetHandfree);
    if (iResetHandfreeVolume > 0 && iResetHandfreeVolume <= 15)
    {
        SetSpkVolume(VT_TALK_HANDFREE, iResetHandfreeVolume);
    }
    int iResetHandsetVolume = configParser_GetConfigInt(kszVolumeTalkResetHandset);
    if (iResetHandsetVolume > 0 && iResetHandsetVolume <= 15)
    {
        SetSpkVolume(VT_TALK_HANDSET, iResetHandsetVolume);
    }
    int iResetHeadsetVolume = configParser_GetConfigInt(kszVolumeTalkResetHeadset);
    if (iResetHeadsetVolume > 0 && iResetHeadsetVolume <= 15)
    {
        SetSpkVolume(VT_TALK_HEADSET, iResetHeadsetVolume);
    }
}
