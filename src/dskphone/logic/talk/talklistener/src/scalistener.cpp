#include "talklistener_inc.h"
#include "talk/talkdsskey/include/dsskeybindmanager.h"

bool CSCAListener::FindByAccount::operator()(CCallInfo* p)
{
    return !p->m_bBLASeizeLine && iAccountID == p->GetAccountID();
}

bool CSCAListener::SCATraver::operator()(CCallInfo* p)
{
    int* iResult = (int*)(msg.GetExtraData());
    if (iResult == NULL)
    {
        return false;
    }

    CListenInfo* priv = lsn.GetCallerData(p);
    if (priv == NULL)
    {
        return false;
    }

    switch (msg.message)
    {
    case SIP_SUB_DRAFT_BLA_SEIZE_LINE_RESULT:
        {
            if (msg.wParam == p->GetAccountID())
            {
                return lsn.OnBLASeizeResult(p, *priv, msg, *iResult);
            }
        }
        break;
    case SIP_SUB_UPDATE_BROADSOFT_SCA_SEIZE_RESULT:
        {
            return lsn.OnSCASeizeResult(p, *priv, msg, *iResult);
        }
        break;
    case SIP_SUB_UPDATE_BROADSOFT_SCA_SEIZE_RESULT2:
        {
            return lsn.OnSCASeize2Result(p, *priv, msg, *iResult);
        }
        break;
    default:
        break;
    }

    return false;
}

CSCAListener::CSCAListener()
    : LSN_CLS_LOG_NAME
{
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_ROUTINE_CHANGED, CSCAListener::OnRoutineChanged);
    REG_SYNC_SIGNAL(TALK_SIG_GET_DATA_TO_UI, CSCAListener::OnGetDataToUI);

    REG_SYNC_SIGNAL(TALK_SIG_SESSION_ACCOUNT_CHANGE, CSCAListener::OnAccountChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_PRE_DELETE, CSCAListener::BeforSessionDestory);
    REG_SYNC_SIGNAL(TALK_SIG_PROCESS_CALLOUT, CSCAListener::OnCallOut);
    REG_SYNC_SIGNAL(TALK_SIG_BEFORE_CALLOUT, CSCAListener::BeforeCallout);

    RegETLMsg(SIP_SUB_DRAFT_BLA_UNSEIZE_LINE_RESULT);
    RegETLMsg(SIP_SUB_UPDATE_BROADSOFT_SCA_SEIZE_RESULT);
    RegETLMsg(SIP_SUB_UPDATE_BROADSOFT_SCA_SEIZE_RESULT2);
    RegETLMsg(SIP_SUB_DRAFT_BLA_SEIZE_LINE_RESULT);
    RegETLMsg(SIP_CALL_RELEASED);
    RegETLMsg(SIP_CALL_FAIL);
}

bool CSCAListener::OnMessage(msgObject & msg)
{
    switch (msg.message)
    {
    case SIP_CALL_RELEASED:
        {
            if (BLA_GENBAND == acc_GetBLAType(msg.wParam))
            {
                bsSCA_UpdateLightInfo(msg.wParam);
                return true;
            }
        }
        break;
    case SIP_CALL_FAIL:
        {
            CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(msg.lParam);
            if (pCallInfo != NULL)
            {
                ReStartTalk(pCallInfo);
            }
        }
        break;
    case SIP_SUB_DRAFT_BLA_UNSEIZE_LINE_RESULT:
        {
            /* DraftBLA只有1可用，暂时只支持index为1 */
            if (msg.lParam != 1)
            {
                return false;
            }

            FindByAccount visitor(msg.wParam);
            CCallInfo* pCallInfo = talklogic_TraverseCallInfo(visitor);
            if (pCallInfo != NULL)
            {
                // 取消占线成功, BLAIndex需 +1
                bsSCA_FreeIndex(msg.wParam, pCallInfo->m_iBLAIndex + 1);
            }

            return true;
        }
        break;
    default:
        {
            if (msg.GetExtraData() == NULL)
            {
                return false;
            }

            SCATraver visitor(msg, *this);
            talklogic_TraverseCallInfo(visitor);
            return true;
        }
    }

    return false;
}

