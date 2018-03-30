#include "genbandcontactmanager.h"
#include "genbandbookaction.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "directorymessagedefine.h"
#include "basemethod.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "taskaction/modtaskaction.h"
#include "contacts/supersearch/include/modsupersearch.h"


#if IF_FEATURE_GBBOOK

#define kszGbReqestFile  "GbookReqest%d.xml"

#define kszXsiType "xsi:type"

bool CompareGenband(int nLeftId, int nRightId)
{
    return g_pGenBandDirManager->GetCompResult(nLeftId, nRightId) < 0;
}

CGenBandContactManager * CGenBandContactManager::GetInstance()
{
    static CGenBandContactManager instance;
    return &instance;
}

CGenBandContactManager::CGenBandContactManager()
{
    m_eDirType = GB_DIR;

    m_iInitTimer = 5 * 1000;

    m_OptType = PAB_GAB_NONE;

    m_strGabSearchkey = "";

    m_bLocalChangeDir = false;

    m_eStatus = FILE_DOWNLOADED_OK;
}

CGenBandContactManager::~CGenBandContactManager()
{

}

bool CGenBandContactManager::Init()
{
    if (configParser_GetConfigInt(kszGbSoupEnable) != 1)
    {
        return false;
    }

    // Init 时候重新设置状态，如果在下载PAB过程中吧开关关掉然后
    // 重新开启，由于之前的状态没有被清空还是下载中，不会再去下载
    m_eStatus = FILE_DOWNLOADED_OK;

    CBaseContactManger::MyInit();

    yl::string strServer = configParser_GetConfigString(kszGbSoupServer);

    if (strServer.substr(0, 4) != "http")
    {
        strServer = "http://" + strServer;
    }

    if (strServer.find("sopi/services") == yl::string::npos)
    {
        strServer.append("/sopi/services/");
    }

    if (strServer.substr(strServer.length() - 1, 1) != "/")
    {
        strServer.append("/");
    }

    m_strUser = configParser_GetConfigString(kszGbSoupUser);
    m_strPassword = configParser_GetConfigString(kszGbSoupPsw);


    DIR_INFO("server[%s]user[%s]pwd[%s]", strServer.c_str(), m_strUser.c_str(), m_strPassword.c_str());

    if (configParser_GetConfigInt(kszGbGabBookEnable) == 1)
    {
        ContactGBMenu * pGab = new ContactGBMenu();

        pGab->m_strDisplayName = TRID_GB_GABBOOK;

        pGab->m_strUrl = strServer;

        pGab->m_strUrl.append("GABUserService");

        InsertMap(pGab, kszGabBookGroupId);
    }

    if (configParser_GetConfigInt(kszGbPabBookEnable) == 1)
    {
        ContactGBMenu * pPab = new ContactGBMenu();

        pPab->m_strDisplayName = TRID_GB_PABBOOK;

        pPab->m_strUrl = strServer;

        pPab->m_strUrl.append("AddressBookUserService");

        pPab->m_strPabGroupUrl = strServer;

        pPab->m_strPabGroupUrl.append("AddressBookGroupUserService");

        InsertMap(pPab, kszPabBookGroupId);

        timerSetThreadTimer((UINT)&m_iInitTimer, m_iInitTimer);
    }
    return true;
}

bool CGenBandContactManager::IsNeedShowAllGroup()
{
    return false;
}

bool CGenBandContactManager::UpdatePabData()
{
    DIR_INFO("CGenBandContactManager::UpdatePabData");

    ClearContactByGroupID(kszPabBookGroupId);
    ClearContactByGroupID(kszAllContactId);

    SuperSearch_SetIsClearLastKey(true);

    return ReqGroupList();
}

void CGenBandContactManager::ReloadData()
{
    if (configParser_GetConfigInt(kszGbRetainSearchFilter) != 1)
    {
        ClearGabSearchInfo();
    }
    MyFinal();
    Init();
}

bool CGenBandContactManager::OnTimeout(unsigned int uTimerID, bool bInTalk)
{
    if (bInTalk)
    {
        // 通话中不去更新
        return false;
    }
    if (uTimerID == (UINT)&m_iInitTimer)
    {
        if (IsNetworkConnected())
        {
            timerKillThreadTimer((UINT)&m_iInitTimer);

            UpdatePabData();
        }
        return true;
    }
    else if (uTimerID == (UINT)&m_bLocalChangeDir)
    {
        timerKillThreadTimer((UINT)&m_bLocalChangeDir);
        m_bLocalChangeDir = false;
    }
    return false;
}

DIRECTORY_RET_CODE CGenBandContactManager::ReqSearchGABByType(const yl::string & strkey,
        GBBOOK_OPERATION_TYPE eType)
{
    if (!IsNetworkConnected())
    {
        return DIR_RET_RERROR_NETWORK_ERROR;
    }

    ContactGBMenu * pMenu = Base2GabMenu(GetMenuDataById(kszGabBookGroupId));

    if (pMenu == NULL)
    {
        return DIR_RET_FAIL;
    }

    if (m_strGabSearchkey.compare(strkey) == 0 && eType ==  m_OptType
            && pMenu->m_eStatus == FILE_DOWNLOADING)
    {
        return GET_LIST_DOWNLOADING;
    }

    m_strGabSearchkey = strkey;
    m_OptType = eType;

    //Save Request File
    SaveGabSearchFile(strkey, eType);

    ClearContactByGroupID(kszGabBookGroupId);

    if (MyExecPost(kszGabBookGroupId, pMenu->m_strUrl, eType))
    {
        pMenu->m_eStatus = FILE_DOWNLOADING;
        return DIR_RET_SUCC;
    }

    return DIR_RET_FAIL;
}

