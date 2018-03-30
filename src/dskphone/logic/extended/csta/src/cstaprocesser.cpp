#include "cstaprocesser.h"
#include "account/include/modaccount.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "talk/fwd_dnd/include/modfwddnd.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talk/talklogic/include/callinfo.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "voip/modvoip.h"
#include <ylstringalgorithm.h>

#define  MAXBUFFER    10240
#define  MAX_TEMP_FILE  200


const char * const kszNodeCSTAErrorCode = "CSTAErrorCode";

const char * const kszNodeSoapEnvelope = "SOAP-ENV:Envelope";

const char * const kszNodeSoapBody = "SOAP-ENV:Body";

const char * const kszNodeMakeCall = "MakeCall";

const char * const kszNodeCallingDevice = "callingDevice"; //呼出账号

const char * const kszNodeCalledDirectoryNumber = "calledDirectoryNumber"; //呼叫号码

const char * const kszNodeAutoOriginate = "autoOriginate"; //使用通话模式


const char * const kszNodeMakeCallRe = "MakeCallResponse";

const char * const kszNodeHoldCall = "HoldCall";

const char * const kszNodeCallToBeHeld = "callToBeHeld";

const char * const kszNodeHoldCallResponse = "HoldCallResponse";

const char * const kszNodeRetrieveCall = "RetrieveCall";

const char * const kszNodeCallToBeRetrieved = "callToBeRetrieved";

const char * const kszNodeRetrieveCallResponse = "RetrieveCallResponse";

const char * const kszNodeAnswerCall = "AnswerCall"; //接起通话节点

const char * const kszNodeCallToBeAnswered = "callToBeAnswered"; //接起通话id节点

const char * const kszNodeAnswerCallResponse = "AnswerCallResponse"; //接起通话回复节点

const char * const kszNodeClearConnection = "ClearConnection"; //取消呼出，结束通话节点

const char * const kszNodeConnectionToBeCleared = "connectionToBeCleared"; //取消呼出id节点

const char * const kszNodeClearConnectionResponse =
    "ClearConnectionResponse"; //取消呼出回复节点

const char * const kszNodeSingleStepTransferCall = "SingleStepTransferCall"; //盲转

const char * const kszNodeActiveCall = "activeCall"; //Call id

const char * const kszNodeTransferredTo = "transferredTo"; //转接目标号码

const char * const kszNodeSingleStepTransferCallResponse =
    "SingleStepTransferCallResponse"; //盲转回复

const char * const kszNodeDeflectCall = "DeflectCall"; //ForWard Call

const char * const kszNodeCallToBeDiverted = "callToBeDiverted";

const char * const kszNodeCallID = "callID";

const char * const kszNodeDedeviceID = "deviceID";

const char * const kszNodeNewDestination = "newDestination";

const char * const kszNodeDevice = "device";

const char * const kszNodeDeflectCallResponse = "DeflectCallResponse";

const char * const kszNodeAlternateCall = "AlternateCall";

const char * const kszNodeHeldCall = "heldCall";

//const char* const kszNodeActiveCall = "csta:activeCall";

const char * const kszNodeAlternateCallResponse = "AlternateCallResponse";

const char * const kszNodeReconnectCall = "ReconnectCall";

const char * const kszNodeReconnectCallResponse = "ReconnectCallResponse";

#define kszResponseXml TEMP_PATH "CSTAResponseXml.xml"

#define kszSipResponseXML TEMP_PATH "CSTAResponseXml%d.xml"

#define kszStrErrorId "Invalid CallID for this command"

#define kszStrCalloutFail "Make Call Fail"

#define kszStrXmlContentError "XML Content error"

//错误回复类型
#define  kszStrInvalidConnectionIdentifierError "invalidConnectionIdentifier"

#define  kszStrServiceNotSupportedError "serviceNotSupported"

#define  kszStrInvalidConnectionStateError "invalidConnectionState"

#define  kszStrResourceBusyError "resourceBusy"

#define kszRequestSystemStatus "RequestSystemStatus"

#define kszserviceCrossRefID "serviceCrossRefID"

#define kszNodedevice "device"

#define kszSystemRegister "SystemRegister"

#define kszMonitorStart "MonitorStart"

#define kszmonitorCrossRefID "monitorCrossRefID"

#define kszGetSwitchingFunctionDevices "GetSwitchingFunctionDevices"

#define kszMonitorStop "MonitorStop"

#define kszGetDoNotDisturb "GetDoNotDisturb"

#define kszGetForwarding "GetForwarding"

#define kszSnapshotDevice "SnapshotDevice"

#define kszGetMessageWaitingIndicator "GetMessageWaitingIndicator"


#define kszNodeConsultationCall "ConsultationCall"

#define kszNodeTransferCall  "TransferCall"

#define kszGenerateDigits "GenerateDigits"
#define kszGenerateDigitsResponse "GenerateDigitsResponse"
#define kszConferenceCall "ConferenceCall"

#define kszSetForwarding  "SetForwarding"

#define kszSetDoNotDisturb "SetDoNotDisturb"

#define kszsysStatRegisterID  "sysStatRegisterID"

#define kszCSTAErrorCode "CSTAErrorCode"
#define kszunspecified "unspecified"

static LRESULT CSTAProcessMessage(msgObject & objMessage)
{

    bool suc = false;
    switch (objMessage.message)
    {
#if !IF_FEATURE_EVENT_MODULE
    case CONFIG_MSG_BCAST_CHANGED:
        if (objMessage.wParam == ST_SIPLINE)
        {
#ifdef IF_FEATURE_PSTN
            // // todo 由账号接口提供判断规则
            int iAccountId = objMessage.lParam;
            if (iAccountId >= PSTN_LINE_BEGIN_ID && iAccountId <= PSTN_LINE_END_ID)
            {
                // PSTN账号配置修改不需要发送SIP消息
                suc = true;
            }
            else
#endif // IF_FEATURE_PSTN
            {
                suc = g_objCSTAManager.SendDeviceChangeInfo();
            }
        }
        //else if (objMessage.wParam == ST_CSTA && objMessage.lParam == 0)
        //{
        //  g_objCSTAManager.OnRemoteControlConfigClose();

        //  return TRUE;
        //}
        break;

    case DND_MESSAGE_STATUS_CHANGED:
        suc = g_objCSTAManager.ProcessDndStatusChanged(objMessage);
        break;
    case FWD_MESSAGE_STATUS_CHANGED:
        suc = g_objCSTAManager.ProcessFwdStatusChanged(objMessage);
        break;
#endif
    default:
        suc = g_objCSTAManager.ProcessCSTA(objMessage);
        break;
    }

    APP_INFO("CSTAProcessMessage suc = [%d]", suc);

    if (objMessage.message == DSK_MSG_CSTA)
    {
        //回复网页
        objMessage.ReplyMessage(suc);
    }

    return suc;
}


CCSTAProcesser & CCSTAProcesser::GetInstace()
{
    static CCSTAProcesser m_sInstace;

    return m_sInstace;
}

CCSTAProcesser::CCSTAProcesser()
#if IF_FEATURE_EVENT_MODULE
    : CEventBase(EMT_CSTA)
#endif
{
    if (configParser_GetConfigInt(kszCSTAControl) == 0
            && configParser_GetConfigInt(kszISUME) == 0)
    {
#if !IF_FEATURE_EVENT_MODULE
        m_bCSTAEnable = false;
#endif //IF_FEATURE_EVENT_MODULE
        return;
    }

#if IF_FEATURE_EVENT_MODULE
    RegisterModule();
#else
    m_bCSTAEnable = true;
#endif //IF_FEATURE_EVENT_MODULE

    //注册网页发送消息
    etl_RegisterMsgHandle(DSK_MSG_CSTA,
                          DSK_MSG_CSTA,
                          CSTAProcessMessage);

    etl_RegisterMsgHandle(SIP_MSG_UACSTA_INFO,
                          SIP_MSG_UACSTA_INFO,
                          CSTAProcessMessage);
#if !IF_FEATURE_EVENT_MODULE
    etl_RegisterMsgHandle(SIP_CALL_INFORMATION_UPDATE,
                          SIP_CALL_INFORMATION_UPDATE,
                          CSTAProcessMessage);

    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED,
                          CONFIG_MSG_BCAST_CHANGED,
                          CSTAProcessMessage);

    // 注册FWD/DND状态改变消息
    SingleMsgReg(DND_MESSAGE_STATUS_CHANGED, CSTAProcessMessage);
    SingleMsgReg(FWD_MESSAGE_STATUS_CHANGED, CSTAProcessMessage);
#endif

    m_nLineId = 0;
}

CCSTAProcesser::~CCSTAProcesser()
{
    //注销网页发送消息
    etl_UnregisterMsgHandle(DSK_MSG_CSTA,
                            DSK_MSG_CSTA,
                            CSTAProcessMessage);

    etl_UnregisterMsgHandle(SIP_MSG_UACSTA_INFO,
                            SIP_MSG_UACSTA_INFO,
                            CSTAProcessMessage);
#if !IF_FEATURE_EVENT_MODULE
    etl_UnregisterMsgHandle(SIP_CALL_INFORMATION_UPDATE,
                            SIP_CALL_INFORMATION_UPDATE,
                            CSTAProcessMessage);

    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED,
                            CONFIG_MSG_BCAST_CHANGED,
                            CSTAProcessMessage);
#endif //IF_FEATURE_EVENT_MODULE
}


bool CCSTAProcesser::ProcessCSTA(msgObject & objMessage)
{
    APP_INFO("CCSTAProcesser::ProcessCSTA messg[%d],wparm[%d],lparm[%d]", objMessage.message,
             objMessage.wParam, objMessage.lParam);
    //to do
    bool bRes = false;

    xml_document doc;

    xml_node node;

    if (objMessage.message == DSK_MSG_CSTA)
    {
        // 这个消息,gui会处理,sip会处理,现在修改为gui收到网页的消息后,再调用sip的接口
        m_bIsUME = true;

        yl::string strContent;

        strContent = (char *)objMessage.GetExtraData();

        strContent.replace("csta:", "");

        if (!strContent.empty())
        {
            doc.load(strContent.c_str());
        }

        node = doc.child(kszNodeSoapEnvelope).child(kszNodeSoapBody);

        if (node.empty())
        {
            SetErrorReponseXmlDoc(kszStrXmlContentError);
            return false;
        }

        node = node.first_child();
    }
    else if (objMessage.message == SIP_MSG_UACSTA_INFO)
    {
        sip_uacsta_req_info_t * pstData = (sip_uacsta_req_info_t *)objMessage.GetExtraData();
        if (pstData == NULL)
        {
            return false;
        }
        yl::string strContent = pstData->body;

        if (!strContent.empty())
        {
            doc.load(strContent.c_str());
        }

        m_bIsUME = false;
        m_nTid = pstData->tid;
        m_nLineId = objMessage.wParam;
        m_nCSTACallId = objMessage.lParam;

        node = doc.first_child();
        //APP_INFO("SIP_MSG_UACSTA_INFO  m_nLineId=[%d] tid=[%d] m_nCSTACallId=[%d]",m_nLineId, m_nTid, m_nCSTACallId);

    }

    if (node.empty())
    {
        SetErrorReponseXmlDoc(kszStrXmlContentError);
        return false;
    }

    bRes = ProcessCmd(node);

    return bRes;
}

#if !IF_FEATURE_EVENT_MODULE
bool CCSTAProcesser::ProcessDndStatusChanged(msgObject & objMessage)
{
    if (objMessage.message != DND_MESSAGE_STATUS_CHANGED)
    {
        return false;
    }
    const int iAccount = objMessage.wParam;
    const bool bEnable = *((bool *)objMessage.GetExtraData());
    SendDnDEvent(iAccount, bEnable);
    return true;
}
bool CCSTAProcesser::ProcessFwdStatusChanged(msgObject & objMessage)
{
    if (objMessage.message != FWD_MESSAGE_STATUS_CHANGED)
    {
        return FALSE;
    }
    const int iAccount = objMessage.wParam;
    FwdMessageStatusData * pFwdInfo = (FwdMessageStatusData *)objMessage.GetExtraData();
    SendForwardEvent(iAccount, pFwdInfo);
    return true;
}
#endif // !IF_FEATURE_EVENT_MODULE

bool CCSTAProcesser::SendSipMsg(bool bIsEvent/* = false*/, bool isEndCsta/* = false*/)
{
    bool suc = false;

    if (isEndCsta)
    {
        return modVoip_HangupCall(m_nLineId, m_nCSTACallId, 0);
    }

    yl::string strContent = yl::dump_file(m_strResFile);

    APP_INFO("SendSipMsg(%d) m_nLineId:%d m_nCSTACallId:%d", bIsEvent, m_nLineId, m_nCSTACallId);

    if (bIsEvent)
    {
        suc = modVoip_SendCSTARequest(m_nLineId, m_nCSTACallId, strContent.c_str());
    }
    else
    {
        suc =  modVoip_SendCSTAResponse(m_nLineId, m_nCSTACallId, m_nTid, strContent.c_str());
    }

    //if (!suc)
    {
        remove(m_strResFile.c_str());
    }

    return suc;
}

