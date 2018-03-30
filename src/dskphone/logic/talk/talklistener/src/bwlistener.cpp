#include "talklistener_inc.h"

CBWListener::CBWListener()
    : LSN_CLS_LOG_NAME
{
#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_ACCOUNT_CHANGE, CBWListener::OnCallInitiationNumber);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_PRE_DELETE, CBWListener::OnCallInitiationNumber);
#endif

    //【bw callId】
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_ROUTINE_CHANGED, CBWListener::OnRoutineChanged);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_PRE_DELETE, CBWListener::OnSessionDestroy);

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_ADD_TO_NET_CONF, CBWListener::OnSessionAddToNetConf);
#endif
    //【bw callId】

    REG_SYNC_SIGNAL(TALK_SIG_ACTION_EVENT, CBWListener::OnUIAction);
    REG_SYNC_SIGNAL(TALK_SIG_PROCESS_CALLOUT, CBWListener::OnCallOut);

    REG_SYNC_SIGNAL(TALK_SIG_SESSION_RETRIEVEPARK, CBWListener::OnRetrivePark);
    REG_SYNC_SIGNAL(TALK_SIG_GET_DATA_TO_UI, CBWListener::OnGetDataToUI);

#ifdef IF_FEATURE_BROADSOFT_CALLPARK
    RegETLMsg(CALL_PARK_NOTIFY);
    RegETLMsg(XSI_MESSAGE_REQ_FAIL);
    RegETLMsg(CALL_PARK_NOTIFY);
#endif
    RegETLMsg(SIP_SUB_BW_NETWORK_CONF_INFO);
}


bool CBWListener::OnMessage(msgObject & msg)
{
    switch (msg.message)
    {
#ifdef IF_FEATURE_BROADSOFT_CALLPARK
    case CALL_PARK_NOTIFY:
#endif // #ifdef IF_FEATURE_BROADSOFT_CALLPARK
    case XSI_MESSAGE_REQ_FAIL:
        {
#ifdef IF_FEATURE_BROADSOFT_CALLPARK
            ProcessCallParkNotifyMsg(msg);
#endif // #ifdef IF_FEATURE_BROADSOFT_CALLPARK
            return true;
        }
        break;
    case SIP_SUB_BW_NETWORK_CONF_INFO:
        {
            int iAccountID = msg.wParam;
            if (0 == configParser_GetConfigInt(kszBroadsoftActive)
                    || 0 == configParser_GetCfgItemIntValue(kszNetConfMemberManagerEable, iAccountID))
            {
                return false;
            }

            CSessionProxy proxy(talklogic_GetSessionIdByCallID(msg.lParam, false));
            CListenInfo* priv = GetCallerData(proxy.GetCallInfo());
            if (priv == NULL)
            {
                return false;
            }

            yl::string strEventInfo((char*)msg.GetExtraData(), msg.GetExtraSize());
            if (!strEventInfo.empty())
            {
                return ParseConfEventInfo(*priv, iAccountID, strEventInfo);
            }
        }
        break;
    default:
        break;
    }
    return false;
}

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
DECL_SIGNAL_HANDLER(CBWListener::OnCallInitiationNumber)
{
    CSessionProxy session(sigp.iSessionID);
    CCallInfo* pCallInfo = session.GetCallInfo();
    if (pCallInfo == NULL)
    {
        return false;
    }

    return BWExeAssis_SetCallInitiationNumber("", pCallInfo->GetAccountID());
}
#endif

