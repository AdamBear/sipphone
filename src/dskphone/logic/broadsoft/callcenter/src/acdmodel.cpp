#include "callcenter_inc.h"
#include "talk/talklogic/include/callinfo.h"

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER

#define ACD_REASON_CODE_SECTION "ReasonCode"
#define ACD_DISPCODE_SECTION "BWDispCode"
#define ACD_SUPERVISOR_SECTION "SupervisorInfo"

bool ACDModel::ReqLogin(acd_accout * pAcd)
{
    return ReqChangeACDStatus(AA_LOGIN);
}

bool ACDModel::ProcessAfterLogin()
{
    ACD_STATUS eInitStatus = (ACD_STATUS)configParser_GetCfgItemIntValue(kszAccountACDInitialState,
                             GetAccountId());

    ACD_ACTION action = AA_AVAIL;

    switch (eInitStatus)
    {
    case AS_UNAVAILABLE:
        action = AA_UNAVAIL;
        break;
    case AS_WRAPUP:
        action = AA_WRAPUP;
        break;
    default:
        break;
    }

    m_bIslaterQeqQueueStatus = m_bIsQueueStatusOn;

    return ReqChangeACDStatus(action);
}

bool ACDModel::ReqChangeACDStatus(ACD_ACTION eACDAction)
{
    CALLCENTER_INFO("ACDModel::ReqChangeACDStatus eACDAction[%d]", eACDAction);
    //为-1表示没有 注册上的账号开启ACD功能
    if (GetAccountId() == -1)
    {
        return false;
    }

    if (!acc_IsAccountAvailable(GetAccountId()))
    {
        return false;
    }

    //如果账号同步不成功，则不允许进行ACD登录
    if (AA_LOGIN == eACDAction
            && !acc_GetSyncSubscribe(GetAccountId()))
    {
        CALLCENTER_INFO("wining! SyncSubscribe not open");
        return false;
    }

    yl::string strCommand;//请求命令
    yl::string strReasonCode;//Unavailable Reason Code

    switch (eACDAction)
    {
    case AA_LOGIN:
        {
            strCommand = "loggedOn";
        }
        break;
    case AA_LOGOUT:
        {
            strCommand = "loggedOff";
        }
        break;
    case AA_AVAIL:
        {
            strCommand = "ready";
        }
        break;
    case AA_WRAPUP:
        {
            strCommand = "workingAfterCall";
        }
        break;
    case AA_UNAVAIL:
        {
            strCommand = "notReady";
            //Reason开启则设置Unavailable Reason
            if (configParser_GetCfgItemIntValue(kszAccountACDUnavailReasonSwitch, GetAccountId()) == 1
                    && !m_strCurReason.empty())
            {
                int nReasonCode = atoi(m_strCurReason.c_str());
                //非None时才上传Reason
                strReasonCode = commonAPI_FormatString("<extensions>\r\n \
													   <privateData>\r\n \
													   <private xmlns:pri=\"http://schema.broadsoft.com/CSTAPrivateData\">\r\n \
													   <pri:AgentNotReadyReasonValue>%d</pri:AgentNotReadyReasonValue>\r\n \
													   </private>\r\n \
													   </privateData>\r\n \
													   </extensions>\r\n",
                                                       nReasonCode);
            }
        }
        break;
    default:
        break;
    }

    // 在发此包给服务器时，长度限制很严格，所以在给sip发postmsg时，带的extradata长度不能加1
    yl::string strActionXml =
        commonAPI_FormatString("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\r\n \
												 <SetAgentState xmlns=\"http://www.ecma-international.org/standards/ecma-323/csta/ed3\">\r\n \
												 <device></device>\r\n \
												 <requestedAgentState>%s</requestedAgentState>\r\n \
												 %s\
												 <agentID></agentID>\r\n \
												 <password></password>\r\n \
												 </SetAgentState>\r\n",
                               strCommand.c_str(),
                               strReasonCode.c_str());

    // 启动超时计时器.
    m_objTimer.SetTimer(5000, NULL, MK_MSG_ENTRY(this, ACDModel::OnTimeoutMessage));

    //发送本地请求给sip
    return modVoip_FeatureKeySync(GetAccountId(), GetAccountId(), strActionXml.c_str());
}