//void CCSTAProcesser::OnRemoteControlConfigClose()
//{
//  if (!m_monitorMap.empty())
//  {
//      m_monitorMap.clear();
//
//      SendSipMsg(false,true);
//  }
//}

bool CCSTAProcesser::ProcessCmd(xml_node & node)
{
    yl::string strName = node.name();

    APP_INFO("CCSTAProcesser::ProcessCmd node Name = [%s]", strName.c_str());
    bool bRes = false;

    if (strName.compare(kszNodeMakeCall) == 0)
    {
        //呼出通话
        bRes = ProcessMakeCall(node);
    }
    else if (strName.compare(kszNodeHoldCall) == 0)
    {
        //Hold通话
        bRes = ProcessHoldCall(node);
    }
    else if (strName.compare(kszNodeRetrieveCall) == 0)
    {
        //unHold通话
        bRes = ProcessRetrieveCall(node);
    }
    else if (strName.compare(kszNodeAnswerCall) == 0)
    {
        //接起通话
        bRes = ProcessAnswerCall(node);
    }
    else if (strName.compare(kszNodeClearConnection) == 0)
    {
        //取消呼出，或挂断通话
        bRes = ProcessClearConnection(node);
    }
    else if (strName.compare(kszNodeSingleStepTransferCall) == 0)
    {
        //盲转
        bRes = ProcessBindTransfer(node);
    }
    else if (strName.compare(kszNodeTransferCall) == 0)
    {
        //转
        bRes = ProcessAttendTransfer(node);
    }
    else if (strName.compare(kszNodeConsultationCall) == 0)
    {
        //新起一路
        bRes = ProcessConsultationCall(node);
    }
    else if (strName.compare(kszNodeDeflectCall) == 0)
    {
        //ForWard通话
        bRes = ProcessDeflectCall(node);
    }
    else if (strName.compare(kszNodeAlternateCall) == 0)
    {
        //切换通话
        bRes = ProcessAlternateCall(node);
    }
    else if (strName.compare(kszNodeReconnectCall) == 0)
    {
        bRes = ProcessReconnectCall(node);
    }
    else if (strName.compare(kszRequestSystemStatus) == 0)
    {
        bRes = ProcessReqSystemStatus();
    }
    else if (strName.compare(kszGetSwitchingFunctionDevices) == 0)
    {
        bRes = ProcessGetSwitchingFunDevices();
    }
    else if (strName.compare(kszSystemRegister) == 0)
    {
        bRes = ProcessSystemRegister(node);
    }
    else if (strName.compare(kszMonitorStart) == 0)
    {
        bRes = ProcessMonitorStart(node);
    }
    else if (strName.compare(kszMonitorStop) == 0)
    {
        bRes = ProcessMonitorStop(node);
    }
    else if (strName.compare(kszGetForwarding) == 0)
    {
        bRes = ProcessGetForwarding(node);
    }
    else if (strName.compare(kszGetDoNotDisturb) == 0)
    {
        bRes = ProcessGetDoNotDisturb(node);
    }
    else if (strName.compare(kszSnapshotDevice) == 0)
    {
        bRes = ProcessSnapshotDevice();
    }
    else if (strName.compare(kszGetMessageWaitingIndicator) == 0)
    {
        bRes = ProcessGetMessageWaitingIndicator(node);
    }
    else if (strName.compare(kszConferenceCall) == 0)
    {
        //会议
        bRes = ProcessConferenceCall(node);
    }
    else if (strName.compare(kszGenerateDigits) == 0)
    {
        //通话中发送dtmf
        bRes = ProcessSendDtmf(node);
    }
    else if (strName.compare(kszSetForwarding) == 0)
    {
        //设置forward
        bRes = ProcessSetForward(node);
    }
    else if (strName.compare(kszSetDoNotDisturb) == 0)
    {
        //设置dnd
        bRes = ProcessSetDnD(node);
    }
    else
    {
        if (!m_bIsUME)
        {
            SendErrorResponse();
        }
    }

    return bRes;
}

bool CCSTAProcesser::ProcessMakeCall(xml_node & node)
{
    APP_INFO("CCSTAProcesser::ProcessMakeCall");

    if (talklogic_IsConnectingSessionExist()
            || talklogic_IsRingSessionExist())
    {
        APP_INFO("not allow make call");
        return true;
    }

    bool suc = false;

    xml_node childNode = node.child(kszNodeCallingDevice);

    yl::string strAccount;

    int nLineId = -1;

    if (!childNode.empty())
    {
        strAccount = childNode.child_value();

        nLineId = GetAcountIdBySipURI(strAccount);
    }

    if (nLineId == -1)
    {
        return false;
    }

    childNode = node.child(kszNodeCalledDirectoryNumber);

    if (childNode.empty())
    {
        return false;
    }

    yl::string strNumber = childNode.child_value();

    APP_INFO("strNumber[%s]", strNumber.c_str());

//  m_nMyCallId = 100;
//
//  SetResponseXmlDoc(kszNodeMakeCallRe,strAccount.c_str());
//
//  suc = talklogic_CallOut(strNumber.c_str(),"",nLineId,m_nMyCallId);


    m_nMyCallId = modVoip_NewCallId();

    SetResponseXmlDoc(kszNodeMakeCallRe, strAccount.c_str());

    if (!strNumber.empty())
    {
        suc = talklogic_CallOut(strNumber.c_str(), "", nLineId, m_nMyCallId);
    }

    if (suc)
    {
        //SetResponseXmlDoc(kszNodeMakeCallRe,strAccount.c_str());
    }
    else
    {
        //异常回复
        SetErrorReponseXmlDoc(kszStrCalloutFail);
    }

    return suc;
}

bool CCSTAProcesser::ProcessAnswerCall(xml_node & node)
{
    bool suc = false;

    xml_node childNode = node.child(kszNodeCallToBeAnswered);

    if (!childNode.empty())
    {
        xml_node nodeCallId = childNode.child(kszNodeCallID);

        yl::string strCallId;

        if (!nodeCallId.empty())
        {
            strCallId = nodeCallId.child_value();
        }
        else
        {
            strCallId = childNode.child_value();
        }

        int nCallId = atoi(strCallId.c_str());

        SetResponseXmlDoc(kszNodeAnswerCallResponse);

        suc = talklogic_AnswerCall(nCallId);
    }

    if (!suc)
    {
        //异常回复
        SetErrorReponseXmlDoc(kszStrErrorId);
    }

    return suc;

}

bool CCSTAProcesser::ProcessClearConnection(xml_node & node)
{
    bool suc = false;

    xml_node childNode = node.child(kszNodeConnectionToBeCleared);

    if (!childNode.empty())
    {
        xml_node nodeCallId = childNode.child(kszNodeCallID);

        yl::string strCallId;

        if (!nodeCallId.empty())
        {
            strCallId = nodeCallId.child_value();
        }
        else
        {
            strCallId = childNode.child_value();
        }

        APP_INFO("ProcessClearConnection strCallId %s", strCallId.c_str());

        YLList<yl::string> listCallId;

        commonAPI_splitStringBySeparator(strCallId, '|', listCallId);


        for (YLList<yl::string>::iterator iter = listCallId.begin(); iter != listCallId.end();
                iter++)
        {

            yl::string & refId = *iter;
            int nCallId = atoi(refId.c_str());

            APP_INFO("ProcessClearConnection callId %d", nCallId);

            yl::string strAccountInfo;

            SetResponseXmlDoc(kszNodeClearConnectionResponse);
            suc = talklogic_ExitSession(nCallId);
            if (suc)
            {
                break;
            }
        }
    }

    if (!suc)
    {
        //异常回复
        SetErrorReponseXmlDoc(kszStrErrorId);
    }

    return suc;

}

bool CCSTAProcesser::ProcessHoldCall(xml_node & node)
{
    bool suc = false;

    xml_node childNode = node.child(kszNodeCallToBeHeld);

    if (!childNode.empty())
    {
        xml_node nodeCallId = childNode.child(kszNodeCallID);

        yl::string strCallId;

        if (!nodeCallId.empty())
        {
            strCallId = nodeCallId.child_value();
        }
        else
        {
            strCallId = childNode.child_value();
        }

        if (!GetCallId(strCallId))
        {
            //异常回复
            SetErrorReponseXmlDoc(kszStrErrorId);
            return false;
        }

//      int index = strCallId.find("|");
//
//      if (index != yl::string::npos)
//      {
//          strCallId = strCallId.substr(0,index);
//      }

        int nCallId = atoi(strCallId.c_str());

        SetResponseXmlDoc(kszNodeHoldCallResponse);

        suc = talklogic_HoldSession(nCallId);
    }

    if (!suc)
    {
        //异常回复
        SetErrorReponseXmlDoc(kszStrErrorId);
    }
//  else
//  {
//      SetResponseXmlDoc(kszNodeHoldCallResponse);
//  }

    return suc;

}

bool CCSTAProcesser::ProcessRetrieveCall(xml_node & node)
{
    bool suc = false;

    xml_node childNode = node.child(kszNodeCallToBeRetrieved);

    if (!childNode.empty())
    {
        xml_node nodeCallId = childNode.child(kszNodeCallID);

        yl::string strCallId;

        if (!nodeCallId.empty())
        {
            strCallId = nodeCallId.child_value();
        }
        else
        {
            strCallId = childNode.child_value();
        }

        if (!GetCallId(strCallId))
        {
            //异常回复
            SetErrorReponseXmlDoc(kszStrErrorId);
            return false;
        }

//      int index = strCallId.find("|");
//
//      if (index != yl::string::npos)
//      {
//          strCallId = strCallId.substr(0,index);
//      }

        int nCallId = atoi(strCallId.c_str());

        SetResponseXmlDoc(kszNodeRetrieveCallResponse);

        suc = talklogic_UnHoldSession(nCallId);
    }

    if (!suc)
    {
        //异常回复
        SetErrorReponseXmlDoc(kszStrErrorId);
    }
//  else
//  {
//      SetResponseXmlDoc(kszNodeRetrieveCallResponse);
//  }

    return suc;

}

bool CCSTAProcesser::ProcessBindTransfer(xml_node & node)
{
    bool suc = false;

    xml_node childNode = node.child(kszNodeActiveCall);

    xml_node numberNode = node.child(kszNodeTransferredTo);

    yl::string strNumber;


    if (!childNode.empty() && !numberNode.empty())
    {
        xml_node nodeCallId = childNode.child(kszNodeCallID);

        yl::string strCallId;

        if (!nodeCallId.empty())
        {
            strCallId = nodeCallId.child_value();
        }
        else
        {
            strCallId = childNode.child_value();
        }

        m_strMyCallId = strCallId;

        if (!GetCallId(strCallId))
        {
            //异常回复
            SetErrorReponseXmlDoc(kszStrErrorId);
            return false;
        }

        m_nMyCallId = atoi(strCallId.c_str());

        strNumber = numberNode.child_value();

        SetResponseXmlDoc(kszNodeSingleStepTransferCallResponse, strNumber.c_str());

        suc = talklogic_BlindTransfer(m_nMyCallId, strNumber.c_str());
    }

    if (!suc)
    {
        //异常回复
        SetErrorReponseXmlDoc(kszStrErrorId);
    }

    return suc;
}


bool CCSTAProcesser::ProcessAttendTransfer(xml_node & node)
{
#define kszTransferCallResponse "TransferCallResponse"
    bool suc = false;

    yl::string strHeldId = GetCallId(node, kszNodeHeldCall);

    yl::string strActiveId = GetCallId(node, kszNodeActiveCall);


    if (!GetCallId(strActiveId) || !GetCallId(strHeldId))
    {
        return false;
    }

    int nHeldId = atoi(strHeldId.c_str());

    m_strMyCallId = GetCallId(node, kszNodeActiveCall);

    int nActiveId = atoi(strActiveId.c_str());

    yl::string strAccountInfo = GetAccountInfo(node, kszNodeHeldCall);

    m_nMyCallId = nActiveId;

    if (nHeldId != -1 && nActiveId != -1 && !strAccountInfo.empty())
    {
        SetResponseXmlDoc(kszTransferCallResponse, strAccountInfo.c_str());
        suc = talklogic_Transfer(nHeldId, nActiveId);
    }

    if (!suc)
    {
        //异常回复
        SetErrorReponseXmlDoc(kszStrErrorId);
    }

    return suc;
}

bool CCSTAProcesser::ProcessDeflectCall(xml_node & node)
{
    bool suc = false;

    xml_node divertedNode = node.child(kszNodeCallToBeDiverted);

    xml_node newDesNode = node.child(kszNodeNewDestination);

    if (!divertedNode.empty() && !newDesNode.empty())
    {
        xml_node deviceNode = newDesNode.child(kszNodeDevice);

        xml_node callIdNode = divertedNode.child(kszNodeCallID);

        if (!callIdNode.empty() && !deviceNode.empty())
        {
            yl::string strCallId = callIdNode.child_value();
            yl::string strNewDesNum = deviceNode.child_value();

            if (!strCallId.empty() && !strNewDesNum.empty())
            {
                SetResponseXmlDoc(kszNodeDeflectCallResponse);
                suc = talklogic_ForwardIncomingCall(atoi(strCallId.c_str()), strNewDesNum);
            }
        }
    }

    if (!suc)
    {
        //异常回复
        SetErrorReponseXmlDoc(kszStrErrorId);
    }

    return suc;
}

