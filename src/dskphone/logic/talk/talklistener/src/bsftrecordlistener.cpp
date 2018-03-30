#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
#include "talklistener_inc.h"

CBroadsoftRecordListener::CBroadsoftRecordListener()
    : LSN_CLS_LOG_NAME
{
    RegETLMsg(CONFIG_MSG_BCAST_CHANGED);
    RegETLMsg(XSI_MESSAGE_REQ_FAIL);
    RegETLMsg(XSI_MESSAGE_REQ_FINISH);
    RegETLMsg(SIP_CALL_BW_CALL_RECORD_STATUS);

    REG_SYNC_SIGNAL(TALK_SIG_ACTION_EVENT, CBroadsoftRecordListener::OnUIAction);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_AFTER_CREATE, CBroadsoftRecordListener::OnSessionCreate);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_ROUTINE_CHANGED, CBroadsoftRecordListener::OnRoutineChanged);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_ADD_TO_NET_CONF, CBroadsoftRecordListener::OnAddToNetConf);
    REG_SYNC_SIGNAL(TALK_SIG_GET_DATA_TO_UI, CBroadsoftRecordListener::OnGetDataToUI);
}

bool CBroadsoftRecordListener::OnMessage(msgObject & msg)
{
    bool ret = false;
    switch (msg.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
        ret = OnModeConfigChanged(msg);
        break;
    case XSI_MESSAGE_REQ_FAIL:
    case XSI_MESSAGE_REQ_FINISH:
        ret = OnXSIReqResult(msg);
        break;
    case SIP_CALL_BW_CALL_RECORD_STATUS:
        ret = OnSIPRecordStatus(msg);
        break;
    default:
        break;
    }
    return ret;
}

bool CBroadsoftRecordListener::OnModeConfigChanged(msgObject& msg)
{
    if (ST_CALLRECORD_MODE != msg.wParam)
    {
        return true;
    }

    YLVector<int> vecSessionID;
    talklogic_GetAllSessionID(vecSessionID, false);

    for (YLVector<int>::iterator it = vecSessionID.begin(); it != vecSessionID.end(); ++it)
    {
        CSessionProxy proxy(*it);
        if (!proxy.IsSingle() || proxy.GetHost())
        {
            continue;
        }

        CCallInfo* pCallInfo = proxy.GetCallInfo();
        if (pCallInfo != NULL && msg.lParam == pCallInfo->GetAccountID())
        {
            pCallInfo->m_eRecordStatus = CALL_RECORD_UNKNOWN;
        }
    }

    return true;
}

