#include "settinguibaseitemdata.h"

#include "datafactory.h"
#include "configfiltermanager.h"
#include "itemtypemanager.h"
#include "settinguierrorinfo.h"

#include "dsklog/log.h"

CSettingUIItemDataBase::CSettingUIItemDataBase(int eItemType /*= SETTING_ITEM_TYPE_BASE*/)
    : m_eWgtType(-1)
    , m_eItemType(eItemType)
    , m_eValueType(-1)
    , m_bResetable(true)
    , m_bEnable(true)
    , m_bShow(true)
    , m_bSecondLevel(false)
    , m_bConfigChangedReboot(false)
    , m_bShowIndex(false)
    , m_bShowColon(true)
    , m_strID("")
    , m_strDisplayName("")
    , m_strConfigID("")
    , m_strAuthorityID("")
    , m_strValue("")
    , m_pExtraData(NULL)
    , m_strExtraData("")
    , m_eExtraDataType(-1)
    , m_nExtraData(-1)
    , m_listItemMsg()
    , m_pSoftkeyData(NULL)
    , m_pErrorInfo(NULL)
{
}

CSettingUIItemDataBase::~CSettingUIItemDataBase()
{
    Reset();
}

int CSettingUIItemDataBase::GetBaseDataItemType()
{
    return SETTING_ITEM_TYPE_BASE;
}

CSettingUIItemDataBase * CSettingUIItemDataBase::GetBaseDataItem(CSettingUIItemDataBase * pDataItem)
{
    if (NULL == pDataItem
            || CSettingUIItemDataBase::GetBaseDataItemType() != pDataItem->GetItemType())
    {
        return NULL;
    }

    return pDataItem;
}

CSettingUIItemDataBase * CSettingUIItemDataBase::CreateBaseDataItem()
{
    CSettingUIItemDataBase * pBaseDataItem = new CSettingUIItemDataBase(SETTING_ITEM_TYPE_BASE);

    return pBaseDataItem;
}


void CSettingUIItemDataBase::Reset()
{
    m_eWgtType = -1;
    m_eItemType = -1;
    m_eValueType = -1;
    m_bResetable = true;
    m_bEnable = true;
    m_bShow = true;
    m_bSecondLevel = false;
    m_strID = "";
    m_strDisplayName = "";
    m_strConfigID = "";
    m_strAuthorityID = "";
    m_strValue = "";
    m_pExtraData = NULL;
    m_strExtraData = "";
    m_eExtraDataType = -1;
    m_nExtraData = -1;

    LIST_SETTING_MSG_DATA::iterator it = m_listItemMsg.begin();
    for (; it != m_listItemMsg.end(); ++it)
    {
        if (NULL != *it)
        {
            delete *it;
        }
    }
    m_listItemMsg.clear();

    if (NULL != m_pSoftkeyData)
    {
        delete m_pSoftkeyData;
        m_pSoftkeyData = NULL;
    }
    if (NULL != m_pErrorInfo)
    {
        delete m_pErrorInfo;
        m_pErrorInfo = NULL;
    }
}

int CSettingUIItemDataBase::TransStringToValueType(const yl::string & strTypeString)
{
    if ("int" == strTypeString)
    {
        return WIDGET_VALUE_TYPE_INT;
    }
    else if ("string" == strTypeString)
    {
        return WIDGET_VALUE_TYPE_STRING;
    }
    else if ("bool" == strTypeString)
    {
        return WIDGET_VALUE_TYPE_BOOL;
    }
    else
    {
        return WIDGET_VALUE_TYPE_UNKNOW;
    }
}