bool CCSTAProcesser::ProcessAlternateCall(xml_node & node)
{
    bool suc = false;

    xml_node heldNode = node.child(kszNodeHeldCall);

    xml_node activeNode = node.child(kszNodeActiveCall);

    if (!heldNode.empty() && !activeNode.empty())
    {
        xml_node heldIdNode = heldNode.child(kszNodeCallID);

        xml_node activeIdNode = activeNode.child(kszNodeCallID);

        if (!heldIdNode.empty() && !activeIdNode.empty())
        {
            yl::string strHeldCallId = heldIdNode.child_value();
            yl::string strActiveCallId = activeIdNode.child_value();

            if (!strHeldCallId.empty() && !strActiveCallId.empty())
            {
                SetResponseXmlDoc(kszNodeAlternateCallResponse);
                suc = talklogic_AlternateCall(atoi(strHeldCallId.c_str()), atoi(strActiveCallId.c_str()));
            }
        }
    }

    if (!suc)
    {
        //异常回复
        SetErrorReponseXmlDoc(kszStrErrorId);
    }

    return suc;
}

bool CCSTAProcesser::ProcessReconnectCall(xml_node & node)
{
    bool suc = false;

    xml_node heldNode = node.child(kszNodeHeldCall);

    xml_node activeNode = node.child(kszNodeActiveCall);

    if (!heldNode.empty() && !activeNode.empty())
    {
        xml_node heldIdNode = heldNode.child(kszNodeCallID);

        xml_node activeIdNode = activeNode.child(kszNodeCallID);

        if (!heldIdNode.empty() && !activeIdNode.empty())
        {
            yl::string strHeldCallId = heldIdNode.child_value();
            yl::string strActiveCallId = activeIdNode.child_value();

            if (!strHeldCallId.empty() && !strActiveCallId.empty())
            {
                SetResponseXmlDoc(kszNodeReconnectCallResponse);
                suc = talklogic_ReconnectCall(atoi(strActiveCallId.c_str()), atoi(strHeldCallId.c_str()));
            }
        }
    }

    if (!suc)
    {
        //异常回复
        SetErrorReponseXmlDoc(kszStrErrorId);
    }

    return suc;
}

bool CCSTAProcesser::ProcessConsultationCall(xml_node & node)
{
#define kszNodeexistingCall "existingCall"

#define kszNodeconsultedDevice "consultedDevice"

#define kszNodeConsultationCallResponse "ConsultationCallResponse"

    bool suc = false;

    xml_node nodeExist = node.child(kszNodeexistingCall);

    xml_node nodeNewOut = node.child(kszNodeconsultedDevice);

    yl::string strAccountInfo;

    if (!nodeExist.empty() && !nodeNewOut.empty())
    {
        yl::string strNewOut = nodeNewOut.child_value();

        xml_node nodeCallId = nodeExist.child(kszNodeCallID);

        xml_node nodeAccountInfo = nodeExist.child(kszNodeDedeviceID);

        if (!nodeCallId.empty() && ! nodeAccountInfo.empty())
        {
            //int nCallId = atoi(nodeCallId.child_value());

            strAccountInfo = nodeAccountInfo.child_value();

            int index = strAccountInfo.find(";tel");
            if (index != yl::string::npos)
            {
                strAccountInfo = strAccountInfo.substr(0, index);
            }

            int nAccount = GetAcountIdBySipURI(strAccountInfo);

            m_nMyCallId = modVoip_NewCallId();

            SetResponseXmlDoc(kszNodeConsultationCallResponse, strAccountInfo.c_str());

            suc = talklogic_CallOut(strNewOut, "", nAccount, m_nMyCallId);
        }
    }

    if (suc)
    {
        //SetResponseXmlDoc(kszNodeConsultationCallResponse,strAccountInfo.c_str());
    }
    else
    {
        //异常回复
        SetErrorReponseXmlDoc(kszStrErrorId);
    }

    return suc;
}

bool CCSTAProcesser::ProcessSendDtmf(xml_node & node)
{
#define kszconnectionToSendDigits "connectionToSendDigits"
#define kszcharactersToSend "charactersToSend"

    bool suc = false;


    xml_node childNode = node.child(kszconnectionToSendDigits);

    if (childNode.empty())
    {
        return false;
    }

    yl::string strCallId = childNode.child_value(kszNodeCallID);

    if (strCallId.empty())
    {
        return false;
    }

    if (!GetCallId(strCallId))
    {
        //异常回复
        SetErrorReponseXmlDoc(kszStrErrorId);
        return false;
    }

    yl::string strAccountInfo;

    yl::string strTestCallId;

    int nCallId = atoi(strCallId.c_str());

    if (nCallId != -1)
    {
        yl::string strKey = node.child_value(kszcharactersToSend);

        if (!strKey.empty())
        {
            SetResponseXmlDoc(kszGenerateDigitsResponse);
            suc = talklogic_SendDtmf(nCallId, strKey);
        }
    }

    if (!suc)
    {
        //异常回复
        SetErrorReponseXmlDoc(kszStrErrorId);
    }

    return suc;
}

bool CCSTAProcesser::ProcessConferenceCall(xml_node & node)
{
    APP_INFO("CCSTAProcesser::ProcessConferenceCall");
#define kszConferenceCallResponse "ConferenceCallResponse"
    bool suc = false;

    yl::string strHeldId = GetCallId(node, kszNodeHeldCall);

    yl::string strActiveId = GetCallId(node, kszNodeActiveCall);

    int nActiveId, nHeldId;

    bool bFound = GetCallId(strHeldId);

    nHeldId = atoi(strHeldId.c_str());

    if (!bFound)
    {
        return false;
    }

    bFound = GetCallId(strActiveId);

    nActiveId = atoi(strActiveId.c_str());

    if (!bFound)
    {
        return false;
    }

    yl::string strAccountInfo = GetAccountInfo(node, kszNodeHeldCall);

    m_nMyCallId = nActiveId;

    APP_INFO("-------nHeldId-%d,nActiveId-%d,", nHeldId, nActiveId);

    if (nHeldId != -1 && nActiveId != -1 && !strAccountInfo.empty())
    {
        SetConfereceReponse(talklogic_GetCallInfoByCallId(strActiveId),
                            talklogic_GetCallInfoByCallId(strHeldId), strAccountInfo.c_str());
        suc = talklogic_ConferenceCall(nActiveId, nHeldId);
    }

    if (!suc)
    {
        //异常回复
        SetErrorReponseXmlDoc(kszStrErrorId);
    }

    return suc;
}

bool CCSTAProcesser::ProcessReqSystemStatus()
{
#define kszReqSystemStatusResponse "RequestSystemStatusResponse"

#define kszsystemStatus "systemStatus"

    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRootNode = doc.append_child(kszReqSystemStatusResponse);
    nodeRootNode.append_attribute("xmlns",
                                  "http://www.ecma-international.org/standards/ecma-323/csta/ed5");

    xml_node node = nodeRootNode.append_child(kszsystemStatus);

    node.text().set("normal");

    doc.save_file(GetNewResFile().c_str());

    return SendSipMsg();
}

bool CCSTAProcesser::ProcessGetSwitchingFunDevices()
{
#define kszGetSwitchingFunDevicesResponse "GetSwitchingFunctionDevicesResponse"


    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRootNode = doc.append_child(kszGetSwitchingFunDevicesResponse);
    nodeRootNode.append_attribute("xmlns",
                                  "http://www.ecmainternational.org/standards/ecma-323/csta/ed5");


    xml_node node = nodeRootNode.append_child(kszserviceCrossRefID);

    //to do syl
    node.text().set("C2527445F5CC4848A7E80CBF0F76A11E");

    doc.save_file(GetNewResFile().c_str());

    bool suc = SendSipMsg();

    if (suc)
    {
        suc = SendDeviceInfo();
    }

    return suc;
}

bool CCSTAProcesser::ProcessSnapshotDevice()
{
#define kszSnapshotDeviceResponse "SnapshotDeviceResponse"
#define kszcrossRefIDorSnapshotData "crossRefIDorSnapshotData"
#define kszsnapshotData "snapshotData"
#define kszsnapshotDeviceResponseInfo "snapshotDeviceResponseInfo"
#define kszconnectionIdentifier "connectionIdentifier"
#define kszlocalCallState "localCallState"
#define kszcompoundCallState "compoundCallState"
#define kszlocalConnectionState "localConnectionState"


    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRootNode = doc.append_child(kszSnapshotDeviceResponse);
    nodeRootNode.append_attribute("xmlns",
                                  "http://www.ecmainternational.org/standards/ecma-323/csta/ed5");

    xml_node nodecrossRefIDorSnapshotData = nodeRootNode.append_child(kszcrossRefIDorSnapshotData);

    xml_node snapShotData  = nodecrossRefIDorSnapshotData.append_child(kszsnapshotData);


    yl::string strCallId;
    yl::string strAccountInfo;

    SESSION_STATE eStatus = talklogic_GetCurrentStatus(strCallId, strAccountInfo);


    if (eStatus != SESSION_UNKNOWN)
    {
        xml_node nodesnapshotDeviceResponseInfo = snapShotData.append_child(kszsnapshotDeviceResponseInfo);

        xml_node nodeconnectionIdentifier = nodesnapshotDeviceResponseInfo.append_child(
                                                kszconnectionIdentifier);

        xml_node nodeCallId = nodeconnectionIdentifier.append_child(kszNodeCallID);

        nodeCallId.text().set(strCallId.c_str());

        xml_node nodeDevice = nodeconnectionIdentifier.append_child(kszNodeDedeviceID);

        nodeDevice.text().set(strAccountInfo.c_str());

        xml_node nodelocalCallState = nodesnapshotDeviceResponseInfo.append_child(kszlocalCallState);

        xml_node nodecompoundCallState = nodelocalCallState.append_child(kszcompoundCallState);

        xml_node nodelocalConnectionState = nodecompoundCallState.append_child(kszlocalConnectionState);

        yl::string strStatus;

        APP_INFO("ProcessSnapshotDevice eStatus [%d]", eStatus);

        if (eStatus == SESSION_RINGING)
        {
            strStatus = "alerting";
        }
        else if (eStatus == SESSION_LOCALCONF)
        {
            APP_INFO("Now conference!");
            CCallInfo * pFirst = NULL;
            CCallInfo * pSecond = NULL;

            int index = strCallId.find("|");

            if (index != yl::string::npos)
            {
                strCallId = strCallId.substr(0, index);
            }

            talklogic_GetLocalConfCallInfo(atoi(strCallId.c_str()), &pFirst, &pSecond);

            if (pFirst != NULL && pFirst->IsLocalHold())
            {
                strStatus = "hold";
            }
            else
            {
                strStatus = "connected";
            }
        }
        else
        {
            const CCallInfo * pCalInfo = talklogic_GetCallInfoByCallId(strCallId);
            if (pCalInfo != NULL && pCalInfo->m_eTalkState == TS_HOLD)
            {
                strStatus = "hold";
            }
            else
            {
                strStatus = "connected";
            }
        }

        nodelocalConnectionState.text().set(strStatus.c_str());
    }

    doc.save_file(GetNewResFile().c_str());

    return SendSipMsg();
}

bool CCSTAProcesser::ProcessGetMessageWaitingIndicator(xml_node & node)
{
    xml_node childNode = node.child(kszNodedevice);

    if (childNode.empty())
    {
        return false;
    }

    yl::string strAccountInfo = childNode.child_value();

    int nAccount = GetAcountIdBySipURI(strAccountInfo);

    if (nAccount < 0)
    {
        return false;
    }

    bool isOn = (bool)configParser_GetCfgItemIntValue(kszAccountSubsribeMWI, nAccount);

#define kszGetMessageWaitingIndicatorResponse "GetMessageWaitingIndicatorResponse"
#define kszmessageWaitingOn "messageWaitingOn"

    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRootNode = doc.append_child(kszGetMessageWaitingIndicatorResponse);
    nodeRootNode.append_attribute("xmlns",
                                  "http://www.ecmainternational.org/standards/ecma-323/csta/ed5");

    xml_node nodeMessageWaitingOn = nodeRootNode.append_child(kszmessageWaitingOn);

    nodeMessageWaitingOn.text().set(isOn ? "true" : "false");

    doc.save_file(GetNewResFile().c_str());

    return SendSipMsg();
}

