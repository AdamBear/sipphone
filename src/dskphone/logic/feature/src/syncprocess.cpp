#include "feature_inc.h"

#ifdef _DECT
#include "handsetinfo/include/modhandsetinfo.h"
#include "notifyqueue/include/modnotifyqueue.h"
#endif

// 同步xml内容中标志的定义
#define SYNC_TRUE   "true"
#define SYNC_FALSE  "false"

// 对Sync的处理基类
CSyncBaseProcess::CSyncBaseProcess()
{
}

CSyncBaseProcess::~CSyncBaseProcess()
{
}

LPCSTR CSyncBaseProcess::GetElementContent(xml_node nodeElementNode, LPCSTR lpszChildName,
        yl::string & strContent)
{
    if (nodeElementNode.empty())
    {
        return NULL;
    }
    xml_node xmlContent = nodeElementNode.child(lpszChildName);
    if (!xmlContent.empty())
    {
        strContent = xmlContent.text().get();
    }

    return strContent.c_str();
}
yl::string CSyncBaseProcess::GetXMLExtraData(int iLineID, VOID * pTypeData)
{
    xml_document XMLDoc;
    FillXMLExtraData(XMLDoc, iLineID, pTypeData);

    yl::string strRetVal = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>";

    // 200k buffer
    char * pBuffer = new char[200 * 1024];
    memset(pBuffer, 0, 200 * 1024);
    xml_writer_buffer writer(pBuffer, 200 * 1024);

    XMLDoc.print(writer, "");
    strRetVal += pBuffer;

    string_replace(strRetVal, "\n", "");

    if (pBuffer != NULL)
    {
        delete[] pBuffer;
        pBuffer = NULL;
    }
    // 去掉空格？？

    return strRetVal;
}


// 对Sync DND的处理类
CSyncDNDProcess::CSyncDNDProcess()
{
}

CSyncDNDProcess::~CSyncDNDProcess()
{

}

VOID CSyncDNDProcess::ProcessXMLInfo(xml_node nodeElementNode, int iAccount)
{
    if (nodeElementNode.empty())
    {
        return;
    }

    // 如果不允许开启DND，则不去同步
    if (!dnd_IsActive()
#if IF_FEATURE_DND_SYNC
            || !feature_IsDNDSync(iAccount)
#endif
       )
    {
        COMMONUNITS_WARN("ProcessXMLInfo ForbiddenDNDOn");
        return;
    }

    yl::string strStatus = "";
    GetElementContent(nodeElementNode, "doNotDisturbOn", strStatus);
    COMMONUNITS_INFO("ProcessXMLInfo strStatus:%s", strStatus.c_str());
    BOOL bStatus = FALSE;
    if (strStatus == SYNC_TRUE)
    {
        bStatus = TRUE;
    }
    else if (strStatus == SYNC_FALSE)
    {
        bStatus = FALSE;
    }
    else
    {
        return;
    }

    // 同步到本地配置

    if (dnd_SetEnable(iAccount, bStatus, DND_CFG_SRC_TYPE_SYNC))
    {
#ifdef _DECT
        NQ_NotifyListChange(LIST_LINE_SETTING, handsetinfo_GetRegHandestsMask());
#endif
    }
}

