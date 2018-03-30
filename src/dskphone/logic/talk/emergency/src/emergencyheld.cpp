#if IF_FEATURE_EMERGENCY_HELD
#include "emergencyheld.h"
#include "account/include/modaccount.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "threadtask/include/networkfileaction.h"
#include "taskaction/modtaskaction.h"
#include "threadtask/include/modthreadtask.h"
#include "extended/event/include/modevent.h"
#include "wireless/wifi/include/modwifi.h"
#include "idlescreen/include/modidlescreen.h"
#include <ylstringalgorithm.h>

#define kszEmergencyHELDXML "/tmp/EmergencyHELD.xml"
#define MAX_ELEMENT_NUM     255
#define NETWORK_CHECK_TIME  3 * 1000 //网络防抖

LRESULT OnE911ConfigChange(msgObject & objMsg)
{
    if (CONFIG_MSG_BCAST_CHANGED != objMsg.message
            || (ST_EMERGENCY != objMsg.wParam))
    {
        return FALSE;
    }

    g_objEmergencyHELDManager.ReloadConfig();
    PhoneInfoEventData sPhoneInfo = event_GetPhoneInfo();
    g_objEmergencyHELDManager.SendPhoneInfo(&sPhoneInfo);

    return TRUE;
}

LRESULT OnE911Timer(msgObject & objMsg)
{
    if (TM_TIMER != objMsg.message)
    {
        return FALSE;
    }
    return g_objEmergencyHELDManager.OnTimer(objMsg.wParam);
}

CEmergencyHELDProcesser & CEmergencyHELDProcesser::GetInstace()
{
    static CEmergencyHELDProcesser m_sInstace;

    return m_sInstace;
}

CEmergencyHELDProcesser::CEmergencyHELDProcesser() : CEventBase(EMT_E911)
{
    Init();
}

CEmergencyHELDProcesser::~CEmergencyHELDProcesser()
{
    UnInit();
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnE911ConfigChange);
    etl_UnregisterMsgHandle(TM_TIMER, TM_TIMER, OnE911Timer);
}

void CEmergencyHELDProcesser::Init()
{
    ReloadConfig();
    SingleMsgReg(CONFIG_MSG_BCAST_CHANGED, OnE911ConfigChange);
    SingleMsgReg(TM_TIMER, OnE911Timer);
}

void CEmergencyHELDProcesser::UnInit()
{
    m_bIsEnable = false;
    m_strLocation.clear();
    UnRegisterModule();
}

void CEmergencyHELDProcesser::ReloadConfig()
{
    yl::string strCfgSource = configParser_GetConfigString(kszAssertedIdSource);
    m_strE911Server = configParser_GetConfigString(kszEmergencyHeldServer);

    yl::string strRequestType = configParser_GetConfigString(kszEmergencyHeldRequestType);
    m_eRequestType = EM_REQ_SIMPLE;
    if (strcasecmp(strRequestType.c_str(), "REDSKY") == 0)
    {
        m_eRequestType = EM_REQ_REDSKY;
    }

    if (strcasecmp(strCfgSource.c_str(), "HELD") == 0
            && configParser_GetConfigInt(kszEmengencySwtich) != 0)
    {
        m_bIsEnable = true;
        RegisterModule();
    }
    else
    {
        UnInit();
    }
}

bool CEmergencyHELDProcesser::SendEvent(EventDataBase * pData)
{
    bool bRet = false;

    switch (pData->m_eEventDataType)
    {
    case EDT_PHONE:
        {
            if (pData->m_eEventType == EVENT_IP_CHANGED)
            {
                SendPhoneInfo(pData);
            }
        }
        break;
    default:
        break;
    }
    return bRet;
}