/*<soapenv:Envelope xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:add="addrbook.ws.nortelnetworks.com">
<soapenv:Header/>
<soapenv:Body>
<add:getAddressBook soapenv:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
</soapenv:Body>
</soapenv:Envelope>*/
bool CGenBandContactManager::ReqGetContactList()
{
    DIR_INFO("CGenBandContactManager::ReqGetContactList()");

    ContactGBMenu * pMenu = Base2GabMenu(GetMenuDataById(kszPabBookGroupId));

    if (pMenu == NULL)
    {
        return false;
    }

    if (pMenu->m_eStatus == FILE_DOWNLOADING)
    {
        return true;
    }

    //Save Request File
    xml_document doc;

    xml_node BodyNode = AppendBodyNode(doc);

    BodyNode.append_child("add:getAddressBook");

    doc.save_file(GetNewFile().c_str());


    if (MyExecPost(kszPabBookGroupId, pMenu->m_strUrl, PAB_OPERATION_GET_CONTACTLIST))
    {
        pMenu->m_eStatus = FILE_DOWNLOADING;
        return true;
    }

    return false;
}

/*<soapenv:Envelope xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:add="addrbook.ws.nortelnetworks.com">
<soapenv:Header/>
<soapenv:Body>
<add:getAddressBookGroups soapenv:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
</soapenv:Body>
</soapenv:Envelope>*/
bool CGenBandContactManager::ReqGroupList()
{
    ContactGBMenu * pMenu = Base2GabMenu(GetMenuDataById(kszPabBookGroupId));

    if (pMenu == NULL)
    {
        DIR_ERR(" CGenBandContactManager::ReqGroupList() group not found!");
        return false;
    }

    if (m_eStatus == FILE_DOWNLOADING)
    {
        return true;
    }

    // NA组始终存在，不管下载组成功还是失败，
    ContactGBMenu * pNaGroup = new ContactGBMenu();
    if (pNaGroup != NULL)
    {
        pNaGroup->m_strDisplayName = TRID_NA;

        InsertMap(pNaGroup, kszGbNAGroupId);

        pNaGroup->m_nParentId = kszPabBookGroupId;

        AddIdToGroup(kszGbNAGroupId, kszPabBookGroupId);
    }


    //Save Request File
    xml_document doc;

    xml_node BodyNode = AppendBodyNode(doc);

    BodyNode.append_child("add:getAddressBookGroups");

    doc.save_file(GetNewFile().c_str());

    if (MyExecPost(kszPabBookGroupId, pMenu->m_strPabGroupUrl, PAB_OPERATION_GET_GROUPLIST))
    {
        m_eStatus = FILE_DOWNLOADING;
        return true;
    }

    return false;
}

/*<soapenv:Envelope xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:add="addrbook.ws.nortelnetworks.com">
<soapenv:Header/>
<soapenv:Body>
<add:addAddressBookGroup soapenv:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
<in0 xsi:type="add:AddrBookGroupDO">
<name xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">Group2</name>
</in0>
</add:addAddressBookGroup>
</soapenv:Body>
</soapenv:Envelope>*/
DIRECTORY_RET_CODE CGenBandContactManager::ReqAddGroup(const yl::string & strName)
{
    if (!IsNetworkConnected())
    {
        return DIR_RET_RERROR_NETWORK_ERROR;
    }

    ContactGBMenu * pMenu = Base2GabMenu(GetMenuDataById(kszPabBookGroupId));

    if (pMenu == NULL)
    {
        return DIR_RET_FAIL;
    }

    if (pMenu->m_eStatus != FILE_DOWNLOADED_OK)
    {
        return DIR_RET_FAIL;
    }

    //Save Request File
    xml_document doc;

    xml_node BodyNode = AppendBodyNode(doc);

    xml_node optNode = BodyNode.append_child("add:addAddressBookGroup");

    xml_node in0Node = optNode.append_child("in0");

    in0Node.append_attribute(kszXsiType, "add:AddrBookGroupDO");

    AppendStringTypeNode(in0Node, "name", strName.c_str());

    doc.save_file(GetNewFile().c_str());


    ContactGBMenu * pNew = new ContactGBMenu();

    pNew->m_strDisplayName = strName;

    if (MyExecPost(kszPabBookGroupId, pMenu->m_strPabGroupUrl, PAB_OPERATION_ADD_GROUP, pNew))
    {
        pNew->m_nParentId = kszPabBookGroupId;
        pNew->m_nId = GetId();
        return DIR_RET_SUCC;
    }
    else
    {
        delete pNew;
    }

    return DIR_RET_FAIL;
}

/*<soapenv:Envelope xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:add="addrbook.ws.nortelnetworks.com">
<soapenv:Header/>
<soapenv:Body>
<add:removeAddressBookGroup soapenv:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
<in0 xsi:type="add:AddrBookGroupNaturalKeyDO" xmlns:add="addrbook.data.ws.nortelnetworks.com">
<name xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</name>
</in0>
</add:removeAddressBookGroup>
</soapenv:Body>
</soapenv:Envelope>*/
DIRECTORY_RET_CODE CGenBandContactManager::ReqRemoveGroup(int nGroupId)
{
    if (!IsNetworkConnected())
    {
        return DIR_RET_RERROR_NETWORK_ERROR;
    }

    ContactGBMenu * pPab = Base2GabMenu(GetMenuDataById(kszPabBookGroupId));

    DirGroupRecord * pMenu = GetMenuDataById(nGroupId);

    if (pMenu == NULL || pPab == NULL)
    {
        return DIR_RET_FAIL;
    }

    //Save Request File
    xml_document doc;

    xml_node BodyNode = AppendBodyNode(doc);

    xml_node optNode = BodyNode.append_child("add:removeAddressBookGroup");

    xml_node in0Node = optNode.append_child("in0");

    in0Node.append_attribute(kszXsiType, "add:AddrBookGroupNaturalKeyDO");

    AppendStringTypeNode(in0Node, "name", pMenu->m_strDisplayName.c_str());

    doc.save_file(GetNewFile().c_str());

    if (MyExecPost(nGroupId, pPab->m_strPabGroupUrl, PAB_OPERATION_REMOVE_GROUP))
    {
        return DIR_RET_SUCC;
    }

    return DIR_RET_FAIL;
}

