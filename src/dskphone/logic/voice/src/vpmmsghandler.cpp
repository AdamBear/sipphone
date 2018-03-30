#include "voicechannel_include.h"
#include "service_ipvp.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "media/commondefine.h"

extern bool g_bAfterInit;

// 获取单例对象
IMPLEMENT_GETINSTANCE(CVPMMSGHandler)

CVPMMSGHandler::CVPMMSGHandler()
{
    m_eRealChannel = RC_INIT;
    m_bRealPlayRing = FALSE;
    m_iPvpMode = -1;
}

CVPMMSGHandler::~CVPMMSGHandler()
{
}

// 打开通道
void CVPMMSGHandler::OpenChannel(REAL_CHANNEL eChannel)
{
    REAL_CHANNEL eNewChannel = CorrectRealChannel(eChannel);

    int iMode = GetIpvpMode(eNewChannel);

#if VOICE_INFO_DEBUG
    VOICE_INFO("CVPMMSGHandler::OpenChannel old[%d] new[%d]", m_eRealChannel, eChannel);
#endif

    // 不重复打开通道
    if (m_eRealChannel == eChannel
            && iMode == m_iPvpMode)
    {
        return;
    }

    // 如果上一次是RC_SPEAKER_ONLY，切换为其他通道后将静音取消
    if (m_eRealChannel == RC_SPEAKER_ONLY)
    {
        SetMute(1, FALSE);
    }

    VOICE_INFO("CVPMMSGHandler::OpenChannel eChannel=[%d]", eChannel);

    int iLastMode = m_iPvpMode;
    m_eRealChannel = eChannel;
    m_iPvpMode = iMode;

    // 此次是RC_SPEAKER_ONLY, 静音
    if (eChannel == RC_SPEAKER_ONLY)
    {
        SetMute(1, TRUE);
    }

    if (!g_bAfterInit)
    {
        return;
    }
#ifdef _CP920
    // http://10.2.1.199/Bug.php?BugID=121698
    // CP920同一模式不重复切换, 避免异常
    if (iLastMode != iMode)
#endif
    {
        ipvp_set_mode(mkGetHandle(), iMode);
    }

    VOICE_INFO("Set vpm: open channel[%d]", iMode);
}

int CVPMMSGHandler::GetIpvpMode(REAL_CHANNEL eChannel)
{
    int iMode = IPVP_MODE_HS;
    switch (eChannel)
    {
    case RC_HANDSET:
        {
            iMode = IPVP_MODE_HS;
        }
        break;
    case RC_HANDFREE:
        {
            iMode = (DEVICELIB_IS_CP(devicelib_GetPhoneType())) ? IPVP_MODE_VCP : IPVP_MODE_HF;
        }
        break;
    case RC_HEADSET:
        {
#if defined(IF_SUPPORT_BLUETOOTH) || defined(IF_SUPPORT_USB_AUDIO)
            if (HEADSET_BT == VoiceManager.GetHeadsetChannelAvailableType())
            {
                iMode = IPVP_MODE_BTES;
            }
            else if (HEADSET_USB == VoiceManager.GetHeadsetChannelAvailableType(false))
            {
                iMode = IPVP_MODE_USB;
            }
            else
#endif // #if defined(IF_SUPPORT_BLUETOOTH) || defined(IF_SUPPORT_USB_AUDIO)
            {
                iMode = IPVP_MODE_ES;
            }
        }
        break;
    case RC_GROUP_HEADSET:
        {
            iMode = IPVP_MODE_EG;
        }
        break;
    case RC_GROUP_HANDSET:
        {
            iMode = IPVP_MODE_HG;
        }
        break;
    case RC_2HEADSET:
        {
            iMode = IPVP_MODE_DU;
        }
        break;
    case RC_SPEAKER_ONLY:
        {
            iMode = IPVP_MODE_HF;
            // 打开免提后静音
            SetMute(1, TRUE);
        }
        break;
    case RC_MIC_ONLY:
        {
            // 新版本已不实现,以前vpm也没做
        }
        break;
    case RC_INIT:
        {
            iMode = IPVP_MODE_IDLE;
        }
        break;
    default:
        break;
    }

    return iMode;
}

