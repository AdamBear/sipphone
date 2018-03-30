#include "talklistener_inc.h"

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER

CCallCenterListener::CCallCenterListener()
    : LSN_CLS_LOG_NAME
{
    REG_SYNC_SIGNAL(TALK_SIG_ACTION_EVENT, CCallCenterListener::OnUIAction);
    REG_SYNC_SIGNAL(TALK_SIG_GET_DATA_TO_UI, CCallCenterListener::OnGetDataToUI);

#if IF_FEATURE_START2START_ACD
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_PRE_DELETE, CCallCenterListener::OnSessionDestory);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_ROUTINE_CHANGED, CCallCenterListener::OnRoutineChanged);
#endif //IF_FEATURE_START2START_ACD

    RegETLMsg(SIP_CALL_INFORMATION_UPDATE);
}

DECL_SIGNAL_HANDLER(CCallCenterListener::OnUIAction)
{
    DataUI2Logic* pDataUI2Logic = (DataUI2Logic*)sigp.wParam.pValue;
    CCallInfo* pCallInfo = CSessionProxy(sigp.iSessionID).GetCallInfo();
    if (pCallInfo == NULL
            || pDataUI2Logic == NULL
            || priv == NULL)
    {
        return false;
    }

    switch (pDataUI2Logic->eAction)
    {
    case TALK_ACTION_CALLINFO:
        {
            priv->SetCCInfo(true);
            SIG_REPLY(true);
        }
        break;
    case TALK_ACTION_CANCEL:
    case TALK_ACTION_KEYPRESS:
    case TALK_ACTION_SCREEN_CLICK:
    case TALK_ACTION_EXIT_FAILD_INFO:
        {
            if (pCallInfo->IsInherits(ROUTINE_TALKING)
                    && priv->IsCCShow())
            {
                priv->SetCCInfo(false);
                return true;
            }
        }
        break;
    default:
        return false;
    }

    return false;
}

#if IF_FEATURE_START2START_ACD
DECL_SIGNAL_HANDLER(CCallCenterListener::OnSessionDestory)
{
    CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(sigp.iCallID);
    if (NULL == pCallInfo
            || TCT_START2START_ACD_AUTO_LOGIN != pCallInfo->m_eTalkCallOutType)
    {
        return true;
    }

    // 通话结束，立即查询ACD状态
    etl_NotifyApp(false, ACD_MESSAGE_ACTION_AUTO_REFRESH, 0, 0);
    return true;
}

DECL_SIGNAL_HANDLER(CCallCenterListener::OnRoutineChanged)
{
    CSessionProxy sp(sigp.iSessionID);
    CCallInfo* pCallInfo = sp.GetCallInfo();
    if (pCallInfo == NULL || priv == NULL)
    {
        return false;
    }

    if (ROUTINE_RINGING == sigp.wParam.iValue)
    {
        priv->SetCCInfo(false);
    }

    if (!priv->m_bReadySendedS2SDmtf
            || TCT_START2START_ACD_AUTO_LOGIN != pCallInfo->m_eTalkCallOutType)
    {
        return true;
    }

    if (ROUTINE_TALKING == pCallInfo->m_eStateOfRoutine
            || ROUTINE_RINGBACK == pCallInfo->m_eStateOfRoutine
            && pCallInfo->m_bIsColorRingback)
    {
        priv->m_bReadySendedS2SDmtf = false;
        priv->m_TimerS2S.SetCallBack(MAKE_TIMER_CALL_BACK(CCallCenterListener::OnTimer));

        int iTimer = CallCenter_GetNextInterval();
        if (iTimer > 0)
        {
            priv->m_TimerS2S.Push(sigp.iSessionID);
            priv->m_TimerS2S.SetTimerOnce(iTimer);
        }
    }

    return true;
}
#endif //IF_FEATURE_START2START_ACD