/*<soapenv:Envelope xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:add="addrbook.ws.nortelnetworks.com">
<soapenv:Header/>
<soapenv:Body>
<add:modifyAddressBookGroup soapenv:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
<in0 xsi:type="add:AddrBookGroupNaturalKeyDO" xmlns:add="addrbook.data.ws.nortelnetworks.com">
<name xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</name>
</in0>
<in1 xsi:type="add:AddrBookGroupDO">
<name xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</name>
</in1>
</add:modifyAddressBookGroup>
</soapenv:Body>
</soapenv:Envelope>*/
DIRECTORY_RET_CODE CGenBandContactManager::ReqModifyGroup(int nGroupId, const yl::string & strName)
{
    if (!IsNetworkConnected())
    {
        return DIR_RET_RERROR_NETWORK_ERROR;
    }

    ContactGBMenu * pPab = Base2GabMenu(GetMenuDataById(kszPabBookGroupId));

    DirGroupRecord * pMenu = GetMenuDataById(nGroupId);

    if (pMenu == NULL || pPab == NULL)
    {
        return DIR_RET_FAIL;
    }

    //Save Request File
    xml_document doc;

    xml_node BodyNode = AppendBodyNode(doc);

    xml_node optNode = BodyNode.append_child("add:modifyAddressBookGroup");

    xml_node in0Node = optNode.append_child("in0");

    in0Node.append_attribute(kszXsiType, "add:AddrBookGroupNaturalKeyDO");

    AppendStringTypeNode(in0Node, "name", pMenu->m_strDisplayName.c_str());

    xml_node in1Node = optNode.append_child("in1");

    in1Node.append_attribute(kszXsiType, "add:AddrBookGroupDO");

    AppendStringTypeNode(in1Node, "name", strName.c_str());

    doc.save_file(GetNewFile().c_str());

    ContactGBMenu * pNew = new ContactGBMenu();

    pNew->m_strDisplayName = strName;

    if (MyExecPost(nGroupId, pPab->m_strPabGroupUrl, PAB_OPERATION_MODIFY_GROUP, pNew))
    {
        return DIR_RET_SUCC;
    }
    else
    {
        delete pNew;
    }

    return DIR_RET_FAIL;
}
/*
<soapenv:Envelope xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:add="addrbook.ws.nortelnetworks.com">
<soapenv:Header/>
<soapenv:Body>
<add:addAddressBookEntry soapenv:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
<in0 xsi:type="add:AddressBookEntryDO">
<name xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</name>
<businessPhoneNumber xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</businessPhoneNumber>
<conferenceURL xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</conferenceURL>
<emailAddress xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</emailAddress>
<fax xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</fax>
<firstName xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</firstName>
<group xsi:type="add1:AddrBookGroupNaturalKeyDO" xmlns:add1="addrbook.data.ws.nortelnetworks.com">
<name xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</name>
</group>
<homePhoneNumber xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</homePhoneNumber>
<lastName xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</lastName>
<mobile xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</mobile>
<pager xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</pager>
<photoURL xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</photoURL>
<primaryContact xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</primaryContact>
</in0>
</add:addAddressBookEntry>
</soapenv:Body>
</soapenv:Envelope>*/
DIRECTORY_RET_CODE CGenBandContactManager::ReqAddContact(ContactGBBookUnit & refData)
{
    if (!IsNetworkConnected())
    {
        return DIR_RET_RERROR_NETWORK_ERROR;
    }
    ContactGBMenu * pPab = Base2GabMenu(GetMenuDataById(kszPabBookGroupId));

    if (pPab == NULL)
    {
        return DIR_RET_FAIL;
    }
    //Save Request File
    xml_document doc;

    xml_node BodyNode = AppendBodyNode(doc);
    xml_node optNode = BodyNode.append_child("add:addAddressBookEntry");

    xml_node in0Node = optNode.append_child("in0");

    in0Node.append_attribute(kszXsiType, "add:AddressBookEntryDO");

    if (!refData.m_strDisplayName.empty())
    {
        AppendStringTypeNode(in0Node, "name", refData.m_strDisplayName);
    }
    if (!refData.m_strEmail.empty())
    {
        AppendStringTypeNode(in0Node, "emailAddress", refData.m_strEmail);
    }

    if (!refData.m_strFirstName.empty())
    {
        AppendStringTypeNode(in0Node, "firstName", refData.m_strFirstName.c_str());
    }

    if (!refData.m_strLastName.empty())
    {
        AppendStringTypeNode(in0Node, "lastName", refData.m_strLastName.c_str());
    }

    if (refData.m_nParentId != kszAllContactId
            && refData.m_nParentId != kszGbNAGroupId)
    {
        DirGroupRecord * pGroup = GetMenuDataById(refData.m_nParentId);
        if (pGroup != NULL)
        {
            AppendGroupNode(in0Node, pGroup->m_strDisplayName);
        }
    }

    for (ListNumber::iterator iter = refData.m_listNumber.begin(); iter != refData.m_listNumber.end();
            iter++)
    {
        stNumber & stNub = *iter;
        AppendStringTypeNode(in0Node, stNub.strName, stNub.strValue);
    }

    doc.save_file(GetNewFile().c_str());

    ContactGBBookUnit * pNew = new ContactGBBookUnit();

    *pNew = refData;

    if (MyExecPost(kszPabBookGroupId, pPab->m_strUrl, PAB_OPERATION_ADD_CONTACT, pNew))
    {
        return DIR_RET_SUCC;
    }
    else
    {
        delete pNew;
    }

    return DIR_RET_FAIL;
}

