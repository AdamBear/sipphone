#if IF_FEATURE_CTRIP_CTI
#include "ctripprocesser.h"
#include "account/include/modaccount.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "contacts/directory/include/moddirinterface.h"
#include "talk/talklogic/include/callinfo.h"
#include "commonunits/sipsender.h"
#include "commonapi/stringhelperapi.h"
#include <ylstringalgorithm.h>

#define    CTRIP_200_OK_HUAWEI                            200
#define    CTRIP_400_BAD_HUAWEI                           400
#define    CTRIP_403_UNKNOW_REERO_HUAWEI                  403
#define    CTRIP_403_SESSION_NOT_EXIST_HUAWEI             403
#define    CTRIP_486_BUSY_HUAWEI                           486
#define    kszCtripEventXML                        TEMP_PATH "CtripEventXml.xml"

#define    CTRIP_200_OK                        0
#define    CTRIP_400_BAD                       1
#define    CTRIP_403_UNKNOW_REERO               2
#define    CTRIP_403_SESSION_NOT_EXIST            3
#define    CTRIP_486_BUSY                         4

const char * const kszNodeCmd = "cmd";
const char * const kszNodeStatus = "status";
const char * const kszNodeCallID = "callID";
const char * const kszNodeCause = "cause";
const char * const kszNodeCalltag = "calltag";
const char * const kszAttrDnis = "Dnis";
const char * const kszAttrXUUI = "X-UUI";
const char * const kszAttrRemoteId = "RemoteId";

const char * const kszActionStartJoint = "startjoint";
const char * const kszActionStopJoint = "stopjoint";
const char * const kszActionSpeaker = "SPEAKER";
const char * const kszActionHangUp = "X";
const char * const kszActionHold = "hold+";
const char * const kszActionUnHold = "unhold+";

static LRESULT CtripProcessMessage(msgObject & objMessage)
{
    switch (objMessage.message)
    {
    case SIP_MSG_UC_REQ_INFO:
        {
            int iResult = g_objCtripManager.ProcessMessageOfeSpace(objMessage);
            g_objCtripManager.SendResponse(objMessage, MT_HUAWEI, iResult);
        }
        break;

    case SIP_MSG_UC_REQ_NOTIFY:
        {
            int iResult = g_objCtripManager.ProcessMessageOfCallControler(objMessage);
            g_objCtripManager.SendResponse(objMessage, MT_FREESWITCH, iResult);
        }
        break;
    default:
        break;
    }
    return 0;
}


CCtripProcesser & CCtripProcesser::GetInstace()
{
    static CCtripProcesser m_sInstace;

    return m_sInstace;
}

CCtripProcesser::CCtripProcesser(): CEventBase(EMT_CTRIP)
{
    m_listLineId.clear();

    etl_RegisterMsgHandle(SIP_MSG_UC_REQ_INFO,
                          SIP_MSG_UC_REQ_INFO,
                          CtripProcessMessage);


    etl_RegisterMsgHandle(SIP_MSG_UC_REQ_NOTIFY,
                          SIP_MSG_UC_REQ_NOTIFY,
                          CtripProcessMessage);
    RegisterModule();
}

CCtripProcesser::~CCtripProcesser()
{
    etl_UnregisterMsgHandle(SIP_MSG_UC_REQ_INFO,
                            SIP_MSG_UC_REQ_INFO,
                            CtripProcessMessage);

    etl_UnregisterMsgHandle(SIP_MSG_UC_REQ_NOTIFY,
                            SIP_MSG_UC_REQ_NOTIFY,
                            CtripProcessMessage);
}


int CCtripProcesser::ProcessMessageOfeSpace(msgObject & objMessage)
{
    APP_INFO("CCtripProcesser::ProcessMessageOfeSpace messg[%d],wparm[%d],lparm[%d]",
             objMessage.message, objMessage.wParam, objMessage.lParam);
    xml_document doc;
    xml_node node;

    sip_uc_req_info_t * pstData = (sip_uc_req_info_t *)objMessage.GetExtraData();
    if (pstData == NULL)
    {
        return CTRIP_400_BAD;
    }
    yl::string strContent = pstData->body;

    APP_INFO("strContent:\n%s", strContent.c_str());

    if (strContent.empty())
    {
        return CTRIP_400_BAD;
    }
    else
    {
        doc.load(strContent.c_str());
    }

    int nLineId = objMessage.wParam;

    node = doc.child(kszNodeCmd);
    if (node.empty())
    {
        node = doc.child(kszNodeStatus);
        if (node.empty())
        {
            return CTRIP_400_BAD;
        }
    }

    yl::string strCmd = node.text().get();

    yl::string strCallID = "";
    node = doc.child(kszNodeCallID);
    if (!node.empty())
    {
        strCallID = node.text().get();
    }

    return ProcessCmd(nLineId, strCmd, strCallID);
}