DECL_SIGNAL_HANDLER(CBWListener::OnRoutineChanged)
{
    CSessionProxy session(sigp.iSessionID);
    int iState = sigp.lParam.iValue;
    CCallInfo* pCallInfo = session.GetCallInfo();
    if (pCallInfo == NULL)
    {
        return false;
    }

    int iSipAccount = pCallInfo->GetAccountID();
    if (iSipAccount == INVALID_HANDLE)
    {
        return false;
    }

    if (ROUTINE_TALKING == iState)
    {
#ifdef IF_FEATURE_BROADSOFT_MEET_ME
        // 变量有效性判断
        yl::string& strDTMF = pCallInfo->m_strAutoDTMF;
        if (strDTMF.empty())
        {
            TALK_INFO("SendPreDTMF PreDTMF(%s)", strDTMF.c_str());
            session.SendDTMF(strDTMF);
            // 输入一次后需要清空该CallInfo的AutoDTMF
            strDTMF.clear();
        }
#endif

        g_objCallIdProcesser.OnDealGetCallId(sigp.iSessionID, iSipAccount, true);
    }
    else if (ROUTINE_RINGBACK == iState
             || ROUTINE_RINGING == iState)
    {
        g_objCallIdProcesser.OnDealGetCallId(sigp.iSessionID, iSipAccount, false);
    }
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    else if (ROUTINE_NETWORKCONF == iState)
    {
        g_objCallIdProcesser.OnDealNetWorkConfEnter(sigp.iSessionID);
    }
#endif
    else if (ROUTINE_NETWORKCONF_CONNECT == iState)
    {
        // 给SIP发消息，加入网络会议号码
        modVoip_EstablishNetworkConf(pCallInfo->GetAccountID(), pCallInfo->GetCallID(), CONFCALLNUM,
                                     pCallInfo->m_iConfCallID);
    }

    return true;
}

DECL_SIGNAL_HANDLER(CBWListener::OnSessionDestroy)
{
    g_objCallIdProcesser.OnDealSessionDestory(sigp.iSessionID);
    return  true;
}

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
DECL_SIGNAL_HANDLER(CBWListener::OnSessionAddToNetConf)
{
    int iAddSession = talklogic_GetSessionIdByCallID(sigp.lParam.iValue);
    g_objCallIdProcesser.OnDealNetWorkConfAddUser(sigp.iSessionID, iAddSession);
    return true;
}
#endif