DECL_SIGNAL_HANDLER(CSCAListener::OnRoutineChanged)
{
    CSessionProxy proxy(sigp.iSessionID);
    CCallInfo* pCallInfo = proxy.GetCallInfo();
    if (pCallInfo == NULL || priv == NULL)
    {
        return false;
    }

    int iAccountID = pCallInfo->GetAccountID();

    switch (sigp.lParam.iValue)
    {
    case ROUTINE_DIALING:
    case ROUTINE_PREDIAL:
        {
            // 检查是否合法的BLA话路
            if (!CheckBLA(pCallInfo))
            {
                return false;
            }
        }
        break;
    case ROUTINE_PRETRAN:
        {
#if IF_BUG_30689
            // pretran时不需要再去占线(BLA)
            if (configParser_GetConfigInt(kszTransferNeedSeizeLine) == 0)
            {
                pCallInfo->m_iBLAIndex = -1;
                pCallInfo->m_bNeedToSeizeLine = false;
            }
            else
#endif
            {
                // 检查是否合法的BLA话路
                CheckBLA(pCallInfo);
            }
        }
        break;
    case ROUTINE_RETRIEVE_CALLPARK:
        {
            priv->m_bBLASeizeLineSucced = false;
            CheckBLA(pCallInfo);
        }
        break;
    case ROUTINE_NETWORKCONF_CONNECT:
        {
            // SCA建立网络会议也需要去占线
            if (!acc_IsBLANeedSeize(iAccountID))
            {
                break;
            }

            pCallInfo->m_iBLAIndex = bsSCA_GetFirstFreeIndex(iAccountID);
            // 取到的占线索引是否合法
            if (pCallInfo->m_iBLAIndex == 0)
            {
                priv->m_BlaProxy.OnBLAError();
            }
            else
            {
                // 跟sip发占线消息
                SeizeLine(pCallInfo);
            }

            SIG_REPLY(0);
        }
        break;
    case ROUTINE_BLACONNECTING:
        {
            priv->m_bProcessSeizeLine = false;
            // 检查是否合法的BLA话路
            if (!CheckBLA(pCallInfo))
            {
                break;
            }

            if (!acc_IsAccountAvailable(iAccountID)
                    || !bsSCA_BeNeedToSeizeLine(iAccountID, pCallInfo->m_iBLAIndex))
            {
                proxy.CallOut();
            }

            // 没有发送占线消息
            if (!pCallInfo->m_bBLASeizeLine)
            {
                SeizeLine(pCallInfo);
            }
        }
        break;
    case ROUTINE_RINGING:
        {
            if (BLA_BROADSOFT_SCA == pCallInfo->GetBLAType())
            {
                CallInUserInfo& userInfo = pCallInfo->GetRingUserInfo();
                // 来电时sip有带BlaIndex过来，就直接使用sip上报的
                pCallInfo->m_iBLAIndex = userInfo.iBlaIndex > 0 ? userInfo.iBlaIndex : bsSCA_GetAlertingIndex(iAccountID);
                SCA_INFO("Incoming call get free bla index [%d]  sip update [%d]", pCallInfo->m_iBLAIndex, userInfo.iBlaIndex);
                return true;
            }
        }
        break;
    case ROUTINE_CONNECTING:
        {
            // 设置显示名
            // 获取显示名信息, 要在呼出后更改，否则直接呼给来电的号码，而不是截答
            yl::string strDisplay;
            yl::string strUri;
            if (bsSCA_GetLineIndexCallInfo(iAccountID, pCallInfo->m_iBLAIndex, strDisplay, strUri))
            {
                pCallInfo->m_tRemoteInfo.sDisplayName = strDisplay;
                yl::string strSIPName;
                yl::string strServerName;
                commonUnits_ParserDialURI(strUri, strSIPName, strServerName);
                if (!strSIPName.empty())
                {
                    pCallInfo->m_tRemoteInfo.sSIPName = strSIPName;
                }
            }
        }
        break;
    default:
        return false;
    }

    return true;
}

