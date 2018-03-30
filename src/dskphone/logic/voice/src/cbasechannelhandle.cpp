///////////////////////////////////////////////////////////
//  CBaseChannelHandle.cpp
//  Implementation of the Class CBaseChannelHandle
//  Created on:      17-12月-2015 17:10:14
//  Original author: Song
///////////////////////////////////////////////////////////
#include "voicechannel_include.h"
#include "ehsheadset.h"

#ifndef _DECT

CBaseChannelHandle::CBaseChannelHandle(REAL_CHANNEL eRealChannel)
{
    m_eRealChannel = eRealChannel;
}

CBaseChannelHandle::~CBaseChannelHandle()
{

}

bool CBaseChannelHandle::OpenRealChannel(REAL_CHANNEL eChannel, CBaseChannelParam * pParam)
{
#if VOICE_INFO_DEBUG
    VOICE_INFO("CBaseChannelHandle::OpenRealChannel [%d] [%d]", eChannel, m_eRealChannel);
#endif
    bool bHandle = true;
    switch (m_eRealChannel)
    {
    case RC_SPEAKER_ONLY:
    case RC_MIC_ONLY:
    case RC_HANDFREE:
        bHandle = OpenHandFreeChannel(eChannel, pParam);
        break;
    case RC_HANDSET:
        bHandle = OpenHandSetChannel(eChannel, pParam);
        break;
    case RC_2HEADSET:
    case RC_HEADSET:
        bHandle = OpenHeadSetChannel(eChannel, pParam);
        break;
    case RC_INIT:
        bHandle = OpenDefaultChannel(eChannel, pParam);
        break;
    default:
        break;
    }

    if (!bHandle)
    {
        return false;
    }

    // 要在设置图标之前就切换当前通道，否则会死循环 http://bugfree.yealink.com/Bug.php?BugID=99462
    //if (NULL != pParam)
    //{
    //  _objVoice.SetCurrentChannel(pParam->m_eChannelMode);
    //}

    VoiceSessionPtr pSession = VoiceManager.GetActiveSession().lock();
    if (!pSession.IsEmpty())
    {
        pSession->OpenRealChannel(eChannel);
    }

    return true;
}

bool CBaseChannelHandle::OpenHandFreeChannel(REAL_CHANNEL eChannel, CBaseChannelParam * pParam)
{
#if VOICE_INFO_DEBUG
    VOICE_INFO("CHandFreeChannelHandle::OpenRealChannel eChannel[%d]", eChannel);
#endif //VOICE_INFO_DEBUG
    if (NULL == pParam)
    {
        return false;
    }

    CHANNEL_MODE eMode = pParam->m_eChannelMode;

    // 如果免提关闭则打开免提
    // http://10.2.1.199/Bug.php?BugID=37176
    VoiceManager.m_objDevices.SetHandfreeStatus(true);

    // 如果不是临时改变通道且当前通道已经是免提，则不再处理
    if (voice_GetCurrentChannel() == eMode)
    {
        VOICE_WARN("current channel is already handfree!");
        return false;
    }

    VOICE_INFO("OpenHandfree eChannel=%d", eChannel);
    voice_SetCurrentChannel(pParam->m_eChannelMode, true);

    // 非播放铃声的前提下免提和耳麦互斥
    VoiceManager.m_objDevices.SetHeadsetStatus(false);
    return true;
}

bool CBaseChannelHandle::OpenHeadSetChannel(REAL_CHANNEL eChannel, CBaseChannelParam * pParam)
{
    if (NULL == pParam)
    {
        return false;
    }

    bool b2HeadsetMode = false;
    bool bOpenEHS = false;

    CHeadsetChannelParam * pHeadsetChannelParam = dynamic_cast<CHeadsetChannelParam *>(pParam);
    if (NULL != pHeadsetChannelParam)
    {
        b2HeadsetMode = pHeadsetChannelParam->m_eChannelMode == CHANNEL_2HEADSET;
        bOpenEHS = pHeadsetChannelParam->m_bEHS;
    }

    // 如果当前通道已经是耳麦，则不再处理
    if (!b2HeadsetMode && voice_GetCurrentChannel() == CHANNEL_HEADSET)
    {
        VOICE_WARN("current channel is already headset!");
        return false;
    }

    if (b2HeadsetMode && voice_GetCurrentChannel() == CHANNEL_2HEADSET)
    {
        VOICE_WARN("current channel is already 2headset!");
        return false;
    }

    voice_SetCurrentChannel(pParam->m_eChannelMode, true);

    VOICE_INFO("OpenHeadset b2HeadsetMode = %d bOpenEHS= %d", b2HeadsetMode, bOpenEHS);

    // 如果免提已打开，则将免提关闭
    VoiceManager.m_objDevices.SetHandfreeStatus(false);
    // 通道切到耳麦时，耳麦一定是开启的
    VoiceManager.m_objDevices.SetHeadsetStatus(true);

    // 只有按耳麦键打开耳麦通道时要打开EHS，关闭时不用关闭EHS
    if (bOpenEHS
            && talklogic_SessionExist())
    {
        VoiceManager.m_objDevices.EHSOpen();
    }

#ifdef IF_FEATURE_HEADSET_PRIOR
    // 耳麦优先模式：保存到文件中，只有Idle界面关闭耳麦才是真的关闭
    if (configParser_GetConfigInt(kszLastHeadSet) == 0)
    {
        configParser_SetConfigInt(kszLastHeadSet, 1, CONFIG_LEVEL_PHONE);
    }
#endif

    return true;
}

bool CBaseChannelHandle::OpenHandSetChannel(REAL_CHANNEL eChannel, CBaseChannelParam * pParam)
{
    // 如果当前通道已经是手柄，则不再处理
    if (voice_GetCurrentChannel() == CHANNEL_HANDSET)
    {
        VOICE_WARN("current channel is already handset!");
        return false;
    }
    voice_SetCurrentChannel(CHANNEL_HANDSET, true);
    // 如果免提已打开，则将免提关闭
    VoiceManager.m_objDevices.SetHandfreeStatus(false);
    // 如果耳麦已打开，则将耳麦关闭
    VoiceManager.m_objDevices.SetHeadsetStatus(false);
    return true;
}

bool CBaseChannelHandle::OpenDefaultChannel(REAL_CHANNEL eChannel, CBaseChannelParam * pParam)
{
    if (pParam == NULL)
    {
        return false;
    }

    if (CHANNEL_NULL == pParam->m_eChannelMode)
    {
        voice_SetCurrentChannel(CHANNEL_NULL, true);
    }

    return true;
}

#endif