bool CBroadsoftRecordListener::OnXSIReqResult(msgObject& msg)
{
    TALK_INFO("CBroadsoftRecordListener::OnXSIReqResult msg:%d wp:%d lp:%d",
              msg.message, msg.wParam, msg.lParam);
    // 只处理Call Record相关请求结果
    if (msg.wParam != XSI_CALL_RECORD_START
            && msg.wParam != XSI_CALL_RECORD_STOP
            && msg.wParam != XSI_CALL_RECORD_PAUSE
            && msg.wParam != XSI_CALL_RECORD_RESUME)
    {
        return false;
    }

    // 获取Call ID
    int iCallId = -1;
    if (msg.message == XSI_MESSAGE_REQ_FINISH && msg.GetExtraData() != NULL)
    {
        iCallId = *(int*)msg.GetExtraData();
    }
    else if (msg.message == XSI_MESSAGE_REQ_FAIL)
    {
        SXsiExtenInfo* pData = (SXsiExtenInfo*)msg.GetExtraData();
        if (pData != NULL)
        {
            iCallId = pData->m_nEntenParm;
        }
    }

    // 根据结果设置状态
    bool bResult = msg.message == XSI_MESSAGE_REQ_FINISH;
    CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(iCallId);
    if (NULL == pCallInfo)
    {
        return false;
    }

    switch (msg.wParam)
    {
    case XSI_CALL_RECORD_START:
        {
            if (bResult)
            {
                pCallInfo->m_eRecordStatus = CALL_RECORD_START;
                pCallInfo->m_eRecordResult = CALL_RECORD_START_SUCCESS;
            }
            else
            {
                pCallInfo->m_eRecordResult = CALL_RECORD_START_FAILED;
            }
        }
        break;
    case XSI_CALL_RECORD_STOP:
        {
            if (bResult)
            {
                pCallInfo->m_eRecordStatus = CALL_RECORD_NORMAL;
                pCallInfo->m_eRecordResult = CALL_RECORD_STOP_SUCCESS;
            }
            else
            {
                pCallInfo->m_eRecordResult = CALL_RECORD_STOP_FAILED;
            }
        }
        break;
    case XSI_CALL_RECORD_PAUSE:
        {
            if (bResult)
            {
                pCallInfo->m_eRecordStatus = CALL_RECORD_PAUSE;
                pCallInfo->m_eRecordResult = CALL_RECORD_PAUSE_SUCCESS;
            }
            else
            {
                pCallInfo->m_eRecordResult = CALL_RECORD_PAUSE_FAILED;
            }
        }
        break;
    case XSI_CALL_RECORD_RESUME:
        {
            if (bResult)
            {
                pCallInfo->m_eRecordStatus = CALL_RECORD_START;
                pCallInfo->m_eRecordResult = CALL_RECORD_RESUME_SUCCESS;
            }
            else
            {
                pCallInfo->m_eRecordResult = CALL_RECORD_RESUME_FAILED;
            }
        }
        break;
    default:
        break;
    }

    TALK_INFO("CBroadsoftRecordListener::OnXSIReqResult CallId[%d] eRecordResutl[%d]", iCallId,
              pCallInfo->m_eRecordResult);

    return true;
}

bool CBroadsoftRecordListener::OnSIPRecordStatus(msgObject& msg)
{
    CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(msg.lParam);
    bw_call_record_t* pInfo = (bw_call_record_t*)msg.GetExtraData();
    if (NULL == pInfo || NULL == pCallInfo)
    {
        return true;
    }

    CALL_RECORD_STATUE eLastStatue = pCallInfo->m_eRecordStatus;
    CALL_RECORD_ACTION_RESULT eLastResult = pCallInfo->m_eRecordResult;

    pCallInfo->m_eRecordResult = CALL_RECORD_NONE;
    if (pInfo->op_result == 0)
    {
        switch (pInfo->audio_record_status)
        {
        case SIP_BW_CALL_RECORD_OFF:
            {
                if (eLastStatue == CALL_RECORD_NORMAL
                        && eLastResult == CALL_RECORD_ACTION_PROCESSING)
                {
                    pCallInfo->m_eRecordResult = CALL_RECORD_START_FAILED;
                }
                else
                {
                    pCallInfo->m_eRecordStatus = CALL_RECORD_NORMAL;
                }
            }
            break;
        case SIP_BW_CALL_RECORD_ON: // pause stop
            {
                if (eLastStatue == CALL_RECORD_START
                        && eLastResult == CALL_RECORD_ACTION_PROCESSING)
                {
                    pCallInfo->m_eRecordResult = CALL_RECORD_STOP_FAILED;
                }
                else
                {
                    pCallInfo->m_eRecordStatus = CALL_RECORD_START;
                }
            }
            break;
        case SIP_BW_CALL_RECORD_PAUSED:
            {
                if (eLastStatue == CALL_RECORD_PAUSE
                        && eLastResult == CALL_RECORD_ACTION_PROCESSING)
                {
                    pCallInfo->m_eRecordResult = CALL_RECORD_RESUME_FAILED;
                }
                else
                {
                    pCallInfo->m_eRecordStatus = CALL_RECORD_PAUSE;
                }
            }
            break;
        default:
            break;
        }
    }
    else if (eLastResult == CALL_RECORD_ACTION_PROCESSING)
    {
        if (eLastStatue == CALL_RECORD_NORMAL)
        {
            pCallInfo->m_eRecordResult = CALL_RECORD_START_FAILED;
        }
        else if (eLastStatue == CALL_RECORD_START)
        {
            pCallInfo->m_eRecordResult = CALL_RECORD_STOP_FAILED;
        }
        else if (eLastStatue == CALL_RECORD_PAUSE)
        {
            pCallInfo->m_eRecordResult = CALL_RECORD_RESUME_FAILED;
        }
    }

    TALK_INFO("Before Call Record eLastStatue[%d] eLastResult[%d] CurrStatue[%d] Result[%d] eRecordResult[%d]",
              eLastStatue, eLastResult, pInfo->audio_record_status, pInfo->op_result, pCallInfo->m_eRecordResult);

    return true;
}