int CCtripProcesser::ProcessMessageOfCallControler(msgObject & objMessage)
{
    sip_uc_req_notify_t * pstData = (sip_uc_req_notify_t *)objMessage.GetExtraData();
    if (pstData == NULL)
    {
        return CTRIP_400_BAD;
    }

    yl::string strInfo = pstData->body;

    APP_INFO("CCtripProcesser::ProcessMessageOfCallControler lineId[%d],CallId[%d] body[%s]"
             , objMessage.wParam, objMessage.lParam, strInfo.c_str());

    if (strInfo.empty())
    {
        return CTRIP_400_BAD;
    }

    return ProcessCallControlCmd(objMessage.wParam, objMessage.lParam, strInfo);
}

int CCtripProcesser::ProcessCmd(int nLineId, yl::string strCmd, yl::string strCallID)
{
    strCmd.trim_both();
    strCmd.trim_both("\r \n");
    string_replace(strCmd, "action=", "");
    string_replace(strCmd, "key=", "");

    int nCallID = 0;
    if (!strCallID.empty())
    {
        YLVector<int> arrSession;
        talklogic_GetAllSessionID(arrSession);
        for (int iIndex = 0; iIndex < arrSession.size(); ++iIndex)
        {
            sip_call_proto_info2_t ucInfo;
            memset(&ucInfo, 0, sizeof(ucInfo));
            _SIPSender.GetCallInfo(nLineId, talklogic_GetCallIdBySessionID(arrSession[iIndex]), &ucInfo);
            if (strcmp(strCallID.c_str(), ucInfo.call_id) == 0)
            {
                nCallID = arrSession[iIndex];
                break;
            }
        }
    }

    if (commonAPI_StringMatch(strCmd, kszActionStartJoint, false, LEAD_MATCH))
    {
        ProcessControl(true, nLineId);
        return CTRIP_200_OK;
    }
    else if (commonAPI_StringMatch(strCmd, kszActionStopJoint, false, LEAD_MATCH))
    {
        ProcessControl(false, nLineId);
        return CTRIP_200_OK;
    }
    else
    {
        if (!IsControlByCtrip(nLineId))
        {
            APP_INFO("ProcessMessageOfeSpace account[%d] no accept control", nLineId);
            return CTRIP_486_BUSY;
        }
        if (commonAPI_StringMatch(strCmd, kszActionSpeaker, false, LEAD_MATCH))
        {
            if (!talklogic_SessionExist())
            {
                talklogic_EnterDialUIByAccount(nLineId);
                return CTRIP_200_OK;
            }
            return ProcessAnswerCall(nLineId, nCallID, "");
        }
        else if (commonAPI_StringMatch(strCmd, kszActionHangUp, false, LEAD_MATCH))
        {
            return ProcessHangUpCall(nLineId, nCallID, "");
        }
        else if (commonAPI_StringMatch(strCmd, kszActionHold, false, LEAD_MATCH))
        {
            yl::string strNum = "";
            int iPos = strCmd.find(kszActionHold);
            if (iPos != yl::string::npos)
            {
                strNum = strCmd.substr(iPos + 5);
            }
            return ProcessHoldCall(nLineId, nCallID, strNum);
        }
        else if (commonAPI_StringMatch(strCmd, kszActionUnHold, false, LEAD_MATCH))
        {
            yl::string strNum = "";
            int iPos = strCmd.find(kszActionUnHold);
            if (iPos != yl::string::npos)
            {
                strNum = strCmd.substr(iPos + 7);
            }
            return ProcessRetrieveCall(nLineId, nCallID, strNum);
        }
    }

    return CTRIP_400_BAD;
}