/*<soapenv:Envelope xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:add="addrbook.ws.nortelnetworks.com">
<soapenv:Header/>
<soapenv:Body>
<add:removeAddressBookEntry soapenv:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
<in0 xsi:type="add:AddrBookEntryNaturalKeyDO" xmlns:add="addrbook.data.ws.nortelnetworks.com">
<name xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</name>
</in0>
</add:removeAddressBookEntry>
</soapenv:Body>
</soapenv:Envelope>*/
DIRECTORY_RET_CODE CGenBandContactManager::ReqRemoveContact(int nId)
{
    ContactGBMenu * pPab = Base2GabMenu(GetMenuDataById(kszPabBookGroupId));

    BaseDirDataRecord * pContact = GetDataById(nId);

    if (pContact == NULL || pPab == NULL)
    {
        return DIR_RET_FAIL;
    }

    //Save Request File
    xml_document doc;

    xml_node BodyNode = AppendBodyNode(doc);

    xml_node optNode = BodyNode.append_child("add:removeAddressBookEntry");

    xml_node in0Node = optNode.append_child("in0");

    in0Node.append_attribute(kszXsiType, "add:AddrBookGroupNaturalKeyDO");

    AppendStringTypeNode(in0Node, "name", pContact->m_strDisplayName.c_str());

    doc.save_file(GetNewFile().c_str());

    if (MyExecPost(pContact->m_nId, pPab->m_strUrl, PAB_OPERATION_REMOVE_CONTACT))
    {
        return DIR_RET_SUCC;
    }

    return DIR_RET_FAIL;
}

/*<soapenv:Envelope xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:add="addrbook.ws.nortelnetworks.com">
<soapenv:Header/>
<soapenv:Body>
<add:modifyAddressBookEntry soapenv:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
<in0 xsi:type="add:AddrBookEntryNaturalKeyDO" xmlns:add="addrbook.data.ws.nortelnetworks.com">
<name xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</name>
</in0>
<in1 xsi:type="add:AddressBookEntryDO">
<name xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</name>
<businessPhoneNumber xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</businessPhoneNumber>
<conferenceURL xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</conferenceURL>
<emailAddress xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</emailAddress>
<fax xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</fax>
<firstName xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</firstName>
<group xsi:type="add1:AddrBookGroupNaturalKeyDO" xmlns:add1="addrbook.data.ws.nortelnetworks.com">
<name xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</name>
</group>
<homePhoneNumber xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</homePhoneNumber>
<lastName xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</lastName>
<mobile xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</mobile>
<pager xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</pager>
<photoURL xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</photoURL>
<primaryContact xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</primaryContact>
</in1>
</add:modifyAddressBookEntry>
</soapenv:Body>
</soapenv:Envelope>*/
DIRECTORY_RET_CODE CGenBandContactManager::ReqModifyContact(int nId, ContactGBBookUnit & refData)
{
    ContactGBMenu * pPab = Base2GabMenu(GetMenuDataById(kszPabBookGroupId));

    ContactGBBookUnit * pContact = Base2GbUnit(GetDataById(nId));

    if (pContact == NULL || pPab == NULL)
    {
        return DIR_RET_FAIL;
    }

    //Save Request File
    xml_document doc;

    xml_node BodyNode = AppendBodyNode(doc);

    xml_node optNode = BodyNode.append_child("add:modifyAddressBookEntry");

    xml_node in0Node = optNode.append_child("in0");

    in0Node.append_attribute(kszXsiType, "add:AddressBookEntryDO");

    AppendStringTypeNode(in0Node, "name", pContact->m_strDisplayName.c_str());

    xml_node in1Node = optNode.append_child("in1");

    in1Node.append_attribute(kszXsiType, "add:AddressBookEntryDO");

    AppendStringTypeNode(in1Node, "name", refData.m_strDisplayName.c_str());

    AppendStringTypeNode(in1Node, "firstName", refData.m_strFirstName.c_str());

    AppendStringTypeNode(in1Node, "lastName", refData.m_strLastName.c_str());

    AppendStringTypeNode(in1Node, "emailAddress", refData.m_strEmail);

    if (refData.m_nParentId != pContact->m_nParentId)
    {
        if (refData.m_nParentId == kszAllContactId
                || refData.m_nParentId == kszGbNAGroupId)
        {
            AppendGroupNode(in1Node, "");
        }
        else
        {
            DirGroupRecord * pGroup = GetMenuDataById(refData.m_nParentId);
            if (pGroup != NULL)
            {
                AppendGroupNode(in1Node, pGroup->m_strDisplayName);
            }
        }
    }

    for (ListNumber::iterator iter = refData.m_listNumber.begin(); iter != refData.m_listNumber.end();
            iter++)
    {
        stNumber & stNub = *iter;
        AppendStringTypeNode(in1Node, stNub.strName, stNub.strValue);
    }

    doc.save_file(GetNewFile().c_str());

    ContactGBBookUnit * pNew = new ContactGBBookUnit();

    *pNew = refData;

    pNew->m_nId = pContact->m_nId;

    if (MyExecPost(nId, pPab->m_strUrl, PAB_OPERATION_MODIFY_CONTACT, pNew))
    {
        return DIR_RET_SUCC;
    }
    else
    {
        delete pNew;
    }

    return DIR_RET_FAIL;
}


