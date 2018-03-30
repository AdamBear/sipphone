///////////////////////////////////////////////////////////
//  CIdleChannelMode.cpp
//  Implementation of the Class CIdleChannelMode
//  Created on:      17-12月-2015 16:57:41
//  Original author: Song
///////////////////////////////////////////////////////////
#include "voicechannel_include.h"

#ifndef _DECT

CIdleChannelMode::CIdleChannelMode(CBaseInitParam * pParam) :
    CBaseChannelMode(pParam)
{

}

CIdleChannelMode::~CIdleChannelMode()
{

}

REAL_CHANNEL CIdleChannelMode::GetRealChannel()
{
#ifdef IF_FEATURE_HEADSET_PRIOR
    bool bHeadsetPrior = (configParser_GetConfigInt(kszHeadsetPriorSwitch) != 0);
    bool bIsLastHeadSet = (configParser_GetConfigInt(kszLastHeadSet) != 0);

    VOICE_INFO("OpenIdleChannel [%d][%d][%d]",
               VoiceManager.m_objDevices.GetHeadsetStatus(), bHeadsetPrior, bIsLastHeadSet);
#else
    VOICE_INFO("OpenIdleChannel [%d]", VoiceManager.m_objDevices.GetHeadsetStatus());
#endif

    REAL_CHANNEL eRealChannel = RC_HANDSET;
    // 优先级：耳麦=>手柄
    if (VoiceManager.m_objDevices.GetHeadsetStatus()
            // 耳麦控制通话, 声道不保留在耳麦
            && (bHeadsetPrior
                || configParser_GetConfigInt(kszHeadsetCtrlCall) == 0))
    {
        eRealChannel = RC_HEADSET;
    }
    else
    {
#ifdef IF_FEATURE_HEADSET_PRIOR
        if (bHeadsetPrior
                && bIsLastHeadSet)
        {
            // 耳麦优先模式：通话中关闭耳麦，返回Idle后再打开（只要通话中间过程有开启过）
            eRealChannel = RC_HEADSET;
            SAFE_DELETE(m_pChannelHandleParam);

            m_pChannelHandleParam = new CHeadsetChannelParam(CHANNEL_HEADSET, TRUE);
        }
        else
#endif
        {
            eRealChannel = RC_HANDSET;
        }
    }

    return eRealChannel;
}

bool CIdleChannelMode::BeforeSwitchChannel(REAL_CHANNEL eRealChannel)
{
    // 先将通道置为Idle，避免在OpenHeadset/OpenHandset中改变通道失败
    voice_SetCurrentChannel(CHANNEL_IDLE, true);
    return true;
}

bool CIdleChannelMode::AfterSwitchChannel(REAL_CHANNEL eRealChannel)
{
    // 再将通道设置回Idle
    voice_SetCurrentChannel(CHANNEL_IDLE, true);
    return true;
}
#endif