//绑定账号ID
void ACDModel::SetAccountId(int iAccountId)
{
    CACDModelBase::SetAccountId(iAccountId);
    if (GetAccountId() != -1)
    {
        m_bIsQueueStatusOn = (configParser_GetCfgItemIntValue(kszAccountEnableQueueStatus, GetAccountId()) == 1);
    }
    CALLCENTER_INFO("acd set account id[%d] queue status[%d] config[%d]", iAccountId, m_bIsQueueStatusOn, configParser_GetCfgItemIntValue(kszAccountEnableQueueStatus, GetAccountId()));
}

//设置ACD登陆初始化显示状态
bool ACDModel::SetInitStatus(ACD_STATUS eStatus)
{
    //直接保存到配置
    return configParser_SetCfgItemIntValue(kszAccountACDInitialState, GetAccountId(), eStatus);
}

ACD_STATUS ACDModel::GetInitStatus()
{
    return (ACD_STATUS)configParser_GetCfgItemIntValue(kszAccountACDInitialState, GetAccountId());
}

// yl::string ACDModel::GetCurReason()
// {
//  return m_strCurReason;
// }

void ACDModel::SetCurReason(const yl::string & strReason)
{
    m_strCurReason = strReason;
}

void ACDModel::GetCurReason(yl::string & strReasonName)
{
    strReasonName = GetReasonCodeNameByCode(m_strCurReason);
}

void ACDModel::GetDispCodeList(DispCodeList & refList)
{

    for (int i = 1; i <= 100; i++)

    {
        DispCode DispCodeData;
        DispCodeData.m_strCode = configParser_GetCfgItemStringValue(kszAccountACDDispCode, GetAccountId(),
                                 i);
        DispCodeData.m_strCodeName = configParser_GetCfgItemStringValue(kszAccountACDDispCodeName,
                                     GetAccountId(), i);

        //保存到list中
        refList.push_back(DispCodeData);
    }
}

void ACDModel::GetSupervisorList(SupervisorList & refList)
{
    for (int i = 1; i <= 100; i++)

    {
        Supervisor DataInfo;
        DataInfo.m_strNumber = configParser_GetCfgItemStringValue(kszAccountACDSupervisorCode,
                               GetAccountId(), i);
        DataInfo.m_strName = configParser_GetCfgItemStringValue(kszAccountACDSupervisorCodeName,
                             GetAccountId(), i);
        //保存到list中
        refList.push_back(DataInfo);
    }
}

//通过Reason Code 获取ReasonCode Name
yl::string ACDModel::GetReasonCodeNameByCode(const yl::string & strCode)
{
    DispCodeList listData;
    if (GetReasonCode(listData))
    {
        for (DispCodeList::iterator iter = listData.begin();
                iter != listData.end(); ++iter)
        {
            const DispCode & codePair = *iter;
            if (strCode == codePair.m_strCode)
            {
                return codePair.m_strCodeName;
            }
        }
    }

    return yl::string();
}

//设置当前队列信息
bool ACDModel::SetCurCallInfo(yl::string & strInfo, int nCallId)
{
    CALLCENTER_INFO("ACDModel::SetCurCallInfo:strInfo = %s", strInfo.c_str());
    //先清空原先数据
    m_curCallInfo.Reset();
    m_nCallerId = nCallId;

    //增加Xml头
    xml_document doc;
    xml_parse_result objResult = doc.load_buffer(strInfo.c_str(), strInfo.size());
    if (objResult.status == status_ok)
    {
        //获取根结点
        xml_node nodeRoot = doc.first_child();
        if (nodeRoot.empty())
        {
            return false;
        }

        xml_node xmlSection = nodeRoot.first_child();
        while (!xmlSection.empty())
        {
            yl::string strSectionName = xmlSection.name();
            if (strstr(strSectionName.c_str(), "waitTime") != NULL)
            {
                //获取WaitTime值
                m_curCallInfo.m_nWaitTime = atoi(xmlSection.text().get());
            }
            else if (strstr(strSectionName.c_str(), "ccUserId") != NULL)
            {
                //获取UserId值
                m_curCallInfo .m_strUserId = xmlSection.text().get();
            }
            else if (strstr(strSectionName.c_str(), "callCenterName") != NULL)
            {
                //获取CallCenterName值
                m_curCallInfo.m_strCallCenterName = xmlSection.text().get();
            }
            else if (strstr(strSectionName.c_str(), "numCallsInQueue") != NULL)
            {
                //获取CountInQueue值
                m_curCallInfo.m_nCountInQueue = atoi(xmlSection.text().get());
            }
            else if (strstr(strSectionName.c_str(), "longestWaitingTime") != NULL)
            {
                //获取CountInQueue值
                m_curCallInfo.m_nLongestTime = atoi(xmlSection.text().get());
            }

            xmlSection = xmlSection.next_sibling();
        }
    }
    else
    {
        return false;
    }

    return true;
}