DECL_SIGNAL_HANDLER(CSCAListener::OnAccountChange)
{
    CSessionProxy proxy(sigp.iSessionID);
    CCallInfo* pCallInfo = proxy.GetCallInfo();
    if (NULL == pCallInfo || priv == NULL)
    {
        return false;
    }

    int oldAcc = sigp.wParam.iValue;

    // 旧账号是BLA类型的，则要取消占线
    if (acc_IsBLANeedSeize(oldAcc))
    {
        SeizeLine(pCallInfo, false, oldAcc);
    }

    pCallInfo->m_bNeedToSeizeLine = true;
    priv->m_bBLASeizeLineSucced = false;

    // 新账号需重新占线
    CheckBLA(pCallInfo);
    return true;
}

DECL_SIGNAL_HANDLER(CSCAListener::BeforSessionDestory)
{
    CSessionProxy proxy(sigp.iSessionID);
    CCallInfo* pCallInfo = proxy.GetCallInfo();
    if (NULL == pCallInfo)
    {
        return false;
    }

    if (configParser_GetConfigInt(kszAutoReleaseLineBySrv) == 0)
    {
        SeizeLine(pCallInfo, false);
    }

    return true;
}

DECL_SIGNAL_HANDLER(CSCAListener::OnGetDataToUI)
{
    CSessionProxy proxy(sigp.iSessionID);
    CCallInfo* pCallInfo = proxy.GetCallInfo();

    if (NULL == pCallInfo || priv == NULL)
    {
        return false;
    }

    if (proxy.IsInherits(ROUTINE_DIALING))
    {
        DataDial* data2UI = (DataDial*)sigp.wParam.pValue;
        if (NULL != data2UI)
        {
            data2UI->bSeizeFailed = priv->m_bBLASeizeLineSucced;
        }

        return true;
    }

    if (ROUTINE_TALKING != proxy.GetRoutineState()
            // http://10.2.1.199/Bug.php?BugID=87961
            // sca账号如果是处于bargein的状态，服务器不允许tran，故屏蔽tran键
            || BLA_BROADSOFT_SCA != pCallInfo->GetBLAType())
    {
        return false;
    }

    // http://10.2.1.199/Bug.php?BugID=87961
    // sca账号如果是处于bargein的状态，服务器不允许tran，故屏蔽tran键
    if (pCallInfo->GetBLAType() == BLA_BROADSOFT_SCA)
    {
        DataTalking2UI* data2UI = (DataTalking2UI*)sigp.wParam.pValue;
        if (NULL != data2UI && data2UI->bAllowTran)
        {
            BLF_STATUS_CODE eStatus = bsSCA_GetStatusByIndex(pCallInfo->m_iBLAIndex, pCallInfo->GetAccountID());
            data2UI->bAllowTran = (CN_BLA_STATUS_BRIDGE_ACITVE != eStatus && CN_BLA_STATUS_BRIDGE_HELD != eStatus);
        }
    }

    return true;
}