DECL_SIGNAL_HANDLER(CBroadsoftRecordListener::OnUIAction)
{
    DataUI2Logic* pDataUI2Logic = (DataUI2Logic*)sigp.wParam.pValue;
    if (pDataUI2Logic == NULL)
    {
        return false;
    }

    TALK_ACTION_TYPE eAction = pDataUI2Logic->eAction;

    CSessionProxy sessionProxy(sigp.iSessionID);
    if (sessionProxy.IsSingle())
    {
        ProcessUIAction(sessionProxy, eAction);
    }
    else if (sessionProxy.IsLocalConf())
    {
        YLVector<CSessionProxy> vec;
        if (!sessionProxy.GetSubSession(vec))
        {
            return false;
        }

        YLVector<CSessionProxy>::iterator it = vec.begin();
        for (; it != vec.end(); ++it)
        {
            ProcessUIAction(*it, eAction);
        }
    }

    return true;
}

bool CBroadsoftRecordListener::ProcessUIAction(const CSessionProxy& sessionProxy,
        TALK_ACTION_TYPE eAction)
{
    CCallInfo* pCallInfo = sessionProxy.GetCallInfo();
    // 可能为NULL http://bugfree.yealink.com/Bug.php?BugID=132413
    if (NULL == pCallInfo)
    {
        return false;
    }
    bool bNetworkConf = (ROUTINE_NETWORKCONF == sessionProxy.GetRoutineState());
    bool bResult = true;
    switch (eAction)
    {
    case TALK_ACTION_RECORD_START:
        {
            bResult = callRecording_StartRecord(pCallInfo->GetCallID(), pCallInfo->GetAccountID(),
                                                bNetworkConf);
            if (!bResult)
            {
                pCallInfo->m_eRecordResult = CALL_RECORD_START_FAILED;
            }
        }
        break;
    case TALK_ACTION_RECORD_STOP:
        {
            bResult = callRecording_StopRecord(pCallInfo->GetCallID(), pCallInfo->GetAccountID(), bNetworkConf);
            if (!bResult)
            {
                pCallInfo->m_eRecordResult = CALL_RECORD_STOP_FAILED;
            }
        }
        break;
    case TALK_ACTION_RECORD_PAUSE:
        {
            bResult = callRecording_PauseRecord(pCallInfo->GetCallID(), pCallInfo->GetAccountID(),
                                                bNetworkConf);
            if (!bResult)
            {
                pCallInfo->m_eRecordResult = CALL_RECORD_PAUSE_FAILED;
            }
        }
        break;
    case TALK_ACTION_RECORD_RESUME:
        {
            bResult = callRecording_ResumeRecord(pCallInfo->GetCallID(), pCallInfo->GetAccountID(),
                                                 bNetworkConf);
            if (!bResult)
            {
                pCallInfo->m_eRecordResult = CALL_RECORD_RESUME_FAILED;
            }
        }
        break;
    default:
        break;
    }

    TALK_INFO("Call Record Result [%d] Record Result[%d]", bResult, pCallInfo->m_eRecordResult);
    return bResult;
}

DECL_SIGNAL_HANDLER(CBroadsoftRecordListener::OnSessionCreate)
{
    AllConfCallRecordStatueSync(CSessionProxy(sigp.iSessionID));
    return true;
}