bool CSettingUIItemDataBase::ParserItemData(const xml_node & node,
        CSettingUIItemDataBase * pItemInfo)
{
    if (NULL == pItemInfo || node.empty())
    {
        return false;
    }

#ifndef _WIN32
#warning default data
#endif
    pItemInfo->SetItemType(g_ItemTypeMgr.GetItemType(node.name()));
    pItemInfo->SetItemID(node.attribute("id").as_string());
    pItemInfo->SetDisplayName(node.attribute("displayname").as_string());
    pItemInfo->SetValue(node.attribute("defaultvalue").as_string());
    pItemInfo->SetShowIndex(node.attribute("showindex").as_bool(true));
    pItemInfo->SetShowColon(node.attribute("showcolon").as_bool(true));
#if IF_SETTING_DISPLAY_DEBUG
    SETTINGUI_INFO("-------------------- displayname[%s], itemType[%d] ======"
                   , pItemInfo->GetDisplayName().c_str(), pItemInfo->GetItemType());
#endif
    pItemInfo->SetConfigID(node.attribute("configid").as_string());
    pItemInfo->SetAuthorityID(node.attribute("authorid").as_string());
    // 静态过滤
    if (!g_pConfigFilterMgr->IsAllowShowStatic(pItemInfo->GetAuthorityID()))
    {
        return false;
    }
    pItemInfo->SetValueType(pItemInfo->TransStringToValueType(node.attribute("valuetype").as_string()));
    pItemInfo->SetEnable(0 != strcmp(node.attribute("enabled").as_string(), "0"));
    // Setting配置文件配置不可用，复位时不可复位
    pItemInfo->SetResetable(0 != strcmp(node.attribute("enabled").as_string(), "0"));
    pItemInfo->SetInnerExtraData(node.attribute("extradata").as_string());
    pItemInfo->SetInnerExtraDataType(pItemInfo->TransStringToValueType(
                                         node.attribute("extradatatype").as_string()));
    pItemInfo->SetSecondLevel(node.attribute("secondlevel").as_bool(false));
    pItemInfo->SetConfigChangedReboot(0 == strcmp(node.attribute("configchangereboot").as_string(),
                                      "yes"));

    g_DataFactory->ParseErrorInfo(node.attribute("error").as_string(), pItemInfo);

    g_DataFactory->ParseMessageInfo(node, pItemInfo->GetPointListMessageData());

    return true;
}

bool CSettingUIItemDataBase::LoadItemData(CSettingUIItemDataBase * pItemData)
{
    return true;
}

bool CSettingUIItemDataBase::SaveItemData(const CSettingUIItemDataBase * pItemData)
{
    return true;
}

const yl::string & CSettingUIItemDataBase::GetItemAction()
{
    return NS_SET::kszEmptyString;
}

const yl::string & CSettingUIItemDataBase::GetItemFocusAction()
{
    return NS_SET::kszEmptyString;
}

int CSettingUIItemDataBase::GetWgtType()
{
    return m_eWgtType;
}

void CSettingUIItemDataBase::SetWgtType(int eWgtType)
{
    m_eWgtType = eWgtType;
}

int CSettingUIItemDataBase::GetItemType()
{
    return m_eItemType;
}

void CSettingUIItemDataBase::SetItemType(int eItemType)
{
    m_eItemType = eItemType;
}

int CSettingUIItemDataBase::GetValueType()
{
    return m_eValueType;
}

void CSettingUIItemDataBase::SetValueType(int eValueType)
{
    m_eValueType = eValueType;
}

bool CSettingUIItemDataBase::IsResetable()
{
    return m_bResetable;
}

void CSettingUIItemDataBase::SetResetable(bool bResetable)
{
    m_bResetable = bResetable;
}

bool CSettingUIItemDataBase::IsEnable()
{
    return m_bEnable;
}

void CSettingUIItemDataBase::SetEnable(bool bEnable)
{
    m_bEnable = bEnable;
}

bool CSettingUIItemDataBase::IsShow()
{
    return m_bShow;
}

void CSettingUIItemDataBase::SetShow(bool bShow)
{
    m_bShow = bShow;
}

bool CSettingUIItemDataBase::IsSecondLevel()
{
    return m_bSecondLevel;
}

void CSettingUIItemDataBase::SetSecondLevel(bool bSecondLevel)
{
    m_bSecondLevel = bSecondLevel;
}

bool CSettingUIItemDataBase::IsConfigChangedReboot()
{
    return m_bConfigChangedReboot;
}

void CSettingUIItemDataBase::SetConfigChangedReboot(bool bConfigChangedReboot)
{
    m_bConfigChangedReboot = bConfigChangedReboot;
}

const yl::string & CSettingUIItemDataBase::GetItemID()
{
    return m_strID;
}

void CSettingUIItemDataBase::SetItemID(const yl::string & strItemID)
{
    m_strID = strItemID;
}

const yl::string & CSettingUIItemDataBase::GetDisplayName()
{
    return m_strDisplayName;
}

void CSettingUIItemDataBase::SetDisplayName(const yl::string & strDisplayName)
{
    m_strDisplayName = strDisplayName;
}

