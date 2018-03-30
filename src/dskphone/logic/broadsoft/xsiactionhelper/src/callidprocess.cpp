#include <ETLLib.hpp>
#include "dsklog/log.h"

#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "etlmsghandler/modetlmsghandler.h"
#include <commonapi/stringhelperapi.h>
#include <commonapi/systemhelperapi.h>
#include "xmlparser/xmlparser.hpp"
#include "service_sip_interface.h"

#include "callidprocess.h"

#include "talk/talklogic/include/talklogic_common.h"
#include "talk/talklogic/include/talkmsgdefine.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "broadsoft/executiveassistant/include/modexecutiveassistant.h"

CCallIdProcesser& CCallIdProcesser::GetInstance()
{
    static CCallIdProcesser s_Inst;

    return s_Inst;
}

CCallIdProcesser::CCallIdProcesser()
{
    XsiAction::InsertMap(XSI_CALLID_GET, "calls");
    XsiAction::InsertMap(XSI_CALLID_DETAILS_GET, "calls");
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    m_ConfId = -1;
#endif
}

CCallIdProcesser::~CCallIdProcesser()
{
}

yl::string CCallIdProcesser::GetXsiCallId(int nLocalCallId)
{
    MapLocalIDXsiID::iterator itr = m_mapLocalIDXsiID.begin();
    for (; itr != m_mapLocalIDXsiID.end(); ++itr)
    {
        if (talklogic_GetCallIdBySessionID(itr->first) == nLocalCallId)
        {
            return itr->second;
        }
    }

    return "";
}

bool CCallIdProcesser::ProcessByType(XsiAction* pAction)
{
    if (pAction == NULL)
    {
        return false;
    }

    xml_document objXmlDoc;
    xml_parse_result objResult = objXmlDoc.load_file(pAction->GetFilePath().c_str());

    if (objResult.status != status_ok)
    {
        XSI_INFO("CAllCallsSummaryActionCmd::OnXSIActionResponse parse error[%d]", objResult.status);
        return false;
    }

    int iLocalSessionId = pAction->GetExtraParam();

    switch (pAction->GetXsiType())
    {
    case XSI_CALLID_GET:
        {
            if (PraseCallId(objXmlDoc.child("Calls"), iLocalSessionId))
            {
                // 继续获取CallDetails
                ProcessGetDetails(iLocalSessionId);
                return true;
            }
        }
        break;
    case XSI_CALLID_DETAILS_GET:
        {
            // 解析Details数据
            return PraseCallIdDetails(objXmlDoc.child("Call"), iLocalSessionId);
        }
        break;
    default:
        break;
    }

    return false;
}

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
bool CCallIdProcesser::IsNetworkConfCallId(int nCallId)
{
    return talklogic_GetCallIdBySessionID(m_ConfId) == nCallId;
}
#endif

bool CCallIdProcesser::PraseCallId(const xml_node& nodeRoot, int iLocalSessionId)
{
    if (nodeRoot.empty())
    {
        return false;
    }

    // 遍历获取所有的CallID
    xml_node nodeCall = nodeRoot.child("call");
    while (!nodeCall.empty())
    {
        // 获取CallID
        yl::string strCallId = nodeCall.child("callId").text().get();
        XSI_INFO("CallId[%s]", strCallId.c_str());
        if (!strCallId.empty())
        {
            m_listCallsID.push_back(strCallId);
        }

        nodeCall = nodeCall.next_sibling("call");
    }

    int iLocalSize = m_mapLocalIDXsiID.size();
    int iRemoteSize = m_listCallsID.size();

    int iIndex = -1;
    bool bRet = false;

    // warring：当前方案依赖严格的时序(多于两路的情况)，时序混乱可能出错
    XSI_WARN("XsiCallsInfo local size=[%d]  remote size=[%d]", iLocalSize, iRemoteSize);
    if (iRemoteSize - iLocalSize >= 2)
    {
        // remote的CallID比local多出一条以上，很可能是出错了。此时取多出来的第一路
        iIndex = iLocalSize;
    }
    else
    {
        // 取最后一路
        iIndex = iRemoteSize - 1;
    }

    if (iIndex >= 0 && iIndex < iRemoteSize)
    {
        yl::string& strCallId = m_listCallsID[iIndex];
        m_mapLocalIDXsiID[iLocalSessionId] = strCallId;

        XSI_INFO("XsiCallsInfo, local[%d], xsi[%s], iSize[%d]", iLocalSessionId, strCallId.c_str(),
                 m_mapLocalIDXsiID.size());

        bRet = true;
    }

    m_listCallsID.clear();

    return bRet;
}