REAL_CHANNEL CVPMMSGHandler::CorrectRealChannel(REAL_CHANNEL eChannel)
{
    REAL_CHANNEL eNewChannel = eChannel;
    if (eChannel == RC_HANDSET
            && !CVoiceManager::IsSupportHandset())
    {
        eNewChannel = RC_HANDFREE;
    }
    else if (eChannel == RC_HEADSET
             && !CVoiceManager::IsSupportHeadset())
    {
        eNewChannel = RC_HANDFREE;
    }

    return eNewChannel;
}

// 播放信号音
void CVPMMSGHandler::PlayTone(int iTone, int iStrategy/* = 1*/, int iDuration /* = 0 */)
{
    int iRealTone = 0;

    switch (iTone)
    {
    case TONE_DIAL:
        {
            iRealTone = IPVP_TONE_DIAL;
        }
        break;
    case TONE_DIAL2:
        {
            iRealTone = IPVP_TONE_EXT_DIAL;
        }
        break;
    case TONE_DIAL3:
        {
            iRealTone = IPVP_TONE_EXT_DIAL2;
        }
        break;
    case TONE_RINGBACK:
        {
            iRealTone = IPVP_TONE_RING;
        }
        break;
    case TONE_AUTOANSWER:
        {
            iRealTone = IPVP_TONE_AA;
        }
        break;
    case TONE_BUSY:
        {
            iRealTone = IPVP_TONE_BUSY;
        }
        break;
    case TONE_CALLWAIT:
        {
            iRealTone =  IPVP_TONE_CALLWAITING;
        }
        break;
    case TONE_CALLWAIT1: // http://10.2.1.199/Bug.php?BugID=37771,这里有调整,原来是alert info对应cw1,现在调整为和vpm一样,callwaiting1-4对应broadsoft的callwaiting1-4
        {
            iRealTone = IPVP_TONE_EXT_CW1;
        }
        break;
    case TONE_CALLWAIT2:
        {
            iRealTone = IPVP_TONE_EXT_CW2;
        }
        break;
    case TONE_CALLWAIT3:
        {
            iRealTone = IPVP_TONE_EXT_CW3;
        }
        break;
    case TONE_CALLWAIT4:
        {
            iRealTone = IPVP_TONE_EXT_CW4;
        }
        break;
    case TONE_VOICEMAIL:
    case TONE_STUTTER:
    case TONE_TRAN_FAILED:
    case TONE_HOLD_ALERT:
        {
            iRealTone = IPVP_TONE_STUTTER;
        }
        break;
    case TONE_TEXT_MSG:
        {
            iRealTone = IPVP_TONE_MESSAGE;
        }
        break;
    case TONE_CALLER_WAIT:
        {
            iRealTone = IPVP_TONE_CALLERWAIT;
        }
        break;
    case TONE_CONF:
        {
            iRealTone = IPVP_TONE_CONF;
        }
        break;
    case TONE_CONGESTION:
        {
            iRealTone = IPVP_TONE_CONGESTON;
        }
        break;
    case TONE_STUTTER_DIAL:
        {
            iRealTone = IPVP_TONE_STUTTER_DIAL;
        }
        break;
    default:
        break;
    }

    if (!g_bAfterInit)
    {
        return;
    }

    // 播放定时信号音：调用后应该马上调用StopTone，由VPM保证播放时长（不可被打断）
    ipvp_tone_play2(mkGetHandle(), iRealTone, iStrategy, iDuration, 0);
    VOICE_INFO("Set vpm: play timed tone[%d][duration=%d]", iRealTone, iDuration);
}

// 停止信号音
void CVPMMSGHandler::StopTone()
{
    if (!g_bAfterInit)
    {
        return;
    }

    ipvp_tone_stop(mkGetHandle(), 0);
    VOICE_INFO("Set vpm: stop tone");

}

// 播放铃声
void CVPMMSGHandler::PlayRing(const yl::string & strRing, int loop/* = -1*/)
{
    if (!g_bAfterInit)
    {
        return;
    }


    // loop: 表示循环的次数，-1 表示一直循环
    ipvp_ring_play(mkGetHandle(), strRing.c_str(), loop);
    VOICE_INFO("Set vpm: play ring[%s], [%d]", strRing.c_str(), loop);

}

// 停止铃声
void CVPMMSGHandler::StopRing()
{
    if (m_bRealPlayRing)
    {
        m_bRealPlayRing = FALSE;

        if (!g_bAfterInit)
        {
            return;
        }
        ipvp_ring_stop(mkGetHandle());
        VOICE_INFO("Set vpm: stop ring");
    }
}