DECL_SIGNAL_HANDLER(CSCAListener::OnCallOut)
{
    CSessionProxy proxy(sigp.iSessionID);
    CCallInfo* pCallInfo = proxy.GetCallInfo();

    if (NULL == pCallInfo || priv == NULL)
    {
        return false;
    }

    CallOut_Param& calloutParam = pCallInfo->GetCallOutParam();
    int iAccountID = pCallInfo->GetAccountID();

    // -1代表普通呼出，非-1是SCA呼出
    int iBlaIndex = calloutParam.iBLAIndex;
    if (iBlaIndex != INVALID_HANDLE)
    {
        pCallInfo->m_iBLAIndex = iBlaIndex;

        if (calloutParam.bSilentBarginIn)
        {
            pCallInfo->m_eTalkCallOutType = TCT_CALL_SILENT_MONITOR;
        }

        if (iBlaIndex > 0)
        {
            pCallInfo->m_bBLASeizeLine = true;
        }
        else
        {
            // callpull抢回需要去占线
            pCallInfo->m_bNeedToSeizeLine = true;
            pCallInfo->m_bBLASeizeLine = false;
        }

        // http://10.2.1.199/Bug.php?BugID=75491
        BLF_STATUS_CODE eStatus = bsSCA_GetStatusByIndex(iBlaIndex, iAccountID);
        if (CN_BLA_STATUS_BRIDGE_ACITVE == eStatus || CN_BLA_STATUS_ACTIVE == eStatus)
        {
            // http://10.2.1.199/Bug.php?BugID=93446
            pCallInfo->m_bNeedToSeizeLine = false;
        }

        dialplan_GetCalloutParam(pCallInfo->GetSessionID(), iAccountID, COT_DSSKEY, calloutParam);
    }

    // 账号为BLA类型且需要占线
    if (pCallInfo->m_bNeedToSeizeLine
            && priv->m_bEnterBLARoutine
            && acc_IsBLANeedSeize(iAccountID)
#ifdef IF_FEATURE_EMERGENCY
            && !Emergency_IsEmergencyMode(sigp.strData.c_str())
#endif //IF_FEATURE_EMERGENCY
       )
    {
        SIG_REPLY(ROUTINE_BLACONNECTING);
    }

    return true;
}

DECL_SIGNAL_HANDLER(CSCAListener::BeforeCallout)
{
    if (sigp.wParam.pValue == NULL)
    {
        return false;
    }

    CallOut_Param& param = (*(CallOut_Param*)sigp.wParam.pValue);
    yl::string& strCullPullCode = param.strNumber;
    int iBlaIndex = param.iBLAIndex;
    int iAccountID = param.iAccountID;

    // 强制抢回SCA话路 默认code= *11
    BLA_TYPE eType = acc_GetBLAType(iAccountID);
    if (eType == BLA_DRAFT_BLA)
    {
        strCullPullCode = acc_GetBLANum(iAccountID);
    }
    else if (strCullPullCode.empty())
    {
        if (configParser_GetConfigInt(kszBargeInUserName) == 1)
        {
            strCullPullCode = configParser_GetCfgItemStringValue(kszAccountUserName, iAccountID).c_str();
        }
        else
        {
            strCullPullCode = configParser_GetCfgItemStringValue(kszAccountRegisterName, iAccountID).c_str();
        }
    }

    TALK_INFO("BeforeCallout index[%d] acc[%d] type[%s] num[%s]", iBlaIndex, iAccountID, eType, strCullPullCode.c_str());
    return true;
}

bool CSCAListener::OnBLASeizeResult(CCallInfo* pCallInfo, CListenInfo& data, msgObject& msg, int iResult)
{
    CListenInfo* priv = GetCallerData(pCallInfo);
    if (pCallInfo == NULL || priv == NULL)
    {
        return false;
    }

    SCA_INFO("index=[%d] acc=[%d] result=[%d]", pCallInfo->m_iBLAIndex, pCallInfo->GetAccountID(), iResult);

    ROUTINE_STATE eRoutine = pCallInfo->GetRoutine();

    switch (eRoutine)
    {
    case ROUTINE_PRETRAN:
    case ROUTINE_DIALING:
        {
            if (pCallInfo->m_bNeedToSeizeLine)
            {
                // 占线失败的处理, 只有第一路提示占线失败，其它路不提示，依然可以呼出
                return OnBLASeizeFinished(pCallInfo, iResult == 0 && pCallInfo->m_iBLAIndex == 1);
            }
        }
        break;
    case ROUTINE_RETRIEVE_CALLPARK:
        {
            priv->m_bEnterBLARoutine = false;
            return true;
        }
        break;
    case ROUTINE_BLACONNECTING:
        {
            if (!pCallInfo->m_bBLASeizeLine
                    || !pCallInfo->m_bNeedToSeizeLine)
            {
                return false;
            }

            // 占线失败的处理, 只有第一路提示占线失败，其它路不提示，依然可以呼出
            if (iResult == 0 && pCallInfo->m_iBLAIndex == 1)
            {
                return data.m_BlaProxy.OnBLAError();
            }
        }
        break;
    case ROUTINE_NETWORKCONF_CONNECT:
        {
            // 给SIP发消息，加入网络会议号码
            modVoip_EstablishSCANetworkConf(pCallInfo->GetAccountID(), pCallInfo->GetCallID(),
                                            CONFCALLNUM, pCallInfo->m_iConfCallID, pCallInfo->m_iBLAIndex);
        }
        break;
    default:
        break;
    }

    return false;
}

