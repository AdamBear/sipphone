///////////////////////////////////////////////////////////
//  CHeadSetChannelMode.cpp
//  Implementation of the Class CHeadSetChannelMode
//  Created on:      18-12月-2015 16:46:33
//  Original author: Song
///////////////////////////////////////////////////////////
#include "voicechannel_include.h"

#ifndef _DECT

CHeadSetChannelMode::CHeadSetChannelMode(CBaseInitParam * pParam) :
    CBaseChannelMode(pParam)
{

}

CHeadSetChannelMode::~CHeadSetChannelMode()
{

}

REAL_CHANNEL CHeadSetChannelMode::GetRealChannel()
{
    bool bEHS = false;
    CHANNEL_MODE eMode;

    if (NULL != m_pChannelInitParam)
    {
        eMode = m_pChannelInitParam->m_eChannelMode;
    }

    CHeadsetInitParam * pHeadsetInitParam = dynamic_cast<CHeadsetInitParam *>(m_pChannelInitParam);
    if (NULL != pHeadsetInitParam)
    {
        bEHS = pHeadsetInitParam->m_bEHS;
    }

    REAL_CHANNEL eRealChannel = RC_INIT;
    if (CHANNEL_HEADSET == eMode
            || CHANNEL_BTHEADSET == eMode)
    {
        eRealChannel = RC_HEADSET;
    }
    else if (CHANNEL_2HEADSET == eMode)
    {
        eRealChannel = RC_2HEADSET;
    }

    SAFE_DELETE(m_pChannelHandleParam);

    m_pChannelHandleParam = new CHeadsetChannelParam(eMode, bEHS);

    return eRealChannel;
}

bool CHeadSetChannelMode::ProcessSwitchChannel(REAL_CHANNEL eRealChannel)
{
    // 蓝牙通道需要先将物理通道设置为RC_INIT,
    // 同时蓝牙耳麦通道队外也是耳麦通道
    if (NULL != m_pChannelHandleParam
            && CHANNEL_BTHEADSET == m_pChannelHandleParam->m_eChannelMode)
    {
        _VPMMSGHandler.SetRealChannel(RC_INIT);
        m_pChannelHandleParam->m_eChannelMode = CHANNEL_HEADSET;
    }

    return CBaseChannelMode::ProcessSwitchChannel(eRealChannel);
}
#endif