bool CCallIdProcesser::PraseCallIdDetails(const xml_node& nodeRoot, int iLocalSessionId)
{
#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    if (nodeRoot.empty())
    {
        return false;
    }

    if (m_mapLocalIDXsiID.find(iLocalSessionId) == m_mapLocalIDXsiID.end())
    {
        return false;
    }

    CallIDDetails callIdDetails;

    xml_node nodeCall = nodeRoot.child("callId");
    if (!nodeCall.empty())
    {
        callIdDetails.strRemoteCallID = nodeCall.text().as_string();
    }
    nodeCall = nodeRoot.child("appearance");
    if (!nodeCall.empty())
    {
        callIdDetails.iApearanceId = nodeCall.text().as_int();
    }
    nodeCall = nodeRoot.child("executiveUserId");
    if (!nodeCall.empty())
    {
        callIdDetails.strExecutive = nodeCall.text().as_string();
        callIdDetails.bExecutiveCall = !callIdDetails.strExecutive.empty();
    }
    nodeCall = nodeRoot.child("recallType");
    if (!nodeCall.empty())
    {
        const char* pStr = nodeCall.text().as_string();
        callIdDetails.bExecutiveRecall = !textEMPTY(pStr);
    }

    XSI_INFO("PraseCallIdDetails callid=[%s] number=[%s] index=[%d] exe=[%d] recall=[%d]",
             callIdDetails.strRemoteCallID.c_str(), callIdDetails.strExecutive.c_str(),
             callIdDetails.iApearanceId,
             callIdDetails.bExecutiveCall, callIdDetails.bExecutiveRecall);

    if (!callIdDetails.strRemoteCallID.empty())
    {
        m_mapLocalIDDetails[iLocalSessionId] = callIdDetails;

        if (talklogic_GetFocusedSessionID() == iLocalSessionId
                && talkLogic_IsSessionInTalking())
        {
            // 获取到信息之后立刻进行刷屏，显示Softkey
            talklogic_UpdateWnd();
        }

        return true;
    }
#endif

    return false;
}

bool CCallIdProcesser::IsExecutiveCall(int iLocalSessionId)
{
#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    MapCallIdDetails::iterator it = m_mapLocalIDDetails.find(iLocalSessionId);
    if (it != m_mapLocalIDDetails.end())
    {
        CallIDDetails& callIdDetails = it->second;
        return callIdDetails.bExecutiveCall;
    }
#endif

    return false;
}

bool CCallIdProcesser::ProcessGetDetails(int iLocalSessionId)
{
#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    int iLineId = talklogic_GetSessionAccount(iLocalSessionId);
    if (SetLineID(iLineId))
    {
        if (BWExeAssis_GetExecutiveListCount(iLineId) > 0)
        {
            yl::string strCallId = GetXsiCallId(talklogic_GetCallIdBySessionID(iLocalSessionId));
            if (!strCallId.empty())
            {
                XsiAction* pAction = ExeXsiAction(iLineId, XSI_CALLID_DETAILS_GET, yl::string("/") + strCallId);
                if (pAction != NULL)
                {
                    pAction->SetExtraParam(iLocalSessionId);
                    return true;
                }
            }
        }
    }
#endif

    return false;
}

