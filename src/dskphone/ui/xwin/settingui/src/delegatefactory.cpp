#include "delegatefactory.h"


IMPLEMENT_GETINSTANCE(CDelegateFactory)

CDelegateFactory::CDelegateFactory()
{
}

CDelegateFactory::~CDelegateFactory()
{
}

void CDelegateFactory::RegisterDelegate(const yl::string & strPageID,
                                        FUN_CREATE_DELEGATE pFunCreateDelegate)
{
    if (NULL == pFunCreateDelegate)
    {
        return;
    }

    m_mapCreateDelegate[strPageID] = pFunCreateDelegate;
}

CSettingUIDelegateBase * CDelegateFactory::GetDelegate(const yl::string & strPageID)
{
    MAP_CREATE_DELEGATE_FUN::iterator it = m_mapCreateDelegate.find(strPageID);
    FUN_CREATE_DELEGATE pFun = NULL;
    if (it != m_mapCreateDelegate.end())
    {
        pFun = it->second;
    }

    if (NULL == pFun)
    {
        return NULL;
    }

    return (*pFun)();
}

CSettingUIDelegateBase * CDelegateFactory::GetDefaultDelegate()
{
    return new CSettingUIDelegateBase();
}