DECL_SIGNAL_HANDLER(CBWListener::OnUIAction)
{
    DataUI2Logic* pActionData = (DataUI2Logic*)sigp.wParam.pValue;
    if (pActionData == NULL)
    {
        return false;
    }

    CSessionStateProxy sp(sigp.iSessionID);
    CCallInfo* pCallInfo = sp.GetCallInfo();
    if (pCallInfo == NULL || priv == NULL)
    {
        return false;
    }

    switch (pActionData->eAction)
    {
#ifdef IF_FEATURE_BROADSOFT_CALLPARK
    case TALK_ACTION_EXIT_FAILD_INFO:
    case TALK_ACTION_SCREEN_CLICK:
    case TALK_ACTION_KEYPRESS:
        {
            // ExitParkFailed();
            if (ROUTINE_TALKING == sp.GetRoutineState())
            {
                if (priv->m_bCallParkFailed)
                {
                    priv->m_bCallParkFailed = false;
                    priv->m_strParkFiled = "";
                }

                if (priv->IsPrePark() || priv->IsPreGPark())
                {
                    priv->SetPark(false);
                    priv->SetGPark(false);
                    return true;
                }
            }
        }
        break;
    case TALK_ACTION_CANCEL:
        {
            if (DIAL_UI_CALLPARK == sp.GetDialUIType()
                    || DIAL_UI_GROUPCALLPARK == sp.GetDialUIType())
            {
                priv->SetPark(false);
                priv->SetGPark(false);
                return true;
            }
        }
        break;
    case TALK_ACTION_SEND:
        {
            // 若当前处于界面Park To
            if (DIAL_UI_CALLPARK == sp.GetDialUIType())
            {
                ProcessCallPark(pCallInfo, pCallInfo->GetDialText());
            }
            // 若当前处于界面Group Park
            else if (DIAL_UI_GROUPCALLPARK == sp.GetDialUIType())
            {
                ProcessGroupCallPark(pCallInfo, pCallInfo->GetDialText());
            }
        }
        break;
    case TALK_ACTION_PARK:
        {
            ProcessCallPark(pCallInfo, pCallInfo->GetDialText());
        }
        break;
    case TALK_ACTION_GROUP_PARK:
        {
            ProcessGroupCallPark(pCallInfo, pCallInfo->GetDialText());
        }
        break;
    case TALK_ACTION_RETRIEVE_PARK:
        {
            if (pActionData->lActionParam != 0)
            {
                DataDial2Logic* pDialData = (DataDial2Logic*)pActionData->pData;
                if (pDialData == NULL)
                {
                    return false;
                }

                return EnterRetrieveParkUI(pDialData->strNumber, pActionData->wActionParam);
            }

            if (ROUTINE_DIALING == sp.GetRoutineState())
            {
                if (configParser_GetConfigInt(kszFeatureDirectSendEnable) == 1)
                {
                    if (configParser_GetConfigInt(kszCallParkMode) != 0)
                    {
                        yl::string strNumber = configParser_GetConfigString(kszRetriveParkCode);

                        if (!strNumber.empty())
                        {
                            CallOut_Param callParam;
                            callParam.strNumber = strNumber;
                            callParam.eCallOutTrigger = COT_DSSKEY;
                            callParam.eRoutineType = ROUTINE_DIALING;
                            talklogic_CreateSession(callParam);
                        }
                    }
                    else
                    {
                        //空字符串默认取回自己，解决自定义name无法取回问题。
                        callPark_RetrieveCallPark("",   pCallInfo->GetAccountID());
                        sp.ExitSession();
                    }
                }
                else
                {
                    EnterRetrieveParkUI("", pCallInfo->GetAccountID());
                }
            }
            else if (ROUTINE_PARK_NOTIFY == sp.GetRoutineState())
            {
                if (netIsNetworkReady())
                {
                    if (configParser_GetConfigInt(kszCallParkMode) != 0)
                    {
                        yl::string strNumber = configParser_GetConfigString(kszRetriveParkCode);
                        if (!strNumber.empty())
                        {
                            // 根据号码和名称呼出
                            talklogic_CallOut(strNumber, "", pCallInfo->GetAccountID());
                        }
                        else
                        {
                            // http://10.2.1.199/Bug.php?BugID=57408
                            EnterRetrieveParkUI(strNumber, pCallInfo->GetAccountID());
                        }
                    }
                    else
                    {
                        // 取回该路通话
                        //空字符串默认取回自己，解决自定义name无法取回问题。
                        if (callPark_RetrieveCallPark("", pCallInfo->GetAccountID()))
                        {
                            // 退出界面
                            sp.ExitSession();
                        }
                    }
                }
            }
        }
        break;
#endif // #ifdef IF_FEATURE_BROADSOFT_CALLPARK
#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    case TALK_ACTION_CALL_PUSH:
        {
            return BWExeAssis_CallPush(pCallInfo->GetCallID(), pCallInfo->GetAccountID());
        }
        break;
#endif //IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    default:
        return false;
        break;
    }

    return true;
}

DECL_SIGNAL_HANDLER(CBWListener::OnCallOut)
{
#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    CSessionProxy proxy(sigp.iSessionID);
    CCallInfo* pCallInfo = proxy.GetCallInfo();
    if (pCallInfo == NULL)
    {
        return false;
    }

    if (pCallInfo->m_eTalkCallOutType == TCT_CALL_INITIATION)
    {
        if (BWExeAssis_CallIntiation(sigp.strData, pCallInfo->GetAccountID()))
        {
            proxy.ExitSession();
            SIG_REPLY(TALK_CALLOUT_FAIL);
        }

        TALK_INFO("CallIntiation failed try normal callout target=[%s] account=[%d]", sigp.strData.c_str(), pCallInfo->GetAccountID());
        // 呼出失败则自动转为常规呼出方式
        pCallInfo->m_eTalkCallOutType = TCT_NORMAL;
    }
#endif //IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    return true;
}

DECL_SIGNAL_HANDLER(CBWListener::OnGetDataToUI)
{
    CSessionProxy proxy(sigp.iSessionID);
    CCallInfo* pCallInfo = proxy.GetCallInfo();

    if (NULL == pCallInfo || priv == NULL)
    {
        return false;
    }

    if (!proxy.IsInherits(ROUTINE_TALKING))
    {
        return false;
    }

    DataTalking2UI* data2UI = (DataTalking2UI*)sigp.wParam.pValue;
    if (NULL != data2UI && data2UI->bAllowTran)
    {
        // 当前处于Park To界面不允许tran
        data2UI->bAllowTran = !priv->IsPrePark() && !priv->IsPreGPark();
    }

    return true;
}

