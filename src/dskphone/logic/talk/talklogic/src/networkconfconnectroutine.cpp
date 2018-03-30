#include "talklogic_inc.h"

// Routine的SIP消息响应
bool CNetworkConfConnectRoutine::OnSIPMsg(msgObject& msg)
{
    bool bHandled = false;

    switch (msg.message)
    {
    case SIP_CALL_ESTABLISH:
        {
            // establish有可能是hold消息
            if (Comm_IsHoldMsg(msg))
            {
                break;
            }

            bHandled = true;

            // 开启通话，而不是到TalkRoutine中开启，否则界面刷新会慢
            ipvp_speech_t voiceParam;
            memset(&voiceParam, 0, sizeof(ipvp_speech_t));

            sip_channels_v20_t sipChannelInfo;
            Comm_GetSipChannelsData(sipChannelInfo, msg);

            Comm_MediaParam_sip2ipvp(&sipChannelInfo, m_pCallInfo->m_strUserAgent, &voiceParam,
                                     &m_pCallInfo->m_stVideoExt);
            Comm_AudioCodec_sip2ipvp(m_pCallInfo->m_stMediaInfo.m_stAudio, &voiceParam);

            CBaseRoutine::StartTalkByIpvpParam(&voiceParam, m_pCallInfo, configParser_GetConfigInt(kszIsNeedHoldAferNetworkConf) == 1);

            // 跳转到对应的会议界面
            if (m_pCallInfo->GetConfType() == CT_BROADSOFT)
            {
                m_pCallSession->EnterRoutine(ROUTINE_NETWORKCONF);
            }
#if 0  // 华为会议不加入中性版本
            else if (m_pCallInfo->GetConfType() == CT_HUAWEI)
            {
                m_pCallSession->EnterRoutine(ROUTINE_ATSCONF);
            }
#endif
        }
        break;
    default:
        break;
    }

    return bHandled || CConnectingRoutine::OnSIPMsg(msg);
}