void CCtripProcesser::SendResponse(msgObject & objMessage, MessageType eType, int iResult)
{
    switch (eType)
    {
    case MT_HUAWEI:
        {
            int iHuaweiResult = g_objCtripManager.TransResponseCodeToHUAWEI(iResult);
            sip_uc_req_info_t * pstData = (sip_uc_req_info_t *)objMessage.GetExtraData();
            if (pstData == NULL || iHuaweiResult == -1)
            {
                return;
            }
            int nTid = pstData->tid;
            _SIPSender.SendCtripHWCTIResponse(objMessage.wParam, objMessage.lParam, nTid, iHuaweiResult);
        }
        break;
    case MT_FREESWITCH:
        {
            sip_uc_req_notify_t * pstData = (sip_uc_req_notify_t *)objMessage.GetExtraData();
            if (pstData == NULL)
            {
                return;
            }
            int nTid = pstData->tid;
            _SIPSender.SendCtripFSCTIResponse(objMessage.wParam, objMessage.lParam, nTid, iResult);
        }
        break;
    default:
        break;
    }
}

bool CCtripProcesser::IsControlByCtrip(int nLineId)
{
    YLList<int>::iterator itr = m_listLineId.begin();
    YLList<int>::iterator itEnd = m_listLineId.end();
    if (itr == itEnd)
    {
        return false;
    }

    while (itr != itEnd)
    {
        if (nLineId == *itr)
        {
            return acc_IsAccountAvailable(nLineId);
        }

        itr++;
    }
    return false;
}

bool CCtripProcesser::SendEvent(EventDataBase * pData)
{
    ACTION_INFO("CCtripProcesser::SendEvent[%d]", pData->m_eEventType);
    bool bRet = false;

#if IF_CUSTOMER_SUPPORT_0_XieCheng
    int nLineID = -1;
    int nCallID = 0;
    yl::string strStauts = "";//状态内容
    yl::string strCalltag = "";//主叫被叫

    switch (pData->m_eEventDataType)
    {
    case EDT_BASE:
        {
            if (pData->m_eEventType == EVENT_LOGIN_ON)
            {
                nLineID = pData->m_nLineID;
                strStauts = "online";
                bRet = true;
            }
        }
        break;
    case EDT_TALK:
        {
            bRet = GetTalkEventInfo(pData, nLineID, nCallID, strStauts, strCalltag);
        }
        break;
    default:
        break;
    }

    if (bRet)
    {
        SendRequest2eSpace(nLineID, nCallID, strStauts, strCalltag, pData->m_eEventType);
    }

#endif
    return bRet;
}

