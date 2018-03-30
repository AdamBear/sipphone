#include "searchaction.h"
#include "supersearchmanager.h"


#define kszNoNeedSearch "~NoNeedSearch"

using namespace NS_TA;

CSearchAction::CSearchAction()
{
    m_bIsLdapPro = false;
    m_eTARequestType = TA_REQUEST_SUPER_SEARCH;

    m_exeType = EXE_NONE;

    m_strLastKey.clear();
    m_strNextKey.clear();
    m_strSearchKey.clear();
}

CSearchAction::~CSearchAction()
{

}

bool CSearchAction::Exec()
{
    SetTaskActionErrorType(TA_OTHER_ERROR);


    yl::string strNextKey = GetNextSearchKey();
    if (m_bIsLdapPro)
    {
        g_refSuperSearchManager.SetLdapResult(m_strLdapKey, m_strSearchKey);
        m_exeType = EXE_LDAP_DATA_GET;

        return true;
    }

    m_exeType = EXE_CONTACT_SEARCH;

    if (strNextKey.empty())
    {
        SetSearchKey(strNextKey);
        // 搜索空字符串.
        if (g_refSuperSearchManager.SearchRecentCallOut())
        {
            SetTaskActionErrorType(TA_NO_ERROR);
        }
    }
    else
    {
#if IF_FEATURE_GBBOOK
        if (g_refSuperSearchManager.IsClearSearchKey())
        {
            m_strLastKey.clear();
        }
#endif
        // 搜索非空字符串.
        Search();
    }
    return true;
}

void CSearchAction::Search()
{

    if (NoNeedSearch())
    {
        SUPERSEARCH_INFO("No Need Search Again!");
        return;
    }

    m_searchCs.Lock();
    m_strSearchKey = m_strNextKey;

    SUPERSEARCH_INFO("CSearchAction::Search() m_strLastKey[%s],m_strSearchKey[%s]",
                     m_strLastKey.c_str(), m_strSearchKey.c_str());

    if (m_strSearchKey.compare(m_strLastKey) == 0)
    {
        SUPERSEARCH_INFO("key no Chang!");
        SetTaskActionErrorType(TA_NO_ERROR);
        m_searchCs.Unlock();
        return;
    }

    m_searchCs.Unlock();
    SearchMethod eSearchMethod = commonAPI_IsDigital(m_strSearchKey) ? SEARCH_BY_T9 : SEARCH_BY_NAME;

    g_refSuperSearchManager.SetSearchType(eSearchMethod);

    bool isSuc = false;

    if (m_strSearchKey.find(m_strLastKey) != yl::string::npos && !m_strLastKey.empty())
    {
        isSuc = g_refSuperSearchManager.SearchFromResult(m_strSearchKey);
    }
    else
    {
        isSuc = g_refSuperSearchManager.MyReSearch(m_strSearchKey);
    }

    m_searchCs.Lock();
    if (isSuc)
    {
        SetTaskActionErrorType(TA_NO_ERROR);

        m_strLastKey = m_strSearchKey;

        if (m_strNextKey.compare(m_strSearchKey) == 0)
        {
            m_strNextKey = kszNoNeedSearch;
        }
    }
    else
    {
        m_strLastKey.clear();
    }

    m_searchCs.Unlock();
}


void CSearchAction::SetNextSearchKey(const yl::string & strKey)
{
    m_searchCs.Lock();
    m_strNextKey = strKey;
    m_searchCs.Unlock();
}

void CSearchAction::SetSearchKey(const yl::string & strKey)
{
    m_searchCs.Lock();
    m_strSearchKey = strKey;
    m_searchCs.Unlock();
}

yl::string CSearchAction::GetSearchKey()
{
    m_searchCs.Lock();
    yl::string strKey = m_strSearchKey;
    m_searchCs.Unlock();
    return strKey;
}

yl::string CSearchAction::GetNextSearchKey()
{
    m_searchCs.Lock();
    yl::string strKey = m_strNextKey;
    m_searchCs.Unlock();
    return strKey;
}

yl::string CSearchAction::GetLastSearchKey()
{
    m_searchCs.Lock();
    yl::string strKey = m_strLastKey;
    m_searchCs.Unlock();
    return strKey;
}

bool CSearchAction::NoNeedSearch()
{
    m_searchCs.Lock();
    bool bRet = (m_strNextKey.compare(kszNoNeedSearch) == 0);
    m_searchCs.Unlock();
    return bRet;
}

void CSearchAction::SetLdapDataProcess(bool isOn, const yl::string & strLdapKey /*= ""*/)
{
    m_bIsLdapPro = isOn;

    m_strLdapKey = strLdapKey;
}

bool CSearchAction::IsProcessLdapData()
{
    return m_exeType == EXE_LDAP_DATA_GET;
}

void CSearchAction::ResetLastKey()
{
    m_searchCs.Lock();
    m_strLastKey.clear();
    m_searchCs.Unlock();
}