// 播放按键音
void CVPMMSGHandler::PlayLocalDTMF(int iKey)
{
    bool bTouchTone = false;
    BYTE iDtmf = 0;
    static const char szTouchSound[] = "/phone/resource/touchtone/TouchSound.wav";
    switch (iKey)
    {
    case PHONE_KEY_0:
        iDtmf = IPVP_DTMF_0;
        break;
    case PHONE_KEY_1:
        iDtmf = IPVP_DTMF_1;
        break;
    case PHONE_KEY_2:
        iDtmf = IPVP_DTMF_2;
        break;
    case PHONE_KEY_3:
        iDtmf = IPVP_DTMF_3;
        break;
    case PHONE_KEY_4:
        iDtmf = IPVP_DTMF_4;
        break;
    case PHONE_KEY_5:
        iDtmf = IPVP_DTMF_5;
        break;
    case PHONE_KEY_6:
        iDtmf = IPVP_DTMF_6;
        break;
    case PHONE_KEY_7:
        iDtmf = IPVP_DTMF_7;
        break;
    case PHONE_KEY_8:
        iDtmf = IPVP_DTMF_8;
        break;
    case PHONE_KEY_9:
        iDtmf = IPVP_DTMF_9;
        break;
    case PHONE_KEY_A:
        iDtmf = IPVP_DTMF_A;
        break;
    case PHONE_KEY_B:
        iDtmf = IPVP_DTMF_B;
        break;
    case PHONE_KEY_C:
        iDtmf = IPVP_DTMF_C;
        break;
    case PHONE_KEY_D:
        iDtmf = IPVP_DTMF_D;
        break;
#if IF_FEATURE_DTMF_FLASH
    case PHONE_KEY_E:
        iDtmf = IPVP_DTMF_FLASH;
        break;
#endif
    case PHONE_KEY_STAR:
        iDtmf = IPVP_DTMF_STAR;
        break;
    case PHONE_KEY_POUND:
        iDtmf = IPVP_DTMF_POUND;
        break;
    case PHONE_KEY_TOUCH:
        break;
    default:
        iDtmf = IPVP_DTMF_A;
        break;
    }

    if (!g_bAfterInit)
    {
        return;
    }
    if (0 == iDtmf)
    {
        int iRet = ipvp_sound_play(mkGetHandle(), szTouchSound, 0);
        VOICE_INFO("Set vpm: play touch ret[%d]", iRet);
    }
    else
    {
        ipvp_dtmf_play(mkGetHandle(), iDtmf, 0);
        VOICE_INFO("Set vpm: play local dtmf[%d]", iDtmf);
    }
}

// 发送远端DTMF
void CVPMMSGHandler::SendRemoteDTMF(int iCallID, int iKey, DTMF_DIR eDtmfDir)
{
    int iDtmf = 0;
    switch (iKey)
    {
    case PHONE_KEY_0:
        iDtmf = IPVP_DTMF_0;
        break;
    case PHONE_KEY_1:
        iDtmf = IPVP_DTMF_1;
        break;
    case PHONE_KEY_2:
        iDtmf = IPVP_DTMF_2;
        break;
    case PHONE_KEY_3:
        iDtmf = IPVP_DTMF_3;
        break;
    case PHONE_KEY_4:
        iDtmf = IPVP_DTMF_4;
        break;
    case PHONE_KEY_5:
        iDtmf = IPVP_DTMF_5;
        break;
    case PHONE_KEY_6:
        iDtmf = IPVP_DTMF_6;
        break;
    case PHONE_KEY_7:
        iDtmf = IPVP_DTMF_7;
        break;
    case PHONE_KEY_8:
        iDtmf = IPVP_DTMF_8;
        break;
    case PHONE_KEY_9:
        iDtmf = IPVP_DTMF_9;
        break;
    case PHONE_KEY_A:
        iDtmf = IPVP_DTMF_A;
        break;
    case PHONE_KEY_B:
        iDtmf = IPVP_DTMF_B;
        break;
    case PHONE_KEY_C:
        iDtmf = IPVP_DTMF_C;
        break;
    case PHONE_KEY_D:
        iDtmf = IPVP_DTMF_D;
        break;
    case PHONE_KEY_STAR:
        iDtmf = IPVP_DTMF_STAR;
        break;
    case PHONE_KEY_POUND:
        iDtmf = IPVP_DTMF_POUND;
        break;
#if IF_FEATURE_DTMF_FLASH
    case PHONE_KEY_E:
        iDtmf = IPVP_DTMF_FLASH;
        break;
#endif
    default:
        iDtmf = IPVP_DTMF_A;
        break;
    }

    int iDir = IPVP_FL_DIR_NONE;
    switch (eDtmfDir)
    {
    case DTMF_DIR_TOTELE:
        iDir = IPVP_FL_DIR_LOCAL;
        break;
    case DTMF_DIR_TONET:
        iDir = IPVP_FL_DIR_NET;
        break;
    case DTMF_DIR_TOBOTH:
        iDir = IPVP_FL_DIR_LOCAL | IPVP_FL_DIR_NET;
        break;
    default:
        break;
    }

    if (!g_bAfterInit)
    {
        return;
    }

    ipvp_dtmf_send(mkGetHandle(), iCallID, iDtmf, iDir, 0);
    VOICE_INFO("Set vpm: send remote dtmf[%d]", iDtmf);
}