bool CSCAListener::OnSCASeizeResult(CCallInfo* pCallInfo, CListenInfo& data, msgObject& msg, int iResult)
{
    if (pCallInfo == NULL)
    {
        return false;
    }

    ROUTINE_STATE eRoutine = pCallInfo->GetRoutine();

    switch (eRoutine)
    {
    case ROUTINE_PRETRAN:
    case ROUTINE_DIALING:
    case ROUTINE_RETRIEVE_CALLPARK:
        {
            if (data.m_bProcessSeizeLine)
            {
                return true;
            }

            return ProcessSeizeLineResult(pCallInfo, iResult == 0 ? SIP_SCA_SEIZE_FAIL : SIP_SCA_SEIZE_SUCC);
        }
        break;
    case ROUTINE_BLACONNECTING:
        {
            if (data.m_bProcessSeizeLine
                    || !pCallInfo->m_bBLASeizeLine)
            {
                return false;
            }

            // 占线失败的处理
            if (iResult == 0)
            {
                return data.m_BlaProxy.OnBLAError();
            }
        }
        break;
    case ROUTINE_NETWORKCONF_CONNECT:
        {
            // 占线失败的处理
            if (msg.lParam == 0)
            {
                data.m_BlaProxy.OnBLAError();
                ReStartTalk(pCallInfo);
            }
            else
            {
                // 给SIP发消息，加入网络会议号码
                modVoip_EstablishSCANetworkConf(pCallInfo->GetAccountID(), pCallInfo->GetCallID(),
                                                CONFCALLNUM, pCallInfo->m_iConfCallID, pCallInfo->m_iBLAIndex);
            }
        }
        break;
    default:
        break;
    }

    return false;
}

bool CSCAListener::OnSCASeize2Result(CCallInfo* pCallInfo, CListenInfo& data, msgObject& msg, int iResult)
{
    if (pCallInfo == NULL)
    {
        return false;
    }

    ROUTINE_STATE eRoutine = pCallInfo->GetRoutine();
    switch (eRoutine)
    {
    case ROUTINE_PRETRAN:
    case ROUTINE_DIALING:
    case ROUTINE_RETRIEVE_CALLPARK:
    case ROUTINE_BLACONNECTING:
        {
            data.m_bProcessSeizeLine = true;
            return ProcessSeizeLineResult(pCallInfo, iResult);
        }
        break;
    default:
        return false;
    }
}

