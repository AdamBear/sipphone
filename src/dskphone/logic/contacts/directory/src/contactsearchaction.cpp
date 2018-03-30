#include "contactsearchaction.h"

using namespace NS_TA;

CContactSearchAction::CContactSearchAction(CBaseContactManger * pManger)
    : m_nGroupId(-1),
      m_nLastGroupId(-1)
{
    m_bIsNeedNotif = true;
    m_eTARequestType = TA_REQUEST_LOCAL_CONTACT_SEARCH;

    m_pManager = pManger;

    SetAutoRelease(false);

    SetCallback(m_pManager);

    SetSync(false);

    m_eSearchMethod = SEARCH_BY_AUTO;
}

CContactSearchAction::~CContactSearchAction()
{
    m_pManager = NULL;
}

bool CContactSearchAction::Exec()
{
    if (m_pManager == NULL)
    {
        return false;
    }

    m_bIsNeedNotif = true;

    m_strSearchKey = m_strNextKey;

    DIR_INFO("-----------------------begin search[%s]", m_strSearchKey.c_str());

    // 如果搜索关键字为空, 则清空缓存.
    if (m_strSearchKey.empty())
    {
        m_bIsNeedNotif = false;
        return true;
    }

    bool isSuc = false;

    // 如果上次搜索字符和此次字符一致, 则直接拷贝联系人.
    if (strcasecmp(m_strSearchKey.c_str(), m_strLastKey.c_str()) == 0 && m_nGroupId == m_nLastGroupId)
    {
        return true;
    }

    m_eSearchMethod = commonAPI_IsDigital(m_strSearchKey) ? SEARCH_BY_T9 : SEARCH_BY_NAME;

    // 在上次基础上搜索.
    if (m_strSearchKey.find(m_strLastKey) != yl::string::npos
            && m_nGroupId == m_nLastGroupId)
    {
        isSuc = m_pManager->MySearchFromResult(m_strSearchKey, m_vSearchResult, m_eSearchMethod);
    }
    else
    {
        m_vSearchResult.clear();
        isSuc = m_pManager->MySearch(m_nGroupId, m_strSearchKey, m_vSearchResult, m_eSearchMethod);
    }

    if (isSuc)
    {
        SetTaskActionErrorType(TA_NO_ERROR);

        m_strLastKey = m_strSearchKey;

        m_nLastGroupId = m_nGroupId;

        if (m_strSearchKey.compare(m_strNextKey) == 0)
        {
            m_strNextKey.clear();
        }
    }
    else
    {
        m_strLastKey.clear();
        m_nGroupId = -1;
        m_vSearchResult.clear();
    }

    return isSuc;
}

void CContactSearchAction::SetSearchGroupId(int nId)
{
    m_nGroupId = nId;
}

void CContactSearchAction::SetSearchKey(const yl::string & strKey)
{
    m_strNextKey = strKey;
}

YLVector<int> & CContactSearchAction::GetSearchResult()
{
    return m_vSearchResult;
}

const yl::string & CContactSearchAction::GetSearchKey()
{
    return m_strSearchKey;
}

void CContactSearchAction::ReSet()
{
    m_strLastKey.clear();
    m_nLastGroupId = -1;
}