bool CCSTAProcesser::SendDeviceChangeInfo()
{
    APP_INFO(" CCSTAProcesser::SendDeviceChangeInfo()");
#define kszSwFunctionDevicesChanged "SwFunctionDevicesChanged"

    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRootNode = doc.append_child(kszSwFunctionDevicesChanged);
    nodeRootNode.append_attribute("xmlns",
                                  "http://www.ecmainternational.org/standards/ecma-323/csta/ed5");

    xml_node node = nodeRootNode.append_child(kszsysStatRegisterID);

    node.text().set("2");

    doc.save_file(GetNewResFile().c_str());

    return SendSipMsg(true);
}

bool CCSTAProcesser::SendDeviceInfo()
{
#define kszSwitchingFunctionDevices "SwitchingFunctionDevices"
#define kszlastSegment "lastSegment"
#define kszdeviceList "deviceList"

    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRootNode = doc.append_child(kszSwitchingFunctionDevices);
    nodeRootNode.append_attribute("xmlns",
                                  "http://www.ecmainternational.org/standards/ecma-323/csta/ed5");


    xml_node node = nodeRootNode.append_child(kszserviceCrossRefID);

    //to do syl
    node.text().set("C2527445F5CC4848A7E80CBF0F76A11E");

    node = nodeRootNode.append_child(kszlastSegment);

    node.text().set("true");

    xml_node nodeDeviceList = nodeRootNode.append_child(kszdeviceList);

    ListAccounts listAccounts = acc_GetAccountList(ACCOUNT_TYPE_ALL, true);

    for (ListAccounts::iterator it = listAccounts.begin();
            it != listAccounts.end();
            it++)
    {
        int iAccount = *it;

        //只上报enable账号
        if (!acc_IsAccountEnable(iAccount) || acc_IsDirectIPAccount(iAccount))
        {
            continue;
        }
        if (!acc_GetServerName(iAccount).empty() && !acc_GetUsername(iAccount).empty())
        {
            yl::string strPort = configParser_GetCfgItemStringValue(kszAccountSIPServerPort, iAccount);
            yl::string strAccountInfo = "sip:" + acc_GetFullName(iAccount);
            strAccountInfo.append(":").append(strPort);

            yl::string strTelUrl = configParser_GetCfgItemStringValue(kszAccountCstaTelUrl, iAccount);

            //NONF需加tel才能建立正常连接
            if (!strTelUrl.empty())
            {
                strAccountInfo.append(";tel=").append(strTelUrl);
                xml_node node = nodeDeviceList.append_child(kszNodedevice);

                node = node.append_child(kszNodeDedeviceID);

                node.text().set(strAccountInfo.c_str());
            }
        }
    }

    for (ListAccounts::iterator it = listAccounts.begin();
            it != listAccounts.end();
            it++)
    {
        int iAccount = *it;

        //只上报enable账号
        if (!acc_IsAccountEnable(iAccount) || acc_IsDirectIPAccount(iAccount))
        {
            continue;
        }
        if (!acc_GetServerName(iAccount).empty() && !acc_GetUsername(iAccount).empty())
        {
            yl::string strPort = configParser_GetCfgItemStringValue(kszAccountSIPServerPort, iAccount).c_str();
            yl::string strAccountInfo = "sip:" + acc_GetFullName(iAccount);
            strAccountInfo.append(":").append(strPort);

            yl::string strTelUrl = configParser_GetCfgItemStringValue(kszAccountCstaTelUrl, iAccount).c_str();

            if (!strTelUrl.empty())
            {
                continue;
            }

            xml_node node = nodeDeviceList.append_child(kszNodedevice);

            node = node.append_child(kszNodeDedeviceID);

            node.text().set(strAccountInfo.c_str());
        }
    }

    doc.save_file(GetNewResFile().c_str());

    return SendSipMsg(true);
}

bool CCSTAProcesser::ProcessSystemRegister(xml_node & node)
{
#define kszSystemRegisterResponse "SystemRegisterResponse"

    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRootNode = doc.append_child(kszSystemRegisterResponse);
    nodeRootNode.append_attribute("xmlns",
                                  "http://www.ecmainternational.org/standards/ecma-323/csta/ed5");


    xml_node nodeChild = nodeRootNode.append_child(kszsysStatRegisterID);

    nodeChild.text().set("2");

    doc.save_file(GetNewResFile().c_str());

    return SendSipMsg();
}

bool CCSTAProcesser::ProcessMonitorStart(xml_node & node)
{
    APP_INFO("CCSTAProcesser::ProcessMonitorStart");
#define kszmonitorObject "monitorObject"
#define kszdeviceObject "deviceObject"

    xml_node childNode = node.child(kszmonitorObject);

    if (childNode.empty())
    {
        return false;
    }

    childNode = childNode.child(kszdeviceObject);

    if (childNode.empty())
    {
        return false;
    }

    yl::string strAccount = childNode.child_value();



    int nLineId = GetAcountIdBySipURI(strAccount);

    if (nLineId < 0)
    {
        return false;
    }

    char szBuf[10];

    sprintf(szBuf, "%d", nLineId);

    if (m_monitorMap.find(nLineId) == m_monitorMap.end())
    {
        MonitorInfo info;
        info.m_strAccountInfo = strAccount;

        int index = info.m_strAccountInfo.find(";tel");
        if (index != yl::string::npos)
        {
            info.m_strAccountInfo = info.m_strAccountInfo.substr(0, index);
        }
        //info.m_strAccountInfo = strAccount.append("6070");
        info.m_strCrossId = szBuf;
        m_monitorMap.put(nLineId, info);
    }

#define kszMonitorStartResponse "MonitorStartResponse"

#define kszactualMonitorMediaClass "actualMonitorMediaClass"

#define kszmonitorExistingCalls "monitorExistingCalls"

    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRootNode = doc.append_child(kszMonitorStartResponse);
    nodeRootNode.append_attribute("xmlns",
                                  "http://www.ecmainternational.org/standards/ecma-323/csta/ed5");


    xml_node nodeChild = nodeRootNode.append_child(kszmonitorCrossRefID);

    nodeChild.text().set(szBuf);

    nodeChild = nodeRootNode.append_child(kszmonitorExistingCalls);

    nodeChild.text().set("true");

    doc.save_file(GetNewResFile().c_str());

    return SendSipMsg();
}

bool CCSTAProcesser::ProcessMonitorStop(xml_node & node)
{
    APP_INFO("CCSTAProcesser::ProcessMonitorStop");
#define kszMonitorStopResponse "MonitorStopResponse"
    xml_node nodeChild = node.child(kszmonitorCrossRefID);

    if (nodeChild.empty())
    {
        return false;
    }

    yl::string crossId = nodeChild.child_value();

    for (MonitorMap::iterator iter = m_monitorMap.begin(); iter != m_monitorMap.end(); iter++)
    {
        MonitorInfo & refInfo = iter->second;

        if (refInfo.m_strCrossId.compare(crossId) == 0)
        {
            m_monitorMap.erase(iter);
            break;
        }
    }

    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRootNode = doc.append_child(kszMonitorStopResponse);
    nodeRootNode.append_attribute("xmlns",
                                  "http://www.ecmainternational.org/standards/ecma-323/csta/ed5");

    doc.save_file(GetNewResFile().c_str());

    return SendSipMsg();
}

bool CCSTAProcesser::ProcessGetForwarding(xml_node & node)
{

    xml_node nodeChild = node.child(kszNodedevice);

    if (nodeChild.empty())
    {
        return false;
    }

    yl::string strAccountInfo = nodeChild.child_value();

    int nAccountId = GetAcountIdBySipURI(strAccountInfo);

#define kszGetForwardingResponse "GetForwardingResponse"
#define kszforwardingList "forwardingList"
#define kszforwardListItem "forwardListItem"
#define kszforwardingType "forwardingType"
#define kszforwardStatus "forwardStatus"
#define kszforwardDN "forwardDN"
#define kszringCount "ringCount"
#define kszforwardImmediate "forwardImmediate"
#define kszforwardBusy "forwardBusy"
#define kszforwardNoAns "forwardNoAns"

    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRootNode = doc.append_child(kszGetForwardingResponse);
    nodeRootNode.append_attribute("xmlns",
                                  "http://www.ecmainternational.org/standards/ecma-323/csta/ed5");


    xml_node nodeList = nodeRootNode.append_child(kszforwardingList);

    {
        yl::string strTarget;
        bool bEnable = fwd_GetInfo(nAccountId, FWD_TYPE_ALWAYS, &strTarget);
        xml_node nodeItem = nodeList.append_child(kszforwardListItem);
        xml_node nodeType = nodeItem.append_child(kszforwardingType);
        nodeType.text().set("forwardImmediate");
        xml_node nodeStatus = nodeItem.append_child(kszforwardStatus);
        nodeStatus.text().set(bEnable ? "true" : "false");
        xml_node nodeforwardDN = nodeItem.append_child(kszforwardDN);
        yl::string strFullTarget = "tel:";
        strFullTarget.append(strTarget);
        nodeforwardDN.text().set(strFullTarget.c_str());
    }
    {
        yl::string strTarget;
        bool bEnable = fwd_GetInfo(nAccountId, FWD_TYPE_BUSY, &strTarget);
        xml_node nodeItem = nodeList.append_child(kszforwardListItem);
        xml_node nodeType = nodeItem.append_child(kszforwardingType);
        nodeType.text().set("forwardBusy");
        xml_node nodeStatus = nodeItem.append_child(kszforwardStatus);
        nodeStatus.text().set(bEnable ? "true" : "false");
        xml_node nodeforwardDN = nodeItem.append_child(kszforwardDN);
        yl::string strFullTarget = "tel:";
        strFullTarget.append(strTarget);
        nodeforwardDN.text().set(strFullTarget.c_str());
    }
    {
        yl::string strTarget;
        int iRingDelay = 0;
        bool bEnable = fwd_GetInfo(nAccountId, FWD_TYPE_BUSY, &strTarget, &iRingDelay);
        xml_node nodeItem = nodeList.append_child(kszforwardListItem);
        xml_node nodeType = nodeItem.append_child(kszforwardingType);
        nodeType.text().set("forwardNoAns");
        xml_node nodeStatus = nodeItem.append_child(kszforwardStatus);
        nodeStatus.text().set(bEnable ? "true" : "false");
        xml_node nodeforwardDN = nodeItem.append_child(kszforwardDN);
        yl::string strFullTarget = "tel:";
        strFullTarget.append(strTarget);
        nodeforwardDN.text().set(strFullTarget.c_str());
        xml_node nodeCount = nodeItem.append_child("ringDuration");
        char szBuf[10];
        sprintf(szBuf, "%d", iRingDelay);
        nodeCount.text().set(szBuf);
    }

    doc.save_file(GetNewResFile().c_str());

    return SendSipMsg();
}

bool CCSTAProcesser::ProcessGetDoNotDisturb(xml_node & node)
{
    xml_node nodeChild = node.child(kszNodedevice);

    if (nodeChild.empty())
    {
        return false;
    }

    yl::string strAccountInfo = nodeChild.child_value();

    int nAccountId = GetAcountIdBySipURI(strAccountInfo);

#define kszGetDoNotDisturbResponse "GetDoNotDisturbResponse"
#define kszdoNotDisturbOn "doNotDisturbOn"

    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRootNode = doc.append_child(kszGetDoNotDisturbResponse);
    nodeRootNode.append_attribute("xmlns",
                                  "http://www.ecma-international.org/standards/ecma-323/csta/ed3");

    xml_node nodeDND = nodeRootNode.append_child(kszdoNotDisturbOn);

    nodeDND.text().set(dnd_IsEnable(nAccountId) ? "true" : "false");


    doc.save_file(GetNewResFile().c_str());

    return SendSipMsg();
}

int GetFwdTypeByString(const yl::string & strFwd)
{
    if (strFwd.compare(kszforwardImmediate) == 0)
    {
        return FWD_TYPE_ALWAYS;
    }
    else if (strFwd.compare(kszforwardBusy) == 0)
    {
        return FWD_TYPE_BUSY;
    }
    else if (strFwd.compare(kszforwardNoAns) == 0)
    {
        return FWD_TYPE_NO_ANSWER;
    }

    return -1;
}

bool CCSTAProcesser::ProcessSetForward(xml_node & node)
{
    APP_INFO("CCSTAProcesser::ProcessSetForward");
#define kszSetForwardingResponse "SetForwardingResponse"
#define kszactivateForward "activateForward"

    yl::string strAccountInfo = node.child_value(kszNodedevice);
    yl::string strFwd = node.child_value(kszforwardingType);
    int nLine = GetAcountIdBySipURI(strAccountInfo);
    int nType = GetFwdTypeByString(strFwd);
    APP_INFO("type %d,nLine %d", nType, nLine);
    if (nLine < 0 || nType < 0)
    {
        return false;
    }

    yl::string strStatus = node.child_value(kszactivateForward);

    bool bEnabled = (strStatus.compare("true") == 0);
    yl::string strTarget = node.child_value(kszforwardDN);
    int iRingDelay = 0;

    APP_INFO("------------strStatus %s,fwdElem.strTarget %s", strStatus.c_str(), strTarget.c_str());

    xml_node nodeRingCount = node.child(kszringCount);
    if (!nodeRingCount.empty())
    {
        iRingDelay = atoi(node.child_value(kszringCount));
    }

    SetResponseXmlDoc(kszSetForwardingResponse);
    bool suc = fwd_SetInfo(nLine, (FWD_TYPE)nType, bEnabled, &strTarget,
                           (!nodeRingCount.empty()) ? &iRingDelay : NULL, FWD_CFG_SRC_TYPE_SYNC);


    APP_INFO("------------------suc = %d", suc);

    if (!suc)
    {
        //异常回复
        SetErrorReponseXmlDoc(kszStrErrorId);
    }

    return true;
}

