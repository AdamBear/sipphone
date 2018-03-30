#include "vqrtcp_inc.h"

#ifdef IF_FEATURE_RTCPXR

static LRESULT OnMSGReceive(msgObject& msg)
{
    switch (msg.message)
    {
    case TM_TIMER:
        {
            return _VQReportManager.OnTimer(msg.wParam);
        }
    case IPVP_MSG_AUDIO_VQREPORT:
        {
            return _VQReportManager.ProcessVQReportMsg(msg);
        }
    case VOICE_VPM_START:
        {
            int nRet = ipvp_request_notify(mkGetHandle(), IPVP_MSG_AUDIO_VQREPORT);

            TALK_INFO("RTCP-XR ipvp_request_notify = %d", nRet);
        }
        break;
    case DSK_MSG_GET_RTP_STATS:
        {
            // web 显示rtp stats
            _VQReportManager.SaveSessionReportDataToWeb();

            // web 使用128的数组存储
            msg.ReplyMessageEx(1, sizeof(szSessionReportFile) + 1, (void*)szSessionReportFile);
        }
        break;
    case SIP_CALL_REPLACED_BY_REMOTE:
        {
            if (msg.GetExtraData() != NULL)
            {
                int iSessionID = talklogic_GetSessionIdByCallID(msg.lParam, false);
                _VQReportManager.OnFocusSessionChange(iSessionID);
            }
        }
        break;
    default:
        return FALSE;
    }

    return TRUE;
}

void vqrtcp_Init()
{
    _VQReportManager.Init();
    SingleMsgReg(VOICE_VPM_START, OnMSGReceive);
    SingleMsgReg(IPVP_MSG_AUDIO_VQREPORT, OnMSGReceive);
    SingleMsgReg(TM_TIMER, OnMSGReceive);
    SingleMsgReg(DSK_MSG_GET_RTP_STATS, OnMSGReceive);
    SingleMsgReg(SIP_CALL_REPLACED_BY_REMOTE, OnMSGReceive);
}

void vqrtcp_UnInit()
{

}

// 获取最后一次通话的语音质量数据
void vqrtcp_RTPStatusGetSessionReport(SVQReportData& sData)
{
    _VQReportManager.GetSessionReportData(sData);
}

// 获取通话中的语音质量数据
bool vqrtcp_RTPStatusGetCurrentReport(SVQReportData& sData)
{
    int nCallId = _VQReportManager.GetActioveCallId();
    _VQReportManager.GetCurrentReportData(nCallId, sData);

    return true;
}

bool vqrtcp_IsRTPStatusSwitch()
{
    return _VQReportManager.IsRTPStatusSwitch();
}

bool vqrtcp_RTPStatusSwitch()
{
    return _VQReportManager.SwitchRTPStatus();
}

#else

void vqrtcp_Init()
{
}

void vqrtcp_UnInit()
{
}

// 获取最后一次通话的语音质量数据
void vqrtcp_RTPStatusGetSessionReport(SVQReportData& sData)
{
}

// 获取通话中的语音质量数据
bool vqrtcp_RTPStatusGetCurrentReport(SVQReportData& sData)
{
    return false;
}

bool vqrtcp_IsRTPStatusSwitch()
{
    return false;
}

bool vqrtcp_RTPStatusSwitch()
{
    return false;
}

#endif