void CEmergencyHELDProcesser::SendPhoneInfo(EventDataBase * pData)
{
    if (!m_bIsEnable)
    {
        idleScreen_DropStatus(PS_STATE_E911_ERROR);
        return;
    }

    PhoneInfoEventData * pPhoneInfoData = static_cast<PhoneInfoEventData *>(pData);
    if (pPhoneInfoData == NULL)
    {
        return;
    }

    timerKillThreadTimer(TIMER_ID(m_phoneInfoData));

    m_phoneInfoData.m_strMAC = pPhoneInfoData->m_strMAC;
    m_phoneInfoData.m_strPhoneType = pPhoneInfoData->m_strPhoneType;
    m_phoneInfoData.m_strFirmware = pPhoneInfoData->m_strFirmware;
    m_phoneInfoData.m_strIpV4 = pPhoneInfoData->m_strIpV4;
    m_phoneInfoData.m_strIpV6 = pPhoneInfoData->m_strIpV6;
    m_phoneInfoData.m_strIp = pPhoneInfoData->m_strIp;

    timerSetThreadTimer(TIMER_ID(m_phoneInfoData), NETWORK_CHECK_TIME);
}

bool CEmergencyHELDProcesser::OnTimer(UINT uTimerID)
{
    if (uTimerID == TIMER_ID(m_phoneInfoData))
    {
        timerKillThreadTimer(TIMER_ID(m_phoneInfoData));
        if (netIsNetworkReady())
        {
            SendPhoneInfo();
        }
        return true;
    }

    return false;
}

/*
<locationRequest xmlns="urn:ietf:params:xml:ns:geopriv:held" responseTime="8">
    <locationType exact="true">locationURI</locationType>
    <device xmlns="urn:ietf:params:xml:ns:geopriv:held:id">
        <ip v="4">192.0.2.5</ip>
        <ip v="6">2001:db8::1:ea7:fee1:d1e</ip>
        <mac>A0-12-34-56-78-90</mac>
        <nai>user@example.net</nai>
        <bssid>84-f8-d7-1c-9f-d9</bssid>
    </device>
    <companyID>6f2f2d50-c385-4b72-b84a-ce0ca3a77cb7</companyID>
</locationRequest>
*/
void CEmergencyHELDProcesser::SendPhoneInfo()
{
    m_strLocation.clear();
    if (m_strE911Server.empty())
    {
        idleScreen_SetStatus(PS_STATE_E911_ERROR);
        return;
    }

    remove(kszEmergencyHELDXML);
    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRootNode = doc.append_child("locationRequest");
    nodeRootNode.append_attribute("xmlns", "urn:ietf:params:xml:ns:geopriv:held");
    nodeRootNode.append_attribute("responseTime", "8");

    xml_node node = nodeRootNode.append_child("locationType");
    node.append_attribute("exact", "true");
    node.text().set("locationURI");

    if (m_eRequestType == EM_REQ_REDSKY)
    {
        xml_node nodeDevice = nodeRootNode.append_child("device");
        nodeDevice.append_attribute("xmlns", "urn:ietf:params:xml:ns:geopriv:held:id");

        //IP地址
        node = nodeDevice.append_child("ip");
        node.append_attribute("v", "4");
        node.text().set(m_phoneInfoData.m_strIpV4.c_str());
        node = nodeDevice.append_child("ip");
        node.append_attribute("v", "6");
        node.text().set(m_phoneInfoData.m_strIpV6.c_str());

        //MAC地址
        node = nodeDevice.append_child("mac");
        node.text().set(m_phoneInfoData.m_strMAC.c_str());

        //账号
        node = nodeDevice.append_child("nai");
        yl::string strAccountInfo = "";
        int nDefAccountId = acc_GetDefaultAccount();
        if (acc_IsAccountIDValid(nDefAccountId)
                && !acc_IsDirectIPAccount(nDefAccountId))
        {
            strAccountInfo = commonAPI_FormatString("%s@%s", acc_GetUsername(nDefAccountId).c_str(), acc_GetServerName(nDefAccountId).c_str());
        }
        node.text().set(strAccountInfo.c_str());

        //wifi dongle的mac地址
        node = nodeDevice.append_child("bssid");
        yl::string strMac;
#ifdef IF_SUPPORT_WIFI
        WIFI_GetLocalMac(strMac);
#endif
        node.text().set(strMac.c_str());
    }

    AddRequestElement(nodeRootNode);

    doc.save_file(kszEmergencyHELDXML);

    yl::string strContent = yl::dump_file(kszEmergencyHELDXML);

    requestLocationbyHELD();
}

