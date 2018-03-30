#if IF_FEATURE_GENBAND_E911
#include "e911addressmanager.h"
#include <xmlparser/xmlparser.hpp>
#include <commonapi/stringhelperapi.h>
#include <etlmsghandler/modetlmsghandler.h>
#include "threadtask/include/networkfileaction.h"
#include "threadtask/include/modthreadtask.h"
#include "account/include/modaccount.h"
#include "dsklog/log.h"
#include "voip/sipsender.h"
#include "regexp/modregexp.h"
#include "service_sip_interface.h"

#define TEMP_FILE   "/tmp/e911address"
#define URL_MAX_LEN 256

const yl::string kszEmptyString = "";

bool CompareAddress(int& lhs,
                    int& rhs)
{
    const yl::string& strLkey = _E911AddressMgr.GetDisplayNameByID(lhs);

    const yl::string& strRkey = _E911AddressMgr.GetDisplayNameByID(rhs);

    if (strLkey.empty())
    {
        return false;
    }
    if (strRkey.empty())
    {
        return true;
    }

    return strcasecmp(strLkey.c_str(), strRkey.c_str()) < 0;
}

IMPLEMENT_GETINSTANCE(CE911AddressManager)


CE911AddressManager::CE911AddressManager()
{
    // 注册消息处理
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, &CE911AddressManager::OnMessage);
    etl_RegisterMsgHandle(SIP_SUB_GENBAND_SERVICE_PACKAGE, SIP_SUB_GENBAND_SERVICE_PACKAGE, &CE911AddressManager::OnMessage);

}

CE911AddressManager::~CE911AddressManager()
{
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, &CE911AddressManager::OnMessage);
    etl_UnregisterMsgHandle(SIP_SUB_GENBAND_SERVICE_PACKAGE, SIP_SUB_GENBAND_SERVICE_PACKAGE, &CE911AddressManager::OnMessage);
}

LRESULT CE911AddressManager::OnMessage(msgObject& objMessage)
{
    BOOL bHandled = TRUE;
    COMMONUNITS_INFO("CE911AddressManager::OnMessage [%d, %d, %d]",
                     objMessage.message, objMessage.wParam, objMessage.lParam);
    switch (objMessage.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
        {
            _E911AddressMgr.OnConfigChange(objMessage);
        }
        break;
    case SIP_SUB_GENBAND_SERVICE_PACKAGE:
        {
            if (!_E911AddressMgr.IsGenbandE911On())
            {
                COMMONUNITS_INFO("e911 off");
                break;
            }

            // Localtion ID 已经设置也更新URL,但是不提示
            char *pUrl = (char *)objMessage.GetExtraData();
            if (pUrl != NULL)
            {
                char szUrl[URL_MAX_LEN] = {0};
                regExp_Replace("(http(s)?://[\\w\\-\\.\\+,@?^=%&:~#]*/)(.*)",
                               pUrl, "$1", szUrl, URL_MAX_LEN);
                APP_INFO("E911 Url %s szUrl %s", pUrl, szUrl);
                configParser_SetConfigString(kszE911SopiUrl, szUrl);

                yl::string strLocationID = configParser_GetConfigString(kszE911LocationID);
                if (!strLocationID.empty())
                {
                    COMMONUNITS_INFO("e911 location not empty [%s]", strLocationID.c_str());
                    break;
                }
//              bool bResult = _E911AddressMgr.ReqE911AddrestList(szUrl);
//
//              //成功提示
//              if (bResult)
                {
                    etl_NotifyApp(FALSE, E911_MESSAGE_DOWNLOAD_ADDRESS, 0, 0);
                }
            }
        }
        break;
    default:
        {
            bHandled = FALSE;
        }
        break;
    }

    return bHandled ? 1 : 0;
}

