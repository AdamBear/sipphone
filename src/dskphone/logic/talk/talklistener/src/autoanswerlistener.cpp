#include "talklistener_inc.h"

CAutoAnswerListener::CAutoAnswerListener()
    : LSN_CLS_LOG_NAME
{
    // 该流程
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_ROUTINE_CHANGED, CAutoAnswerListener::OnDealAutoAnswer);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_PRE_DELETE, CAutoAnswerListener::OnSessionDelete);
    RegETLMsg(SIP_CMD_CALL_ACCEPT);
}

DECL_SIGNAL_HANDLER(CAutoAnswerListener::OnSessionDelete)
{
    CSessionStateProxy sessionSp(sigp.iSessionID);
    ROUTINE_STATE eState = sessionSp.GetRoutineState();
    if (sessionSp.IsInherits(ROUTINE_RINGING))
    {
        if (priv)
        {
            priv->m_AutoRefuseProxy.Kill();
        }
    }
    return true;
}

DECL_SIGNAL_HANDLER(CAutoAnswerListener::OnDealAutoAnswer)
{
    CSessionStateProxy session = CSessionProxy(sigp.iSessionID);
    CCallInfo* pCallInfo = CSessionProxy(sigp.iSessionID).GetCallInfo();
    if (pCallInfo == NULL)
    {
        return false;
    }

    if (!session.IsInherits(ROUTINE_RINGING))
    {
        return false;
    }

    if (priv == NULL)
    {
        return false;
    }
    //初始化标记
    pCallInfo->m_bIsAutoAnswerCall = false;

    ROUTINE_STATE eRoutine = session.GetRoutineState();
    if (eRoutine == ROUTINE_RINGING)
    {
        // 启动自动拒接部分
#ifdef IF_FEATURE_PSTN
        if (!acc_IsPstnAccount(pCallInfo->GetAccountID()))
#endif // IF_FEATURE_PSTN
        {
            // 初始化自动拒接的时间
            priv->m_AutoRefuseProxy.SetAutoRefuseCallTimer(session);
        }

        // 自动应答,分为本地的和sip参数指定的
        if (priv->m_AutoAnserProxy.ProcessAutoAnswer(pCallInfo))
        {
            TiptelA_ResetFlage();
            pCallInfo->m_bIsAutoAnswerCall = true;
            TALK_INFO("auto answer incomming call");
            SIG_REPLY(true);
        }
    }
    else if (eRoutine == ROUTINE_RECALL)
    {
        if (priv->m_AutoAnserProxy.ProcessAccountAutoAnswer(pCallInfo))
        {
            pCallInfo->m_bIsAutoAnswerCall = true;
            TALK_INFO("auto answer incomming call");
            SIG_REPLY(true);
        }
    }
    return true;
}


bool CAutoAnswerListener::OnMessage(msgObject& msg)
{
    bool bRet = true;
    switch (msg.message)
    {
    case SIP_CMD_CALL_ACCEPT:
        {
            CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(msg.lParam);
            CListenInfo* pInfoData = GetCallerData(pCallInfo);
            if (pCallInfo == NULL || pInfoData == NULL)
            {
                bRet = false;
                break;
            }

            if (!pCallInfo->m_bAnswered)
            {
                //RingRoutine处理SIP_CMD_CALL_ACCEPT消息当作Intercom方式来处理
                pCallInfo->m_bIsIntercom = true;
                if (pInfoData->m_AutoAnserProxy.ProcessIntercom(pCallInfo)
                        && pCallInfo->IsPreForword())
                {
                    pCallInfo->SetFWD(false);
                }
            }
        }
        break;
    default:
        break;
    }
    return bRet;
}
