#include "settinguisoftkeydata.h"


///////////////////////////////// CSettingUISoftkeyItemData //////////////////////////////////////
CSettingUISoftkeyItemData::CSettingUISoftkeyItemData()
    : m_nIndex(-1)
    , m_strDisplayName("")
    , m_strAction("")
    , m_strIconPath("")
{
}

CSettingUISoftkeyItemData::~CSettingUISoftkeyItemData()
{
}

CSettingUISoftkeyItemData & CSettingUISoftkeyItemData::operator = (const CSettingUISoftkeyItemData &
        objSoftkeyItemData)
{
    if (this != &objSoftkeyItemData)
    {
        m_nIndex = objSoftkeyItemData.m_nIndex;
        m_strIconPath = objSoftkeyItemData.m_strIconPath;
        m_strDisplayName = objSoftkeyItemData.m_strDisplayName;
        m_strAction = objSoftkeyItemData.m_strAction;
    }

    return (*this);
}

bool CSettingUISoftkeyItemData::GetSoftkeyData(CSettingUISoftkeyItemData & objSoftkeyItemData)
{
    if (this != &objSoftkeyItemData)
    {
        objSoftkeyItemData = *this;
    }
    return true;
}

bool CSettingUISoftkeyItemData::SetSoftkeyData(const CSettingUISoftkeyItemData & objSoftkeyItemData)
{
    if (this != &objSoftkeyItemData)
    {
        *this = objSoftkeyItemData;
    }
    return true;
}

void CSettingUISoftkeyItemData::Reset()
{
    m_nIndex = -1;
    m_strIconPath = "";
    m_strDisplayName = "";
    m_strAction = "";
}

int CSettingUISoftkeyItemData::GetIndex() const
{
    return m_nIndex;
}

void CSettingUISoftkeyItemData::SetIndex(int nIndex)
{
    m_nIndex = nIndex;
}

const yl::string & CSettingUISoftkeyItemData::GetDisplayName() const
{
    return m_strDisplayName;
}

void CSettingUISoftkeyItemData::SetDisplayName(const yl::string & strDisplayName)
{
    m_strDisplayName = strDisplayName;
}

const yl::string & CSettingUISoftkeyItemData::GetAction() const
{
    return m_strAction;
}

void CSettingUISoftkeyItemData::SetAction(const yl::string & strAction)
{
    m_strAction = strAction;
}

const yl::string & CSettingUISoftkeyItemData::GetIconPath() const
{
    return m_strIconPath;
}

void CSettingUISoftkeyItemData::SetIconPath(const yl::string & strIconPath)
{
    m_strIconPath = strIconPath;
}


///////////////////////////////// CSettingUISoftkeyData //////////////////////////////////////
CSettingUISoftkeyData::CSettingUISoftkeyData()
    : m_listSoftkeyData()
{
}

CSettingUISoftkeyData::~CSettingUISoftkeyData()
{
    Clear();
}

CSettingUISoftkeyData & CSettingUISoftkeyData::operator = (const CSettingUISoftkeyData &
        objSoftkeyData)
{
    if (this != &objSoftkeyData)
    {
        Clear();
        LIST_SOFTKEY_ITEM_DATA_PTR::const_iterator iter = objSoftkeyData.m_listSoftkeyData.begin();
        for (; iter != objSoftkeyData.m_listSoftkeyData.end(); ++iter)
        {
            CSettingUISoftkeyItemData * pSoftkeyItemData = *iter;
            CSettingUISoftkeyItemData * pNewItemData = new CSettingUISoftkeyItemData();
            if (NULL != pSoftkeyItemData && NULL != pNewItemData)
            {
                *pNewItemData = *pSoftkeyItemData;
                AddSoftkeyItem(pNewItemData);
            }
        }
    }

    return (*this);
}

void CSettingUISoftkeyData::Reset()
{
    LIST_SOFTKEY_ITEM_DATA_PTR::iterator iter = m_listSoftkeyData.begin();
    for (; iter != m_listSoftkeyData.end(); ++iter)
    {
        CSettingUISoftkeyItemData * pSoftkeyItemData = *iter;
        if (NULL != pSoftkeyItemData)
        {
            pSoftkeyItemData->Reset();
        }
    }
}

void CSettingUISoftkeyData::Clear()
{
    LIST_SOFTKEY_ITEM_DATA_PTR::iterator iter = m_listSoftkeyData.begin();
    for (; iter != m_listSoftkeyData.end(); ++iter)
    {
        CSettingUISoftkeyItemData * pSoftkeyItemData = *iter;
        if (NULL != pSoftkeyItemData)
        {
            delete pSoftkeyItemData;
            pSoftkeyItemData = NULL;
        }
    }

    m_listSoftkeyData.clear();
}

bool CSettingUISoftkeyData::AddSoftkeyItem(CSettingUISoftkeyItemData * objSoftkeyItemData)
{
    if (NULL != objSoftkeyItemData)
    {
        m_listSoftkeyData.push_back(objSoftkeyItemData);
        return true;
    }

    return false;
}

bool CSettingUISoftkeyData::DelSoftkeyItem(int nIndex)
{
    return true;
}

CSettingUISoftkeyItemData * CSettingUISoftkeyData::GetSoftkeyItem(int nIndex)
{
    LIST_SOFTKEY_ITEM_DATA_PTR::iterator iter = m_listSoftkeyData.begin();
    for (; iter != m_listSoftkeyData.end(); ++iter)
    {
        CSettingUISoftkeyItemData * pSoftkeyItemData = *iter;
        if (NULL != pSoftkeyItemData && nIndex == pSoftkeyItemData->GetIndex())
        {
            return pSoftkeyItemData;
        }
    }

    return NULL;
}

LIST_SOFTKEY_ITEM_DATA_PTR CSettingUISoftkeyData::GetSoftkeyData()
{
    return m_listSoftkeyData;
}