bool CSCAListener::CheckBLA(CCallInfo* pCallInfo)
{
    if (NULL == pCallInfo)
    {
        return false;
    }

    int iAccountID = pCallInfo->GetAccountID();
    // 如果不是BLA类型，则不再检查
    if (!acc_IsBLANeedSeize(iAccountID))
    {
        return true;
    }

    if (!pCallInfo->m_bNeedToSeizeLine
            || pCallInfo->m_bBLASeizeLine)
    {
        // 该话路不需要去占线，比如ConfTo
        return true;
    }

    if (pCallInfo->GetRoutine() == ROUTINE_BLACONNECTING)
    {
        // 该账号是否有空闲的index
        pCallInfo->m_iBLAIndex = bsSCA_GetFirstFreeIndex(iAccountID);
        SCA_INFO("First free BLA index (%d)", pCallInfo->m_iBLAIndex);

        // 如果有绑定，则判断是否拥有空闲的BLA线路
        return pCallInfo->m_iBLAIndex != 0;
    }

    CSessionProxy proxy(pCallInfo);

    // 该账号是否有空闲的index
    if (pCallInfo->m_iBLAIndex == 0)
    {
        int iDsskeyDataID = _DsskeyBindManager.GetDsskeyBySession(proxy, true);

        SCA_INFO("CheckBLA pDsskey [%d]", iDsskeyDataID);

        pCallInfo->m_iBLAIndex = bsSCA_GetBLAIndex(iDsskeyDataID);
    }

    SCA_INFO("CheckBLA first free index (%d)", pCallInfo->m_iBLAIndex);

    // 如果有绑定，则判断是否拥有空闲的BLA线路
    if (pCallInfo->m_iBLAIndex == 0)
    {
        CListenInfo* priv = GetSessionData(proxy);
        if (priv != NULL)
        {
            // http://10.2.1.199/Bug.php?BugID=40257
            // 占线错误，未呼出，不保存呼叫记录
            priv->m_bSaveCallLog = false;

            priv->m_BlaProxy.OnBLAError();
        }

        return false;
    }

    // 拨号界面才要去占线，如果拥有空闲的BLA线路，则要去占线
    SeizeLine(pCallInfo);
    return true;
}

bool CSCAListener::OnBLASeizeFinished(CCallInfo* pCallInfo, bool bFailed)
{
    CListenInfo* priv = GetCallerData(pCallInfo);
    if (NULL == pCallInfo || NULL == priv)
    {
        return false;
    }
    // 占线成功则不需要再进入BLARoutine
    priv->m_bEnterBLARoutine = false;
    SCA_INFO("OnBLASeizeFinished failed=[%d]", bFailed);
    bool bEmergency = Emergency_IsEmergencyModeEnable();
    // 占线失败的处理
    if (bFailed)
    {
        // http://10.2.1.199/Bug.php?BugID=40257
        // 占线错误，未呼出，不保存呼叫记录
        priv->m_bSaveCallLog = false;

        if (bEmergency)
        {
            modtklsn_emit_sync(TALK_SIG_SESSION_PLAY_BUSY_TONE, pCallInfo);
        }
        else
        {
            priv->m_BlaProxy.OnBLAError();
        }

        return true;
    }
    else
    {
        priv->m_bBLASeizeLineSucced = true;
        priv->bSeizeFailed = false;

        if (bEmergency)
        {
            modtklsn_emit_sync(TALK_SIG_SESSION_DIAL_TONE, pCallInfo);
        }
    }

    return false;
}

bool CSCAListener::ProcessSeizeLineResult(CCallInfo* pCallInfo, int iResult)
{
    CListenInfo* priv = GetCallerData(pCallInfo);
    if (pCallInfo == NULL || priv == NULL)
    {
        return false;
    }

    SCA_INFO("ProcessSeizeLineResult iResult[%d]", iResult);
    ROUTINE_STATE eRoutine = pCallInfo->GetRoutine();
    CSessionProxy proxy(pCallInfo);

    switch (iResult)
    {
    case SIP_SCA_SEIZE_SUCC:
        {
            switch (eRoutine)
            {
            case ROUTINE_BLACONNECTING:
                {
                    proxy.CallOut();
                }
                break;
            case ROUTINE_RETRIEVE_CALLPARK:
                {
                    priv->m_bEnterBLARoutine = false;
                }
                break;
            default:
                return OnBLASeizeFinished(pCallInfo, false);
            }
        }
        break;
    case SIP_SCA_SEIZE_FAIL:
        {
            switch (eRoutine)
            {
            case ROUTINE_BLACONNECTING:
            case ROUTINE_RETRIEVE_CALLPARK:
                {
                    CListenInfo* priv = GetCallerData(pCallInfo);
                    if (priv != NULL)
                    {
                        priv->m_BlaProxy.OnBLAError();
                    }
                }
                break;
            default:
                return OnBLASeizeFinished(pCallInfo, true);
            }
        }
        break;
    default:
        {
            priv->m_bEnterBLARoutine = false;
            pCallInfo->m_bNeedToSeizeLine = false;
            acc_SetSupportSCA(pCallInfo->GetAccountID(), false);

            if (eRoutine == ROUTINE_BLACONNECTING)
            {
                proxy.CallOut();
            }
        }
        break;
    }

    return true;
}

