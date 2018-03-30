#include "itemdatafactory.h"


IMPLEMENT_GETINSTANCE(CItemDataFactory)


CItemDataFactory::CItemDataFactory()
{
}

CItemDataFactory::~CItemDataFactory()
{
}

void CItemDataFactory::RegisterItemData(const yl::string & strItemType,
                                        FUN_CREATE_ITEM_DATA pFunCreateItemData)
{
    if (NULL == pFunCreateItemData)
    {
        return;
    }

    m_mapCreateItemData[strItemType] = pFunCreateItemData;
}

CSettingUIItemDataBase * CItemDataFactory::GetItemData(const yl::string & strItemType)
{
    MAP_CREATE_ITEM_DATA_FUN::iterator it = m_mapCreateItemData.find(strItemType);
    FUN_CREATE_ITEM_DATA pFun = NULL;
    if (it != m_mapCreateItemData.end())
    {
        pFun = it->second;
    }

    if (NULL == pFun)
    {
        return NULL;
    }

    return (*pFun)();
}

CSettingUIItemDataBase * CItemDataFactory::GetDefaultItemData()
{
    return new CSettingUIItemDataBase(SETTING_ITEM_TYPE_BASE);
}