void CE911AddressManager::OnTaskActionCallback(CTaskAction* pTaskAction)
{
    if (pTaskAction == NULL)
    {
        return;
    }

    COMMONUNITS_INFO("CE911AddressManager::OnTaskActionCallback Begin");

    // 返回结果的类型为请求网络文件.
    CNetworkFileAction* pNetAction =
        dynamic_cast<CNetworkFileAction*>(pTaskAction);
    if (pNetAction == NULL)
    {
        return;
    }

    if (m_eStatus != E911_DOWNLOAD_STATUS_DOWNLOADING)
    {
        remove(pNetAction->GetFilePath().c_str());
        return;
    }

    bool suc = pNetAction->IsOperationSucceed();

    if (suc)
    {
        suc = ParseE911AddressList(pNetAction->GetFilePath());
    }

    COMMONUNITS_INFO("CE911AddressManager::OnTaskActionCallback suc [%d] [%d]",
                     pNetAction->IsOperationSucceed(), suc);


    m_eStatus = suc ? E911_DOWNLOAD_STATUS_DOWNLOAD_SUCC :
                E911_DOWNLOAD_STATUS_DOWNLOAD_FAIL;

    etl_NotifyApp(
        false,
        E911_MESSAGE_DOWNLOAD_RESULT,
        suc,
        0);

    remove(pNetAction->GetFilePath().c_str());

    COMMONUNITS_INFO("CE911AddressManager::OnTaskActionCallback End [%d]", suc);
}

void CE911AddressManager::Init()
{
    m_vAddressList.clear();
    m_eStatus = E911_DOWNLOAD_STATUS_NONE;
}


bool CE911AddressManager::ReqE911AddrestList(yl::string strUrl /*= ""*/)
{
    if (!IsGenbandE911On())
    {
        return false;
    }

    yl::string strReqUrl = strUrl;
    if (strReqUrl.empty())
    {
        strReqUrl = configParser_GetConfigString(kszE911SopiUrl);
    }

    ClearE911Address();

    if (strReqUrl.empty())
    {
        return false;
    }

    if (m_eStatus == E911_DOWNLOAD_STATUS_DOWNLOADING)
    {
        return false;
    }

    int nAccountID = configParser_GetConfigInt(kszEmergencyAccountID) - 1;
    yl::string strUserName = acc_GetUsername(nAccountID);
    strUserName.append("@");
    strUserName.append(acc_GetServerName(nAccountID));
    yl::string strPassword = acc_GetPassward(nAccountID);

    strReqUrl.append("sopi/services/LocationUserService");

    yl::string strFile = TEMP_FILE;
    AppandReqE911ListNote(strFile);

    COMMONUNITS_INFO("ReqE911AddrestList Server URL[%s] UserName[%s]",
                     strUrl.c_str(), strUserName.c_str());

    CNetworkFileAction* pAction = TaskAction_CreateNetworkFileAction(strFile.c_str(), strReqUrl.c_str());

    if (pAction != NULL)
    {
        pAction->SetRead(false);  // post文件.
        pAction->SetSync(false);  // 异步.
        pAction->SetCallback(this);  // 回调函数.
        pAction->SetUsername(strUserName);
        pAction->SetPassword(strPassword);

        pAction->SetDownLoadMode(MODE_BASIC_AUTH);
        yl::string strHead;
        strHead = commonAPI_FormatString("application/xml;charset=%s\r\n%s", pAction->m_strCharset.c_str(), "SOAPAction: NA");
        pAction->SetHeader(strHead);

        m_eStatus = E911_DOWNLOAD_STATUS_DOWNLOADING;

        TaskAction_ExecAction(pAction);
        return true;
    }
    return false;

}

bool CE911AddressManager::GetE911AddressListData(E911AddressListData *pData, int nParentID)
{
    if (pData == NULL || pData->m_nIndexFrom < 0)
    {
        return false;
    }

    YLVector<int>* pVlist = NULL;

    if (nParentID == kszE911RootID)
    {
        pVlist = &m_vAddressList;
    }
    else
    {
        E911AddressData* pDetial = GetAddressDetailByID(nParentID);

        if (pDetial != NULL)
        {
            pVlist = &(pDetial->m_vChildrenID);
        }
    }

    if (pVlist == NULL)
    {
        return false;
    }

    pData->m_nTotalCount = pVlist->size();

    int nCountGet = pData->m_nCountGet == 0 ? pVlist->size() : pData->m_nCountGet;

    for (int i = pData->m_nIndexFrom; i < pVlist->size(); i++)
    {
        if (nCountGet <= 0)
        {
            break;
        }

        pData->m_vlistID.push_back((*pVlist)[i]);
        nCountGet--;
    }

    return true;
}