DECL_SIGNAL_HANDLER(CCallCenterListener::OnGetDataToUI)
{
    CSessionProxy sp(sigp.iSessionID);
    ROUTINE_STATE eState = sp.GetRoutineState();
    if (!sp.IsInherits(ROUTINE_RINGING))
    {
        return false;
    }

    DataRing* pRingData = (DataRing*)sigp.wParam.pValue;
    if (pRingData == NULL || !pRingData->bAllowFwd)
    {
        return false;
    }

    //flexible seating禁用来电时的Forward
    if (CallCenter_IsFlexibleSeatingOn(sp.GetAccountID()))
    {
        pRingData->bAllowFwd = false;
        return true;
    }

    return false;
}

#if IF_FEATURE_START2START_ACD
int CCallCenterListener::OnTimer(CScopedTimer& timer)
{
    CCallInfo* pCallInfo = talklogic_GetCallInfoBySessionID(timer.Get());
    if (pCallInfo == NULL)
    {
        return false;
    }

    yl::string strNextParam = CallCenter_GetNextCallOutParam();
    if (!strNextParam.empty())
    {
        return false;
    }

    talklogic_SendDtmf(pCallInfo->GetCallID(), strNextParam);

    int iTimer = CallCenter_GetNextInterval();
    if (iTimer > 0)
    {
        timer.SetTimerOnce(iTimer);
    }

    return 1;
}
#endif //IF_FEATURE_START2START_ACD

bool CCallCenterListener::OnMessage(msgObject& msg)
{
    if (SIP_CALL_INFORMATION_UPDATE == msg.message)
    {
        // 判断是否是要处理的消息类型
        sip_call_infomation_header_t* pInfo = (sip_call_infomation_header_t*)msg.GetExtraData();
        if (NULL == pInfo || SIP_CALL_INFORMATION_BSFT_CALL_CENTER_INFO != pInfo->type)
        {
            return false;
        }

        // 判断功能是否开启
        if (!CallCenter_IsCallInfoOn())
        {
            return false;
        }

        CSessionProxy proxy(talklogic_GetCallInfoByCallId(msg.lParam));
        // 判断是否是来电界面
        if (!proxy.IsRinging())
        {
            return false;
        }

        return SetCallCenterInfo(proxy.GetCallInfo(), pInfo->payload);
    }

    return false;
}

bool CCallCenterListener::SetCallCenterInfo(CCallInfo* pCallInfo, const yl::string& strInfo)
{
    CListenInfo* priv = GetCallerData(pCallInfo);
    if (pCallInfo == NULL || priv == NULL)
    {
        return false;
    }

    TALK_INFO("CRingingRoutine::Set call center info: strInfo = %s", strInfo.c_str());

    //增加Xml头
    xml_document doc;
    xml_parse_result objResult = doc.load_buffer(strInfo.c_str(), strInfo.length());
    if (objResult.status == status_ok)
    {
        //获取根结点
        xml_node nodeRoot = doc.first_child();
        if (nodeRoot.empty())
        {
            return false;
        }

        CallCenterInfo& CCInfo = pCallInfo->GetCCInfo();

        xml_node xmlSection = nodeRoot.first_child();
        while (!xmlSection.empty())
        {
            yl::string strSectionName = xmlSection.name();
            if (strstr(strSectionName.c_str(), "waitTime") != NULL)
            {
                //获取WaitTime值
                CCInfo.m_nWaitTime = atoi(xmlSection.text().get());
            }
            else if (strstr(strSectionName.c_str(), "ccUserId") != NULL)
            {
                //获取UserId值
                CCInfo.m_strUserId = xmlSection.text().get();
            }
            else if (strstr(strSectionName.c_str(), "callCenterName") != NULL)
            {
                //获取CallCenterName值
                CCInfo.m_strCallCenterName = xmlSection.text().get();
            }
            else if (strstr(strSectionName.c_str(), "numCallsInQueue") != NULL)
            {
                //获取CountInQueue值
                CCInfo.m_nCountInQueue = atoi(xmlSection.text().get());
            }
            else if (strstr(strSectionName.c_str(), "longestWaitingTime") != NULL)
            {
                //获取CountInQueue值
                CCInfo.m_nLongestTime = atoi(xmlSection.text().get());
            }

            xmlSection = xmlSection.next_sibling();
        }
        priv->SetCCInfo(true);
        return true;
    }

    return false;
}

#endif //IF_FEATURE_BROADSOFT_CALL_CENTER
