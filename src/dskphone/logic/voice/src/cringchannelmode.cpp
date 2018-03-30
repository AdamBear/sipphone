///////////////////////////////////////////////////////////
//  CRingChannelMode.cpp
//  Implementation of the Class CRingChannelMode
//  Created on:      17-12月-2015 16:57:42
//  Original author: Song
///////////////////////////////////////////////////////////
#include "voicechannel_include.h"
#ifndef _DECT

CRingChannelMode::CRingChannelMode(CBaseInitParam * pParam) :
    CBaseChannelMode(pParam)
{

}

CRingChannelMode::~CRingChannelMode()
{

}

REAL_CHANNEL CRingChannelMode::GetRealChannel()
{
    REAL_CHANNEL eRealChannel = _VPMMSGHandler.GetRealChannel();
    if (VoiceManager.IsRingHeadset())
    {
        // 打开物理通道
        if ((RING_DEVICE) configParser_GetConfigInt(kszRingerDevice) == RD_HEADSET_GROUP)
        {
            eRealChannel = RC_GROUP_HEADSET;
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
    return  eRealChannel;
}

bool CRingChannelMode::BeforeSwitchChannel(REAL_CHANNEL eRealChannel)
{
    if ((RC_HEADSET == eRealChannel || RC_GROUP_HEADSET == eRealChannel)
            && VoiceManager.IsRingHeadset())
    {
        // 通道切到耳麦时，耳麦一定是开启的
        VoiceManager.m_objDevices.SetHeadsetStatus(true);
    }

    if (talklogic_IsAllSessionRinging())
    {
        // bugid:23125
        // 响铃不影响免提状态
        VoiceManager.m_objDevices.SetHandfreeStatus(false);
    }

    return true;
}

bool CRingChannelMode::ProcessSwitchChannel(REAL_CHANNEL eRealChannel)
{
    SAFE_DELETE(m_pChannelHandle);

    // 获取基础的处理类,
    // TODO 此处会造成先切换成其他通道，再切换回铃声通道，有冗余的操作
    m_pChannelHandle = GetRealChannelHandle(RC_INIT);

    if (NULL != m_pChannelHandle)
    {
        voice_SetCurrentChannel(CHANNEL_RING, true);
        return m_pChannelHandle->OpenRealChannel(eRealChannel, m_pChannelHandleParam);
    }

    return false;
}

bool CRingChannelMode::AfterSwitchChannel(REAL_CHANNEL eRealChannel)
{
    return true;
}
#endif