bool CSCAListener::SeizeLine(CCallInfo* pCallInfo, bool bSeize /*= true*/, int iAccountID /*= AUTO_SELECT_LINE_ID*/)
{
    if (pCallInfo == NULL)
    {
        return false;
    }

    if (!pCallInfo->m_bNeedToSeizeLine)
    {
        // 该话路不需要去占线，比如ConfTo
        return false;
    }

    if (pCallInfo->m_bBLASeizeLine == bSeize)
    {
        // 已经占线则不处理
        return false;
    }

    if (iAccountID == AUTO_SELECT_LINE_ID)
    {
        iAccountID = pCallInfo->GetAccountID();
    }

    if (!acc_IsAccountIDValid(iAccountID))
    {
        // 账号为空则不处理
        return false;
    }

    BLA_TYPE eBLAType = (BLA_TYPE)acc_GetBLAType(iAccountID);
    int iIndex = 0;
    if (eBLAType == BLA_BROADSOFT_SCA
            && pCallInfo->m_iBLAIndex != 0)
    {
        iIndex = pCallInfo->m_iBLAIndex;
    }
    else if (eBLAType == BLA_DRAFT_BLA
             && !commonAPI_IsEmpty(configParser_GetCfgItemStringValue(kszBLANumber, iAccountID).c_str()))
    {
        iIndex = pCallInfo->m_iBLAIndex;
    }
    else
    {
        return false;
    }

    SCA_INFO("SeizeLine [seize:%d][type:%d][iIndex:%d]", bSeize, eBLAType, iIndex);
    int iCallID = pCallInfo->GetCallID();

    if (eBLAType == BLA_DRAFT_BLA)
    {
        // 逻辑层index和sca一样从1开始、服务器从0开始(03只支持一个为0的index，所以必须从0开始)
        modVoip_DraftBLASeizeLine(iAccountID, iCallID, bSeize, iIndex - 1);
    }
    else if (eBLAType == BLA_BROADSOFT_SCA)
    {
        modVoip_BLASeizeLine(iAccountID, iCallID, bSeize, iIndex);
    }

    if (!bSeize
            && eBLAType == BLA_BROADSOFT_SCA)
    {
        // 取消占线
        bsSCA_FreeIndex(pCallInfo->m_iBLAIndex, iAccountID);
    }

    pCallInfo->m_bBLASeizeLine = bSeize;
    return true;
}

bool CSCAListener::ReStartTalk(CCallInfo* pCallInfo)
{
    if (pCallInfo == NULL || pCallInfo->GetRoutine() != ROUTINE_NETWORKCONF_CONNECT)
    {
        return false;
    }

    CSessionStateProxy proxy(pCallInfo->GetSessionID());

    proxy.ShowSessionAddToConf();

    // http://10.2.1.199/Bug.php?BugID=19108
    // 恢复通话,建立会议前,会将通话的那路先停止通话
    CSessionStateProxy focus;

    if (!focus)
    {
        return false;
    }

    CListenInfo* priv = GetSessionData(focus);
    if (priv == NULL)
    {
        return false;
    }

    // 获取上一次通话session
    int iLastSessionID = priv->iLastFocusSession;
    if (iLastSessionID == INVALID_SESSION_ID)
    {
        return false;
    }

    CSessionStateProxy last(iLastSessionID);
    // 恢复通话
    return last.StartTalk();
}
