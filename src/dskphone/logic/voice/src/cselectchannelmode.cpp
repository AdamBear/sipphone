///////////////////////////////////////////////////////////
//  CSelectChannelMode.cpp
//  Implementation of the Class CSelectChannelMode
//  Created on:      17-12月-2015 16:57:42
//  Original author: Song
///////////////////////////////////////////////////////////
#include "voicechannel_include.h"

#ifndef _DECT

CSelectChannelMode::CSelectChannelMode(CBaseInitParam * pParam) :
    CBaseChannelMode(pParam)
{
#if VOICE_INFO_DEBUG
    VOICE_INFO("new CSelectChannelMode");
#endif //
    m_pSubChannelMode = NULL;
}

CSelectChannelMode::~CSelectChannelMode()
{
#if VOICE_INFO_DEBUG
    VOICE_INFO("~CSelectChannelMode");
#endif //
    if (NULL != m_pSubChannelMode)
    {
        // 注意：子模式会与CSelectChannelMode共用指针参数，会造成重复释放
        if (m_pSubChannelMode->IsSameInitParam(m_pChannelInitParam))
        {
            m_pChannelInitParam = NULL;
        }

        if (m_pSubChannelMode->IsSameChannelHandleParam(m_pChannelHandleParam))
        {
            m_pChannelHandleParam = NULL;
        }

        if (m_pSubChannelMode->IsSameChannelHandle(m_pChannelHandle))
        {
            m_pChannelHandle = NULL;
        }

        delete m_pSubChannelMode;
        m_pSubChannelMode = NULL;
    }
}

REAL_CHANNEL CSelectChannelMode::GetRealChannel()
{
#if VOICE_INFO_DEBUG
    VOICE_INFO("CSelectChannelMode::GetRealChannel()");
#endif //VOICE_INFO_DEBUG

    bool bPlayToneOnIdle = (NULL == m_pChannelInitParam) ? false : (m_pChannelInitParam->m_eChannelMode
                           == CHANNEL_TONE_IDLE);

    if (!bPlayToneOnIdle
            && voice_IsPlayingRing())
    {

        if (NULL != m_pSubChannelMode)
        {
            delete m_pSubChannelMode;
            m_pSubChannelMode = NULL;
        }

        // 注意：子模式会与CSelectChannelMode共用指针参数，会造成重复释放
        m_pSubChannelMode = new CRingChannelMode(m_pChannelInitParam);

        return m_pSubChannelMode->GetRealChannelParam();
    }

    REAL_CHANNEL eRealChannel = RC_INIT;

#ifdef IF_FEATURE_DUALHEADSET
    if (voice_IsCallCenterMode())
    {
        // 呼叫中心模式
        if (VoiceManager.m_objDevices.GetHeadsetStatus())
        {
            if (VoiceManager.m_objDevices.GetHandsetStatus())
            {
                eRealChannel = RC_2HEADSET;

                SAFE_DELETE(m_pChannelHandleParam);
                m_pChannelHandleParam = new CHeadsetChannelParam(CHANNEL_2HEADSET, FALSE);
            }
            else
            {
                eRealChannel = RC_HEADSET;
            }
        }
        else
        {
            eRealChannel = RC_HANDFREE;
        }
    }
    else
#endif
    {
        // 优先级：耳麦=>手柄=>免提
        if (VoiceManager.m_objDevices.GetHeadsetStatus())
        {
            eRealChannel = RC_HEADSET;
        }
        else if (!bPlayToneOnIdle
                 && VoiceManager.m_objDevices.GetHandsetStatus())
        {
            eRealChannel = RC_HANDSET;
        }
        else
        {
            eRealChannel = RC_HANDFREE;
        }
    }

    return eRealChannel;
}

CBaseChannelParam * CSelectChannelMode::GetChannelParam()
{
    return (NULL != m_pSubChannelMode) ? m_pSubChannelMode->GetChannelParam() : m_pChannelHandleParam;
}

bool CSelectChannelMode::BeforeSwitchChannel(REAL_CHANNEL eRealChannel)
{
    if (NULL != m_pSubChannelMode)
    {
        return m_pSubChannelMode->BeforeSwitchChannel(eRealChannel);
    }

    return CBaseChannelMode::BeforeSwitchChannel(eRealChannel);
}

bool CSelectChannelMode::ProcessSwitchChannel(REAL_CHANNEL eRealChannel)
{
    if (NULL != m_pSubChannelMode)
    {
        return m_pSubChannelMode->ProcessSwitchChannel(eRealChannel);
    }

    return CBaseChannelMode::ProcessSwitchChannel(eRealChannel);
}

bool CSelectChannelMode::AfterSwitchChannel(REAL_CHANNEL eRealChannel)
{
    if (NULL != m_pSubChannelMode)
    {
        return m_pSubChannelMode->AfterSwitchChannel(eRealChannel);
    }

    return CBaseChannelMode::AfterSwitchChannel(eRealChannel);
}
#endif
