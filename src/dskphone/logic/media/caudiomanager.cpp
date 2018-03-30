///////////////////////////////////////////////////////////
//  CAudioManager.cpp
//  Implementation of the Class CAudioManager
//  Created on:      05-二月-2015 21:05:04
//  Original author: huangwz
///////////////////////////////////////////////////////////

#ifdef _T49
#include "caudiomanager.h"
#include "cmediamanager.h"
#include "commondefine.h"

#include "service_ipvp.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talk/talklogic/include/callinfo.h"

CAudioManager::CAudioManager()
{
}


CAudioManager::~CAudioManager()
{

}

bool CAudioManager::OnEvent(MediaEventData * pData)
{
    if (NULL == pData)
    {
        return false;
    }

    bool bHandled = true;
    switch (pData->eType)
    {
    case MET_STATS_INFO:
        {
            if (NULL != pData->pExt)
            {
                GetStats(pData->wEventPara, *(MediaStatsInfo *)pData->pExt);
            }
        }
        break;
    case MET_TALK_STOP:
        {
            OnStopAudio(pData->wEventPara);
        }
        break;
    case MET_TALK_START:
        {
            OnStartAudio(pData->wEventPara);
        }
        break;
    case MET_STATS_UPDATE:
        {
            UpdateStats();
        }
        break;
    case MET_TALK_AUDIO_SRTP_REQ:
        {
            // 暂不处理：v81
            OnReqSrtpLifeTime(pData->wEventPara);
        }
        break;
    case MET_IPVP_START:
        {
            //
            VOICE_INFO("audio register notify ipvp");
            // 注册监听消息，部分ipvp的消息需要GUI注册实例到ipvp进程
            ipvp_request_notify(mkGetHandle(), IPVP_MSG_AUDIO_SRTP_REINVITE);
        }
        break;
    default:
        bHandled = false;
        break;
    }

    return bHandled;
}

static void PrintStats(AudioStatsInfo & audioStats)
{
    VOICE_INFO("~~~~Audio stats info~~~");
    VOICE_INFO("	Audio codec Rec[%s] Send[%s]", audioStats.RecCodec, audioStats.SendCodec);
    VOICE_INFO("	Audio bitrate Rec[%d] Send[%d]", audioStats.RecBitrate, audioStats.SendBitrate);
    VOICE_INFO("	Audio samplerate Rec[%d] Send[%d]", audioStats.RecSamplerate,
               audioStats.SendSamplerate);
    VOICE_INFO("	Audio rtp packet Rec[%d] Send[%d]", audioStats.RecRtpPackets,
               audioStats.SendRtpPackets);
    VOICE_INFO("	Audio rtcp packet Rec[%d] Send[%d]", audioStats.RecRtcpPackets,
               audioStats.SendRtcpPackets);

    VOICE_INFO("	Audio lostfrac Rec[%d] Send[%d]", audioStats.RecLostFrac, audioStats.SendLostFrac);
    VOICE_INFO("	Audio losttotal Rec[%d] Send[%d]", audioStats.RecLostTotal, audioStats.SendLostTotal);
    VOICE_INFO("	Audio jitCur Rec[%d] Send[%d]", audioStats.RecJitCur, audioStats.SendJitCur);
    VOICE_INFO("	Audio jitAvg Rec[%d] Send[%d]", audioStats.RecJitAvg, audioStats.SendJitAvg);
    VOICE_INFO("	Audio jitMax Rec[%d] Send[%d]", audioStats.RecJitMax, audioStats.SendJitMax);
    VOICE_INFO("	Audio Delay Rec[%d] Send[%d]", audioStats.RecDelay, audioStats.SendDelay);

    return;
}