#if VOICE_INFO_DEBUG
static void PringVoiceInfo(ipvp_speech_t & ipvpParam)
{
    ipvp_audio_t & stAudio = ipvpParam.audio;
    // 通话调试打印, 未减少重复编码，勿删
    VOICE_INFO("~~~~~~~~~~~~audio code info~~~~~~~~~~~~~~");
    VOICE_INFO("codec_name [%s]", stAudio.codec_name);
    VOICE_INFO("codec_ptype [%d]", stAudio.codec_ptype);
    VOICE_INFO("codec_rate [%d]", stAudio.codec_rate);
    VOICE_INFO("codec_clock [%d]", stAudio.codec_clock);
    VOICE_INFO("ptime [%d]", stAudio.ptime);

    VOICE_INFO("ext_annexa [%d]", stAudio.ext_annexa);
    VOICE_INFO("ext_toc_mode [%d]", stAudio.ext_toc_mode);
    VOICE_INFO("ext_octet_align [%d]", stAudio.ext_octet_align);

    VOICE_INFO("codec list[%d] codec[%s]", stAudio.codec_list_cnt, stAudio.codec_list[0].codec_name);
    VOICE_INFO("rtp_ka_ptype[%d] rtp_ka_interval[%d]", stAudio.rtp_ka_ptype, stAudio.rtp_ka_interval);

    VOICE_INFO("loopback_type[%d] loopback_pkt_mode[%d] loopback_role[%d] loopback_pkt_dynamic_pl[%d]",
               stAudio.loopback_type, stAudio.loopback_pkt_mode, stAudio.loopback_role,
               stAudio.loopback_pkt_dynamic_pl);

    VOICE_INFO("local maxplaybackrate[%d] maxcapturerate[%d] remote maxplaybackrate[%d] maxcapturerate[%d]",
               stAudio.local_maxplaybackrate, stAudio.local_maxcapturerate, stAudio.remote_maxplaybackrate,
               stAudio.remote_maxcapturerate);
    VOICE_INFO("~~~~~~~~~~~~Audio Code info~~~~~~~~~~~~");

    return;
}
#endif //VOICE_INFO_DEBUG

// 开启通话
void CVPMMSGHandler::StartTalk(int iCallID, void * pTalkParam)
{
    if (pTalkParam == NULL)
    {
        return;
    }

    if (!g_bAfterInit)
    {
        return;
    }

    ipvp_speech_t ipvpParam;
    memcpy(&ipvpParam, pTalkParam, sizeof(ipvp_speech_t));
    ipvpParam.mode = IPVP_FL_VAM_AUDIO;

    ipvp_speech_start(mkGetHandle(), iCallID, &ipvpParam, 0);
    VOICE_INFO("Set vpm: start talk[%d]", iCallID);

#if VOICE_INFO_DEBUG
    PringVoiceInfo(ipvpParam);
#endif //VOICE_INFO_DEBUG
    return;
}

// 停止通话
void CVPMMSGHandler::StopTalk(int iCallID)
{
    if (!g_bAfterInit)
    {
        return;
    }
    // 因为目前会先停止声音，再处理到停止视频
    // 需求：此处需要打印当前通话信息
    ipvp_speech_stop(mkGetHandle(), iCallID, 0);
    VOICE_INFO("Set vpm: stop talk[%d]", iCallID);

}

