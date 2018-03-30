///////////////////////////////////////////////////////////
//  CBaseChannelMode.cpp
//  Implementation of the Class CBaseChannelMode
//  Created on:      17-12月-2015 16:57:41
//  Original author: Song
///////////////////////////////////////////////////////////
#include "voicechannel_include.h"

#ifndef _DECT

CBaseChannelMode::CBaseChannelMode(CBaseInitParam * pParam)
    : m_pChannelInitParam(pParam)
{
    m_eChannelMode = (NULL == m_pChannelInitParam) ? CHANNEL_NULL : m_pChannelInitParam->m_eChannelMode;

    m_pChannelHandleParam = NULL;

    m_pChannelHandle = NULL;
}

CBaseChannelMode::~CBaseChannelMode()
{
#if VOICE_INFO_DEBUG
    VOICE_INFO("~CBaseChannelMode");
#endif //
    SAFE_DELETE(m_pChannelInitParam);

    SAFE_DELETE(m_pChannelHandle);

    SAFE_DELETE(m_pChannelHandleParam);
}

REAL_CHANNEL CBaseChannelMode::GetRealChannelParam()
{
    REAL_CHANNEL eRealChannel = GetRealChannel();

    if (NULL == GetChannelParam())
    {
        GenrateChannelParam(eRealChannel, m_eChannelMode);
    }

    VOICE_INFO("Base Mode channelmode[%d], realchannel[%d] param [%s]", m_eChannelMode,
               eRealChannel, GetParamTypeString(GetChannelParam()).c_str());

    return eRealChannel;
}

// 获取物理通道
REAL_CHANNEL CBaseChannelMode::GetRealChannel()
{
    REAL_CHANNEL eRealChannel = RC_INIT;

    if (NULL == m_pChannelInitParam)
    {
        return eRealChannel;
    }

    CHANNEL_MODE eMode = m_pChannelInitParam->m_eChannelMode;

    if (CHANNEL_HANDFREE == eMode)
    {
        eRealChannel = RC_HANDFREE;
    }
    else if (CHANNEL_MIC_ONLY == eMode)
    {
        eRealChannel = RC_MIC_ONLY;
    }
    else if (CHANNEL_SPEAKER_ONLY == eMode)
    {
        eRealChannel = RC_SPEAKER_ONLY;
    }
    else if (CHANNEL_HANDSET == eMode)
    {
        eRealChannel = RC_HANDSET;
    }
    else if (CHANNEL_NULL == eMode)
    {
        eRealChannel = RC_INIT;
    }

    //默认需要重置，获取默认的参数。
    SAFE_DELETE(m_pChannelHandleParam);

    return eRealChannel;
}

CBaseChannelHandle * CBaseChannelMode::GetRealChannelHandle(REAL_CHANNEL eRealChannel)
{
    return new CBaseChannelHandle(eRealChannel);
}

void CBaseChannelMode::GenrateChannelParam(REAL_CHANNEL eRealChannel, CHANNEL_MODE eChannelMode)
{
    // 最终都只会有handfree、handset、headset 几个通道，所以需要根据real channel，重新设置channel mode
    CHANNEL_MODE eNewMode = CorrectChannelMode(eRealChannel, eChannelMode);

    switch (eRealChannel)
    {
    case RC_HEADSET:
        m_pChannelHandleParam = new CHeadsetChannelParam(eNewMode);
        break;
    default:
        m_pChannelHandleParam = new CBaseChannelParam(eNewMode);
        break;
    }
    return;
}

bool CBaseChannelMode::SwitchChannel(REAL_CHANNEL eRealChannel)
{
    BeforeSwitchChannel(eRealChannel);
    ProcessSwitchChannel(eRealChannel);
    return AfterSwitchChannel(eRealChannel);
}

bool CBaseChannelMode::ProcessSwitchChannel(REAL_CHANNEL eRealChannel)
{
    if (NULL == GetChannelParam())
    {
        VOICE_ERR("m_pChannelHandleParam is NULL");
        return false;
    }

    VOICE_INFO("open realchannel realchannel[%d] channel mode[%d]", eRealChannel,
               GetChannelParam()->m_eChannelMode);

    if (NULL != m_pChannelHandle)
    {
        SAFE_DELETE(m_pChannelHandle);
    }

    m_pChannelHandle = GetRealChannelHandle(eRealChannel);

    if (NULL != m_pChannelHandle)
    {
        return m_pChannelHandle->OpenRealChannel(eRealChannel, GetChannelParam());
    }

    return false;
}

bool CBaseChannelMode::BeforeSwitchChannel(REAL_CHANNEL eRealChannel)
{
#if VOICE_INFO_DEBUG
    VOICE_INFO("BaseChannelMode BeforeSwitch handfree[%d] mode[%d]",
               VoiceManager.m_objDevices.GetHandfreeStatus(), m_eChannelMode);
#endif

    // 无可用通道，要清空通道参数
    if (CHANNEL_NULL == m_eChannelMode)
    {
        VoiceManager.ResetChannelParam(false);
    }

    return true;
}

bool CBaseChannelMode::AfterSwitchChannel(REAL_CHANNEL eRealChannel)
{
    // 无可用通道，要清空通道状态
    if (CHANNEL_NULL == m_eChannelMode)
    {
        VoiceManager.ResetChannelStatus(false);
    }
    return true;
}

yl::string CBaseChannelMode::GetParamTypeString(CBaseChannelParam * pParam)
{
    if (NULL == pParam)
    {
        return "";
    }

    if (NULL != dynamic_cast<CHeadsetChannelParam *>(pParam))
    {
        return "CHeadsetChannelParam";
    }

    return "CBaseChannelParam";
}

CHANNEL_MODE CBaseChannelMode::CorrectChannelMode(REAL_CHANNEL eRealChannel, CHANNEL_MODE eMode)
{
    CHANNEL_MODE eNewMode = eMode;

    if (RC_HANDFREE == eRealChannel)
    {
        if (eMode != CHANNEL_HANDFREE
                && eMode != CHANNEL_SPEAKER_ONLY
                && eMode != CHANNEL_MIC_ONLY)
        {
            eNewMode = CHANNEL_HANDFREE;
        }
    }
    else if (RC_HEADSET == eRealChannel)
    {
        if (eMode != CHANNEL_HEADSET
                && eMode != CHANNEL_2HEADSET
                && eMode != CHANNEL_BTHEADSET)
        {
            eNewMode = CHANNEL_HEADSET;
        }
    }
    else if (RC_HANDSET == eRealChannel)
    {
        eNewMode = CHANNEL_HANDSET;
    }

    VOICE_INFO("correct channel mode old[%d] to new[%d]", eMode, eNewMode);

    return eNewMode;
}

// 判断是否同一初始化参数对象
bool CBaseChannelMode::IsSameInitParam(CBaseInitParam * pInitParam)
{
    return (m_pChannelInitParam == pInitParam);
}

// 判断是否同一通话参数对象
bool CBaseChannelMode::IsSameChannelHandleParam(CBaseChannelParam * pHandleParam)
{
    return (m_pChannelHandleParam == pHandleParam);
}

// 判断是否同一通话处理对象
bool CBaseChannelMode::IsSameChannelHandle(CBaseChannelHandle * pHandle)
{
    return (m_pChannelHandle == pHandle);
}
#endif