DECL_SIGNAL_HANDLER(CBWListener::OnRetrivePark)
{
    return EnterRetrieveParkUI(sigp.strData, sigp.wParam.iValue);
}

#ifdef IF_FEATURE_BROADSOFT_CALLPARK
bool CBWListener::ProcessCallParkNotifyMsg(msgObject& msg)
{
    if (msg.message == XSI_MESSAGE_REQ_FAIL
            && (msg.wParam == XSI_CALLPARK
                || msg.wParam == XSI_GROUPPARK
                || msg.wParam == XSI_EXECUTIVE_ASSISTANT_CALLPUSH))
    {
        bool bCallPush = msg.wParam == XSI_EXECUTIVE_ASSISTANT_CALLPUSH;
        SXsiExtenInfo* pData = (SXsiExtenInfo*)msg.GetExtraData();
        if (pData != NULL)
        {
            CSessionProxy p(talklogic_GetSessionIdByCallID(pData->m_nEntenParm, false));
            if (p.IsInherits(ROUTINE_TALKING))
            {
                yl::string strInfo = commonAPI_FormatString("%s:%s",
                                     bCallPush ? TRID_CALLPUSH_FAILED : TRID_PARK_CALL_FAILED,
                                     pData->m_strErrInfo.c_str());

                ParkFailed(p, strInfo, bCallPush);
            }
        }
    }
    else if (msg.message == CALL_PARK_NOTIFY)
    {
        if (msg.lParam == true)
        {
            // 可以正确的创建Notify界面则刷新UI
            int iResult = EnterCallParkNotify(msg);
        }
        else
        {
            // 销毁Notify界面
            talklogic_ExitSession(ROUTINE_PARK_NOTIFY);
        }
    }

    return true;
}

bool CBWListener::EnterRetrieveParkUI(const yl::string& strNumber, int iAccountId)
{
    yl::string strRetriveParkCode;
    if (configParser_GetConfigInt(kszCallParkMode) != 0)
    {
        // FAC模式时，当有配Retrive code 直接呼出code
        strRetriveParkCode = configParser_GetConfigString(kszRetriveParkCode);
    }

    if (configParser_GetConfigInt(kszFeatureDirectSendEnable) == 1)
    {
        if (!strNumber.empty()
                || !strRetriveParkCode.empty())
        {
            // 号码不为空时直接取回通话
            return callPark_RetrieveCallPark(strNumber, iAccountId);
        }
    }
    else if (!strNumber.empty())
    {
        // 号码不为空时直接取回通话
        return callPark_RetrieveCallPark(strNumber, iAccountId);
    }

    CallOut_Param callOutParam;
    callOutParam.iAccountID = iAccountId;
    callOutParam.strNumber = strNumber;
    callOutParam.eRoutineType = ROUTINE_RETRIEVE_CALLPARK;
    return talklogic_CreateSession(callOutParam);
}

// 处理有来电Park到话机上的情况
int CBWListener::EnterCallParkNotify(msgObject& msg)
{
    // 判断账号状态
    int iAccount = msg.wParam;
    if (!acc_IsAccountIDValid(iAccount))
    {
        TALK_WARN("Call income with invalid lineid(%d), Ignor.", msg.lParam);
        return 404; // 404 Not Found
    }

    // 网络异常时,不处理该消息
    if (!netIsNetworkReady())
    {
        return 0;
    }

    //执行流程变化，由接口中实现具体分支
    CallOut_Param callOutParam;
    callOutParam.iAccountID = msg.wParam;
    callOutParam.eCallOutType = TCT_CALLPARK;
    callOutParam.eRoutineType = ROUTINE_PARK_NOTIFY;
    DataParkNotifyEx* pDataNotify = (DataParkNotifyEx*)msg.GetExtraData();
    if (pDataNotify != NULL)
    {
        callOutParam.strCallParkNumber = pDataNotify->szNumber;
        callOutParam.strCallParkDiaplayName = pDataNotify->szDisplayName;
    }

    talklogic_CreateSession(callOutParam);
    return 1;
}