bool CCallIdProcesser::IsExecutiveCallPushRecall(int iLocalSessionId)
{
#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    MapCallIdDetails::iterator it = m_mapLocalIDDetails.find(iLocalSessionId);
    if (it != m_mapLocalIDDetails.end())
    {
        CallIDDetails& callIdDetails = it->second;
        return callIdDetails.bExecutiveRecall;
    }
#endif

    return false;
}

bool CCallIdProcesser::IsGetCallID(int iLineId)
{
    // callpark开启且模式为xsi、GroupCallPark开启且模式为xsi、老板秘书功能开启，才请求CALLID
    return (
               (configParser_GetConfigInt(kszCallParkSwitch) != 0
                && configParser_GetConfigInt(kszCallParkMode) == 0)
               || (configParser_GetConfigInt(kszGroupCallParkSwitch) != 0
                   && configParser_GetConfigInt(kszCallParkMode) == 0)
               || (configParser_GetCfgItemIntValue(kszAccountCallRecordSwitch, iLineId) != 0
                   && configParser_GetConfigInt(kszCallRecordMode) == 0)
#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
               || (BWExeAssis_IsExecutive(iLineId) || BWExeAssis_IsAssistant(iLineId))
#endif
           );
}

void CCallIdProcesser::OnDealGetCallId(int iSessionId, int iAccountId, bool isEnterTalk)
{
    if (configParser_GetConfigInt(kszBroadsoftActive) == 0)
    {
        return;
    }
    if (!BWCallControl_IsAccountXSIEnable(iAccountId))
    {
        return;
    }
    // http://10.2.1.199/Bug.php?BugID=123572
    // 目前获取callid 在ring和ringback时候
    // 通话建立后没有获取到callid 需要再去获取一下，park等操作不会进入ring 或 ringback
    if (isEnterTalk && m_mapLocalIDXsiID.find(iSessionId) != m_mapLocalIDXsiID.end())
    {
        return;
    }
    // 通话建立, 请求XSICallsId
    if (!SetLineID(iAccountId))
    {
        return;
    }
    // 判断是否需要获取callid
    if (IsGetCallID(iAccountId))
    {
        XsiAction* pAction = ExeXsiAction(GetLineID(), XSI_CALLID_GET);
        if (NULL != pAction)
        {
            pAction->SetExtraParam(iSessionId);
        }
    }
}

void CCallIdProcesser::OnDealSessionDestory(int iSessionId)
{
    // 通话结束, 清除数据
    MapLocalIDXsiID::iterator itr = m_mapLocalIDXsiID.find(iSessionId);
    if (itr != m_mapLocalIDXsiID.end())
    {
        m_mapLocalIDXsiID.erase(itr);
    }

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    if (m_ConfId == iSessionId)
    {
        XSI_INFO("Clean ConfId !");
        m_listConfCallID.clear();
        m_ConfId = -1;
    }
#endif

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    MapCallIdDetails::iterator it = m_mapLocalIDDetails.find(iSessionId);
    if (it != m_mapLocalIDDetails.end())
    {
        m_mapLocalIDDetails.erase(it);
    }
#endif
}
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
void CCallIdProcesser::OnDealNetWorkConfEnter(int iSessionId)
{
    m_ConfId = iSessionId;
    MapLocalIDXsiID::iterator itr = m_mapLocalIDXsiID.begin();
    for (; itr != m_mapLocalIDXsiID.end(); ++itr)
    {
        XSI_INFO("Network Conference Id [%s]", itr->second.c_str());
        m_listConfCallID.push_back(itr->second);
    }
}

void CCallIdProcesser::OnDealNetWorkConfAddUser(int iSessionId, int iAddSessionId)
{
    if (m_ConfId == iSessionId)
    {
        MapLocalIDXsiID::iterator itr = m_mapLocalIDXsiID.find(iAddSessionId);
        if (itr != m_mapLocalIDXsiID.end())
        {
            m_listConfCallID.push_back(itr->second);
        }
    }
}
#endif