// 重置通话
void CVPMMSGHandler::ResetTalk(int iCallID, void * pTalkParam)
{
    if (pTalkParam == NULL)
    {
        return;
    }

    if (!g_bAfterInit)
    {
        return;
    }

    // 目前需要拷贝一次，设置成音频
    ipvp_speech_t ipvpParam;
    memcpy(&ipvpParam, pTalkParam, sizeof(ipvp_speech_t));
    ipvpParam.mode = IPVP_FL_VAM_AUDIO;

#if IF_BUG_31951 // vpm T4x计算mos和其他机型不一致 v81也存在
    switch (devicelib_GetPhoneType())
    {
    case PT_T41:
    case PT_T42:
    case PT_T46:
    case PT_T48:
    case PT_CP860:
    case PT_CP920:
        {
            yl::string strAccountInfo;
            yl::string strTestCallId;
            bool isConf = talklogic_GetCurrentStatus(strTestCallId, strAccountInfo) == SESSION_LOCALCONF;
            if (isConf)
            {
                ipvp_speech_modify(mkGetHandle(), iCallID, &ipvpParam, 0);
            }
            else
            {
                ipvp_speech_stop(mkGetHandle(), iCallID, 0);
                ipvp_speech_start(mkGetHandle(), iCallID, &ipvpParam, 0);
            }
            break;
        }
    default:
        ipvp_speech_modify(mkGetHandle(), iCallID, &ipvpParam, 0);
        break;
    }
#else
    ipvp_speech_modify(mkGetHandle(), iCallID, &ipvpParam, 0);
#endif //IF_BUG_31951

    VOICE_INFO("Set vpm: reset talk[%d]", iCallID);

#if VOICE_INFO_DEBUG
    PringVoiceInfo(ipvpParam);
#endif //VOICE_INFO_DEBUG

    return;
}

// 设置远端静音
void CVPMMSGHandler::SetFarMute(int iCallID, void * pTalkParam, bool bFarMute)
{
    if (iCallID == -1 || pTalkParam == NULL)
    {
        return;
    }

    ipvp_speech_t paramReset;
    memcpy(&paramReset, (ipvp_speech_t *)pTalkParam, sizeof(ipvp_speech_t));

    if (bFarMute)
    {
        paramReset.audio.flag = IPVP_ADDR_FL_SENDONLY;
    }

    if (!g_bAfterInit)
    {
        return;
    }


#ifndef _CP920
    paramReset.mode = IPVP_FL_VAM_AUDIO;
    ipvp_speech_modify(mkGetHandle(), iCallID, &paramReset, 0);
#else // _CP920
    ipvp_speech_farmute(mkGetHandle(), iCallID, bFarMute, 0);
#endif // !_CP920

    VOICE_INFO("Set vpm: set far mute[%d][%d]", iCallID, bFarMute);

}

// 建立会议
void CVPMMSGHandler::StartConf(int iCallSum, int * pCallIDList, bool isVideoConf)
{
    if (pCallIDList == NULL)
    {
        return;
    }

    ipvp_confer_t confInfo;
    confInfo.count = iCallSum;
    for (int i = 0; i < iCallSum; i++)
    {
        confInfo.callID[i] = pCallIDList[i];
    }

    if (!g_bAfterInit)
    {
        return;
    }

    ipvp_confer_merge(mkGetHandle(), &confInfo, 0);

    VOICE_INFO("Set vpm: start conf");

}

// 停止会议
void CVPMMSGHandler::StopConf(int iCallSum, int * pCallIDList, bool isVideoConf)
{
    if (pCallIDList == NULL)
    {
        return;
    }

    ipvp_confer_t confInfo;
    confInfo.count = iCallSum;
    for (int i = 0; i < iCallSum; i++)
    {
        confInfo.callID[i] = pCallIDList[i];
    }

    if (!g_bAfterInit)
    {
        return;
    }

    ipvp_confer_split(mkGetHandle(), &confInfo, 0);

    VOICE_INFO("Set vpm: stop conf");

}