bool CSettingUIItemDataBase::IsShowIndex()
{
    return m_bShowIndex;
}

void CSettingUIItemDataBase::SetShowIndex(bool bShowIndex)
{
    m_bShowIndex = bShowIndex;
}

bool CSettingUIItemDataBase::IsShowColon()
{
    return m_bShowColon;
}

void CSettingUIItemDataBase::SetShowColon(bool bShowColon)
{
    m_bShowColon = bShowColon;
}

const yl::string & CSettingUIItemDataBase::GetConfigID()
{
    return m_strConfigID;
}

void CSettingUIItemDataBase::SetConfigID(const yl::string & strConfigID)
{
    m_strConfigID = strConfigID;
}

const yl::string & CSettingUIItemDataBase::GetAuthorityID()
{
    return m_strAuthorityID;
}

void CSettingUIItemDataBase::SetAuthorityID(const yl::string & strAuthorityID)
{
    m_strAuthorityID = strAuthorityID;
}

const yl::string & CSettingUIItemDataBase::GetValue()
{
    return m_strValue;
}

void CSettingUIItemDataBase::SetValue(const yl::string & strValue)
{
#if IF_SETTING_DISPLAY_DEBUG
    SETTINGUI_INFO("################### set value[%s], dis[%s] ======="
                   , strValue.c_str(), m_strDisplayName.c_str());
#endif
    m_strValue = strValue;
}

void * CSettingUIItemDataBase::GetExtraData()
{
    if (NULL != m_pExtraData)
    {
        return m_pExtraData;
    }

    switch (m_eExtraDataType)
    {
    case WIDGET_VALUE_TYPE_INT:
        {
            m_nExtraData = atoi(m_strExtraData.c_str());
            m_pExtraData = (void*)(&m_nExtraData);
        }
        break;
    case WIDGET_VALUE_TYPE_STRING:
        {
            m_pExtraData = (void*)(&m_strExtraData);
        }
        break;
    default:
        break;
    }

#if IF_SETTING_DISPLAY_DEBUG
    SETTINGUI_INFO("----- [CSettingUIItemDataBase::GetExtraData()] extraData[%s] ========="
                   , m_strExtraData.c_str());
#endif

    return m_pExtraData;
}

void CSettingUIItemDataBase::SetExtraData(void * pExtraData)
{
    m_pExtraData = pExtraData;
}

CSettingUIErrorInfo * CSettingUIItemDataBase::GetItemErrorInfo()
{
    return m_pErrorInfo;
}

void CSettingUIItemDataBase::SetItemErrorInfo(CSettingUIErrorInfo * pItemErrorInfo)
{
    if (NULL != pItemErrorInfo)
    {
        if (NULL != m_pErrorInfo)
        {
            delete m_pErrorInfo;
            m_pErrorInfo = NULL;
        }
        m_pErrorInfo = pItemErrorInfo;
    }
}

CSettingUISoftkeyData * CSettingUIItemDataBase::GetListSoftkeyData()
{
    return m_pSoftkeyData;
}

LIST_SETTING_MSG_DATA CSettingUIItemDataBase::GetListMessageData()
{
    return m_listItemMsg;
}

LIST_SETTING_MSG_DATA* CSettingUIItemDataBase::GetPointListMessageData()
{
    return &m_listItemMsg;
}

bool CSettingUIItemDataBase::AddItemMessageData(CSettingUIMessageDataPtr pMsgData)
{
    if (NULL == pMsgData)
    {
        return false;
    }

    m_listItemMsg.push_back(pMsgData);
    return true;
}

bool CSettingUIItemDataBase::DelItemMessageData(CSettingUIMessageDataPtr pMsgData)
{
    if (NULL == pMsgData)
    {
        return false;
    }

    return true;
}

const yl::string & CSettingUIItemDataBase::GetInnerExtraData() const
{
    return m_strExtraData;
}

void CSettingUIItemDataBase::SetInnerExtraData(const yl::string & strInnerExtraData)
{
    m_strExtraData = strInnerExtraData;
}

int CSettingUIItemDataBase::GetInnerExtraDataType() const
{
    return m_eExtraDataType;
}

void CSettingUIItemDataBase::SetInnerExtraDataType(int nInnerExtraDataType)
{
    m_eExtraDataType = nInnerExtraDataType;
}