bool CAudioManager::GetStats(int iCallId, MediaStatsInfo & statsInfo)
{
    ipvp_astats_t audioStats;
    memset(&audioStats, 0, sizeof(ipvp_astats_t));
    audioStats.flags = 0;

    // 怀疑出现卡住问题
    VOICE_WARN("get audio stats");
    ipvp_get_audio_stats(mkGetHandle(), iCallId, &audioStats);

    strncpy(statsInfo.audio.RecCodec, audioStats.rx_codec, IPVP_MAX_CODECNAME);
    strncpy(statsInfo.audio.SendCodec, audioStats.tx_codec, IPVP_MAX_CODECNAME);

    statsInfo.audio.RecBitrate = audioStats.rx_bitrate;
    statsInfo.audio.SendBitrate = audioStats.tx_bitrate;
    statsInfo.audio.RecSamplerate = audioStats.rx_samplerate;
    statsInfo.audio.SendSamplerate = audioStats.tx_samplerate;
    statsInfo.audio.RecJitCur = audioStats.rx_jit_cur;
    statsInfo.audio.SendJitCur = audioStats.tx_jit_cur;

    statsInfo.audio.RecJitAvg = audioStats.rx_jit_avg;
    statsInfo.audio.SendJitAvg = audioStats.tx_jit_avg;

    statsInfo.audio.RecJitMax = audioStats.rx_jit_max;
    statsInfo.audio.SendJitMax = audioStats.tx_jit_max;

    statsInfo.audio.RecDelay = audioStats.rx_delay_rt;
    statsInfo.audio.SendDelay = audioStats.tx_delay_rt;

    statsInfo.audio.RecRtpPackets = audioStats.rx_rtp;
    statsInfo.audio.SendRtpPackets = audioStats.tx_rtp;
    statsInfo.audio.RecRtcpPackets = audioStats.rx_rtcp;
    statsInfo.audio.SendRtcpPackets = audioStats.tx_rtcp;
    statsInfo.audio.RecLostFrac = audioStats.rx_lost_frac;
    statsInfo.audio.SendLostFrac = audioStats.tx_lost_frac;
    statsInfo.audio.RecLostTotal = audioStats.rx_lost_total;
    statsInfo.audio.SendLostTotal = audioStats.tx_lost_total;

#if 1
    PrintStats(statsInfo.audio);
#endif

    return true;
}

bool CAudioManager::OnStopAudio(int iCallId)
{
    return DeleteAudioInfo(iCallId);
}

bool CAudioManager::OnStartAudio(int iCallID)
{
    TALK_INFO("audio on start audeo [%d]", iCallID);
    CCallInfo * pInfo = talklogic_FindCallInfoByCallId(iCallID);
    if (NULL == pInfo)
    {
        TALK_INFO("audio not found call info");
        return false;
    }

    if (NULL != GetAudioInfo(iCallID))
    {
        TALK_ERR("audio started");
        return false;
    }

    AudioInfo audioInfo;
    audioInfo.iCallId = pInfo->GetCallID();
    audioInfo.audioInfo = pInfo->m_tVPMParam.audio;

    m_listAudioInfo.push_back(audioInfo);

    return true;
}

AudioInfo * CAudioManager::GetAudioInfo(int iCallID)
{
    YLList<AudioInfo>::iterator itBeg = m_listAudioInfo.begin();

    for (; itBeg != m_listAudioInfo.end(); ++itBeg)
    {
        AudioInfo & audioInfo = (*itBeg);
        if (iCallID == audioInfo.iCallId)
        {
            return &(*itBeg);
        }
    }

    return NULL;
}

bool CAudioManager::DeleteAudioInfo(int iCallID)
{
    YLList<AudioInfo>::iterator itBeg = m_listAudioInfo.begin();

    for (; itBeg != m_listAudioInfo.end(); ++itBeg)
    {
        AudioInfo & audioInfo = (*itBeg);
        if (iCallID == audioInfo.iCallId)
        {
            TALK_INFO("delete audio [%d]", iCallID);
            m_listAudioInfo.erase(itBeg);
            return true;
        }
    }

    return false;
}

bool CAudioManager::UpdateStats()
{
    bool bUpdate = false;
    YLList<AudioInfo>::iterator it = m_listAudioInfo.begin();
    for (; it != m_listAudioInfo.end(); ++it)
    {
        AudioInfo & info = (*it);

        //视频话路，由视频更新即可
        if (talklogic_IsVideoTalk(info.iCallId))
        {
            continue;
        }

        //只计算Talking 的那路
        CCallInfo * pCallInfo = talklogic_GetCallInfoByCallId(info.iCallId);
        if (pCallInfo == NULL || !pCallInfo->IsTalkStart())
        {
            continue;
        }

        bUpdate |= UpdateStats(info.iCallId, &info);
    }

    return bUpdate;
}

bool CAudioManager::UpdateStats(int iCallID, AudioInfo * pAudioInfo)
{
    if (iCallID <= 0
            || NULL == pAudioInfo)
    {
        return false;
    }

    MediaStatsInfo mediaStats;

    GetStats(iCallID, mediaStats);

    return _MediaManager.ProcessStats((void *) pAudioInfo, mediaStats, false);
}

bool CAudioManager::OnReqSrtpLifeTime(int iCallId)
{
//  VOICE_INFO("Ipvp request srtp life time[%d]", iCallId);
//  CCallInfo *pCallInfo = talklogic_FindCallInfoByCallId(iCallId);
//  if (NULL == pCallInfo)
//  {
    return false;
//  }
//
//  return modVoip_RequestSrtpLifetime(pCallInfo->GetAccountID(), iCallId);
}

#endif //#ifdef _T49