bool CE911AddressManager::GetE911AddressList(YLVector<int> &listout, int nParentID /*= -1*/)
{
    if (nParentID == -1)
    {
        listout = m_vAddressList;
    }
    else
    {
        E911AddressData* pData = GetAddressDetailByID(nParentID);
        if (pData != NULL)
        {
            listout = pData->m_vChildrenID;
        }
    }

    return listout.size() > 0;
}

E911AddressData* CE911AddressManager::GetAddressDetailByID(int nID)
{
    MapE911Address::iterator iter = m_mapAddress.find(nID);

    if (iter != m_mapAddress.end())
    {
        return iter->second;
    }

    return NULL;
}

int CE911AddressManager::GetParentIDByID(int nID)
{
    E911AddressData* pData = GetAddressDetailByID(nID);
    if (pData)
    {
        return pData->m_nParentID;
    }
    return -1;
}

int CE911AddressManager::GetIDByIndex(int nParentID, int nIndex)
{
    int nID = -1;
    if (nParentID == kszE911RootID
            && m_vAddressList.size() > nIndex)
    {
        nID = m_vAddressList[nIndex];
    }
    else
    {
        MapE911Address::iterator iter = m_mapAddress.find(nParentID);
        if (iter != m_mapAddress.end())
        {
            E911AddressData *pData = iter->second;
            if (pData != NULL && pData->m_vChildrenID.size() > nIndex)
            {
                nID = pData->m_vChildrenID[nIndex];
            }
        }
    }
    return nID;
}

const yl::string& CE911AddressManager::GetDisplayNameByID(int nID)
{
    MapE911Address::iterator iter = m_mapAddress.find(nID);

    if (iter != m_mapAddress.end())
    {
        if ((iter->second) != NULL)
        {
            return iter->second->m_strDisplayName;
        }
    }

    return kszEmptyString;
}

bool CE911AddressManager::GetFullLocationAddr(int& nID, yl::string& strFullAddr)
{
    E911AddressData* pData = GetAddressDetailByID(nID);
    if (pData == NULL || pData->m_nParentID < 0)
    {
        return false;
    }

    if (!strFullAddr.empty())
    {
        strFullAddr.append(",");
    }
    strFullAddr.append(pData->m_strDisplayName);

    if (pData->m_nParentID > 0)
    {
        GetFullLocationAddr(pData->m_nParentID, strFullAddr);
    }

    return true;
}

bool CE911AddressManager::SetLocationID(int& nID, yl::string& strFullAddr)
{
    COMMONUNITS_INFO("CE911AddressManager::SetLocationID [%d]", nID);
    //ClearE911Address();
    char szTmp[64];
    sprintf(szTmp, "%d", nID);
    configParser_SetConfigString(kszE911LocationID, szTmp);
    configParser_SetConfigString(kszE911LocationAddr, strFullAddr);
    int nAccountID = configParser_GetConfigInt(kszEmergencyAccountID) - 1;
    return _SIPSender.SetLocationID(nAccountID, nID);
}

int CE911AddressManager::GetAddressListCount(int nID)
{
    if (nID == 0)
    {
        return m_vAddressList.size();
    }

    E911AddressData* pData = GetAddressDetailByID(nID);
    if (pData != NULL)
    {
        return pData->m_vChildrenID.size();
    }

    return 0;
}

bool CE911AddressManager::CancelDownload()
{
    m_eStatus = E911_DOWNLOAD_STATUS_CANCEL_DOWNLOAD;
    return true;
}

