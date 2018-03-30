#include "menuiconlistitemdata.h"

#if IF_SUPPORT_MENUICON_ITEM
CMenuIconListItemData::CMenuIconListItemData()
    : m_strIdName("")
    , m_strDisplayName("")
    , m_strAuthorityID("")
    , m_strSelectedIconName("")
    , m_strClickAction("")
    , m_strLongClickAction("")
    , m_bShow(true)
{
}

CMenuIconListItemData::~CMenuIconListItemData()
{
}

void CMenuIconListItemData::Reset()
{
    m_strIdName = "";
    m_strDisplayName = "";
    m_strAuthorityID = "";
    m_strIconName = "";
    m_strSelectedIconName = "";
    m_strClickAction = "";
    m_strLongClickAction = "";
    m_bShow = true;
}

CSettingUIItemDataBase * CMenuIconListItemData::CreateMenuIconListItem()
{
    CMenuIconListItemData * pItemData = new CMenuIconListItemData;
    return (CSettingUIItemDataBasePtr)pItemData;
}

const yl::string & CMenuIconListItemData::GetItemID()
{
    return m_strIdName;
}

void CMenuIconListItemData::SetItemID(const yl::string & strItemID)
{
    m_strIdName = strItemID;
}

bool CMenuIconListItemData::IsShow()
{
    return m_bShow;
}

void CMenuIconListItemData::SetShow(bool bShow)
{
    m_bShow = bShow;
}

const yl::string & CMenuIconListItemData::GetDisplayName()
{
    return m_strDisplayName;
}

void CMenuIconListItemData::SetDisplayName(const yl::string & strDisplayName)
{
    m_strDisplayName = strDisplayName;
}

const yl::string & CMenuIconListItemData::GetAuthorityID()
{
    return m_strAuthorityID;
}

void CMenuIconListItemData::SetAuthorityID(const yl::string & strAuthorityID)
{
    m_strAuthorityID = strAuthorityID;
}

const yl::string & CMenuIconListItemData::GetIconName()
{
    return m_strIconName;
}

void CMenuIconListItemData::SetIconName(const yl::string & strIconName)
{
    m_strIconName = strIconName;
}

const yl::string & CMenuIconListItemData::GetSelectedIconName()
{
    return m_strSelectedIconName;
}

void CMenuIconListItemData::SetSelectedIconName(const yl::string & strSelectedIconName)
{
    m_strSelectedIconName = strSelectedIconName;
}

const yl::string & CMenuIconListItemData::GetClickAction()
{
    return m_strClickAction;
}

void CMenuIconListItemData::SetClickAction(const yl::string & strClickAction)
{
    m_strClickAction = strClickAction;
}

const yl::string & CMenuIconListItemData::GetLongClickAction()
{
    return m_strLongClickAction;
}

void CMenuIconListItemData::SetLongClickAction(const yl::string & strLongClickAction)
{
    m_strLongClickAction = strLongClickAction;
}
#endif

