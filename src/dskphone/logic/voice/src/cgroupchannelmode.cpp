///////////////////////////////////////////////////////////
//  CGroupChannelMode.cpp
//  Implementation of the Class CGroupChannelMode
//  Created on:      17-12月-2015 16:57:41
//  Original author: Song
///////////////////////////////////////////////////////////
#include "voicechannel_include.h"

#ifndef _DECT

CGroupChannelMode::CGroupChannelMode(CBaseInitParam * pParam) :
    CBaseChannelMode(pParam)
{
    m_bIsNeedPreProcess = true;
}

CGroupChannelMode::~CGroupChannelMode()
{

}

REAL_CHANNEL CGroupChannelMode::GetRealChannel()
{
    REAL_CHANNEL eRealChannel = _VPMMSGHandler.GetRealChannel();

#ifdef IF_SUPPORT_BLUETOOTH
    if (VoiceManager.IsBluetoothMode())
    {
        VOICE_WARN("Not support group mode while channel is bluetooth headset");
        m_bIsNeedPreProcess = false;
        // 蓝牙耳麦不支持组模式
        return eRealChannel;
    }
#endif //IF_SUPPORT_BLUETOOTH

#ifdef IF_FEATURE_DUALHEADSET
    if (voice_IsCallCenterMode())
    {
        if (voice_GetCurrentChannel() == CHANNEL_HEADSET)
        {
            eRealChannel = RC_HEADSET;

            SAFE_DELETE(m_pChannelHandleParam);

            m_pChannelHandleParam = new CHeadsetChannelParam(CHANNEL_2HEADSET, FALSE);
        }
        // 双耳麦模式下不再处理群听
        m_bIsNeedPreProcess = false;
        return eRealChannel;
    }
#endif

    CGroupChannelInitParam * pGroupInitParam = dynamic_cast<CGroupChannelInitParam *>
            (m_pChannelInitParam);
    bool bForceOpen = (NULL == pGroupInitParam) ? false : pGroupInitParam->m_bForceOpen;
    CVoiceSession::GROUP_TYPE eGroupType = CVoiceSession::GROUP_NONE;

    if (voice_GetCurrentChannel() == CHANNEL_HANDSET)
    {
        eGroupType = CVoiceSession::GROUP_HANDSET;
    }


    // 当前是Group通道则关闭
    else if (voice_GetCurrentChannel() == CHANNEL_HEADSET)
    {
        eGroupType = CVoiceSession::GROUP_HEADSET;
    }
    else
    {
        if (VoiceManager.m_objDevices.GetHandsetStatus())
        {
            eGroupType = CVoiceSession::GROUP_HANDSET;
        }
    }

    VOICE_INFO("grouplisten forceopen = [%d] current channel = [%d] group type = [%d]", bForceOpen,
               voice_GetCurrentChannel(), eGroupType);
    // 通话中摘手柄按免提键（免提键按下，通道被切为免提）
    if (CVoiceSession::GROUP_NONE == eGroupType)
    {
        m_bIsNeedPreProcess = false;
        return eRealChannel;
    }

    VoiceSessionPtr pSession = VoiceManager.GetActiveSession().lock();
    if (!pSession.IsEmpty())
    {
        pSession->SetGroupType(eGroupType);
    }
    if (voice_GetCurrentChannel() == CHANNEL_GROUP && !bForceOpen)
    {
        eRealChannel = ((eGroupType == CVoiceSession::GROUP_HANDSET)
                        ? RC_HANDSET : RC_HEADSET);
    }
    else // 打开Group通道
    {
        eRealChannel = ((eGroupType == CVoiceSession::GROUP_HANDSET)
                        ? RC_GROUP_HANDSET : RC_GROUP_HEADSET);
    }

    return eRealChannel;
}

//http://10.2.1.199/Bug.php?BugID=86523 群听时初始通道当做是非法处理


bool CGroupChannelMode::BeforeSwitchChannel(REAL_CHANNEL eRealChannel)
{
    if (!m_bIsNeedPreProcess)
    {
        return true;
    }

    // 群听时认为免提打开
    VoiceManager.m_objDevices.SetHandfreeStatus(TRUE);
    voice_SetCurrentChannel(CHANNEL_GROUP, true);
    return true;
}
#endif
