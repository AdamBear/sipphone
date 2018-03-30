#ifndef __ITEM_TYPE_MANAGER_H__
#define __ITEM_TYPE_MANAGER_H__

#include <ylhashmap.h>
#include <singletonclassdefine.h>
#include "ylstring.h"

typedef YLHashMap<yl::string, int> MAP_ITEM_TYPE;

class CItemTypeMgr
{
    DECLARE_SINGLETON_CLASS(CItemTypeMgr);

public:
    bool AddItemType(const yl::string & strItemType, int eItemType);
    bool DelItemType(const yl::string & strItemType);

    int GetItemType(const yl::string & strItemType);

protected:
    MAP_ITEM_TYPE   m_mapItemType;
};

#define g_ItemTypeMgr GET_SINGLETON_INSTANCE(CItemTypeMgr)

#endif // __ITEM_TYPE_MANAGER_H__

