#include "delegatefactory.h"
#include "contactgroupdelegate.h"
#include "callloggroupdelegate.h"
#include "browserdelegate.h"
#include "detaildelegate.h"
#include "precisesearchdelegate.h"
#include "searchdelegate.h"
#include "settingdelegate.h"
#include "additiondelegate.h"


IMPLEMENT_GETINSTANCE(CDelegateFactory)

CDelegateFactory::CDelegateFactory(void)
{
}

CDelegateFactory::~CDelegateFactory(void)
{
    Clear();
}

CGroupDelegatePtr CDelegateFactory::FindGroupDelegate(int nType)
{
    MAP_GROUP_DELEGATE::iterator iter = m_mapGroupDelegate.find(nType);

    if (iter != m_mapGroupDelegate.end())
    {
        return iter->second;
    }

    return CGroupDelegatePtr(NULL);
}

CGroupDelegatePtr CDelegateFactory::GetGroupDelegate(int nType)
{
    CGroupDelegatePtr pDelegate = FindGroupDelegate(nType);

    if (!pDelegate.IsEmpty())
    {
        return pDelegate;
    }

    switch (nType)
    {
    case DIR_MOD_TYPE_CONTACT:
        {
            pDelegate = CGroupDelegatePtr(new CContactGroupDelegate);
            DIRUI_INFO("new CContactGroupDelegate:%p", pDelegate.Get());
        }
        break;
    case DIR_MOD_TYPE_CALLLOG:
        {
            pDelegate = CGroupDelegatePtr(new CCallLogGroupDelegate);
            DIRUI_INFO("new CCallLogGroupDelegate:%p", pDelegate.Get());
        }
        break;
    default:
        break;
    }

    if (!pDelegate.IsEmpty())
    {
        m_mapGroupDelegate[nType] = pDelegate;
        pDelegate->Init();
    }

    return pDelegate;
}

CDirectoryListDelegatePtr CDelegateFactory::FindContactDelegate(int nType)
{
    MAP_CONTACT_DELEGATE::iterator iter = m_mapContactDelegate.find(nType);

    if (iter != m_mapContactDelegate.end())
    {
        return iter->second;
    }

    return CDirectoryListDelegatePtr(NULL);
}

CDirectoryListDelegatePtr CDelegateFactory::GetContactDelegate(int nType)
{
    CDirectoryListDelegatePtr pDelegate = FindContactDelegate(nType);

    if (!pDelegate.IsEmpty())
    {
        return pDelegate;
    }

    switch (nType)
    {
    case DIR_UI_STATE_BROWSER:
        {
            pDelegate = CDirectoryListDelegatePtr(new CBrowserDelegate);
            DIRUI_INFO("new CBrowserDelegate:%p", pDelegate.Get());
        }
        break;
    case DIR_UI_STATE_SEARCH:
        {
            pDelegate = CDirectoryListDelegatePtr(new CSearchDelegate);
            DIRUI_INFO("new CSearchDelegate:%p", pDelegate.Get());
        }
        break;
    case DIR_UI_STATE_DETAIL:
        {
            pDelegate = CDirectoryListDelegatePtr(new CDetailDelegate);
            DIRUI_INFO("new CDetailDelegate:%p", pDelegate.Get());
        }
        break;
    case DIR_UI_STATE_ADDITION:
        {
            pDelegate = CDirectoryListDelegatePtr(new CAdditionDelegate);
            DIRUI_INFO("new CAdditionDelegate:%p", pDelegate.Get());
        }
        break;
    case DIR_UI_STATE_PRECISE_SEARCH:
        {
            pDelegate = CDirectoryListDelegatePtr(new CPreciseSearchDelegate);
            DIRUI_INFO("new CPreciseSearchDelegate:%p", pDelegate.Get());
        }
        break;
    case DIR_UI_STATE_SETTING:
        {
            pDelegate = CDirectoryListDelegatePtr(new CSettingDelegate);
            DIRUI_INFO("new CSettingDelegate:%p", pDelegate.Get());
        }
        break;
    default:
        break;
    }

    if (!pDelegate.IsEmpty())
    {
        m_mapContactDelegate[nType] = pDelegate;
    }

    return pDelegate;
}

void CDelegateFactory::Clear()
{
    m_mapGroupDelegate.clear();
    m_mapContactDelegate.clear();
}