DECL_SIGNAL_HANDLER(CBroadsoftRecordListener::OnRoutineChanged)
{
    CSessionProxy sessionProxy(sigp.iSessionID);

    if (OnPreNetworkConfRoutineCreate(sessionProxy))
    {
        return true;
    }

    if (sessionProxy.GetRoutineState() == ROUTINE_TALKING)
    {
        CCallInfo* pCallInfo = sessionProxy.GetCallInfo();

        if (pCallInfo != NULL
                && callRecord_IsCallRecordOn(pCallInfo->GetAccountID())
                && pCallInfo->m_eRecordStatus == CALL_RECORD_UNKNOWN)
        {
            CALL_RECORDING_MODE eMode = callRecord_GetRecordMode(pCallInfo->GetAccountID());
            if (eMode == CRM_ALWAYS || eMode == CRM_ALWAYS_PAUSE_RESUME)
            {
                pCallInfo->m_eRecordStatus = CALL_RECORD_START;
            }
            else if (eMode == CRM_DEMAND || eMode == CRM_DEMAND_USER_START)
            {
                pCallInfo->m_eRecordStatus = CALL_RECORD_NORMAL;
            }
        }
    }

    return true;
}

DECL_SIGNAL_HANDLER(CBroadsoftRecordListener::OnAddToNetConf)
{
    //将会议的callid 附带在wparam传递过来，省去遍历查找的麻烦
    CCallInfo* pNetworkConfInfo = talklogic_GetCallInfoByCallId(sigp.iCallID);
    CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(sigp.lParam.iValue);

    if (NULL != pNetworkConfInfo && NULL != pCallInfo)
    {
        if (pNetworkConfInfo->m_eRecordStatus == CALL_RECORD_NORMAL)
        {
            pNetworkConfInfo->m_eRecordStatus = CALL_RECORD_NORMAL;
            if (pCallInfo->m_eRecordStatus == CALL_RECORD_START)
            {
                pNetworkConfInfo->m_eRecordStatus = CALL_RECORD_START;
                callRecording_StartRecord(pNetworkConfInfo->GetCallID(), pNetworkConfInfo->GetAccountID(), true);
            }
            else if (pCallInfo->m_eRecordStatus == CALL_RECORD_PAUSE)
            {
                pNetworkConfInfo->m_eRecordStatus = CALL_RECORD_START;
                callRecording_StartRecord(pNetworkConfInfo->GetCallID(), pNetworkConfInfo->GetAccountID(), true);
                callRecording_ResumeRecord(pCallInfo->GetCallID(), pCallInfo->GetAccountID());
            }
        }
        else if (pNetworkConfInfo->m_eRecordStatus == CALL_RECORD_START)
        {
            pNetworkConfInfo->m_eRecordStatus = CALL_RECORD_START;
            if (pCallInfo->m_eRecordStatus == CALL_RECORD_NORMAL)
            {
                callRecording_StartRecord(pCallInfo->GetCallID(), pCallInfo->GetAccountID());
            }
            else if (pCallInfo->m_eRecordStatus == CALL_RECORD_PAUSE)
            {
                callRecording_ResumeRecord(pCallInfo->GetCallID(), pCallInfo->GetAccountID());
            }
        }
        else if (pNetworkConfInfo->m_eRecordStatus == CALL_RECORD_PAUSE)
        {
            pNetworkConfInfo->m_eRecordStatus = CALL_RECORD_START;
            callRecording_ResumeRecord(pNetworkConfInfo->GetCallID(), pNetworkConfInfo->GetAccountID(), true);
            if (pCallInfo->m_eRecordStatus == CALL_RECORD_NORMAL)
            {
                callRecording_StartRecord(pCallInfo->GetCallID(), pCallInfo->GetAccountID());
            }
            else if (pCallInfo->m_eRecordStatus == CALL_RECORD_PAUSE)
            {
                callRecording_ResumeRecord(pCallInfo->GetCallID(), pCallInfo->GetAccountID());
            }
        }
    }

    return true;
}