bool CGenBandContactManager::MyExecPost(int nId, const yl::string & strUrl,
                                        GBBOOK_OPERATION_TYPE eType, BaseDirDataRecord * pData)
{
    DIR_INFO("CGenBandContactManager::MyExecPost url = %s,eType[%d]", strUrl.c_str(), eType);

    CGenbandBookAction * pAction = new CGenbandBookAction(m_strReqestFile.c_str(), strUrl.c_str(),
            eType);

    if (pAction != NULL)
    {
        pAction->SetRead(false);  // post文件.
        pAction->SetSync(false);  // 异步.
        pAction->SetCallback(this);  // 回调函数.
        pAction->SetUsername(m_strUser);
        pAction->SetPassword(m_strPassword);
        pAction->SetData(pData);
        pAction->SetExtraParam(nId);
        if (nId == kszGabBookGroupId)
        {
            pAction->SetGabKey(m_strGabSearchkey);
        }
        pAction->SetDownLoadMode(MODE_BASIC_AUTH);
        yl::string strHead;
        strHead = commonAPI_FormatString("application/xml;charset=%s\r\n%s", pAction->m_strCharset.c_str(),
                                         "SOAPAction: NA");
        pAction->SetHeader(strHead);

        if (eType >= PAB_OPERATION_ADD_GROUP && eType <= PAB_OPERATION_MODIFY_CONTACT)
        {
            m_bLocalChangeDir = true;
            timerSetThreadTimer((UINT)&m_bLocalChangeDir, 15 * 1000);
        }

        TaskAction_ExecAction(pAction);
        return true;
    }
    return false;
}


/*<soapenv:Envelope xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:gab="gab.ws.nortelnetworks.com">
<soapenv:Header/>
<soapenv:Body>
<gab:searchGABByLastNameRange soapenv:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
<in0 xsi:type="gab:SearchCriteriaDO">
<searchCriteria xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</searchCriteria>
</in0>
<in1 xsi:type="com:StartStopDO" xmlns:com="common.ws.nortelnetworks.com">
<start xsi:type="xsd:int">?</start>
<stop xsi:type="xsd:int">?</stop>
</in1>
</gab:searchGABByLastNameRange>
</soapenv:Body>
</soapenv:Envelope>*/
void CGenBandContactManager::SaveGabSearchFile(const yl::string & strkey,
        GBBOOK_OPERATION_TYPE eType)
{
    xml_document doc;

    xml_node BodyNode = AppendBodyNode(doc, true);

    yl::string strName;

    switch (eType)
    {
    case GAB_SEARCH_NAME:

        strName = "gab:searchGABByNameRange";
        break;
    case GAB_SEARCH_LASTNAME:

        strName = "gab:searchGABByLastNameRange";
        break;
    case GAB_SEARCH_FIRSTNAME:

        strName = "gab:searchGABByFirstNameRange";
        break;
    default:
        strName = "gab:searchGABByPhoneNumberRange";
        break;
    }

    xml_node searchNode = BodyNode.append_child(strName.c_str());

    AppendSearchContentNode(searchNode, strkey);

    doc.save_file(GetNewFile().c_str());
}

void CGenBandContactManager::SavePabFileByType(GBBOOK_OPERATION_TYPE eType)
{
    xml_document doc;

    xml_node BodyNode = AppendBodyNode(doc);

    yl::string strName;

    switch (eType)
    {
    case PAB_OPERATION_GET_CONTACTLIST:

        strName = "add:getAddressBook";
        break;
    case PAB_OPERATION_GET_GROUPLIST:

        strName = "add:getAddressBookGroups";
        break;
    case GAB_SEARCH_FIRSTNAME:

        strName = "gab:searchGABByFirstNameRange";
        break;
    default:
        break;
    }

    xml_node searchNode = BodyNode.append_child(strName.c_str());

    doc.save_file(GetNewFile().c_str());
}