bool CE911AddressManager::ParseE911AddressList(const yl::string& strFile)
{
    if (strFile.empty())
    {
        return false;
    }

    COMMONUNITS_INFO("CE911AddressManager::ParseE911AddressList [%s]", strFile.c_str());

    xml_document doc;

    if (!doc.load_file(strFile.c_str()))
    {
        COMMONUNITS_INFO("CE911AddressManager::ParseE911AddressList Fail");
        return false;
    }

    xml_node BodyNode = doc.child("soapenv:Envelope").child("soapenv:Body");

    if (BodyNode.empty())
    {
        COMMONUNITS_INFO("CE911AddressManager::ParseE911AddressList Body Empty");
        return false;
    }

    for (xml_node multiRef = BodyNode.child("multiRef"); !multiRef.empty(); multiRef = multiRef.next_sibling("multiRef"))
    {
        xml_node LocIDNode = multiRef.child("locationId");
        xml_node NameNode = multiRef.child("name");
        xml_node ParIDNode = multiRef.child("parentLocationId");
        if (!LocIDNode.empty() && !NameNode.empty() && !ParIDNode.empty())
        {
            E911AddressData* pNew = new E911AddressData();
            if (pNew != NULL)
            {
                pNew->m_nID = atoi(LocIDNode.child_value());
                pNew->m_strDisplayName = NameNode.child_value();
                pNew->m_nParentID = atoi(ParIDNode.child_value());
                if (pNew->m_nParentID == 0)
                {
                    m_vAddressList.push_back(pNew->m_nID);
                }
                m_mapAddress.put(pNew->m_nID, pNew);
            }
        }
    }

    ProcessE911ChildList();

    yl::stable_sort(m_vAddressList.begin(), m_vAddressList.end(), CompareAddress);

    return true;
}

bool CE911AddressManager::ProcessE911ChildList()
{
    MapE911Address::iterator iter = m_mapAddress.begin();
    for (; iter != m_mapAddress.end(); ++iter)
    {
        E911AddressData* pNode = iter->second;
        int nID = iter->first;
        //ParentID 为 0 为根节点
        if (pNode != NULL && pNode->m_nParentID != kszE911RootID)
        {
            MapE911Address::iterator ParIdIter = m_mapAddress.find(pNode->m_nParentID);
            E911AddressData* pParIDNode = ParIdIter->second;
            if (pParIDNode != NULL)
            {
                pParIDNode->m_vChildrenID.push_back(nID);
            }
        }
    }

    iter = m_mapAddress.begin();
    for (; iter != m_mapAddress.end(); ++iter)
    {
        E911AddressData* pNode = iter->second;
        if (pNode != NULL && pNode->m_vChildrenID.size() > 0)
        {
            yl::stable_sort(pNode->m_vChildrenID.begin(), pNode->m_vChildrenID.end(), CompareAddress);
        }
    }

    return true;
}

/******************************************************************************
<?xml version="1.0" encoding="utf-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:ns2="common.ws.nortelnetworks.com" xmlns:ns3="core.data.ws.nortelnetworks.com" xmlns:ns4="http://xml.apache.org/xml-soap" xmlns:ns1="location.ws.nortelnetworks.com" xmlns:hns="SOPI">
    <SOAP-ENV:Header>
        <hns:locale>zh_CN</hns:locale>
        <hns:version>1.0</hns:version>
    </SOAP-ENV:Header>
    <SOAP-ENV:Body SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
        <ns1:getLocationList>
            <in0 xsi:type="ns2:UserNaturalKeyDO">
                <name>y5978000@yealink.a2e01.gbiot</name>
            </in0>
            <in1 xsi:type="ns1:ClientLocationInputDO">
                <forceDownload>true</forceDownload>
                <timestamp>1498102145000</timestamp>
            </in1>
        </ns1:getLocationList>
    </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
******************************************************************************/
bool CE911AddressManager::AppandReqE911ListNote(yl::string& strFile)
{
    xml_document doc;

    xml_node root;
    AppendDocument(doc, root);

    AppendHeaderNode(root);

    AppendBodyNode(root);

    doc.save_file(strFile.c_str());

    return true;
}