DECL_SIGNAL_HANDLER(CBroadsoftRecordListener::OnGetDataToUI)
{
    CSessionProxy sessionProxy(sigp.iSessionID);

    if (sessionProxy.IsLocalConf())
    {
        YLVector<CSessionProxy> vec;
        if (!sessionProxy.GetSubSession(vec))
        {
            return false;
        }

        YLVector<CSessionProxy>::iterator it = vec.begin();
        for (; it != vec.end(); ++it)
        {

            // 获取对应Session作为subSession时的CallInfo
            CCallInfo* pCallInfo = it->GetCallInfo();
            if (callRecord_IsCallRecordOn(pCallInfo->GetAccountID())
                    && pCallInfo->m_eRecordStatus == CALL_RECORD_UNKNOWN)
            {
                CALL_RECORDING_MODE eMode = callRecord_GetRecordMode(pCallInfo->GetAccountID());
                if (eMode == CRM_ALWAYS || eMode == CRM_ALWAYS_PAUSE_RESUME)
                {
                    pCallInfo->m_eRecordStatus = CALL_RECORD_START;
                }
                else if (eMode == CRM_DEMAND || eMode == CRM_DEMAND_USER_START)
                {
                    pCallInfo->m_eRecordStatus = CALL_RECORD_NORMAL;
                }
            }
        }
    }
    else
    {
        CCallInfo* pCallInfo = sessionProxy.GetCallInfo();

        if (pCallInfo != NULL
                && pCallInfo->m_eStateOfRoutine == ROUTINE_TALKING
                && pCallInfo->m_eRecordStatus == CALL_RECORD_UNKNOWN
                && callRecord_IsCallRecordOn(pCallInfo->GetAccountID()))
        {
            CALL_RECORDING_MODE eMode = callRecord_GetRecordMode(pCallInfo->GetAccountID());
            //TALK_INFO("Call Record Status[%d] Mode[%d]", m_pCallInfo->m_eRecordStatus, eMode);
            if (eMode == CRM_ALWAYS || eMode == CRM_ALWAYS_PAUSE_RESUME)
            {
                pCallInfo->m_eRecordStatus = CALL_RECORD_START;
            }
            else if (eMode == CRM_DEMAND || eMode == CRM_DEMAND_USER_START)
            {
                pCallInfo->m_eRecordStatus = CALL_RECORD_NORMAL;
            }
        }
    }
    return true;
}