//返回当前队列信息
const CallCenterInfo & ACDModel::GetCallInfo()
{
    return m_curCallInfo;
}

ACDModel::ACDModel()
{
    //初始化时就立即绑定可用账号，如未-1则说明绑定失败
    // For Test-syl

//  configParser_SetCfgItemIntValue(kszAccountEnableACD, 0, 1);
//  configParser_SetCfgItemIntValue(kszAccountEnableHoteling, 0, 1);
//  configParser_SetCfgItemIntValue(kszAccountEnableCallInfo, 0, 1);
//  configParser_SetCfgItemIntValue(kszAccountEnableDiapositionCode, 0, 1);
//  configParser_SetCfgItemIntValue(kszAccountEnableOriginatedTrace, 0, 1);
//  configParser_SetCfgItemIntValue(kszAccountEnableQueueStatus, 0, 1);

    //configParser_SetCfgItemIntValue(kszAccountACDInitialState, 0, 2);
//  GetAccountId() = -1;
//
//  int iAccountId = _AccountManager.GetACDAccountId();
//  SetAccountId(iAccountId);
//  m_eACDLineState = AS_IDLE;

    m_strCurReason.clear();
    Set_ACDType(ACD_BROADSOFT);
}

ACDModel::~ACDModel()
{
}

bool ACDModel::OriginatedTrace(int nCallId)
{
    if (!acc_IsAccountAvailable(GetAccountId()))
    {
        return false;
    }

    yl::string strCommand = "sip:customer-originated-trace@";
    strCommand.append(acc_GetServerName(GetAccountId()));

    if (nCallId != -1)
    {
        return modVoip_SendAcdInfoReq(GetAccountId(), nCallId, strCommand.c_str());
    }
    else
    {
        //非通话状态，发起一路通话
        return CallOut("customer-originated-trace", "", GetAccountId());
    }
}

bool ACDModel::SendDispCode(const yl::string & strCode, int nCallId/* = -1*/)
{

    CALLCENTER_INFO("ACDModel::SendDispCode[%s]", strCode.c_str());
    if (!acc_IsAccountAvailable(GetAccountId()))
    {
        return false;
    }

    //非通话中，不需要callId参数
    if (!talklogic_SessionExist())
    {
        nCallId = -1;
    }
    else
    {
        //通话中先获取当前焦点通话的callId
        int nSessionId = talklogic_GetFocusedSessionID();

        CCallInfo * pCallInfo = (CCallInfo *)talklogic_GetCallInfoBySessionID(nSessionId);
        if (pCallInfo == NULL)
        {
            return false;
        }
        nCallId = pCallInfo->GetCallID();
    }

    yl::string strCommand = "sip:cc-disposition-code@";
    strCommand.append(acc_GetServerName(GetAccountId()));
    if (!strCode.empty())
    {
        strCommand.append(";code=").append(strCode);
    }

    if (nCallId != -1)
    {
        return  modVoip_SendAcdInfoReq(GetAccountId(), nCallId, strCommand.c_str());
    }
    else
    {
        //非通话状态，发起一路通话

        CallOut_Param param;
        param.m_strCode = strCode.c_str();
        return CallOut("cc-disposition-code", "", GetAccountId(), &param);
    }
}

bool ACDModel::EmergencyEscalation(const yl::string & strNumber, int nCallId)
{
    if (!acc_IsAccountAvailable(GetAccountId()))
    {
        return false;
    }

    //通话中才有此功能
    if (!talklogic_SessionExist())
    {
        return false;
    }

    if (nCallId == -1)
    {
        //通话中先获取当前焦点通话的callId
        int nSessionId = talklogic_GetFocusedSessionID();

        CCallInfo * pCallInfo = (CCallInfo *)talklogic_GetCallInfoBySessionID(nSessionId);
        if (pCallInfo == NULL)
        {
            return false;
        }

        nCallId = pCallInfo->GetCallID();
    }

    yl::string strCommand = "sip:emergency-escalation@";

    strCommand.append(acc_GetServerName(GetAccountId()));
    if (!strNumber.empty())
    {
        strCommand.append(";supervisor=").append(strNumber);
    }

    return modVoip_SendAcdInfoReq(GetAccountId(), nCallId, strCommand.c_str());

}