bool CCSTAProcesser::ProcessSetDnD(xml_node & node)
{
#define kszSetDoNotDisturbResponse "SetDoNotDisturbResponse"
    yl::string strAccountInfo = node.child_value(kszNodedevice);

    int nLine = GetAcountIdBySipURI(strAccountInfo);

    yl::string strStatus = node.child_value(kszdoNotDisturbOn);

    bool bEnabled = (strStatus.compare("true") == 0);

    if (nLine < 0)
    {
        return false;
    }
    bool suc = false;

    SetResponseXmlDoc(kszSetDoNotDisturbResponse);

    suc = dnd_SetEnable(nLine, bEnabled, DND_CFG_SRC_TYPE_SYNC);

    if (!suc)
    {
        //异常回复
        SetErrorReponseXmlDoc(kszStrErrorId);
    }

    return true;
}

void CCSTAProcesser::AddResponseInfo(xml_node & node, const yl::string & strChildNodeName,
                                     const char * pExt)
{
    xml_node nodeAppend = node.append_child(strChildNodeName.c_str());

    xml_node nodeChild = nodeAppend.append_child(kszNodeCallID);

    if (strChildNodeName.compare("transferredCall") == 0)
    {
        nodeChild.text().set(m_strMyCallId.c_str());
    }
    else
    {
        char szBuf[256];
        sprintf(szBuf, "%d", m_nMyCallId);

        nodeChild.text().set(szBuf);
    }

    if (pExt != NULL)
    {
        nodeChild = nodeAppend.append_child(kszNodeDedeviceID);

        nodeChild.text().set(pExt);
    }
}



void CCSTAProcesser::SetResponseXmlDoc(const char * pChar, const char * pExt/* = NULL*/)
{
    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node node = doc.append_child(pChar);

    node.append_attribute("xmlns", "http://www.ecma-international.org/standards/ecma-323/csta/ed5");

    if (strcmp(pChar, kszNodeMakeCallRe) == 0)
    {
        AddResponseInfo(node, kszNodeCallingDevice, pExt);
    }
    else if (strcmp(pChar, kszNodeConsultationCallResponse) == 0)
    {
        AddResponseInfo(node, "initiatedCall", pExt);
    }
    else if (strcmp(pChar, kszNodeSingleStepTransferCallResponse) == 0
             || strcmp(pChar, kszTransferCallResponse) == 0)
    {
        AddResponseInfo(node, "transferredCall", pExt);
    }
    else if (strcmp(pChar, kszCSTAErrorCode) == 0)
    {
        node.append_child(kszunspecified);
    }

    if (m_bIsUME)
    {
        doc.save_file(kszResponseXml);
    }
    else
    {
        doc.save_file(GetNewResFile().c_str());

        SendSipMsg();

        APP_INFO("------------------SetResponseXmlDoc,%s", pChar);
    }
}

void CCSTAProcesser::AddConnectionListItem(xml_node & node, const yl::string & strConfId,
        const CCallInfo * pCallInfo, const yl::string & strDeviceUrl)
{
#define kszconnectionListItem "connectionListItem"
#define ksznewConnection "newConnection"
#define kszoldConnection "oldConnection"

    xml_node nodeItem = node.append_child(kszconnectionListItem);

    if (!nodeItem.empty())
    {
        xml_node nodeConnection = nodeItem.append_child(ksznewConnection);

        xml_node nodeChild = nodeConnection.append_child(kszNodeCallID);

        nodeChild.text().set(strConfId.c_str());

        nodeChild = nodeConnection.append_child(kszNodeDedeviceID);

        nodeChild.text().set(strDeviceUrl.c_str());


        nodeConnection = nodeItem.append_child(kszoldConnection);

        nodeChild = nodeConnection.append_child(kszNodeCallID);

        if (pCallInfo->m_strConfId.empty())
        {
            char szbuf[256] = {0};

            sprintf(szbuf, "%d", pCallInfo->GetCallID());
            nodeChild.text().set(szbuf);
        }
        else
        {
            nodeChild.text().set(pCallInfo->m_strConfId.c_str());
        }


        nodeChild = nodeConnection.append_child(kszNodeDedeviceID);

        nodeChild.text().set(strDeviceUrl.c_str());
    }
}



void CCSTAProcesser::SetConfereceReponse(const CCallInfo * pActivCallInfo,
        const CCallInfo * pHeldICallInfo, const char * pExt)
{
#define kszconferenceCall "conferenceCall"
#define kszconnections "connections"

    if (pActivCallInfo == NULL || pHeldICallInfo == NULL)
    {
        return;
    }

//  CCallInfo* pFirst = NULL;
//  CCallInfo* pSencend = NULL;

    char szBuf[256] = {0};


    sprintf(szBuf, "%d", pHeldICallInfo->GetCallID());

    yl::string strHeldId = szBuf;


    memset(szBuf, 0, 256);

    sprintf(szBuf, "%d", pActivCallInfo->GetCallID());

    yl::string strActiveId = szBuf;


    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node node = doc.append_child(kszConferenceCallResponse);

    node.append_attribute("xmlns", "http://www.ecma-international.org/standards/ecma-323/csta/ed5");

    xml_node nodeAppend = node.append_child(kszconferenceCall);

    xml_node nodeChild = nodeAppend.append_child(kszNodeCallID);

    memset(szBuf, 0, 256);

    sprintf(szBuf, "%d|%d", pHeldICallInfo->GetCallID(), pActivCallInfo->GetCallID());

    yl::string strConfId = szBuf;

    nodeChild.text().set(strConfId.c_str());

    if (pExt != NULL)
    {
        nodeChild = nodeAppend.append_child(kszNodeDedeviceID);

        nodeChild.text().set(pExt);
    }

    xml_node nodeConnections = node.append_child(kszconnections);

    if (!nodeConnections.empty())
    {
        AddConnectionListItem(nodeConnections, strConfId, pActivCallInfo, pExt);

        AddConnectionListItem(nodeConnections, strConfId, pActivCallInfo,
                              pActivCallInfo->GetRemoteSipURL());

        AddConnectionListItem(nodeConnections, strConfId, pHeldICallInfo,
                              pHeldICallInfo->GetRemoteSipURL());
    }

    if (m_bIsUME)
    {
        doc.save_file(kszResponseXml);
    }
    else
    {
        doc.save_file(GetNewResFile().c_str());

        SendSipMsg();

        APP_INFO("------------------SetConfereceReponse");
    }
}

void CCSTAProcesser::SetErrorReponseXmlDoc(const yl::string & strErrorInfo)
{
    APP_INFO("CCSTAProcesser::SetErrorReponseXmlDoc");
    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRootNode = doc.append_child(kszNodeSoapEnvelope);
    nodeRootNode.append_attribute("xmlns:SOAP-ENV", "http://schemas.xmlsoap.org/soap/envelope/");
    nodeRootNode.append_attribute("xmlns:csta",
                                  "http://www.ecma-international.org/standards/ecma-323/csta/ed4");

    xml_node nodeBody = nodeRootNode.append_child(kszNodeSoapBody);

    xml_node faultNode = nodeBody.append_child("SOAP-ENV:Fault");

    xml_node faultcode = faultNode.append_child("SOAP-ENV:faultcode");

    xml_node faultstring = faultNode.append_child("SOAP-ENV:faultstring");

    faultcode.text().set("Client");

    faultstring.text().set("CSTA Error");


    xml_node detail = faultNode.append_child("SOAP-ENV:detail");

    xml_node CSTAErrorCode = detail.append_child("csta:CSTAErrorCode");

    CSTAErrorCode.append_attribute("xmlns:csta",
                                   "http://www.ecma-international.org/standards/ecma-323/csta/ed4");

    xml_node operation = CSTAErrorCode.append_child("csta:operation");

    operation.text().set(strErrorInfo.c_str());

    doc.save_file(kszResponseXml);
}

int CCSTAProcesser::GetAcountIdBySipURI(const yl::string & strSipURI)
{
    yl::string strCpy = strSipURI;
    if (strCpy.length() > 5
            && strCpy.substr(0, 5).compare("sips:") == 0)
    {
        strCpy = strCpy.substr(5);
    }

    if (strCpy.length() > 4
            && strCpy.substr(0, 4).compare("sip:") == 0)
    {
        strCpy = strCpy.substr(4);
    }
    //strSipURI = CommonAPI_TrimString(strSipURI,"sip:",TRIM_LEFT);

    int index = strCpy.find("@");

    yl::string strUserName = strCpy;
    yl::string strHost;

    if (index != yl::string::npos)
    {
        strUserName = strCpy.substr(0, index);
        strHost = strCpy.substr(index + 1);
    }

    index = strHost.find(":");
    if (index != yl::string::npos)
    {
        strHost = strHost.substr(0, index);
    }

    index = strHost.find(";tel");

    if (index != yl::string::npos)
    {
        strHost = strHost.substr(0, index);
    }

    APP_INFO("strUserName[%s],strHost[%s]", strUserName.c_str(), strHost.c_str());
    return acc_GetAccountIDByName(strUserName.c_str(), strHost.c_str());
}


const yl::string & CCSTAProcesser::GetNewResFile()
{
    static int s_iTempFileId = 0;
    if (s_iTempFileId >= MAX_TEMP_FILE)
    {
        s_iTempFileId = 0;
    }
    // 生成本地保存路径.
    char szFileId[256] = {0};
    sprintf(szFileId, kszSipResponseXML, ++s_iTempFileId);
    m_strResFile = szFileId;

    APP_INFO("GetConfigFileName(%s)", m_strResFile.c_str());
    return m_strResFile;
}

bool CCSTAProcesser::IsNeedSendCstaEvent(int nLineId)
{
    if (m_monitorMap.find(nLineId) != m_monitorMap.end())
    {
        return true;
    }

    //APP_INFO("m_monitorMap size[%d]",m_monitorMap.size());

    return false;
}

#if IF_FEATURE_EVENT_MODULE
bool CCSTAProcesser::SendEvent(EventDataBase * pData)
{
    if (configParser_GetConfigInt(kszCSTAControl) == 0)
    {
        return false;
    }

    ACTION_INFO("CCSTAProcesser::SendEvent[%d]", pData->m_eEventType);
    bool bRet = false;
    switch (pData->m_eEventDataType)
    {
    case EDT_BASE:
        {
            bRet |= SendBaseEvent(pData);
        }
        break;
    case EDT_TALK:
        {
            bRet |= SendTalkEvent(pData);
        }
        break;
    case EDT_FWD:
        {
            //if (pData->m_lParam == FWD_CFG_SRC_TYPE_COMMON)
            {
                bRet |= SendForwardEvent(pData);
            }
        }
        break;
    default:
        break;
    }

    return bRet;
}