void CGenBandContactManager::OnTaskActionCallback(CTaskAction * pTaskAction)
{
    CBaseContactManger::OnTaskActionCallback(pTaskAction);

    DIR_INFO("CGenBandContactManager::OnTaskActionCallback begin");
    if (pTaskAction == NULL)
    {
        return;
    }

    if (pTaskAction->GetReqType() != NS_TA::TA_REQUEST_NETWORK_FILE)
    {
        return;
    }
    // 返回结果的类型为请求网络文件.
    CGenbandBookAction * pGenbandBookAction =
        dynamic_cast<CGenbandBookAction *>(pTaskAction);
    if (pGenbandBookAction == NULL)
    {
        return;
    }

    GBBOOK_OPERATION_TYPE eType = pGenbandBookAction->GetOptType();

    int nId = pGenbandBookAction->GetExtraParam();

    bool suc = pGenbandBookAction->IsOperationSucceed();

    DIR_INFO("CGenBandContactManager::OnTaskActionCallback eType[%d] suc[%d]",
             eType, suc);

    if (!suc)
    {
        DIR_INFO("CGenBandContactManager::OnTaskActionCallback Error[%d]",
                 pGenbandBookAction->GetTaskActionErrorType());

        etl_NotifyApp(false, GENBAND_BOOK_REQ_RESULT, eType, suc);

        if (eType == PAB_OPERATION_GET_CONTACTLIST
                || nId == kszGabBookGroupId)
        {
            DirGroupRecord * pGroup = GetMenuDataById(pGenbandBookAction->GetExtraParam());
            if (pGroup != NULL)
            {
                m_eStatus = FILE_DOWNLOAD_FAIL;
                pGroup->m_eStatus = FILE_DOWNLOAD_FAIL;
            }
        }

        if (eType == PAB_OPERATION_GET_GROUPLIST
                || eType == PAB_OPERATION_GET_CONTACTLIST)
        {
            m_eStatus = FILE_DOWNLOAD_FAIL;
        }

        remove(pGenbandBookAction->GetFilePath().c_str());
        return;
    }

    switch (eType)
    {
    case PAB_OPERATION_GET_GROUPLIST:
        {
            suc = PrasePabGroupList(pGenbandBookAction->GetFilePath());

            ReqGetContactList();
        }
        break;

    case PAB_OPERATION_GET_CONTACTLIST:
        {
            DirGroupRecord * pGroup = GetMenuDataById(pGenbandBookAction->GetExtraParam());
            if (pGroup != NULL)
            {
                suc = PrasePabContactList(pGenbandBookAction->GetFilePath());

                pGroup->m_eStatus = suc ? FILE_DOWNLOADED_OK : FILE_DOWNLOAD_FAIL;

                m_eStatus = suc ? FILE_DOWNLOADED_OK : FILE_DOWNLOAD_FAIL;
            }
            else
            {
                suc = false;
            }
        }
        break;
    case GAB_SEARCH_NAME:
    case GAB_SEARCH_FIRSTNAME:
    case GAB_SEARCH_LASTNAME:
    case GAB_SEARCH_NUMBER:
        {
            DirGroupRecord * pGroup = GetMenuDataById(pGenbandBookAction->GetExtraParam());

            if (pGroup == NULL
                    || pGroup->m_eStatus == FILE_DOWNLOAD_CANCELED
                    || m_strGabSearchkey.compare(pGenbandBookAction->GetGabKey()) != 0
                    || m_OptType != eType)
            {
                remove(pGenbandBookAction->GetFilePath().c_str());

                DIR_INFO("Not now Key(%s),return key(%s)", m_strGabSearchkey.c_str(),
                         pGenbandBookAction->GetGabKey().c_str());
                return;
            }
            suc = PraseSearchResult(pGenbandBookAction->GetFilePath());

            pGroup->m_eStatus = suc ? FILE_DOWNLOADED_OK : FILE_DOWNLOAD_FAIL;
        }
        break;

    case PAB_OPERATION_ADD_CONTACT:
        {
            ContactGBBookUnit * pAdd = Base2GbUnit(pGenbandBookAction->GetData());

            pGenbandBookAction->SetData(NULL);

            AddContact(pAdd, pAdd->m_nParentId);
        }
        break;
    case PAB_OPERATION_ADD_GROUP:
        {
            ContactGBMenu * pAdd = Base2GabMenu(pGenbandBookAction->GetData());
            pGenbandBookAction->SetData(NULL);
            int nNewId = InsertMap(pAdd);
            AddIdToGroup(nNewId, kszPabBookGroupId);
        }
        break;

    case PAB_OPERATION_REMOVE_CONTACT:
    case PAB_OPERATION_REMOVE_GROUP:
        {
            if (PAB_OPERATION_REMOVE_GROUP == eType)
            {
                ContactGBMenu * pRemoveGroup = Base2GabMenu(GetMenuDataById(nId));
                if (pRemoveGroup != NULL)
                {
                    int nSize = pRemoveGroup->m_vChildrenId.size();
                    for (int i = 0; i < nSize; i++)
                    {
                        ContactGBBookUnit * pContact = Base2GbUnit(GetDataById(pRemoveGroup->m_vChildrenId[i]));
                        if (pContact == NULL)
                        {
                            continue;
                        }
                        pContact->m_nParentId = kszGbNAGroupId;
                        AddIdToGroup(pRemoveGroup->m_vChildrenId[i], kszGbNAGroupId);
                    }
                }
            }

            VectorId vecId;

            vecId.push_back(nId);

            RemoveRecord(vecId, GetParentIdById(nId));
            RemoveIdFromGroup(vecId, kszSearchGroupId);

        }
        break;

    case PAB_OPERATION_MODIFY_GROUP:
        {
            ContactGBMenu * pData = Base2GabMenu(pGenbandBookAction->GetData());

            if (pData == NULL)
            {
                suc = false;
                break;
            }

            DirGroupRecord * pGroup = GetMenuDataById(nId);

            if (pGroup != NULL)
            {
                pGroup->m_strDisplayName = pData->m_strDisplayName;
            }
            else
            {
                suc = false;
            }
        }
        break;
    case PAB_OPERATION_MODIFY_CONTACT:
        {
            ContactGBBookUnit * pData = Base2GbUnit(pGenbandBookAction->GetData());

            if (pData == NULL)
            {
                suc = false;
                break;
            }

            ContactGBBookUnit * pContact = Base2GbUnit(GetDataById(nId));

            if (pContact != NULL)
            {
                if (pData->m_nParentId != pContact->m_nParentId)
                {
                    if (pContact->m_nParentId != kszAllContactId)
                    {
                        VectorId revId;
                        revId.push_back(nId);
                        RemoveIdFromGroup(revId, pContact->m_nParentId);
                    }

                    if (pData->m_nParentId != kszAllContactId)
                    {
                        AddIdToGroup(nId, pData->m_nParentId);
                    }
                }

                //在同一个组里面 联系人名字改变需要从新排序
                bool bSort = pData->m_nParentId == pContact->m_nParentId;


                if (pData->m_strDisplayName.compare(pContact->m_strDisplayName) != 0)
                {
                    pData->m_strNameIndex = commonAPI_transT9String(pData->m_strDisplayName);
                }
                *pContact = *pData;

                if (bSort)
                {
                    // 在同一个组里面 联系人名字改变需要从新排序
                    MySort(pData->m_nParentId);

                    if (pData->m_nParentId != kszAllContactId)
                    {
                        MySort(kszAllContactId);
                    }
                }
            }
            else
            {
                suc = false;
            }
        }
        break;
    }

    DIR_INFO("Send Msg eType [%d], suc [%d]", eType, suc);
    etl_NotifyApp(false, GENBAND_BOOK_REQ_RESULT, eType, suc);

    remove(pGenbandBookAction->GetFilePath().c_str());
}