void CBWListener::ProcessCallPark(CCallInfo* pCallInfo, const yl::string& strNumber)
{
    CListenInfo* priv = GetCallerData(pCallInfo);
    if (pCallInfo == NULL || priv == NULL)
    {
        return;
    }

    if (priv->IsPrePark())
    {
        //ExitPrePark();
        priv->SetPark(false);
        priv->SetGPark(false);

        yl::string strURI = strNumber;
        // 进行Park的操作
        if (configParser_GetConfigInt(kszCallParkMode) != 0)
        {
            strURI = configParser_GetConfigString(kszCallParkCode) + strNumber;
        }

        callPark_CallPark(pCallInfo->GetCallID(), strURI, pCallInfo->GetAccountID());
    }
    else
    {
        bool bDirectSent = configParser_GetConfigInt(kszFeatureDirectSendEnable) == 1;
        yl::string strCallParkCode = configParser_GetConfigString(kszCallParkCode);
        // Fac模式并且code不为空才park
        if (configParser_GetConfigInt(kszCallParkMode) != 0
                && !strCallParkCode.empty()
                && bDirectSent)
        {
            //直接呼出特制码
            callPark_CallPark(pCallInfo->GetCallID(), strCallParkCode, pCallInfo->GetAccountID());
        }
        else
        {
            priv->SetPark(true);
        }
    }
}

void CBWListener::ProcessGroupCallPark(CCallInfo* pCallInfo, const yl::string& strNumber)
{
    CListenInfo* priv = GetCallerData(pCallInfo);
    if (pCallInfo == NULL || priv == NULL)
    {
        return;
    }

    if (priv->IsPreGPark())
    {
        if (!strNumber.empty())
        {
            priv->SetPark(false);
            priv->SetGPark(false);

            // 呼出输入的特征码
            CallOut_Param tCallParam;
            tCallParam.eCallOutType = TCT_CALLPARK;
            talklogic_CallOut(strNumber, yl::string(""), pCallInfo->GetAccountID(), &tCallParam, false);
        }
    }
    else
    {
        // XSI模式时直接Gpark；FAC模式时（1表示FAC），Code不为空才能Gpark
        if (configParser_GetConfigInt(kszCallParkMode) != 1
                || configParser_GetConfigString(kszGroupCallParkCode) != "")
        {
            // 执行GroupCallPark操作
            callPark_GroupCallPark(pCallInfo->GetCallID(), pCallInfo->GetAccountID());
        }
        else
        {
            priv->SetGPark(true);
        }
    }
}
#endif

void CBWListener::ParkFailed(const CSessionProxy& session, const yl::string& strFailedInfo, bool bCallPush/* = false*/)
{
    CCallInfo* pCallInfo = session.GetCallInfo();
    CListenInfo* priv = GetCallerData(pCallInfo);
    if (pCallInfo == NULL || priv == NULL)
    {
        return;
    }

    if (bCallPush)
    {
        priv->m_bCallPushFailed = true;
    }
    else
    {
        session.HoldSession(true);
        priv->m_bCallParkFailed = true;
    }

    priv->m_strParkFiled = strFailedInfo;
}

