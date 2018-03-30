#ifndef __ITEM_DATA_FACTORY_H__
#define __ITEM_DATA_FACTORY_H__

#include <ylhashmap.h>
#include <singletonclassdefine.h>
#include "ylstring.h"
#include "../src/settinguibaseitemdata.h"

typedef CSettingUIItemDataBase * (*FUN_CREATE_ITEM_DATA)();
typedef YLHashMap<yl::string, FUN_CREATE_ITEM_DATA> MAP_CREATE_ITEM_DATA_FUN;

class CItemDataFactory
{
    DECLARE_SINGLETON_CLASS(CItemDataFactory);

public:
    void RegisterItemData(const yl::string & strItemType, FUN_CREATE_ITEM_DATA pFunCreateItemData);

    CSettingUIItemDataBase * GetItemData(const yl::string & strItemType);

    CSettingUIItemDataBase * GetDefaultItemData();

protected:
    MAP_CREATE_ITEM_DATA_FUN    m_mapCreateItemData;
};

#define g_ItemDataFactory GET_SINGLETON_INSTANCE(CItemDataFactory)

#endif