bool CGenBandContactManager::PraseSearchResult(const yl::string & strFile)
{
    xml_document doc;

    if (!doc.load_file(strFile.c_str()))
    {
        DIR_INFO("CGenBandContactManager::PraseSearchResult Fail[%s]!",
                 strFile.c_str());
        return false;
    }

    xml_node BodyNode = doc.child("soapenv:Envelope").child("soapenv:Body");

    if (BodyNode.empty())
    {
        return false;
    }

    for (xml_node multiRef = BodyNode.child("multiRef"); !multiRef.empty();
            multiRef = multiRef.next_sibling("multiRef"))
    {
        ContactGBBookUnit * pNew = new ContactGBBookUnit();

        if (pNew->ParseFromXMLNode(multiRef))
        {
            pNew->m_nParentId = kszGabBookGroupId;
            int nId = InsertMap(pNew);

            AddIdToGroup(nId, kszGabBookGroupId);
        }
        else
        {
            delete pNew;
        }
    }

    return true;
}


/*<?xml version="1.0" encoding="UTF-8"?>
<soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
<soapenv:Body>
<ns1:getAddressBookGroupsResponse soapenv:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:ns1="addrbook.ws.nortelnetworks.com">
<getAddressBookGroupsReturn soapenc:arrayType="ns1:AddrBookGroupDO[2]" xsi:type="soapenc:Array" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">
<getAddressBookGroupsReturn href="#id0"/>
<getAddressBookGroupsReturn href="#id1"/></getAddressBookGroupsReturn>
</ns1:getAddressBookGroupsResponse>
<multiRef id="id0" soapenc:root="0" soapenv:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xsi:type="ns2:AddrBookGroupDO" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/" xmlns:ns2="addrbook.ws.nortelnetworks.com">
<name xsi:type="xsd:string">Testxx2</name>
</multiRef>
<multiRef id="id1" soapenc:root="0" soapenv:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xsi:type="ns3:AddrBookGroupDO" xmlns:ns3="addrbook.ws.nortelnetworks.com" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">
<name xsi:type="xsd:string">xxxxxnm</name>
</multiRef>
</soapenv:Body>
</soapenv:Envelope>*/
bool CGenBandContactManager::PrasePabGroupList(const yl::string & strFile)
{
    xml_document doc;

    if (!doc.load_file(strFile.c_str()))
    {
        DIR_INFO("CGenBandContactManager::PrasePabGroupList Fail[%s]!",
                 strFile.c_str());
        return false;
    }

    xml_node BodyNode = doc.child("soapenv:Envelope").child("soapenv:Body");

    if (BodyNode.empty())
    {
        DIR_INFO("CGenBandContactManager::PrasePabGroupList Body Empty");
        return false;
    }

    m_mapGroupId.clear();

//  ContactGBMenu* pNaGroup = new ContactGBMenu();
//  if (pNaGroup != NULL)
//  {
//      pNaGroup->m_strDisplayName = TRID_NA;
//
//      InsertMap(pNaGroup,kszGbNAGroupId);
//
//      pNaGroup->m_nParentId = kszPabBookGroupId;
//
//      AddIdToGroup(kszGbNAGroupId, kszPabBookGroupId);
//  }

    for (xml_node multiRef = BodyNode.child("multiRef"); !multiRef.empty();
            multiRef = multiRef.next_sibling("multiRef"))
    {
        xml_node NameNode = multiRef.child("name");

        if (!NameNode.empty())
        {
            ContactGBMenu * pNew = new ContactGBMenu();

            if (pNew == NULL)
            {
                continue;
            }

            pNew->m_strDisplayName = NameNode.child_value();
            pNew->m_nParentId = kszPabBookGroupId;

            int nId = InsertMap(pNew);

            AddIdToGroup(nId, kszPabBookGroupId);

            m_mapGroupId.put(pNew->m_strDisplayName, nId);
        }
    }

    DIR_INFO("CGenBandContactManager::PrasePabGroupList size %d", m_mapGroupId.size());

    return true;
}

bool CGenBandContactManager::PrasePabContactList(const yl::string & strFile)
{
    xml_document doc;

    if (!doc.load_file(strFile.c_str()))
    {
        DIR_INFO("CGenBandContactManager::PrasePabContactList Fail[%s]!",
                 strFile.c_str());
        return false;
    }

    xml_node BodyNode = doc.child("soapenv:Envelope").child("soapenv:Body");

    if (BodyNode.empty())
    {
        DIR_INFO("CGenBandContactManager::PrasePabContactList BodyNode empty!");
        return false;
    }

    for (xml_node multiRef = BodyNode.child("multiRef"); !multiRef.empty();
            multiRef = multiRef.next_sibling("multiRef"))
    {
        xml_attribute xsiType = multiRef.attribute(kszXsiType);

        yl::string strXsiType = xsiType.as_string();

        //联系人
        if (strXsiType.find("AddressBookEntryDO") != yl::string::npos)
        {
            ContactGBBookUnit * pNew = new ContactGBBookUnit();

            if (pNew->ParseFromXMLNode(multiRef))
            {
                int nId = InsertMap(pNew);

                AddIdToGroup(nId, kszAllContactId);
            }
            else
            {
                delete pNew;
            }
        }
        else
        {
            xml_node NameNode = multiRef.child("name");

            if (!NameNode.empty())
            {
                yl::string strName = NameNode.child_value();

                xml_attribute idAttr = multiRef.attribute("id");

                if (idAttr.empty())
                {
                    continue;
                }

                yl::string strId = idAttr.as_string();

                IdChangeMap::iterator iter = m_mapGroupId.find(strName);

                if (iter != m_mapGroupId.end())
                {
                    int nId = iter->second;

                    m_mapGroupId.put(strId, nId);
                }
            }
        }
    }



    DirGroupRecord * pAllGroup = GetMenuDataById(kszAllContactId);

    if (pAllGroup != NULL)
    {
        DIR_INFO("CGenBandContactManager::PrasePabContactList %d ", pAllGroup->m_vChildrenId.size());
        for (int i = 0; i < pAllGroup->m_vChildrenId.size(); i++)
        {
            ContactGBBookUnit * pContact = Base2GbUnit(GetDataById(pAllGroup->m_vChildrenId[i]));

            if (pContact == NULL)
            {
                continue;
            }


            IdChangeMap::iterator iter = m_mapGroupId.find(pContact->m_strGroupFlag);

            if (iter != m_mapGroupId.end())
            {
                int nId = iter->second;

                pContact->m_nParentId = nId;

                AddIdToGroup(pContact->m_nId, nId);
            }
            else
            {
                pContact->m_nParentId = kszGbNAGroupId;
                AddIdToGroup(pContact->m_nId, kszGbNAGroupId);
            }
        }
    }

    m_mapGroupId.clear();

    return true;
}