// 解析conf event数据
bool CBWListener::ParseConfEventInfo(CListenInfo& priv, int iAccountID, const yl::string& strEventInfo)
{
    if (strEventInfo.empty())
    {
        return false;
    }

    xml_document doc;
    xml_node nodeRoot;
    doc.load(strEventInfo.c_str());
    nodeRoot = doc.child(XML_NET_EVENT_INFO_ROOT);

    if (nodeRoot.empty())
    {
        return false;
    }

    xml_node nodeChild;
    nodeChild = nodeRoot.child(XML_NET_EVENT_INFO_DESCRIPTION);
    if (!nodeChild.empty())
    {
        xml_node nodeSubject = nodeChild.child(XML_NET_EVENT_INFO_SUBJECT);
        if (!nodeSubject.empty())
        {
            priv.m_strConfName = nodeSubject.child_value();
        }
    }

    xml_node nodeUsers = nodeRoot.child(XML_NET_EVENT_INFO_USERS);
    if (nodeUsers.empty())
    {
        return true;
    }

    xml_node nodeUserInfo = nodeUsers.child(XML_NET_EVENT_INFO_USER);
    while (!nodeUserInfo.empty()
            && strcmp("user", nodeUserInfo.name()) == 0)
    {
        yl::string strUserId = nodeUserInfo.attribute("entity").value();
        yl::string strUserName = strUserId;
        if (strUserName.substr(0, 4).compare("sip:") == 0)
        {
            strUserName = strUserName.substr(4);
        }

        int index = strUserName.find("@");

        if (index != yl::string::npos)
        {
            strUserName = strUserName.substr(0, index);
        }

        DataNetConfUserInfo dataUserInfo;
        //标记是否为会议建立者
        if (strUserName == configParser_GetCfgItemStringValue(kszAccountUserName, iAccountID))
        {
            dataUserInfo.bConfCreator = true;
        }

        dataUserInfo.strUserID = strUserId;

        xml_node nodeDisplayText = nodeUserInfo.child(XML_NET_EVENT_INFO_DISPLAYTEXT);
        if (nodeDisplayText.empty())
        {
            dataUserInfo.strUserName = strUserName;//dataUserInfo.strUserID;
        }
        else
        {
            dataUserInfo.strUserName = nodeDisplayText.child_value();
        }

        yl::string strUserState = nodeUserInfo.attribute("state").value();
        //state属性为delete则说明与会者已经退出会议
        if (0 == strcmp(strUserState.c_str(), "deleted"))
        {
            DeleteParticipantByUserId(priv, iAccountID, dataUserInfo.strUserID);
            nodeUserInfo = nodeUserInfo.next_sibling();
            continue;
        }
        //endpoint节点下的Status节点为disconnected则说明与会者已经退出会议
        xml_node nodeEndPoint = nodeUserInfo.child(XML_NET_EVENT_INFO_ENDPOINT);
        if (!nodeEndPoint.empty())
        {
            xml_node nodeStatus = nodeEndPoint.child(XML_NET_EVENT_INFO_STATUS);
            if (!nodeStatus.empty()
                    && 0 == strcmp("disconnected", nodeStatus.child_value()))
            {
                DeleteParticipantByUserId(priv, iAccountID, dataUserInfo.strUserID);
                nodeUserInfo = nodeUserInfo.next_sibling();
                continue;
            }
        }

        //其他情况则将userinfo加入与会者list中
        YLList<DataNetConfUserInfo>& list = priv.m_listNetUserInfo;
        YLList<DataNetConfUserInfo>::iterator it = list.begin();
        for (; it != list.end(); ++it)
        {
            DataNetConfUserInfo& TempUserInfo = (*it);
            if (TempUserInfo == dataUserInfo)
            {
                continue;
            }
            else if (TempUserInfo.strUserID == dataUserInfo.strUserID)
            {
                //userid相同、username不同则需要更新username
                TempUserInfo.strUserName = dataUserInfo.strUserName;
                continue;
            }
        }

        list.push_back(dataUserInfo);
        nodeUserInfo = nodeUserInfo.next_sibling();
    }

    return true;
}

//通过userid删除与会者list信息
bool CBWListener::DeleteParticipantByUserId(CListenInfo& priv, int iAccountID, const yl::string& strUserId)
{
    YLList<DataNetConfUserInfo>& list = priv.m_listNetUserInfo;
    YLList<DataNetConfUserInfo>::iterator it = list.begin();
    for (; it != list.end(); ++it)
    {
        DataNetConfUserInfo& TempUserInfo = (*it);
        if (TempUserInfo.strUserID == strUserId)
        {
            list.erase(it);
            return true;
        }
    }

    return false;
}

