#include "pagedatafactory.h"
#include "settinguibasepagedata.h"

IMPLEMENT_GETINSTANCE(CPageDataFactory)


CPageDataFactory::CPageDataFactory()
{
}

CPageDataFactory::~CPageDataFactory()
{
}

void CPageDataFactory::RegisterPageData(const yl::string & strPageID,
                                        FUN_CREATE_PAGE_DATA pFunCreatePageData)
{
    if (NULL == pFunCreatePageData)
    {
        return;
    }

    m_mapCreatePageData[strPageID] = pFunCreatePageData;
}

CSettingUIPageDataBase * CPageDataFactory::GetPageData(const yl::string & strPageID)
{
    MAP_CREATE_PAGE_DATA_FUN::iterator it = m_mapCreatePageData.find(strPageID);
    FUN_CREATE_PAGE_DATA pFun = NULL;
    if (it != m_mapCreatePageData.end())
    {
        pFun = it->second;
    }
    // 返回默认创建界面类
    if (NULL == pFun)
    {
        return GetDefaultPageData();
    }

    return (*pFun)();
}

CSettingUIPageDataBase * CPageDataFactory::GetDefaultPageData()
{
    return new CSettingUIPageDataBase(SETTING_PAGE_DATA_TYPE_BASE);
}

