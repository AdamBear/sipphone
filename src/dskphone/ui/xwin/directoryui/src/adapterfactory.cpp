#include "adapterfactory.h"
#include "bsftpwdadapter.h"
#include "browseadapter.h"
#include "detailadapter.h"
#include "numberadapter.h"
#include "precisesearchadapter.h"
#include "searchadapter.h"
#if IF_FEATURE_34162
#include "ldappwdadapter.h"
#endif
#include "accountadapter.h"

IMPLEMENT_GETINSTANCE(CAdapterFactory)

CAdapterFactory::CAdapterFactory(void)
{
}

CAdapterFactory::~CAdapterFactory(void)
{
    Clear();
}

CDirectoryAdapterPtr CAdapterFactory::FindContactAdapter(int nType)
{
    MAP_CONTACT_ADAPTER::iterator iter = m_mapContactAdapter.find(nType);

    if (iter != m_mapContactAdapter.end())
    {
        return iter->second;
    }

    return CDirectoryAdapterPtr(NULL);
}

CDirectoryAdapterPtr CAdapterFactory::GetContactAdapter(int nType)
{
    CDirectoryAdapterPtr pAdapter = FindContactAdapter(nType);

    if (!pAdapter.IsEmpty())
    {
        return pAdapter;
    }

    switch (nType)
    {
    case DIR_UI_STATE_BROWSER:
        {
            pAdapter = CDirectoryAdapterPtr(new CBrowseAdapter);
            DIRUI_DEBUG("new CBrowseAdapter:%p", pAdapter.Get());
        }
        break;
    case DIR_UI_STATE_SEARCH:
        {
            pAdapter = CDirectoryAdapterPtr(new CSearchAdapter);
            DIRUI_DEBUG("new CSearchAdapter:%p", pAdapter.Get());
        }
        break;
    case DIR_UI_STATE_DETAIL:
        {
            pAdapter = CDirectoryAdapterPtr(new CDetailAdapter);
            DIRUI_DEBUG("new CDetailAdapter:%p", pAdapter.Get());
        }
        break;
    case DIR_UI_STATE_PRECISE_SEARCH:
        {
            pAdapter = CDirectoryAdapterPtr(new CPreciseSearchAdapter);
            DIRUI_DEBUG("new CPreciseSearchAdapter:%p", pAdapter.Get());
        }
        break;
    case DIR_UI_STATE_CHOOSE_NUMBER:
        {
            pAdapter = CDirectoryAdapterPtr(new CNumberAdapter);
            DIRUI_DEBUG("new CNumberAdapter:%p", pAdapter.Get());
        }
        break;
    case DIR_UI_STATE_CHG_PWD:
        {
            pAdapter = CDirectoryAdapterPtr(new CBsftPwdAdapter);
            DIRUI_DEBUG("new CBsftPwdAdapter:%p", pAdapter.Get());
        }
        break;
#if IF_FEATURE_34162
    case DIR_UI_STATE_LDAP_CHG_PWD:
        {
            pAdapter = CDirectoryAdapterPtr(new CLDAPPwdAdapter);
            DIRUI_DEBUG("new CLDAPPwdAdapter:%p", pAdapter.Get());
        }
        break;
#endif
    case DIR_UI_STATE_CHOOSE_ACCOUNT:
        {
            pAdapter = CDirectoryAdapterPtr(new CAccountAdapter);
            DIRUI_DEBUG("new CAccountAdapter:%p", pAdapter.Get());
        }
        break;
    default:
        break;
    }

    if (!pAdapter.IsEmpty())
    {
        m_mapContactAdapter[nType] = pAdapter;
    }

    return pAdapter;
}

void CAdapterFactory::Clear()
{
    m_mapContactAdapter.clear();
}
