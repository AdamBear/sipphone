#if IF_FEATURE_TALKINFO_REPORT
#include "talkreportprocesser.h"

CTalkInfoReportProcesser & CTalkInfoReportProcesser::GetInstance()
{
    static CTalkInfoReportProcesser m_sInstace;

    return m_sInstace;
}

bool CTalkInfoReportProcesser::SendEvent(EventDataBase * pData)
{
    bool bRet = false;

    switch (pData->m_eEventDataType)
    {
    case EDT_TALK:
        {
            bRet |= SendTalkEvent(pData);
        }
        break;
    case EDT_BASE:
        {
            bRet |= SendBaseEvent(pData);
        }
        break;
    default:
        break;
    }
    return bRet;
}

bool CTalkInfoReportProcesser::SendTalkEvent(EventDataBase * pData)
{
    bool bRet = true;
    TalkEventData * pTalkEventData = static_cast<TalkEventData *>(pData);

    int nCallID = -1;
    if (pTalkEventData->m_vecCallID.size() > 0)
    {
        nCallID = pTalkEventData->m_vecCallID[0];
    }
    else
    {
        return false;
    }

    TALK_DEBUG("CTalkInfoReportProcesser::SendTalkEvent call id[%d] event type[%d]", nCallID, pTalkEventData->m_eEventType);

    switch (pTalkEventData->m_eEventType)
    {
    case EVENT_ESTABLISHED:
        {
            const CCallInfo * pCallInfo = talklogic_GetCallInfoByCallId(nCallID);
            if (pCallInfo == NULL)
            {
                ACTION_WARN("Can not Get CallInfo by CallID[%d]", nCallID);
                return false;
            }

            if (pCallInfo->m_eCallType == CT_INCOMING)
            {
                //来电接起建立通话
                msgBroadpostThreadMsg(DSK_MSG_TALK_STATS_INFO, INCOMING_CALLS_ANSWERED, pCallInfo->GetAccountID());
            }
            else if (pCallInfo->m_eCallType == CT_OUTGOING)
            {
                //呼出建立通话
                msgBroadpostThreadMsg(DSK_MSG_TALK_STATS_INFO, OUTGOING_CALLS_ANSWERED, pCallInfo->GetAccountID());
            }
            //发送通话callid
            yl::string strCallid = commonAPI_FormatString("%d", nCallID);
            msgBroadpostThreadMsgEx(DSK_MSG_TALK_STATS_INFO, TALK_INFO_CALL_ID, pCallInfo->GetAccountID(), strCallid.length() + 1, strCallid.c_str());
        }
        break;
    case EVENT_INCOMING_CALL:
        {
            const CCallInfo * pCallInfo = talklogic_GetCallInfoByCallId(nCallID);
            if (pCallInfo == NULL)
            {
                ACTION_WARN("Can not Get CallInfo by CallID[%d]", nCallID);
                return false;
            }

            if (!pCallInfo->m_tRemoteInfo.sSIPName.empty())
            {
                //进入响铃的来电数
                msgBroadpostThreadMsg(DSK_MSG_TALK_STATS_INFO, INCOMING_CALLS_CONNECTED, pCallInfo->GetAccountID());
            }
        }
        break;
    case EVENT_TERMINATED:
    case EVENT_END_DIAL:
        {
            const CCallInfo * pCallInfo = talklogic_GetCallInfoByCallId(nCallID);
            if (pCallInfo == NULL
                    || pCallInfo->m_tRemoteInfo.sSIPName.empty()
                    || pCallInfo->m_iCallDuration <= 0)
            {
                ACTION_WARN("Can not Get CallInfo by CallID[%d]", nCallID);
                return false;
            }

            yl::string strCallDuration = commonAPI_FormatString("%d", pCallInfo->m_iCallDuration);
            yl::string strCallid = commonAPI_FormatString("%d", nCallID);

            //通话结束，上报通话时长
            msgBroadpostThreadMsgEx(DSK_MSG_TALK_STATS_INFO, TOTAL_CALL_TIME, pCallInfo->GetAccountID(), strCallDuration.length() + 1, strCallDuration.c_str());

            //通话结束时的callid，TR069需要通过该消息更新保存的当前通话的callid
            msgBroadpostThreadMsgEx(DSK_MSG_TALK_STATS_INFO, CALL_END_CALLID, pCallInfo->GetAccountID(), strCallid.length() + 1, strCallid.c_str());
        }
        break;
    case EVENT_RINGBACK:
        {
            //进入回铃，呼出建立连接
            msgBroadpostThreadMsg(DSK_MSG_TALK_STATS_INFO, OUTGOING_CALLS_CONNECTED, pTalkEventData->m_wParam);
        }
        break;
    case EVENT_OUTGOING_CALLS_ATTEMPTED:
        {
            //尝试呼出
            msgBroadpostThreadMsg(DSK_MSG_TALK_STATS_INFO, OUTGOING_CALLS_ATTEMPTED, pTalkEventData->m_wParam);
        }
        break;
    case EVENT_CALL_BY_REPLACED:
        {
            yl::string strCallid = commonAPI_FormatString("%d", nCallID);
            //咨询转，callid改变
            msgBroadpostThreadMsgEx(DSK_MSG_TALK_STATS_INFO, TALK_INFO_CALL_ID, pTalkEventData->m_wParam, strCallid.length() + 1, strCallid.c_str());
        }
        break;
    default:
        {
            bRet = false;
        }
        break;
    }

    return bRet;
}

bool CTalkInfoReportProcesser::SendBaseEvent(EventDataBase * pData)
{
    bool bRet = true;
    switch (pData->m_eEventType)
    {
    case EVENT_INCOMING_CALLS_RECEIVED:
        {
            //总的收到的来电数
            msgBroadpostThreadMsg(DSK_MSG_TALK_STATS_INFO, INCOMING_CALLS_RECEIVED, pData->m_wParam);
        }
        break;
    default:
        {
            bRet = false;
        }
        break;
    }

    return bRet;
}

CTalkInfoReportProcesser::CTalkInfoReportProcesser() : CEventBase(EMT_TALK_INFO_REPORT)
{
    RegisterModule();
}

CTalkInfoReportProcesser::~CTalkInfoReportProcesser()
{
    UnRegisterModule();
}

#endif
