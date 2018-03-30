#include "setuicachefactory.h"


IMPLEMENT_GETINSTANCE(CSetUICacheFactory)

CSetUICacheFactory::CSetUICacheFactory()
{
}

CSetUICacheFactory::~CSetUICacheFactory()
{
}

void CSetUICacheFactory::RegisterDlgCache(const yl::string & strPageID,
        FUN_CREATE_DLG_CACHE pFunCreateDlgCache)
{
    if (NULL == pFunCreateDlgCache)
    {
        return;
    }

    m_mapCreateDlgCache[strPageID] = pFunCreateDlgCache;
}

CSetUIDlgBaseCache * CSetUICacheFactory::GetDlgCache(const yl::string & strPageID)
{
    MAP_CREATE_DLG_CACHE::iterator iter = m_mapCreateDlgCache.find(strPageID);
    FUN_CREATE_DLG_CACHE pFun = NULL;
    if (iter != m_mapCreateDlgCache.end())
    {
        pFun = iter->second;
    }

    if (NULL == pFun)
    {
        return g_SetUIDlgBaseCache;
    }

    return (*pFun)();
}