VOID CSyncDNDProcess::FillXMLExtraData(xml_document & docXmlFile, int iLineID, VOID * pTypeData)
{
    if (!docXmlFile)
    {
        return;
    }

    xml_node nodeDataNode = docXmlFile.prepend_child("SetDoNotDisturb");

    if (!nodeDataNode.empty())
    {
        xml_attribute attr = nodeDataNode.append_attribute("xmlns");
        if (!attr.empty())
        {
            attr.set_value("http://www.ecma-international.org/standards/ecma-323/csta/ed3");
        }
        xml_node ndoeTmpNode = nodeDataNode.prepend_child("doNotDisturbOn");
        if (!ndoeTmpNode.empty())
        {
            if (dnd_IsEnable(iLineID))
            {
                ndoeTmpNode.text().set(SYNC_TRUE);
            }
            else
            {
                ndoeTmpNode.text().set(SYNC_FALSE);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// 对Sync FWD的处理类
CSyncFWDProcess::CSyncFWDProcess()
{

}

CSyncFWDProcess::~CSyncFWDProcess()
{

}

VOID CSyncFWDProcess::ProcessXMLInfo(xml_node nodeElementNode, int iAccount)
{
    if (nodeElementNode.empty())
    {
        return;
    }

    // 获取FWD状态
    BOOL bFWDStatus = FALSE;
    yl::string strStatus = "";
    GetElementContent(nodeElementNode, "forwardStatus", strStatus);
    if (strStatus == SYNC_TRUE)
    {
        bFWDStatus = TRUE;
    }
    else if (strStatus == SYNC_FALSE)
    {
        bFWDStatus = FALSE;
    }
    else
    {
        return;
    }

    // 获取FWD类型
    FWD_TYPE eFWDType = FWD_TYPE_MANUAL;
    yl::string strType = "";
    GetElementContent(nodeElementNode, "forwardingType", strType);
    if (strType == "forwardImmediate")
    {
        eFWDType = FWD_TYPE_ALWAYS;
    }
    else if (strType == "forwardBusy")
    {
        eFWDType = FWD_TYPE_BUSY;
    }
    else if (strType == "forwardNoAns")
    {
        eFWDType = FWD_TYPE_NO_ANSWER;
    }
    else
    {
        return;
    }

    // 获取FWD号码
    yl::string strNumber = "";
    GetElementContent(nodeElementNode, "forwardTo", strNumber);
    // http://192.168.1.99/Bug.php?BugID=23726 bFWDStatus为true且号码为空时才返回
    if (bFWDStatus && strNumber.empty())
    {
        return;
    }

    // 获取NoAns的时间
    int iTimes = 0;
    if (eFWDType == FWD_TYPE_NO_ANSWER)
    {
        yl::string strTime = "";
        GetElementContent(nodeElementNode, "ringCount", strTime);

        if (!strTime.empty())
        {
            iTimes = atoi(strTime.c_str());
        }

        //对于非0到20整数置2（默认）
        if (iTimes < 0 || iTimes > 20)
        {
            iTimes = 2;
        }
    }

    // 调用接口设置
    if (fwd_SetInfo(iAccount, eFWDType, bFWDStatus, &strNumber, &iTimes, FWD_CFG_SRC_TYPE_SYNC))
    {
#ifdef _DECT
        NQ_NotifyListChange(LIST_LINE_SETTING, handsetinfo_GetRegHandestsMask());
#endif
    }
}

VOID CSyncFWDProcess::FillXMLExtraData(xml_document & docXmlFile, int iLineID, VOID * pTypeData)
{
    if (!docXmlFile)
    {
        return;
    }

    if (pTypeData == NULL)
    {
        COMMONUNITS_WARN("FwdSync: Invalid Forward Sync Data !");
        return;
    }

    // 类型
    FWD_TYPE eFWDType = *(FWD_TYPE *)pTypeData;
    yl::string strTarget;
    int iRingDelay = 0;
    bool bEnable = fwd_GetInfo(iLineID, eFWDType, &strTarget, &iRingDelay);

    xml_node nodeDataNode = docXmlFile.prepend_child("SetForwarding");

    if (!nodeDataNode.empty())
    {
        xml_attribute attrTmp = nodeDataNode.append_attribute("xmlns");
        if (!attrTmp.empty())
        {
            attrTmp.set_value("http://www.ecma-international.org/standards/ecma-323/csta/ed3");
        }

        xml_node nodeTmpNode;
        // 设置延迟时间
        if (eFWDType == FWD_TYPE_NO_ANSWER)
        {
            nodeTmpNode = nodeDataNode.prepend_child("ringCount");
            if (!nodeTmpNode.empty())
            {
                yl::string strTime = commonAPI_FormatString("%d", iRingDelay);
                nodeTmpNode.text().set(strTime.c_str());
            }
        }

        // 设置目标
        nodeTmpNode = nodeDataNode.prepend_child("forwardDN");
        if (!nodeTmpNode.empty())
        {
            nodeTmpNode.text().set(strTarget.c_str());
        }

        // 设置状态
        nodeTmpNode = nodeDataNode.prepend_child("activateForward");
        if (!nodeTmpNode.empty())
        {
            LPCSTR lpszStatus = bEnable ? SYNC_TRUE : SYNC_FALSE;

            if (lpszStatus != NULL)
            {
                nodeTmpNode.text().set(lpszStatus);
            }
        }

        // 设置类型
        nodeTmpNode = nodeDataNode.prepend_child("forwardingType");
        if (!nodeTmpNode.empty())
        {
            yl::string strType = GetFWDTypeString(eFWDType, iLineID);

            if (!strType.empty())
            {
                nodeTmpNode.text().set((char *)strType.c_str());
            }
        }
    }
}

yl::string CSyncFWDProcess::GetFWDTypeString(int iFWDType, int iLineID)
{
    switch (iFWDType)
    {
    case FWD_TYPE_ALWAYS:
        return "forwardImmediate";
        break;
    case FWD_TYPE_BUSY:
        return "forwardBusy";
        break;
    case FWD_TYPE_NO_ANSWER:
        return "forwardNoAns";
        break;
    default:
        break;
    }

    return "";
}

//////////////////////////////////////////////////////////////////////////
// 对Sync ACD的处理类
CSyncACDProcess::CSyncACDProcess()
{

}

CSyncACDProcess::~CSyncACDProcess()
{

}

VOID CSyncACDProcess::ProcessXMLInfo(xml_node nodeElementNode, int iAccount)
{
    if (nodeElementNode.empty())
    {
        return;
    }
    LPCSTR lpszNodeName = nodeElementNode.name();

    if (lpszNodeName == NULL)
    {
        return;
    }
//添加Break code notification功能（ACD），登录默认状态为Available
//http://192.168.1.99/Bug.php?BugID=16316
    ACD_STATUS eType = AS_IDLE;
    // Broadsoft服务器下ACD功能Unavailable的reason code
    yl::string strReasonCode = "";
    if (strcmp(lpszNodeName, ACD_LOGON_NAME) == 0)
    {
        //收到服务器下发signIn状态，处理登陆后，如请求初始状态等
        ACD_ProcessAfterLogin();
        return;
    }
    else if (strcmp(lpszNodeName, ACD_READY_NAME) == 0)
    {
        eType = AS_AVAILABLE;
    }
    else if (strcmp(lpszNodeName, ACD_NOTREADY_NAME) == 0)
    {
        eType = AS_UNAVAILABLE;
        // 获取Broadsoft服务器下ACD功能Unavailable的reason code
        nodeElementNode = nodeElementNode.child("extensions");
        if (!nodeElementNode.empty())
        {
            nodeElementNode = nodeElementNode.child("privateData");
            if (!nodeElementNode.empty())
            {

                nodeElementNode = nodeElementNode.child("private");
                if (!nodeElementNode.empty())
                {
                    nodeElementNode = nodeElementNode.child("pri:AgentNotReadyReasonValue");
                    if (!nodeElementNode.empty())
                    {
                        strReasonCode = nodeElementNode.text().get();
                    }
                }
            }
        }
    }
    else if (strcmp(lpszNodeName, ACD_LOGOFF_NAME) == 0)
    {
        eType = AS_IDLE;
    }
#if NABLECOMM_BUGFREE_16773
    else if (strcmp(lpszNodeName, ACD_WRAPUP_NAME) == 0)
    {
        eType = AS_WRAPUP;
    }
#endif
    else
    {
        return;
    }

    //发送消息到CallCenter 模块处理
    msgPostMsgToThreadEx(msgGetThreadByName(VP_PHONE_THREAD_NAME), DSK_MSG_ACD_RESULT, iAccount, eType,
                         strReasonCode.size() + 1, strReasonCode.c_str());
}

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
//////////////////////////////////////////////////////////////////////////
// 对Executive Assistant的处理类
VOID CSyncExecutiveAssistantProcess::ProcessXMLInfo(xml_node nodeElementNode, int iAccount)
{
    if (nodeElementNode.empty())
    {
        return;
    }

    LPCSTR lpszNodeName = nodeElementNode.name();

    if (lpszNodeName == NULL)
    {
        return;
    }

    if (strcmp(lpszNodeName, EXECUTIVE_NODE_NAME) == 0)
    {
        CExeFilterInfo dataInfo;
        GetXmlChildText(nodeElementNode, "xBw:callFilteringOn", dataInfo.bSwitchOn);
        BWExeAssis_SetUserType(EET_Executive, iAccount, &dataInfo);
    }
    else if (strcmp(lpszNodeName, ASSISTANT_NODE_NAME) == 0)
    {
        CAssCommon dataInfo;
        GetXmlChildText(nodeElementNode, "xBw:divertStatus", dataInfo.bDivertOn);
        GetXmlChildText(nodeElementNode, "xBw:divertAddr", dataInfo.strDivertNumber);

        xml_node nodeList = nodeElementNode.child("xBw:execDataList");
        if (!nodeList.empty())
        {
            xml_node childNode = nodeList.child("xBw:execData");
            while (!childNode.empty())
            {
                CUserItem AssistantItem;
                GetXmlChildText(childNode, "xBw:execUserId", AssistantItem.ItemName.strUserId);
                GetXmlChildText(childNode, "xBw:callFilteringOn", AssistantItem.bFilterOn);

                xml_node childNodeEx = childNode.child("xBw:execIds");
                if (!childNodeEx.empty())
                {
                    GetXmlChildText(childNodeEx, "xBw:execName", AssistantItem.ItemName.strUserName);
                    GetXmlChildText(childNodeEx, "xBw:execDN", AssistantItem.ItemName.strDN);
                    GetXmlChildText(childNodeEx, "xBw:execExtn", AssistantItem.ItemName.strExt);
                }

                if (AssistantItem)
                {
                    dataInfo.listData.push_back(AssistantItem);
                }

                childNode = childNode.next_sibling("xBw:execData");
            }
        }

        BWExeAssis_SetUserType(EET_Assistant, iAccount, &dataInfo);
    }
    else
    {
        COMMONUNITS_WARN("unkown node [%s]", lpszNodeName);
    }
}
#endif



#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
/////////////////////////////////////////////////////////////////////////
// 对Sync CallRecord的处理类
CSyncCallRecordProcess::CSyncCallRecordProcess()
{

}

CSyncCallRecordProcess::~CSyncCallRecordProcess()
{

}

VOID CSyncCallRecordProcess::ProcessXMLInfo(xml_node nodeElementNode, int iAccount)
{
    if (nodeElementNode.empty())
    {
        return;
    }
    LPCSTR lpszNodeName = nodeElementNode.name();

    if (lpszNodeName == NULL)
    {
        return;
    }

    COMMONUNITS_INFO("CSyncCallRecordProcess::ProcessXMLInfo ");

    CALL_RECORDING_MODE eType = CRM_NEVER;

    yl::string strType = "";
    GetElementContent(nodeElementNode, "xBw:callRecordingMode", strType);

    COMMONUNITS_INFO("CSyncCallRecordProcess::ProcessXMLInfo strType[%s]", strType.c_str());
    if (strType == "never")
    {
        eType = CRM_NEVER;
    }
    else if (strType == "always")
    {
        eType = CRM_ALWAYS;
    }
    else if (strType == "always-pause-resume")
    {
        eType = CRM_ALWAYS_PAUSE_RESUME;
    }
    else if (strType == "on-demand")
    {
        eType = CRM_DEMAND;
    }
    else if (strType == "on-demand-user-start")
    {
        eType = CRM_DEMAND_USER_START;
    }
    else
    {
        return;
    }

    callRecord_SetRecordMode(iAccount, eType);
}

#endif