void CEmergencyHELDProcesser::AddRequestElement(xml_node & xmlNode)
{
    xml_node node;
    for (int i = 1; i <= MAX_ELEMENT_NUM; i++)
    {
        yl::string strElementName = configParser_GetCfgItemStringValue(kszEmergencyHeldElementName, i);
        yl::string strElementValue = configParser_GetCfgItemStringValue(kszEmergencyHeldElementValue, i);
        if (strElementName.empty() || strElementValue.empty())
        {
            return;
        }

        node = xmlNode.append_child(strElementName.c_str());
        node.text().set(strElementValue.c_str());
    }
}

void CEmergencyHELDProcesser::requestLocationbyHELD()
{
    if (m_strE911Server.empty())
    {
        return;
    }

    CNetworkFileAction * pAction = TaskAction_CreateNetworkFileAction(
                                       kszEmergencyHELDXML, m_strE911Server.c_str());
    if (pAction != NULL)
    {
        pAction->SetSync(false);  // 设置异步.
        pAction->SetRead(false);  // 设置下载.
        pAction->SetPut(false);
        pAction->SetCallback(this);
        pAction->SetHeader("application/held+xml;charset=utf-8\r\nAccept: application/held+xml");
        TaskAction_ExecAction(pAction);
    }
}

void CEmergencyHELDProcesser::OnTaskActionCallback(NS_TA::CTaskAction * pTaskAction)
{
    if (pTaskAction == NULL
            || pTaskAction->GetReqType() != NS_TA::TA_REQUEST_NETWORK_FILE)
    {
        return;
    }

    CNetworkFileAction * pAction = static_cast<CNetworkFileAction *>(pTaskAction);
    if (pAction != NULL)
    {
        if (pTaskAction->GetTaskActionErrorType() == NS_TA::TA_NO_ERROR
                && ParseLocation(pAction->GetFilePath()))
        {
            idleScreen_DropStatus(PS_STATE_E911_ERROR);
        }
        else
        {
            ACTION_INFO("Parse Location Fail");
            idleScreen_SetStatus(PS_STATE_E911_ERROR);
        }
    }

    remove(kszEmergencyHELDXML);
}

/*
<s:Body xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema">
<locationResponse xmlns="urn:ietf:params:xml:ns:geopriv:held">
    <locationUriSet expires="<some future time>"
        <locationURI>
            https://anywhere.redskytech.com/e911Anywhere/heldref?<code>&companyId=<uuid>&deviceId=<mac>
        </locationURI>
        <locationURI>
            https://anywhere2.redskytech.com/e911Anywhere/heldref?<code>&companyId=<uuid>&deviceId=<mac>
        </locationURI>
    </locationUriSet>
</locationResponse>
*/
bool CEmergencyHELDProcesser::ParseLocation(const yl::string & strFile)
{
    if (strFile.empty())
    {
        ACTION_INFO("Parse Location Fail File Is empty.");
        return false;
    }

    yl::string strContent = yl::dump_file(strFile.c_str());

    int nIndex1 = strContent.find("<locationURI>");
    int nIndex2 = strContent.find("</locationURI>");

    yl::string strLocation = "";
    while (nIndex1 != strContent.npos && nIndex2 != strContent.npos)
    {
        yl::string strTempLocaltion = strContent.substr(nIndex1 + 13, nIndex2 - nIndex1 - 13);
        strTempLocaltion.trim_both(" \r\n\t");
        if (!strTempLocaltion.empty())
        {
            if (!strLocation.empty())
            {
                strLocation.append(",");
            }
            strLocation.append("<").append(strTempLocaltion.c_str()).append(">");
        }

        nIndex1 = strContent.find("<locationURI>", nIndex2 + 14);
        nIndex2 = strContent.find("</locationURI>", nIndex2 + 14);
    }

    setLocaltion(strLocation);
    ACTION_INFO("HELD Location[%s]", strLocation.c_str());

    return !strLocation.empty();
}

#endif