bool CBroadsoftRecordListener::OnPreNetworkConfRoutineCreate(const CSessionProxy& sessionProxy)
{
    if (sessionProxy.GetRoutineState() != ROUTINE_NETWORKCONF_CONNECT)
    {
        return false;
    }

    //上面的routine判断已包含该指针的判空，此处不再进行判空
    CCallInfo* pNetworkConfInfo = sessionProxy.GetCallInfo();
    CCallInfo* pAddToConfInfo1 = NULL;
    CCallInfo* pAddToConfInfo2 = NULL;

    YLVector<int> vec;
    talklogic_GetAllSessionID(vec);//获取所有sessionID
    for (int i = 0; i < vec.size(); ++i)
    {
        CSessionProxy s(vec[i]);
        if (!s.IsSingle())
        {
            continue;
        }

        CCallInfo* pCallInfo = s.GetCallInfo();
        if (!pCallInfo->m_bAddToNetworkConf)
        {
            continue;
        }

        if (NULL == pAddToConfInfo1)
        {
            pAddToConfInfo1 = pCallInfo;
        }
        else
        {
            pAddToConfInfo2 = pCallInfo;
        }
    }

    if (NULL == pAddToConfInfo1 || NULL == pAddToConfInfo2)
    {
        return true;
    }

    if (pAddToConfInfo2->m_eRecordStatus == CALL_RECORD_NORMAL)
    {
        pNetworkConfInfo->m_eRecordStatus = CALL_RECORD_NORMAL;
        if (pAddToConfInfo1->m_eRecordStatus == CALL_RECORD_START)
        {
            pNetworkConfInfo->m_eRecordStatus = CALL_RECORD_START;
            callRecording_StartRecord(pAddToConfInfo2->GetCallID(), pAddToConfInfo2->GetAccountID());
        }
        else if (pAddToConfInfo1->m_eRecordStatus == CALL_RECORD_PAUSE)
        {
            pNetworkConfInfo->m_eRecordStatus = CALL_RECORD_START;
            callRecording_StartRecord(pAddToConfInfo2->GetCallID(), pAddToConfInfo2->GetAccountID());
            callRecording_ResumeRecord(pAddToConfInfo1->GetCallID(), pAddToConfInfo1->GetAccountID());
        }
    }
    else if (pAddToConfInfo2->m_eRecordStatus == CALL_RECORD_START)
    {
        pNetworkConfInfo->m_eRecordStatus = CALL_RECORD_START;
        if (pAddToConfInfo1->m_eRecordStatus == CALL_RECORD_NORMAL)
        {
            callRecording_StartRecord(pAddToConfInfo1->GetCallID(), pAddToConfInfo1->GetAccountID());
        }
        else if (pAddToConfInfo1->m_eRecordStatus == CALL_RECORD_PAUSE)
        {
            callRecording_ResumeRecord(pAddToConfInfo1->GetCallID(), pAddToConfInfo1->GetAccountID());
        }
    }
    else if (pAddToConfInfo2->m_eRecordStatus == CALL_RECORD_PAUSE)
    {
        pNetworkConfInfo->m_eRecordStatus = CALL_RECORD_START;
        callRecording_ResumeRecord(pAddToConfInfo2->GetCallID(), pAddToConfInfo2->GetAccountID());
        if (pAddToConfInfo1->m_eRecordStatus == CALL_RECORD_NORMAL)
        {
            callRecording_StartRecord(pAddToConfInfo1->GetCallID(), pAddToConfInfo1->GetAccountID());
        }
        else if (pAddToConfInfo1->m_eRecordStatus == CALL_RECORD_PAUSE)
        {
            callRecording_ResumeRecord(pAddToConfInfo1->GetCallID(), pAddToConfInfo1->GetAccountID());
        }
    }

    return true;
}

void CBroadsoftRecordListener::AllConfCallRecordStatueSync(const CSessionProxy& sessionProxy)
{
    if (!sessionProxy.IsLocalConf())
    {
        return;
    }

    bool bRecordStart = false;

    YLVector<CSessionProxy> vec;
    if (!sessionProxy.GetSubSession(vec))
    {
        return;
    }

    YLVector<CSessionProxy>::iterator it = vec.begin();
    for (; it != vec.end(); ++it)
    {
        // 获取对应Session作为subSession时的CallInfo
        CCallInfo* pCallInfo = it->GetCallInfo();
        // 判断当前会议中两路的callid是否合法
        if (pCallInfo == NULL || pCallInfo->GetCallID() == -1)
        {
            continue;
        }
        TALK_INFO("CallId[%d] RecordStatus[%d]", pCallInfo->GetCallID(), pCallInfo->m_eRecordStatus);
        // 录音开启，这开启所有通话录音，
        if (pCallInfo->m_eRecordStatus == CALL_RECORD_START)
        {
            bRecordStart = true;
        }
        else if (pCallInfo->m_eRecordStatus == CALL_RECORD_PAUSE)
        {
            bRecordStart = true;
            callRecording_ResumeRecord(pCallInfo->GetCallID(), pCallInfo->GetAccountID());
        }
    }

    // 其中一路通话录音开启，开启所有通话录音
    if (bRecordStart)
    {
        for (it = vec.begin(); it != vec.end(); ++it)
        {
            // 获取对应Session作为subSession时的CallInfo
            CCallInfo* pCallInfo = it->GetCallInfo();
            // 判断当前会议中两路的callid是否合法
            if (pCallInfo == NULL || pCallInfo->GetCallID() == -1)
            {
                continue;
            }
            if (pCallInfo->m_eRecordStatus == CALL_RECORD_NORMAL)
            {
                callRecording_StartRecord(pCallInfo->GetCallID(), pCallInfo->GetAccountID());
            }
        }
    }
}
#endif // IF_FEATURE_BROADSOFT_CALLRECORDING