bool CCtripProcesser::GetTalkEventInfo(EventDataBase * pData, int & nLineId, int & nCallID,
                                       yl::string & strStauts, yl::string & strCalltag)
{
    TalkEventData * pTalkEventData = static_cast<TalkEventData *>(pData);
    if (pTalkEventData == NULL)
    {
        return false;
    }

    if (pTalkEventData->m_vecCallID.size() > 0)
    {
        nCallID = pTalkEventData->m_vecCallID[0];
    }

    const CCallInfo * pCallInfo = talklogic_GetCallInfoByCallId(nCallID);
    if (pCallInfo == NULL)
    {
        APP_WARN("CCtripProcesser Can not Get CallInfo by CallID[%d]", nCallID);
        return false;
    }

    nLineId = pCallInfo->GetAccountID();

    if (!IsControlByCtrip(nLineId))
    {
        APP_INFO("GetTalkEventInfo account[%d] no accept control", nLineId);
        return false;
    }

    bool bRet = true;
    switch (pTalkEventData->m_eEventType)
    {
    case EVENT_ENTER_DIAL:
        {
            nCallID = 0;
            strStauts = "offhook";
        }
        break;
    case EVENT_END_DIAL:
        {
            nCallID = 0;
            strStauts = "onhook";
        }
        break;
    case EVENT_TERMINATED:
        {
            strStauts = "onhook+";
            strStauts.append(pCallInfo->m_tRemoteInfo.sSIPName.c_str());
        }
        break;
    case EVENT_CALLOUT:
        {
            strStauts = "outgoing+";
            strStauts.append(pCallInfo->m_tRemoteInfo.sSIPName.c_str());
        }
        break;
    case EVENT_INCOMING_CALL:
        {
            strStauts = "ringing+";
            strStauts.append(pCallInfo->m_tRemoteInfo.sSIPName.c_str());
        }
        break;
    case EVENT_ESTABLISHED:
        {
            strStauts = "established+";
            strStauts.append(pCallInfo->m_tRemoteInfo.sSIPName.c_str());
            if (pCallInfo->m_eCallType == CT_OUTGOING)
            {
                strCalltag = "caller";
            }
            else
            {
                strCalltag = "callee";
            }
        }
        break;
    case EVENT_HOLD:
        {
            nCallID = 0;
            strStauts = "hold+";
            strStauts.append(pCallInfo->m_tRemoteInfo.sSIPName.c_str());
        }
        break;
    case EVENT_UNHOLD:
        {
            nCallID = 0;
            strStauts = "unhold+";
            strStauts.append(pCallInfo->m_tRemoteInfo.sSIPName.c_str());
        }
        break;
    case EVENT_CALL_FAIL:
    case EVENT_CANCEL_CALLOUT:
    case EVENT_CALL_REMOTE_NO_ANSWER:
        {
            strStauts = "failed+";
            strStauts.append(pCallInfo->m_tRemoteInfo.sSIPName.c_str());
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

void CCtripProcesser::SendRequest2eSpace(int & nLineId, int & nCallID, yl::string & strStauts,
        yl::string & strCalltag, EventType eType)
{
    APP_INFO("CCtripProcesser::SendRequest2eSpace nLineId[%d] nCallID[%d] strStauts[%s] strCalltag[%s] eType[%d]",
             nLineId, nCallID, strStauts.c_str(), strCalltag.c_str(), eType);
    sip_uc_req_send_info_t ucInfo;
    memset(&ucInfo, 0, sizeof(ucInfo));

    ucInfo.uuid = 0;

    if (eType == EVENT_UNHOLD
            || eType == EVENT_HOLD)
    {
        strStauts.insert(0, "key=");
        strcpy(ucInfo.cmd, strStauts.c_str());
    }
    else
    {
        strStauts.insert(0, "action=");
        strcpy(ucInfo.status, strStauts.c_str());
    }

    if (!strCalltag.empty())
    {
        strCalltag.insert(0, "tag=");
        strcpy(ucInfo.calltag, strCalltag.c_str());
    }

    if (eType == EVENT_CALL_FAIL
            || eType == EVENT_CANCEL_CALLOUT
            || eType == EVENT_CALL_REMOTE_NO_ANSWER)
    {
        strcpy(ucInfo.cause, "reason=failed");
    }

    _SIPSender.SendCtripHWCTIRequest(nLineId, nCallID, &ucInfo);

    /*
    sip_call_proto_info2_t ucInfo;
    memset(&ucInfo, 0, sizeof(ucInfo));
    if (nCallID != 0)
    {
        _SIPSender.GetCallInfo(nLineId, nCallID, &ucInfo);
    }

    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeStatus;
    if (eType == EVENT_UNHOLD
        || eType == EVENT_HOLD)
    {
        nodeStatus = doc.append_child(kszNodeCmd);
        strStauts.insert(0, "key=");
        nodeStatus.text().set(strStauts.c_str());
    }
    else
    {
        nodeStatus = doc.append_child(kszNodeStatus);
        strStauts.insert(0, "action=");
        nodeStatus.text().set(strStauts.c_str());
    }

    if (eType == EVENT_ESTABLISHED
        && !strCalltag.empty())
    {
        xml_node nodeCalltag;
        nodeCalltag = doc.append_child(kszNodeCalltag);
        strCalltag.insert(0, "tag=");
        nodeCalltag.text().set(strCalltag.c_str());
    }

    if (eType == EVENT_CALL_FAIL
        || eType == EVENT_CANCEL_CALLOUT)
    {
        xml_node nodeCause;
        nodeCause = doc.append_child(kszNodeCause);
        nodeCause.text().set("reason=failed");
    }

    yl::string strCallID = ucInfo.call_id;
    if (nCallID != 0
        && !strCallID.empty())
    {
        xml_node nodeCallID;
        nodeCallID = doc.append_child(kszNodeCallID);
        strCallID.insert(0, "callid=");
        nodeCallID.text().set(strCallID.c_str());
    }

    doc.save_file(kszCtripEventXML);

    yl::string strContent = yl::dump_file(kszCtripEventXML);
    APP_INFO("-=-=-= CCtripProcesser::SendRequest2eSpace strContent:\n%s\n=-=-=-", strContent.c_str());
    if (!strContent.empty())
    {
        _SIPSender.SendCtripHWCTIRequest(nLineId, nCallID, strContent.c_str());
    }
    */
}

bool CCtripProcesser::ProcessControl(bool bEnable, int nLineId)
{
    APP_INFO("CCtripProcesser::ProcessControl bEnable[%d] nLineId[%d]", bEnable, nLineId);
    YLList<int>::iterator itr = m_listLineId.begin();
    YLList<int>::iterator itEnd = m_listLineId.end();
    while (itr != itEnd)
    {
        if (nLineId == *itr)
        {
            m_listLineId.erase(itr);
            break;
        }
        itr++;
    }

    if (bEnable)
    {
        m_listLineId.push_back(nLineId);
    }

    return true;
}

int CCtripProcesser::ProcessMakeCall(int nLineId, yl::string strNum, yl::string strXUUI/* = ""*/)
{
    if (strNum.empty())
    {
        return CTRIP_400_BAD;
    }

    //该号码已在通话中
    /*CCallInfo *pCallInfo = (CCallInfo *)talklogic_GetCallInfoByAccountAndNum(nLineId, strNum);
    if (pCallInfo != NULL)
    {
        return CTRIP_486_BUSY;
    }*/

    CallOut_Param tCallParam;
    tCallParam.strXUUI = strXUUI;


    yl::string strName = "";
    Dir_GetCotactNameByNumber(strNum, strName);
    if (talklogic_CallOut(strNum, strName, nLineId, &tCallParam))
    {
        return CTRIP_200_OK;
    }
    return CTRIP_403_UNKNOW_REERO;
}


int CCtripProcesser::ProcessAnswerCall(int nLineId, int nCallID, yl::string strNum)
{
    APP_INFO("CCtripProcesser  ProcessAnswerCall(%d, %d, %s)", nLineId, nCallID, strNum.c_str());
    if (nCallID != 0)
    {
        if (talklogic_AnswerCall(nCallID))
        {
            return CTRIP_200_OK;
        }
        return CTRIP_403_SESSION_NOT_EXIST;
    }

    CCallInfo * pCallInfo = (CCallInfo *)talklogic_GetCallInfoByAccountAndNum(nLineId, strNum,
                            SESSION_RINGING);
    if (pCallInfo == NULL)
    {
        return CTRIP_403_SESSION_NOT_EXIST;
    }

    if (talklogic_AnswerCall(pCallInfo->GetCallID()))
    {
        return CTRIP_200_OK;
    }
    return CTRIP_403_UNKNOW_REERO;
}

int CCtripProcesser::ProcessHangUpCall(int nLineId, int nCallID, yl::string strNum)
{
    APP_INFO("CCtripProcesser  ProcessHangUpCall(%d, %d, %s)", nLineId, nCallID, strNum.c_str());
    if (nCallID != 0)
    {
        if (talklogic_ExitSession(nCallID))
        {
            return CTRIP_200_OK;
        }
        return CTRIP_403_SESSION_NOT_EXIST;
    }

    CCallInfo * pCallInfo = (CCallInfo *)talklogic_GetCallInfoByAccountAndNum(nLineId, strNum);
    if (pCallInfo == NULL)
    {
        return CTRIP_403_SESSION_NOT_EXIST;
    }

    if (talklogic_ExitSession(pCallInfo->GetCallID()))
    {
        return CTRIP_200_OK;
    }
    return CTRIP_403_UNKNOW_REERO;
}

int CCtripProcesser::ProcessHoldCall(int nLineId, int nCallID, yl::string strNum)
{
    APP_INFO("CCtripProcesser  ProcessHoldCall(%d, %d, %s)", nLineId, nCallID, strNum.c_str());
    if (nCallID != 0)
    {
        if (talklogic_HoldSession(nCallID))
        {
            return CTRIP_200_OK;
        }
        return CTRIP_403_SESSION_NOT_EXIST;
    }

    CCallInfo * pCallInfo = (CCallInfo *)talklogic_GetCallInfoByAccountAndNum(nLineId, strNum);
    if (pCallInfo == NULL)
    {
        //携程那边服务器有问题,推送下来的号码不是对方的号码导致找不到Session
        int iSessionID = talklogic_GetFocusedSessionID();
        pCallInfo = (CCallInfo *)talklogic_GetCallInfoBySessionID(iSessionID);
        APP_INFO("can not find [%s], get focus session[%d] [%p]", strNum.c_str(), iSessionID, pCallInfo);
        if (pCallInfo == NULL)
        {
            return CTRIP_403_SESSION_NOT_EXIST;
        }
    }

    if (!pCallInfo->IsLocalHold())
    {
        talklogic_HoldSession(pCallInfo->GetCallID());
        return CTRIP_200_OK;
    }
    return CTRIP_403_UNKNOW_REERO;
}

int CCtripProcesser::ProcessRetrieveCall(int nLineId, int nCallID, yl::string strNum)
{
    APP_INFO("CCtripProcesser  ProcessRetrieveCall(%d, %d, %s)", nLineId, nCallID, strNum.c_str());
    if (nCallID != 0)
    {
        if (talklogic_UnHoldSession(nCallID))
        {
            return CTRIP_200_OK;
        }
        return CTRIP_403_SESSION_NOT_EXIST;
    }

    CCallInfo * pCallInfo = (CCallInfo *)talklogic_GetCallInfoByAccountAndNum(nLineId, strNum);
    if (pCallInfo == NULL)
    {
        int iSessionID = talklogic_GetFocusedSessionID();
        pCallInfo = (CCallInfo *)talklogic_GetCallInfoBySessionID(iSessionID);
        APP_INFO("can not find [%s], get focus session[%d] [%p]", strNum.c_str(), iSessionID, pCallInfo);
        if (pCallInfo == NULL)
        {
            return CTRIP_403_SESSION_NOT_EXIST;
        }
    }

    if (pCallInfo->IsLocalHold())
    {
        talklogic_UnHoldSession(pCallInfo->GetCallID());
        return CTRIP_200_OK;
    }
    return CTRIP_403_UNKNOW_REERO;
}

//携程消息解析,
//拨号消息：<?xml version='1.0' encoding='utf-8'?><cmd Dnis='2801' X-UUI=' PYAI4953'>MakeCall</cmd> dnis欲拨号号码，xuui传给sip
//接听消息：<?xml version='1.0' encoding='utf-8'?><cmd>Answer</cmd>
//挂断消息： <?xml version='1.0' encoding='utf-8'?><cmd RemoteId ='4602'>HangUp</cmd> remoteid来电的号码
int CCtripProcesser::ProcessCallControlCmd(int nLineId, int iCallId, yl::string & strInfo)
{
    xml_document doc;
    xml_parse_result xmlResult = doc.load(strInfo.c_str());
    if (status_ok != xmlResult.status)
    {
        return CTRIP_400_BAD;
    }

    xml_node node = doc.child(kszNodeCmd);
    if (!node)
    {
        return CTRIP_400_BAD;
    }

    //解析消息结构体
    yl::string strCmd = node.text().get();
    yl::string strDnis = node.attribute(kszAttrDnis).value();
    yl::string strXUUI = node.attribute(kszAttrXUUI).value();
    yl::string strRemoteId = node.attribute(kszAttrRemoteId).value();

    if (strCmd.empty())
    {
        return CTRIP_400_BAD;
    }

    if (strCmd == "MakeCall")
    {
        return ProcessMakeCall(nLineId, strDnis, strXUUI);
    }
    else if (strCmd == "Answer")
    {
        return ProcessAnswerCall(nLineId, iCallId, "");
    }
    else if (strCmd == "HangUp")
    {
        return  ProcessHangUpCall(nLineId, iCallId, strRemoteId);
    }
    else
    {
        return CTRIP_403_UNKNOW_REERO;
    }

    return CTRIP_200_OK;
}

int CCtripProcesser::TransResponseCodeToHUAWEI(const int iResCode)
{
    switch (iResCode)
    {
    case CTRIP_200_OK:
        {
            return CTRIP_200_OK_HUAWEI;
        }
        break;

    case CTRIP_400_BAD:
        {
            return CTRIP_400_BAD_HUAWEI;
        }
        break;

    case CTRIP_403_SESSION_NOT_EXIST:
        {
            return CTRIP_403_SESSION_NOT_EXIST_HUAWEI;
        }
        break;

    case CTRIP_403_UNKNOW_REERO:
        {
            return CTRIP_403_UNKNOW_REERO_HUAWEI;
        }
        break;

    case CTRIP_486_BUSY:
        {
            return CTRIP_486_BUSY_HUAWEI;
        }
        break;

    default:
        {
            return -1;
        }
        break;
    }
}

#endif