const yl::string & CGenBandContactManager::GetNewFile()
{
    m_strReqestFile = g_strTempPath;

    static int nFileIndex = 0;

    if (++nFileIndex == 4)
    {
        nFileIndex = 0;
    }
    //Refresh需同时下载多个，一个文件会出现覆盖写的问题。
    char szTemp[100];
    memset(szTemp, 0, 100);
    sprintf(szTemp, kszGbReqestFile, nFileIndex);
    m_strReqestFile.append(szTemp);

    return m_strReqestFile;
}

// 联系人排序
void CGenBandContactManager::MySort(int nGroupId)
{
    DirGroupRecord * pGroup = GetMenuDataById(nGroupId);

    if (pGroup != NULL)
    {
        yl::stable_sort(pGroup->m_vChildrenId.begin(), pGroup->m_vChildrenId.end(), CompareGenband);
    }
}

//<soapenv:Envelope xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:add="addrbook.ws.nortelnetworks.com">
xml_node CGenBandContactManager::AppendBodyNode(xml_document & doc, bool bIsGab)
{

    xml_node root = doc.append_child("soapenv:Envelope");

    root.append_attribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
    root.append_attribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
    root.append_attribute("xmlns:soapenv", "http://schemas.xmlsoap.org/soap/envelope/");

    //xmlns:gab="gab.ws.nortelnetworks.com"
    if (bIsGab)
    {
        root.append_attribute("xmlns:gab", "gab.ws.nortelnetworks.com");
    }
    else
    {
        root.append_attribute("xmlns:add", "addrbook.ws.nortelnetworks.com");
    }


    root.append_child("soapenv:Header");

    return root.append_child("soapenv:Body");
}

/*<in0 xsi:type="gab:SearchCriteriaDO">
<searchCriteria xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</searchCriteria>
</in0>
<in1 xsi:type="com:StartStopDO" xmlns:com="common.ws.nortelnetworks.com">
<start xsi:type="xsd:int">?</start>
<stop xsi:type="xsd:int">?</stop>
</in1>*/

void CGenBandContactManager::AppendSearchContentNode(xml_node & node, const yl::string & strKey)
{
    xml_node in0Node = node.append_child("in0");

    in0Node.append_attribute(kszXsiType, "gab:SearchCriteriaDO");

    AppendStringTypeNode(in0Node, "searchCriteria", strKey);

    xml_node in1Node = node.append_child("in1");

    in1Node.append_attribute(kszXsiType, "com:StartStopDO");

    in1Node.append_attribute("xmlns:com", "common.ws.nortelnetworks.com");

    xml_node startNode = in1Node.append_child("start");
    startNode.append_attribute(kszXsiType, "xsd:int");
    startNode.text().set("0");
    xml_node stopNode = in1Node.append_child("stop");
    stopNode.append_attribute(kszXsiType, "xsd:int");
    stopNode.text().set("1000");
}

//举例<searchCriteria xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</searchCriteria>
void CGenBandContactManager::AppendStringTypeNode(xml_node & node, const yl::string & strName,
        const yl::string & strValue)
{
    xml_node nodeChild = node.append_child(strName.c_str());

    nodeChild.append_attribute(kszXsiType, "soapenc:string");
    nodeChild.append_attribute("xmlns:soapenc", "http://schemas.xmlsoap.org/soap/encoding/");

    nodeChild.text().set(strValue.c_str());
}


/*<group xsi:type="add1:AddrBookGroupNaturalKeyDO" xmlns:add1="addrbook.data.ws.nortelnetworks.com">
<name xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">?</name>
</group>*/
void CGenBandContactManager::AppendGroupNode(xml_node & node, const yl::string strGroupName)
{
    xml_node groupNode = node.append_child("group");

    groupNode.append_attribute(kszXsiType, "add1:AddrBookGroupNaturalKeyDO");
    groupNode.append_attribute("xmlns:add1", "addrbook.data.ws.nortelnetworks.com");

    AppendStringTypeNode(groupNode, "name", strGroupName.c_str());
}

int CGenBandContactManager::IsContactNameExist(const yl::string & strName)
{
    DirGroupRecord * pGroup = GetMenuDataById(kszAllContactId);
    if (pGroup == NULL)
    {
        return -1;
    }

    int j = 0;
    for (; j < pGroup->m_vChildrenId.size(); j++)
    {
        BaseDirDataRecord * pBase = GetDataById(pGroup->m_vChildrenId[j]);

        if (pBase == NULL)
        {
            continue;
        }

        if (strcasecmp(pBase->m_strDisplayName.c_str(), strName.c_str()) == 0)
        {
            return pBase->m_nId;
        }
    }

    return -1;
}

int CGenBandContactManager::IsGroupNameExist(const yl::string & strName)
{
    DirGroupRecord * pGroup = GetMenuDataById(kszPabBookGroupId);
    if (pGroup == NULL)
    {
        return -1;
    }

    int j = 0;
    for (; j < pGroup->m_vChildrenId.size(); j++)
    {
        BaseDirDataRecord * pBase = GetDataById(pGroup->m_vChildrenId[j]);

        if (pBase == NULL)
        {
            continue;
        }

        if (strcasecmp(pBase->m_strDisplayName.c_str(), strName.c_str()) == 0)
        {
            return pBase->m_nId;
        }
    }

    return -1;
}

#endif
