#ifndef __PAGE_DATA_FACTORY_H__
#define __PAGE_DATA_FACTORY_H__

#include <ylhashmap.h>
#include <singletonclassdefine.h>
#include "ylstring.h"

class CSettingUIPageDataBase;

typedef CSettingUIPageDataBase * (*FUN_CREATE_PAGE_DATA)();
typedef YLHashMap<yl::string, FUN_CREATE_PAGE_DATA> MAP_CREATE_PAGE_DATA_FUN;

class CPageDataFactory
{
    DECLARE_SINGLETON_CLASS(CPageDataFactory);

public:
    // 注册界面类
    void RegisterPageData(const yl::string & strPageID, FUN_CREATE_PAGE_DATA pFunCreatePageData);
    // 根据pageId获取创建界面类，若没有返回默认界面类
    CSettingUIPageDataBase * GetPageData(const yl::string & strPageID);
    // 获取默认创建界面类
    CSettingUIPageDataBase * GetDefaultPageData();

protected:
    MAP_CREATE_PAGE_DATA_FUN    m_mapCreatePageData;
};

#define g_PageDataFactory GET_SINGLETON_INSTANCE(CPageDataFactory)

#endif // __PAGE_DATA_FACTORY_H__