// 设置是否静音
void CVPMMSGHandler::SetMute(int iCallID, bool bMute)
{
    if (!g_bAfterInit)
    {
        return;
    }

    ipvp_set_mute(mkGetHandle(), bMute ? IPVP_MUTE : IPVP_UNMUTE);
    VOICE_INFO("Set vpm: set mute[%d][%d]", iCallID, bMute);

}

// hold通话  http://192.168.1.99/Bug.php?BugID=12664
void CVPMMSGHandler::SetCallHold(int iCallID, bool bHold/* = true*/)
{
    if (!g_bAfterInit)
    {
        return;
    }

    ipvp_speech_hold(mkGetHandle(), iCallID, bHold ? 1 : 0, 0);
    VOICE_INFO("Set vpm: hold talk[%d][%d]", iCallID, bHold);

}

// 设置音量
void CVPMMSGHandler::SetVolume(int iVolume)
{
    if (!g_bAfterInit)
    {
        return;
    }

    ipvp_set_volume(mkGetHandle(), iVolume);
    VOICE_INFO("Set vpm: set volume[%d]", iVolume);

}

#ifdef IF_BT_SUPPORT_PHONE
//开启蓝牙通话
void CVPMMSGHandler::StartBtTalk(int iCallID)
{
    if (!g_bAfterInit)
    {
        return;
    }


    ipvp_set_slave(mkGetHandle(), IPVP_SLAVE_BTHF, IPVP_FL_DIR_LOCAL | IPVP_FL_DIR_NET, 0);

    VOICE_INFO("Set vpm: start bt talk[%d]", iCallID);

}

//hold bt 通话
void CVPMMSGHandler::SetBtCallHold(int iCallID, bool bHold /* = true */)
{
    if (!g_bAfterInit)
    {
        return;
    }


#ifdef IF_NEW_CHANNEL_MODE
    ipvp_set_slave(mkGetHandle(), IPVP_SLAVE_BTHF,
                   bHold ? IPVP_FL_DIR_NONE : IPVP_FL_DIR_LOCAL | IPVP_FL_DIR_NET, 0);
    VOICE_INFO("Set vpm: hold bt talk[%d][%d]", iCallID, bHold);
#endif // IF_NEW_CHANNEL_MODE

}

//重置蓝牙通话
void CVPMMSGHandler::ResetBtTalk(int iCallID)
{
    if (!g_bAfterInit)
    {
        return;
    }


    //先stop，再start ----> v81蓝牙手机通话没有参数, 目前不需要reset
#ifdef IF_NEW_CHANNEL_MODE
    ipvp_set_slave(mkGetHandle(), IPVP_SLAVE_BTHF, IPVP_FL_DIR_NONE, 0);
    ipvp_set_slave(mkGetHandle(), IPVP_SLAVE_BTHF, IPVP_FL_DIR_LOCAL | IPVP_FL_DIR_NET, 0);
    VOICE_INFO("Set vpm: reset bt talk[%d]", iCallID);
#endif // IF_NEW_CHANNEL_MODE

}

// 停止蓝牙通话
void CVPMMSGHandler::StopBtTalk(int iCallID)
{
    if (!g_bAfterInit)
    {
        return;
    }


    ipvp_set_slave(mkGetHandle(), IPVP_SLAVE_BTHF, IPVP_FL_DIR_NONE, 0);
    VOICE_INFO("Set vpm: stop bt talk[%d]", iCallID);
}

// mute bt 通话
void CVPMMSGHandler::SetBtFarMute(int iCallID, bool bFarMute)
{
    if (!g_bAfterInit)
    {
        return;
    }

    ipvp_set_slave(mkGetHandle(), IPVP_SLAVE_BTHF, bFarMute ?
                   IPVP_FL_DIR_NET : IPVP_FL_DIR_LOCAL | IPVP_FL_DIR_NET, 0);
    VOICE_INFO("Set vpm: set bt far mute[%d][%d]", iCallID, bFarMute);
}

#endif // IF_BT_SUPPORT_PHONE

// 播放远端声音(手机)
void CVPMMSGHandler::PlayA2dp(bool bPlay)
{
    if (!g_bAfterInit)
    {
        return;
    }

    ipvp_set_slave(mkGetHandle(), IPVP_SLAVE_BTA2DP, bPlay ? IPVP_FL_DIR_ALL : IPVP_FL_DIR_NONE, 0);
    VOICE_INFO("Set vpm: play a2dp[%d]", bPlay);
}

