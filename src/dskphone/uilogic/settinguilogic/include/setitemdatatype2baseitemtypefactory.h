#ifndef __SET_ITEM_DATA_TYPE_2_BASE_ITEM_TYPE_FACTORY_H__
#define __SET_ITEM_DATA_TYPE_2_BASE_ITEM_TYPE_FACTORY_H__

#include <ylhashmap.h>
#include <singletonclassdefine.h>

typedef  YLHashMap<int, int>  MAP_ITEM_DATA_TYPE_TRANS;

class CItemDataTypeTransFactory
{
    DECLARE_SINGLETON_CLASS(CItemDataTypeTransFactory);

public:
    void RegisterItemDataTypeTrans(int eSetItemDataType, int eBaseItemType);

    int GetItemDataTypeTrans(int eSetItemDataType);

    int GetDefaultItemDataTypeTrans();

protected:
    MAP_ITEM_DATA_TYPE_TRANS        m_mapCreateItemDataTypeTrans;
};

#define g_ItemDataTypeTransFactory GET_SINGLETON_INSTANCE(CItemDataTypeTransFactory)

#endif // __SET_ITEM_DATA_TYPE_2_BASE_ITEM_TYPE_FACTORY_H__