bool ACDModel::ProcessQueueStatusXml(yl::string & strInfo)
{
    //先清空原先状态信息
    m_curQueueStatus.Reset();
    //第一个xml的开始位置
    int nIdex1 = strInfo.find("<list");
    //第一个xml结束位置
    int nIndex2 = strInfo.find("</list>");
    //第二个xml的开始位置
    int nIndex3 = strInfo.find("<CallCenterStatus");
    //第二个xml的结束位置
    int nIndex4 = strInfo.find("</CallCenterStatus>");
    yl::string strXml1;
    yl::string strXml2;

    if (nIdex1 != strXml1.npos && nIndex2 != strXml1.npos && nIdex1 < nIndex2)
    {
        //截取第一个xml
        strXml1 = strInfo.substr(nIdex1, nIndex2 - nIdex1 + 7);
        //增加Xml头
        xml_document doc;
        xml_parse_result objResult = doc.load_buffer(strXml1.c_str(), strXml1.size());
        if (objResult.status == status_ok)
        {
            //获取根结点
            xml_node nodeRoot = doc.first_child();
            if (nodeRoot.empty())
            {
                return false;
            }
            xml_node xmlResource = nodeRoot.child("resource");
            if (!xmlResource.empty())
            {
                xml_node xmlName = xmlResource.child("name");
                if (!xmlName.empty())
                {
                    //获取name值
                    m_curQueueStatus.m_strName = xmlName.text().get();
                }
            }
        }
    }

    if (nIndex3 != strXml1.npos && nIndex4 != strXml1.npos && nIndex3 < nIndex4)
    {
        //截取第二个xml信息
        strXml2 = strInfo.substr(nIndex3, nIndex4 - nIndex3 + 19);
        //增加Xml头
        xml_document doc;
        xml_parse_result objResult = doc.load_buffer(strXml2.c_str(), strXml2.size());
        if (objResult.status == status_ok)
        {
            //获取根结点
            xml_node nodeRoot = doc.first_child();
            if (nodeRoot.empty())
            {
                return false;
            }

            xml_node xmlNode = nodeRoot.child("ccUserId");
            if (!xmlNode.empty())
            {
                //获取userId值
                m_curQueueStatus.m_strUserId = xmlNode.text().get();
            }

            xmlNode = nodeRoot.child("status");
            if (!xmlNode.empty())
            {
                //获取状态值
                yl::string strStatus;
                strStatus = xmlNode.text().get();
                if (strStatus.compare("Threshold Exceeded") == 0)
                {
                    m_curQueueStatus.m_eStatus = QUEUE_THRESHOLD_EXCEEDED;
                }
                else if (strStatus.compare("Empty") == 0)
                {
                    m_curQueueStatus.m_eStatus = QUEUE_EMPTY;
                }
                else if (strStatus.compare("Normal") == 0)
                {
                    m_curQueueStatus.m_eStatus = QUEUE_NORMAL;
                }
            }
        }
    }
    return true;
}

bool ACDModel::ReqQueueStatus(bool isOn)
{
    if (!isOn)
    {
        //重置队列信息
        m_curQueueStatus.Reset();
    }

    if (!acc_IsAccountAvailable(GetAccountId()))
    {
        return false;
    }

    CALLCENTER_INFO("req queue status [%d]", isOn);
    return modVoip_ReqQueueStatus(GetAccountId(), isOn);
}

bool ACDModel::IsUnavailReasonCodeOn()
{
    if (GetAccountId() == -1)
    {
        return false;
    }
    return configParser_GetCfgItemIntValue(kszAccountACDUnavailReasonSwitch, GetAccountId()) == 1;
}

BOOL ACDModel::OnTimeoutMessage(msgObject & refObj)
{
    if (m_objTimer.IsTimerRuning())
    {
        m_objTimer.KillTimer();
    }
    CALLCENTER_INFO("ACD Login TimeOut!");

    //发送消息通知UI请求超时
    etl_NotifyApp(false,
                  ACD_MESSAGE_LOGIN_TIMEOUT, 0, 0);

    return TRUE;
}

void ACDModel::SetStatus(ACD_STATUS eStatus)
{
    CACDModelBase::SetStatus(eStatus);

    if (m_objTimer.IsTimerRuning())
    {
        m_objTimer.KillTimer();
    }
}


#endif //IF_FEATURE_BROADSOFT_CALL_CENTER