bool CE911AddressManager::AppendDocument(xml_document& doc, xml_node& root)
{
    root = doc.append_child("SOAP-ENV:Envelope");

    root.append_attribute("xmlns:SOAP-ENV", "http://schemas.xmlsoap.org/soap/envelope/");
    root.append_attribute("xmlns:SOAP-ENC", "http://schemas.xmlsoap.org/soap/encoding/");
    root.append_attribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
    root.append_attribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
    root.append_attribute("xmlns:ns2", "common.ws.nortelnetworks.com");
    root.append_attribute("xmlns:ns3", "core.data.ws.nortelnetworks.com");
    root.append_attribute("xmlns:ns4", "http://xml.apache.org/xml-soap");
    root.append_attribute("xmlns:ns1", "location.ws.nortelnetworks.com");
    root.append_attribute("xmlns:hns", "SOPI");

    return true;
}

bool CE911AddressManager::AppendHeaderNode(xml_node& root)
{
    root.append_child("SOAP-ENV:Header");
    return true;
}

/**********************************************
<SOAP-ENV:Body SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
    <ns1:getLocationList>
        <in0 xsi:type="ns2:UserNaturalKeyDO">
            <name>y5978000@yealink.a2e01.gbiot</name>
        </in0>
        <in1 xsi:type="ns1:ClientLocationInputDO">
            <forceDownload>true</forceDownload>
            <timestamp>1498102145000</timestamp>
        </in1>
    </ns1:getLocationList>
</SOAP-ENV:Body>
**********************************************/
bool CE911AddressManager::AppendBodyNode(xml_node& root)
{
    xml_node BodyNode = root.append_child("SOAP-ENV:Body");
    BodyNode.append_attribute("SOAP-ENV:encodingStyl", "http://schemas.xmlsoap.org/soap/encoding/");

    xml_node optNode = BodyNode.append_child("ns1:getLocationList");

    xml_node in0Node = optNode.append_child("in0");
    in0Node.append_attribute("xsi:type", "ns2:UserNaturalKeyDO");

    int nAccountID = configParser_GetConfigInt(kszEmergencyAccountID) - 1;
    yl::string strUserName = acc_GetUsername(nAccountID);
    strUserName.append("@");
    strUserName.append(acc_GetServerName(nAccountID));
    AppendStringTypeNode(in0Node, "name", strUserName.c_str());

    xml_node in1Node = optNode.append_child("in1");
    in1Node.append_attribute("xsi:type", "ns1:ClientLocationInputDO");
    AppendStringTypeNode(in1Node, "forceDownload", "true");
    AppendStringTypeNode(in1Node, "timestamp", "0");

    return true;
}

void CE911AddressManager::AppendStringTypeNode(xml_node& node, const yl::string& strName, const yl::string& strValue)
{
    xml_node nodeChild = node.append_child(strName.c_str());
    nodeChild.text().set(strValue.c_str());
}

void CE911AddressManager::ClearE911Address()
{
    MapE911Address::iterator iter = m_mapAddress.begin();
    for (; iter != m_mapAddress.end(); ++iter)
    {
        E911AddressData *pData = iter->second;
        if (pData != NULL)
        {
            delete pData;
        }
    }

    m_mapAddress.clear();
    m_vAddressList.clear();
}

bool CE911AddressManager::IsGenbandE911On()
{
    yl::string strCfgSource = configParser_GetConfigString(kszAssertedIdSource);
    return strcasecmp(strCfgSource.c_str(), "GENBAND") == 0;
}

void CE911AddressManager::OnConfigChange(msgObject& objMessage)
{
    if (objMessage.wParam == ST_EMERGENCY
            && !IsGenbandE911On())
    {
        ClearE911Address();
        configParser_SetConfigString(kszE911LocationID, "");
        configParser_SetConfigString(kszE911LocationAddr, "");
        int nAccountID = configParser_GetConfigInt(kszEmergencyAccountID) - 1;
        _SIPSender.SetLocationID(nAccountID, 0);
    }
}
#endif
