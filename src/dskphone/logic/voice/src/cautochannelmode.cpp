///////////////////////////////////////////////////////////
//  CAutoChannelMode.cpp
//  Implementation of the Class CAutoChannelMode
//  Created on:      17-12月-2015 16:57:41
//  Original author: Song
///////////////////////////////////////////////////////////

#include "voicechannel_include.h"

#ifndef _DECT

CAutoChannelMode::CAutoChannelMode(CBaseInitParam * pParam) :
    CBaseChannelMode(pParam)
{
#if VOICE_INFO_DEBUG
    VOICE_INFO("CAutoChannelMode");
#endif //
    m_pSubChannelMode = NULL;
}

CAutoChannelMode::~CAutoChannelMode()
{
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

REAL_CHANNEL CAutoChannelMode::GetRealChannel()
{
    if (NULL != m_pSubChannelMode)
    {
        delete m_pSubChannelMode;
        m_pSubChannelMode = NULL;
    }

    if (!talklogic_SessionExist())
    {
        m_pSubChannelMode = VoiceManager.GetChannelModePtr(CHANNEL_IDLE);
    }
    else
    {
        m_pSubChannelMode = VoiceManager.GetChannelModePtr(CHANNEL_SELECT);
    }

    return m_pSubChannelMode->GetRealChannelParam();
}

CBaseChannelParam * CAutoChannelMode::GetChannelParam()
{
    return (NULL != m_pSubChannelMode) ? m_pSubChannelMode->GetChannelParam() : m_pChannelHandleParam;
}

bool CAutoChannelMode::ProcessSwitchChannel(REAL_CHANNEL eRealChannel)
{
    if (NULL != m_pSubChannelMode)
    {
        return m_pSubChannelMode->ProcessSwitchChannel(eRealChannel);
    }

    return false;
}


bool CAutoChannelMode::BeforeSwitchChannel(REAL_CHANNEL eRealChannel)
{
    if (NULL != m_pSubChannelMode)
    {
        return m_pSubChannelMode->BeforeSwitchChannel(eRealChannel);
    }

    return false;
}

bool CAutoChannelMode::AfterSwitchChannel(REAL_CHANNEL eRealChannel)
{
    if (NULL != m_pSubChannelMode)
    {
        return m_pSubChannelMode->AfterSwitchChannel(eRealChannel);
    }

    return false;
}
#endif