bool CCSTAProcesser::SendBaseEvent(EventDataBase * pData)
{
    bool bRet = true;
    switch (pData->m_eEventType)
    {
    case EVENT_LOGIN_ON:
    case EVENT_LOGIN_OFF:
    case EVENT_REGISTER_FAILED:
        {
            SendDeviceChangeInfo();
        }
        break;
    case EVENT_OPEN_DND:
    case EVENT_CLOSE_DND:
        {
			// 3CX认证服务器推送DND状态本地修改完之后还要发送事件给服务器
            //if (pData->m_lParam == DND_CFG_SRC_TYPE_COMMON)
            {
                SendDnDEvent(pData->m_wParam, (pData->m_eEventType == EVENT_OPEN_DND));
            }
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

bool CCSTAProcesser::SendForwardEvent(EventDataBase * pData)
{
    FWDEventData * pFWDEventData = static_cast<FWDEventData *>(pData);
    if (pFWDEventData == NULL)
    {
        return false;
    }

    if (pFWDEventData->m_wParam == FWDDND_ACCOUNT_GLOBAL)
    {
        pFWDEventData->m_wParam = m_nLineId;
    }

    bool bRet = true;
    yl::string strForwardType = "";
    yl::string strEnable = "false";

    switch (pFWDEventData->m_eEventType)
    {
    case EVENT_OPEN_ALWAYS_FORWARD:
        {
            strEnable = "true";
        }
    case EVENT_CLOSE_ALWAYS_FORWARD:
        {
            strForwardType = kszforwardImmediate;
        }
        break;
    case EVENT_OPEN_BUSY_FORWARD:
        {
            strEnable = "true";
        }
    case EVENT_CLOSE_BUSY_FORWARD:
        {
            strForwardType = kszforwardBusy;
        }
        break;
    case EVENT_OPEN_NO_ANSWER_FORWARD:
        {
            strEnable = "true";
        }
    case EVENT_CLOSE_NO_ANSWER_FORWARD:
        {
            strForwardType = kszforwardNoAns;
        }
        break;
    default:
        {
            bRet = false;
        }
        break;
    }

#define kszForwardingEvent "ForwardingEvent"
#define forwardTo "forwardTo"

    if (!IsNeedSendCstaEvent(pFWDEventData->m_wParam)
            || !bRet)
    {
        return false;
    }

    APP_INFO("CSTA SendForwardEvent LindId = [%d] EventType[%d]", pFWDEventData->m_wParam,
             pFWDEventData->m_eEventType);
    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRootNode = doc.append_child(kszForwardingEvent);
    nodeRootNode.append_attribute("xmlns",
                                  "http://www.ecma-international.org/standards/ecma-323/csta/ed5");

    xml_node node = nodeRootNode.append_child(kszmonitorCrossRefID);

    MonitorMap::iterator iter = m_monitorMap.find(pFWDEventData->m_wParam);

    if (iter == m_monitorMap.end())
    {
        APP_WARN("eror not found monitorCrossRefID");
        return false;
    }

    MonitorInfo & refInfo = iter->second;

    node.text().set(refInfo.m_strCrossId.c_str());

    AddDevice(nodeRootNode, kszNodedevice, refInfo.m_strAccountInfo);

    {
        xml_node nodeType = nodeRootNode.append_child(kszforwardingType);

        nodeType.text().set(strForwardType.c_str());

        xml_node nodeStatus = nodeRootNode.append_child(kszforwardStatus);

        nodeStatus.text().set(strEnable.c_str());

        xml_node nodeforwardDN = nodeRootNode.append_child(forwardTo);

        yl::string strFowdTar = "tel:";

        strFowdTar.append(pFWDEventData->m_strFwdNum.c_str());

        nodeforwardDN.text().set(strFowdTar.c_str());

        if (pFWDEventData->m_eEventType == EVENT_OPEN_NO_ANSWER_FORWARD
                || pFWDEventData->m_eEventType == EVENT_CLOSE_NO_ANSWER_FORWARD)
        {
            xml_node nodeRingCount = nodeRootNode.append_child(kszringCount);
            nodeRingCount.text().set(pFWDEventData->m_strFwdNoAnswerTime.c_str());
        }

        doc.save_file(GetNewResFile().c_str());

        SendSipMsg(true);
    }

    return true;
}

bool CCSTAProcesser::SendTalkEvent(EventDataBase * pData)
{
    TalkEventData * pTalkEventData = static_cast<TalkEventData *>(pData);
    if (pTalkEventData == NULL)
    {
        return false;
    }

    int nCallID = -1;
    if (pTalkEventData->m_vecCallID.size() > 0)
    {
        nCallID = pTalkEventData->m_vecCallID[0];
    }

    const CCallInfo * pCallInfo = talklogic_GetCallInfoByCallId(nCallID);
    if (pCallInfo == NULL)
    {
        APP_WARN("Can not Get CallInfo by CallID[%d]", nCallID);
        return false;
    }

    int nLineId = pCallInfo->GetAccountID();

    if (!IsNeedSendCstaEvent(nLineId))
    {
        return false;
    }

    bool bRet = true;
    switch (pTalkEventData->m_eEventType)
    {
    case EVENT_RINGBACK:
        {
            SendDeliveredEvent(pCallInfo, DELIVERED_CALLOUT);
        }
        break;
    case EVENT_INCOMING_CALL:
        {
            SendDeliveredEvent(pCallInfo, DELIVERED_CALLIN);
        }
        break;
    case EVENT_ENTER_DIAL:
        {
            SendServiceInitiatedEvent(pCallInfo);
        }
        break;
    case EVENT_ESTABLISHED:
        {
            SendEstablishedEvent(pCallInfo);
        }
        break;
    case EVENT_END_DIAL:
    case EVENT_TERMINATED:
        {
            SendConnectionClearedEvent(pCallInfo);
        }
        break;
    case EVENT_CALLOUT:
        {
            SendOriginatedEvent(pCallInfo);
        }
        break;
    case EVENT_HOLD:
        {
            SendHeldEvent(pCallInfo);
        }
        break;
    case EVENT_UNHOLD:
        {
            SendRetrievedEvent(pCallInfo);
        }
        break;
    case EVENT_BLIND_TRANSFER:
        {
            SendSigleTransferedEvent(pCallInfo);
        }
        break;
    case EVENT_ATTENDED_TRANSFER:
        {
            SendSigleTransferedEvent(pCallInfo, true);
        }
        break;
    case EVENT_FORWARD_INCOMING_CALL:
        {
            SendDivertedEvent(pCallInfo);
        }
        break;
    case EVENT_CALL_FAIL:
        {
            SendCallFailedEvent(pCallInfo);
        }
        break;
    case EVENT_DTMF:
        {
            SendDtmfEvent(pCallInfo, pData->m_tParam);
        }
        break;
    case EVENT_CONFERENCE:
        {
            if (pTalkEventData->m_vecCallID.size() > 1)
            {
                const CCallInfo * pAnotherCallInfo = talklogic_GetCallInfoByCallId(pTalkEventData->m_vecCallID[1]);
                if (pCallInfo != NULL)
                {
                    SendConferenceEvent(pAnotherCallInfo, pCallInfo);
                }
            }
        }
        break;
    case EVENT_BLIND_TRANSFER_FAILED:
        {
            //盲转失败,进入Recall,发送来电事件
            SendDeliveredEvent(pCallInfo, DELIVERED_CALLIN);
        }
        break;
    case EVENT_ATTENDED_TRANSFER_FAILED:
        {
            //半咨询转失败,发送事件重新建立通话
            SendDeliveredEvent(pCallInfo, DELIVERED_CALLIN);
            SendEstablishedEvent(pCallInfo);
            SendHeldEvent(pCallInfo);
        }
        break;
    case EVENT_CALL_INFO_UPDATE:
        {
            //咨询转，半咨询转通话信息更新
            SendEstablishedEvent(pCallInfo);
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
#else
void CCSTAProcesser::SendCSTAEvent(ActionUrlType eType, const CCallInfo * pCallInfo)
{
    if (!m_bCSTAEnable)
    {
        return;
    }

    if (pCallInfo == NULL)
    {
        return;
    }

    int nLineId = pCallInfo->GetAccountID();

    if (!IsNeedSendCstaEvent(nLineId))
    {
        return;
    }

    APP_INFO("CSTA SendCSTAEvent eType %d", eType);

    if (eType == ACTION_CSTA_RINGBACK)
    {
        SendDeliveredEvent(pCallInfo, DELIVERED_CALLOUT);
    }
    else if (eType == ACTION_CSTA_INCOMING_CALL)
    {
        SendDeliveredEvent(pCallInfo, DELIVERED_CALLIN);
    }
    else if (eType == ACTION_CSTA_ENTER_PRE_CALL)
    {
        SendServiceInitiatedEvent(pCallInfo);
    }
    else if (eType == ACTION_CSTA_ESTABLISHED)
    {
        SendEstablishedEvent(pCallInfo);
    }
    else if (eType == ACTION_CSTA_TERMINATED)
    {
        SendConnectionClearedEvent(pCallInfo);
    }
    else if (eType == ACTION_CSAT_BEFORE_CALLOUT)
    {
        SendOriginatedEvent(pCallInfo);
    }
    else if (eType == ACTION_CSTA_HOLD)
    {
        SendHeldEvent(pCallInfo);
    }
    else if (eType == ACTION_CSTA_RETRIEVE)
    {
        SendRetrievedEvent(pCallInfo);
    }
    else if (eType == ACTION_CSTA_SINGLE_TRANSFER)
    {
        SendSigleTransferedEvent(pCallInfo);
    }
    else if (eType == ACTION_CSTA_ATTEND_TRANSFER)
    {
        SendSigleTransferedEvent(pCallInfo, true);
    }
    else if (eType == ACTION_CSTA_CONFERENCE)
    {
        //SendConferenceEvent(pCallInfo);
    }
    else if (eType == ACTION_CSTA_DTMF)
    {
        SendDtmfEvent(pCallInfo);
    }
    else if (eType == ACTION_CSTA_FORWARD_CALL)
    {
        SendDivertedEvent(pCallInfo);
    }
    else if (eType == ACTION_CSTA_CALL_FAIL)
    {
        SendCallFailedEvent(pCallInfo);
    }
}
#endif

#define kszalertingDevice "alertingDevice"
#define kszdeviceIdentifier "deviceIdentifier"
#define kszcallingDevice "callingDevice"
#define kszcalledDevice "calledDevice"
#define kszlastRedirectionDevice "lastRedirectionDevice"
#define ksznotRequired "notRequired"
#define ksznotKnown "notKnown"
#define kszlocalConnectionInfo "localConnectionInfo"
#define kszcause "cause"
#define kszansweringDevice "answeringDevice"
#define kszinitiatingDevice "initiatingDevice"

MonitorInfo EmptyInfo;

MonitorInfo & CCSTAProcesser::AddHeadInfoNode(xml_node & nodeRootNode, const char * pNodeName,
        const CCallInfo * pCallInfo, bool isNeedConf/* = false*/, bool isTest/* = false*/)
{
    xml_node node = nodeRootNode.append_child(kszmonitorCrossRefID);

    MonitorMap::iterator iter = m_monitorMap.find(pCallInfo->GetAccountID());

    if (iter == m_monitorMap.end())
    {
        APP_INFO("eror not found monitorCrossRefID");
        return EmptyInfo;
    }

    MonitorInfo & refInfo = iter->second;

    node.text().set(refInfo.m_strCrossId.c_str());

    node = nodeRootNode.append_child(pNodeName);

    xml_node nodeCallID = node.append_child(kszNodeCallID);


    char szBuf[256] = {0};

    if (isNeedConf && !pCallInfo->m_strConfId.empty())
    {
        nodeCallID.text().set(pCallInfo->m_strConfId.c_str());
    }
    else
    {
        sprintf(szBuf, "%d", pCallInfo->GetCallID());
        nodeCallID.text().set(szBuf);
    }

    xml_node nodeDeviceID = node.append_child(kszNodeDedeviceID);

    if (isTest)
    {
        nodeDeviceID.text().set(pCallInfo->GetRemoteSipURL().c_str());
    }
    else
    {
        nodeDeviceID.text().set(refInfo.m_strAccountInfo.c_str());
    }

    return refInfo;
}

void CCSTAProcesser::AddDevice(xml_node & nodeRootNode, const yl::string & strName,
                               const yl::string & strValue)
{
    xml_node node = nodeRootNode.append_child(strName.c_str());

    xml_node deviceIdentifier = node.append_child(kszdeviceIdentifier);

    deviceIdentifier.text().set(strValue.c_str());
}

void CCSTAProcesser::AddLocalConnectInfo(xml_node & nodeRootNode, const yl::string & strValue,
        const yl::string & strCauseValue)
{
    xml_node node = nodeRootNode.append_child(kszlocalConnectionInfo);

    node.text().set(strValue.c_str());

    node = nodeRootNode.append_child(kszcause);

    node.text().set(strCauseValue.c_str());

}


void CCSTAProcesser::SendDeliveredEvent(const CCallInfo * pCallInfo, DeliveredEventType eType)
{
#define kszDeliveredEvent "DeliveredEvent"

#define kszconnection "connection"
#define kszdivertingDevice "divertingDevice"
#define ksznewDestination "newDestination"

    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRootNode = doc.append_child(kszDeliveredEvent);
    nodeRootNode.append_attribute("xmlns",
                                  "http://www.ecma-international.org/standards/ecma-323/csta/ed5");


    MonitorInfo & refInfo = AddHeadInfoNode(nodeRootNode, kszconnection, pCallInfo);

    if (eType == DELIVERED_CALLOUT)
    {
        AddDevice(nodeRootNode, kszalertingDevice, pCallInfo->GetRemoteSipURL());
        AddDevice(nodeRootNode, kszcallingDevice, refInfo.m_strAccountInfo);
        AddDevice(nodeRootNode, kszcalledDevice, pCallInfo->GetRemoteSipURL());
    }
    else
    {
        AddDevice(nodeRootNode, kszalertingDevice, refInfo.m_strAccountInfo);
        AddDevice(nodeRootNode, kszcallingDevice, pCallInfo->GetRemoteSipURL());
        AddDevice(nodeRootNode, kszcalledDevice, refInfo.m_strAccountInfo);
    }

    xml_node node = nodeRootNode.append_child(kszlastRedirectionDevice);

    node.append_child(ksznotKnown);


    AddLocalConnectInfo(nodeRootNode, eType == DELIVERED_CALLOUT ? "connected" : "alerting", "normal");


    doc.save_file(GetNewResFile().c_str());

    SendSipMsg(true);
}

void CCSTAProcesser::SendCallFailedEvent(const CCallInfo * pCallInfo)
{
#define kszFailedEvent "FailedEvent"

#define kszfailedConnection "failedConnection"

#define kszfailingDevice "failingDevice"


    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRootNode = doc.append_child(kszFailedEvent);
    nodeRootNode.append_attribute("xmlns",
                                  "http://www.ecma-international.org/standards/ecma-323/csta/ed5");


    MonitorInfo & refInfo = AddHeadInfoNode(nodeRootNode, kszfailedConnection, pCallInfo);

    AddDevice(nodeRootNode, kszfailingDevice, refInfo.m_strAccountInfo);

    AddDevice(nodeRootNode, kszcallingDevice, pCallInfo->m_tRemoteInfo.sSIPName);

    AddDevice(nodeRootNode, kszcalledDevice, refInfo.m_strAccountInfo);

    xml_node node = nodeRootNode.append_child(kszlastRedirectionDevice);

    node.append_child(ksznotKnown);


    AddLocalConnectInfo(nodeRootNode, "null", "callCancelled");

    doc.save_file(GetNewResFile().c_str());

    SendSipMsg(true);
}

void CCSTAProcesser::SendDivertedEvent(const CCallInfo * pCallInfo)
{
#define kszDivertedEvent "DivertedEvent"

#define kszdivertingDevice "divertingDevice"
#define ksznewDestination "newDestination"

    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRootNode = doc.append_child(kszDivertedEvent);
    nodeRootNode.append_attribute("xmlns",
                                  "http://www.ecma-international.org/standards/ecma-323/csta/ed5");


    MonitorInfo & refInfo = AddHeadInfoNode(nodeRootNode, kszconnection, pCallInfo);

    AddDevice(nodeRootNode, kszdivertingDevice, refInfo.m_strAccountInfo);

    AddDevice(nodeRootNode, ksznewDestination, pCallInfo->m_strForwardTarget);

    xml_node node = nodeRootNode.append_child(kszlastRedirectionDevice);

    node.append_child(ksznotKnown);


    AddLocalConnectInfo(nodeRootNode, "null", "normal");

    doc.save_file(GetNewResFile().c_str());

    SendSipMsg(true);
}

void CCSTAProcesser::SendServiceInitiatedEvent(const CCallInfo * pCallInfo)
{
#define kszServiceInitiatedEvent "ServiceInitiatedEvent"
#define kszinitiatedConnection "initiatedConnection"

    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRootNode = doc.append_child(kszServiceInitiatedEvent);
    nodeRootNode.append_attribute("xmlns",
                                  "http://www.ecma-international.org/standards/ecma-323/csta/ed5");

    MonitorInfo & refInfo = AddHeadInfoNode(nodeRootNode, kszinitiatedConnection, pCallInfo);

    AddDevice(nodeRootNode, kszinitiatingDevice, refInfo.m_strAccountInfo);

    AddLocalConnectInfo(nodeRootNode, "initiated", "makeCall");

    doc.save_file(GetNewResFile().c_str());

    SendSipMsg(true);
}

void CCSTAProcesser::SendOriginatedEvent(const CCallInfo * pCallInfo)
{
#define kszOriginatedEvent "OriginatedEvent"
#define kszoriginatedConnection "originatedConnection"

    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRootNode = doc.append_child(kszOriginatedEvent);
    nodeRootNode.append_attribute("xmlns",
                                  "http://www.ecma-international.org/standards/ecma-323/csta/ed5");

    MonitorInfo & refInfo = AddHeadInfoNode(nodeRootNode, kszoriginatedConnection, pCallInfo);

    AddDevice(nodeRootNode, kszcallingDevice, refInfo.m_strAccountInfo);

    AddDevice(nodeRootNode, kszcalledDevice, pCallInfo->GetRemoteSipURL());

    AddLocalConnectInfo(nodeRootNode, "connected", "normal");

    doc.save_file(GetNewResFile().c_str());

    SendSipMsg(true);
}

void CCSTAProcesser::SendEstablishedEvent(const CCallInfo * pCallInfo)
{
#define kszEstablishedEvent "EstablishedEvent"
#define kszestablishedConnection "establishedConnection"

    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRootNode = doc.append_child(kszEstablishedEvent);
    nodeRootNode.append_attribute("xmlns",
                                  "http://www.ecma-international.org/standards/ecma-323/csta/ed5");

    MonitorInfo & refInfo = AddHeadInfoNode(nodeRootNode, kszestablishedConnection, pCallInfo);


    if (pCallInfo->m_eCallType == CT_INCOMING)
    {
        AddDevice(nodeRootNode, kszansweringDevice, refInfo.m_strAccountInfo);
        AddDevice(nodeRootNode, kszcallingDevice, pCallInfo->GetRemoteSipURL());
        AddDevice(nodeRootNode, kszcalledDevice, refInfo.m_strAccountInfo);
    }
    else
    {
        AddDevice(nodeRootNode, kszansweringDevice, pCallInfo->GetRemoteSipURL());
        AddDevice(nodeRootNode, kszcallingDevice, refInfo.m_strAccountInfo);
        AddDevice(nodeRootNode, kszcalledDevice, pCallInfo->GetRemoteSipURL());
    }

    xml_node node = nodeRootNode.append_child(kszlastRedirectionDevice);

    node.append_child(ksznotKnown);

    AddLocalConnectInfo(nodeRootNode, "connected", "normal");

    doc.save_file(GetNewResFile().c_str());

    SendSipMsg(true);
}

void CCSTAProcesser::SendConnectionClearedEvent(const CCallInfo * pCallInfo)
{
#define kszConnectionClearedEvent "ConnectionClearedEvent"
#define kszdroppedConnection "droppedConnection"
#define kszreleasingDevice "releasingDevice"

    //转接通话不需要再发结束
    if (!pCallInfo->m_strForwardTarget.empty() || pCallInfo->m_iTransferId != -1)
    {
        return;
    }

    bool isSingleExit = pCallInfo->m_bIsSingleExitCof;

    if (pCallInfo->m_strConfId.empty() || !pCallInfo->m_bIsSingleExitCof)
    {
        isSingleExit = false;
        xml_document doc;
        // 设置Xml文件头.
        xml_node nodeDecl = doc.prepend_child(node_declaration);
        nodeDecl.append_attribute("version", "1.0");
        nodeDecl.append_attribute("encoding", "utf-8");

        xml_node nodeRootNode = doc.append_child(kszConnectionClearedEvent);
        nodeRootNode.append_attribute("xmlns",
                                      "http://www.ecma-international.org/standards/ecma-323/csta/ed5");

        //test-syl
        MonitorInfo & refInfo = AddHeadInfoNode(nodeRootNode, kszdroppedConnection, pCallInfo, true);

        AddDevice(nodeRootNode, kszreleasingDevice, refInfo.m_strAccountInfo);

        AddLocalConnectInfo(nodeRootNode, "null", "normalClearing");

        doc.save_file(GetNewResFile().c_str());

        SendSipMsg(true);
    }


    if (!pCallInfo->m_strConfId.empty())
    {
        xml_document doc1;
        // 设置Xml文件头.
        xml_node nodeDecl = doc1.prepend_child(node_declaration);
        nodeDecl.append_attribute("version", "1.0");
        nodeDecl.append_attribute("encoding", "utf-8");

        xml_node nodeRootNode = doc1.append_child(kszConnectionClearedEvent);
        nodeRootNode.append_attribute("xmlns",
                                      "http://www.ecma-international.org/standards/ecma-323/csta/ed5");

        //test
        MonitorInfo & refInfo = AddHeadInfoNode(nodeRootNode, kszdroppedConnection, pCallInfo, true, true);

        AddDevice(nodeRootNode, kszreleasingDevice, pCallInfo->GetRemoteSipURL());

//      if (pCallInfo->GetRemoteSipURL().find("123") != yl::string::npos)
//      {
//          AddLocalConnectInfo(nodeRootNode,"connected","normalClearing");
//
//      }
//      else
//      {
        AddLocalConnectInfo(nodeRootNode, isSingleExit ? "connected" : "null", "normalClearing");
//      }



        doc1.save_file(GetNewResFile().c_str());

        SendSipMsg(true);
    }

//
//
//  xml_document doc2;
//  // 设置Xml文件头.
//  nodeDecl = doc2.prepend_child(node_declaration);
//  nodeDecl.append_attribute("version", "1.0");
//  nodeDecl.append_attribute("encoding", "utf-8");
//
//  nodeRootNode = doc2.append_child(kszConnectionClearedEvent);
//  nodeRootNode.append_attribute("xmlns","http://www.ecma-international.org/standards/ecma-323/csta/ed5");
//
//  //test
//
//  xml_node node = nodeRootNode.append_child(kszmonitorCrossRefID);
//
//
//  node.text().set(refInfo.m_strCrossId.c_str());
//
//  node = nodeRootNode.append_child(kszdroppedConnection);
//
//  xml_node nodeCallID = node.append_child(kszNodeCallID);
//
//
//  char szBuf[256] = {0};
//
//     sprintf(szBuf,"%d|%d",pCallInfo->GetCallID(),pCallInfo->m_iOtherConfId);
//
//  nodeCallID.text().set(szBuf);
//
//  xml_node nodeDeviceID = node.append_child(kszNodeDedeviceID);
//
//
//
// nodeDeviceID.text().set("sip:1452@10.2.1.199");
//
//
//  AddDevice(nodeRootNode,kszreleasingDevice,"sip:1452@10.2.1.199");
//
//  AddLocalConnectInfo(nodeRootNode,"connected","normalClearing");
//
//  doc2.save_file(GetNewResFile().c_str());
//
//  SendSipMsg(true);
}

void CCSTAProcesser::SendHeldEvent(const CCallInfo * pCallInfo)
{
#define kszHeldEvent "HeldEvent"
#define kszheldConnection "heldConnection"
#define kszholdingDevice "holdingDevice"

    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRootNode = doc.append_child(kszHeldEvent);
    nodeRootNode.append_attribute("xmlns",
                                  "http://www.ecma-international.org/standards/ecma-323/csta/ed5");

    yl::string strCallId;
    yl::string strAccountInfo;

    SESSION_STATE eStatus = talklogic_GetCurrentStatus(strCallId, strAccountInfo);

    MonitorInfo & refInfo = AddHeadInfoNode(nodeRootNode, kszheldConnection, pCallInfo, true,
                                            eStatus == SESSION_LOCALCONF);

    AddDevice(nodeRootNode, kszholdingDevice, refInfo.m_strAccountInfo);

    APP_INFO("CCSTAProcesser::SendHeldEvent pCallInfo->m_strConfId = %s",
             pCallInfo->m_strConfId.c_str());

//  if (pCallInfo->m_strConfId.empty())
//  {
    AddLocalConnectInfo(nodeRootNode, "hold", "normal");
//  }
//  else
//  {
//      AddLocalConnectInfo(nodeRootNode,"hold","consultation");
//  }

    doc.save_file(GetNewResFile().c_str());

    SendSipMsg(true);
}

void CCSTAProcesser::SendRetrievedEvent(const CCallInfo * pCallInfo)
{
#define kszRetrievedEvent "RetrievedEvent"
#define kszretrievedConnection "retrievedConnection"
#define kszretrievingDevice "retrievingDevice"

    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRootNode = doc.append_child(kszRetrievedEvent);
    nodeRootNode.append_attribute("xmlns",
                                  "http://www.ecma-international.org/standards/ecma-323/csta/ed5");

    // test -syl
    MonitorInfo & refInfo = AddHeadInfoNode(nodeRootNode, kszretrievedConnection, pCallInfo, true,
                                            pCallInfo->m_bIsInConf);

    AddDevice(nodeRootNode, kszretrievingDevice, refInfo.m_strAccountInfo);

    AddLocalConnectInfo(nodeRootNode, "connected", "normal");

    doc.save_file(GetNewResFile().c_str());

    SendSipMsg(true);
}


void CCSTAProcesser::SendSigleTransferedEvent(const CCallInfo * pCallInfo,
        bool isAttend/* = false*/)
{
#define kszTransferedEvent "TransferedEvent"
#define kszprimaryOldCall "primaryOldCall"
#define ksztransferringDevice "transferringDevice"
#define ksztransferredToDevice "transferredToDevice"
#define ksztransferredConnections "transferredConnections"
#define kszconnectionListItem "connectionListItem"

#define kszsecondaryOldCall "secondaryOldCall"

    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRootNode = doc.append_child(kszTransferedEvent);
    nodeRootNode.append_attribute("xmlns",
                                  "http://www.ecma-international.org/standards/ecma-323/csta/ed5");

    MonitorInfo & refInfo = AddHeadInfoNode(nodeRootNode, kszprimaryOldCall, pCallInfo, true);

    if (isAttend)
    {
        xml_node node = nodeRootNode.append_child(kszsecondaryOldCall);

        xml_node nodeCallID = node.append_child(kszNodeCallID);

        char szBuf[256] = {0};
        sprintf(szBuf, "%d", pCallInfo->m_iTransferId);

        nodeCallID.text().set(szBuf);

        xml_node nodeDeviceID = node.append_child(kszNodeDedeviceID);

        nodeDeviceID.text().set(refInfo.m_strAccountInfo.c_str());

    }

    AddDevice(nodeRootNode, ksztransferringDevice, refInfo.m_strAccountInfo);

    AddDevice(nodeRootNode, ksztransferredToDevice, pCallInfo->GetRemoteSipURL());

    xml_node transferredConnections = nodeRootNode.append_child(ksztransferredConnections);

    transferredConnections.append_child(kszconnectionListItem);

    AddLocalConnectInfo(nodeRootNode, "null", isAttend ? "transfer" : "singleStepTransfer");

    doc.save_file(GetNewResFile().c_str());

    SendSipMsg(true);
}

void CCSTAProcesser::SendConferenceEvent(const CCallInfo * pFirst, const CCallInfo * pSencend)
{
#if !IF_FEATURE_EVENT_MODULE
    if (!m_bCSTAEnable)
    {
        return;
    }
#endif

#define kszConferencedEvent "ConferencedEvent"
#define kszconferencingDevice "conferencingDevice"
#define kszconferenceConnections "conferenceConnections"

    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRootNode = doc.append_child(kszConferencedEvent);
    nodeRootNode.append_attribute("xmlns",
                                  "http://www.ecma-international.org/standards/ecma-323/csta/ed5");


    char szBuf[256] = {0};

    sprintf(szBuf, "%d|%d", pFirst->GetCallID(), pSencend->GetCallID());

    yl::string strConfId = szBuf;

//  if (!pSencend->m_strConfId.empty())
//  {
//      yl::string strCopy = pSencend->m_strConfId;
//
//      pSencend->m_strConfId = strConfId;
//
//      MonitorInfo& refInfo = AddHeadInfoNode(nodeRootNode,kszprimaryOldCall,pSencend,true);
//
//      pSencend->m_strConfId = strCopy;
//  }
//  else
//  {
    MonitorInfo & refInfo = AddHeadInfoNode(nodeRootNode, kszprimaryOldCall, pSencend, true);
    //}


    xml_node node = nodeRootNode.append_child(kszsecondaryOldCall);

    xml_node nodeCallID = node.append_child(kszNodeCallID);

    if (pFirst->m_strConfId.empty())
    {
        char szBuf[256] = {0};

        sprintf(szBuf, "%d", pFirst->GetCallID());

        nodeCallID.text().set(szBuf);
    }
    else
    {
        nodeCallID.text().set(pFirst->m_strConfId.c_str());
    }


    xml_node nodeDeviceID = node.append_child(kszNodeDedeviceID);

    nodeDeviceID.text().set(refInfo.m_strAccountInfo.c_str());

    AddDevice(nodeRootNode, kszconferencingDevice, refInfo.m_strAccountInfo);

    xml_node nodeConnections = nodeRootNode.append_child(kszconferenceConnections);

    if (!nodeConnections.empty())
    {


        AddConnectionListItem(nodeConnections, strConfId, pSencend, pSencend->GetRemoteSipURL());

        AddConnectionListItem(nodeConnections, strConfId, pFirst, pFirst->GetRemoteSipURL());

        AddConnectionListItem(nodeConnections, strConfId, pSencend, refInfo.m_strAccountInfo);
    }


    AddLocalConnectInfo(nodeRootNode, "connected", "normal");

    doc.save_file(GetNewResFile().c_str());

    SendSipMsg(true);
}

// void CCSTAProcesser::SendConferenceEvent(const CCallInfo* pCallInfo)
// {
// #define kszConferencedEvent "ConferencedEvent"
// #define kszconferencingDevice "conferencingDevice"
// #define kszconferenceConnections "conferenceConnections"
//
//  xml_document doc;
//  // 设置Xml文件头.
//  xml_node nodeDecl = doc.prepend_child(node_declaration);
//  nodeDecl.append_attribute("version", "1.0");
//  nodeDecl.append_attribute("encoding", "utf-8");
//
//  xml_node nodeRootNode = doc.append_child(kszConferencedEvent);
//  nodeRootNode.append_attribute("xmlns","http://www.ecma-international.org/standards/ecma-323/csta/ed5");
//
//  CCallInfo* pFirst = NULL;
//  CCallInfo* pSencend = NULL;
//
//  if (!talklogic_GetLocalConfCallInfo(pCallInfo->GetCallID(),&pFirst,&pSencend))
//  {
//      SYZ_TRACE_LINE();
//      return;
//  }
//
//  MonitorInfo& refInfo = AddHeadInfoNode(nodeRootNode,kszprimaryOldCall,pSencend);
//
//  xml_node node = nodeRootNode.append_child(kszsecondaryOldCall);
//
//  xml_node nodeCallID = node.append_child(kszNodeCallID);
//
//  char szBuf[256] = {0};
//
//  SYZ_TRACE("-----callId = %d",pFirst->GetCallID());
//
//  sprintf(szBuf,"%d",pFirst->GetCallID());
//
//  SYZ_TRACE_LINE();
//
//  nodeCallID.text().set(szBuf);
//
//  xml_node nodeDeviceID = node.append_child(kszNodeDedeviceID);
//
//  nodeDeviceID.text().set(refInfo.m_strAccountInfo.c_str());
//
//  AddDevice(nodeRootNode,kszconferencingDevice,refInfo.m_strAccountInfo);
//
//  xml_node nodeConnections = nodeRootNode.append_child(kszconferenceConnections);
//
//  if (!nodeConnections.empty())
//  {
//      yl::string strConfId = pFirst->m_strConfId;
//
//      AddConnectionListItem(nodeConnections,strConfId,pSencend->GetCallID(),pSencend->GetRemoteSipURL());
//
//      AddConnectionListItem(nodeConnections,strConfId,pFirst->GetCallID(),pFirst->GetRemoteSipURL());
//
//      AddConnectionListItem(nodeConnections,strConfId,pSencend->GetCallID(),refInfo.m_strAccountInfo);
//  }
//
//
//  AddLocalConnectInfo(nodeRootNode,"connected","normal");
//
//  doc.save_file(GetNewResFile().c_str());
//
//  SendSipMsg(true);
// }

void CCSTAProcesser::SendDtmfEvent(const CCallInfo * pCallInfo, int iKey)
{
#define kszDigitsGeneratedEvent  "DigitsGeneratedEvent"
#define kszdigitGeneratedList "digitGeneratedList"

    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRootNode = doc.append_child(kszDigitsGeneratedEvent);
    nodeRootNode.append_attribute("xmlns",
                                  "http://www.ecma-international.org/standards/ecma-323/csta/ed5");

    AddHeadInfoNode(nodeRootNode, kszconnection, pCallInfo, true);

    xml_node nodeKey = nodeRootNode.append_child(kszdigitGeneratedList);

    char szBuf[256] = {0};
    sprintf(szBuf, "%d", iKey);

    nodeKey.text().set(szBuf);

    doc.save_file(GetNewResFile().c_str());

    SendSipMsg(true);
}

yl::string GetFwdStringByType(int nFwdType)
{
    switch (nFwdType)
    {
    case FWD_TYPE_ALWAYS:
        return kszforwardImmediate;
        break;
    case FWD_TYPE_BUSY:
        return kszforwardBusy;
        break;
    case FWD_TYPE_NO_ANSWER:
        return kszforwardNoAns;
        break;
    default:
        break;
    }

    return "";
}

#if !IF_FEATURE_EVENT_MODULE
void CCSTAProcesser::SendForwardEvent(int nLine, FwdMessageStatusData * pFwdData)
{
    if (!m_bCSTAEnable)
    {
        return;
    }
#define kszForwardingEvent "ForwardingEvent"
#define forwardTo "forwardTo"

    if (!IsNeedSendCstaEvent(nLine))
    {
        return;
    }

    APP_INFO("CSTA SendForwardEvent LineId %d,nFwdType %d", nLine, nFwdType);

    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRootNode = doc.append_child(kszForwardingEvent);
    nodeRootNode.append_attribute("xmlns",
                                  "http://www.ecma-international.org/standards/ecma-323/csta/ed5");


    xml_node node = nodeRootNode.append_child(kszmonitorCrossRefID);

    MonitorMap::iterator iter = m_monitorMap.find(nLine);

    if (iter == m_monitorMap.end())
    {
        APP_INFO("eror not found monitorCrossRefID");
        return;
    }

    MonitorInfo & refInfo = iter->second;

    node.text().set(refInfo.m_strCrossId.c_str());

    AddDevice(nodeRootNode, kszNodedevice, refInfo.m_strAccountInfo);

    if (pFwdData)
    {
        xml_node nodeType = nodeRootNode.append_child(kszforwardingType);

        nodeType.text().set(GetFwdStringByType(pFwdData->eType).c_str());

        xml_node nodeStatus = nodeRootNode.append_child(kszforwardStatus);

        nodeStatus.text().set(pFwdData->bEnabled ? "true" : "false");

        xml_node nodeforwardDN = nodeRootNode.append_child(forwardTo);

        yl::string strFowdTar = "tel:";

        strFowdTar.append(pFwdData->szTarget);

        nodeforwardDN.text().set(strFowdTar.c_str());

        if (pFwdData->eType == FWD_TYPE_NO_ANSWER)
        {
            xml_node nodeRingCount = nodeRootNode.append_child(kszringCount);

            char szBuf[256] = {0};
            sprintf(szBuf, "%d", pFwdData->iRingDelay);

            nodeRingCount.text().set(szBuf);
        }

        doc.save_file(GetNewResFile().c_str());

        SendSipMsg(true);
    }
}
#endif

void CCSTAProcesser::SendDnDEvent(int nLine, bool bEnable)
{
#if !IF_FEATURE_EVENT_MODULE
    if (!m_bCSTAEnable)
    {
        return;
    }
#endif
#define kszDoNotDisturbEvent "DoNotDisturbEvent"
#define kszdoNotDisturbOn "doNotDisturbOn"

    if (nLine == FWDDND_ACCOUNT_GLOBAL)
    {
        nLine = m_nLineId;
    }

    if (!IsNeedSendCstaEvent(nLine))
    {
        return;
    }

    APP_INFO("CSTA SendDnDEvent LindId = [%d] bEnable[%d]", nLine, bEnable);

    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRootNode = doc.append_child(kszDoNotDisturbEvent);
    nodeRootNode.append_attribute("xmlns",
                                  "http://www.ecma-international.org/standards/ecma-323/csta/ed5");


    xml_node node = nodeRootNode.append_child(kszmonitorCrossRefID);

    MonitorMap::iterator iter = m_monitorMap.find(nLine);

    if (iter == m_monitorMap.end())
    {
        return;
    }

    MonitorInfo & refInfo = iter->second;

    node.text().set(refInfo.m_strCrossId.c_str());

    AddDevice(nodeRootNode, kszNodedevice, refInfo.m_strAccountInfo);

    node = nodeRootNode.append_child(kszdoNotDisturbOn);

    node.text().set(bEnable ? "true" : "false");

    doc.save_file(GetNewResFile().c_str());

    SendSipMsg(true);
}


yl::string CCSTAProcesser::GetCallId(xml_node & node, const yl::string & strChildName)
{
    xml_node childNode = node.child(strChildName.c_str());

    if (childNode.empty())
    {
        return "";
    }

    xml_node nodeCallId = childNode.child(kszNodeCallID);

    if (!nodeCallId.empty())
    {
        yl::string strCallId = nodeCallId.child_value();
        bool bret = GetCallId(strCallId);
        if (bret)
        {
            return strCallId;
        }
        else
        {
            return "";
        }
    }

    return "";
}

yl::string CCSTAProcesser::GetAccountInfo(xml_node & node, const yl::string & strChildName)
{
    yl::string strAccountInfo;
    xml_node childNode = node.child(strChildName.c_str());

    if (childNode.empty())
    {
        return "";
    }

    xml_node nodeCallId = childNode.child(kszNodeDedeviceID);

    if (!nodeCallId.empty())
    {
        strAccountInfo = nodeCallId.child_value();

        int index = strAccountInfo.find(";tel=");

        if (index != yl::string::npos)
        {
            strAccountInfo = strAccountInfo.substr(0, index);
        }
        return strAccountInfo;
    }

    return "";
}

bool CCSTAProcesser::GetCallId(yl::string & strCallId)
{
    YLList<yl::string> listCallId;

    commonAPI_splitStringBySeparator(strCallId, '|', listCallId);

    yl::string strAccountInfo;

    bool bIsInconf = talklogic_GetCurrentStatus(strCallId, strAccountInfo) == SESSION_LOCALCONF;

    for (YLList<yl::string>::iterator iter = listCallId.begin(); iter != listCallId.end();
            iter++)
    {
        yl::string & refId = *iter;

        if (bIsInconf)
        {
            strCallId = refId;
            return true;
        }

        if (talklogic_GetCallInfoByCallId(refId) != NULL)
        {
            APP_INFO("CCSTAProcesser::GetCallId callId %s", refId.c_str());
            strCallId = refId;
            return true;
        }
    }

    return false;
}

void CCSTAProcesser::SendErrorResponse()
{
    SetResponseXmlDoc(kszCSTAErrorCode);
}
