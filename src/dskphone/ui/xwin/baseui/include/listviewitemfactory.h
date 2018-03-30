#ifndef __LIST_VIEW_ITEM_FACTORY_H__
#define __LIST_VIEW_ITEM_FACTORY_H__

#include <ylhashmap.h>
#include "xwindow/xlistviewitem.h"


typedef xListViewItem * (*FUN_CREATE_ITEM)();
typedef YLHashMap<int, FUN_CREATE_ITEM> MAP_CREATE_ITEM_FUN;

class CListViewItemFactory
{
public:
    CListViewItemFactory();
    ~CListViewItemFactory();

    static CListViewItemFactory & GetInstance();

public:
    void RegisterItem(int nItemType, FUN_CREATE_ITEM pFunCreateItem);

    xListViewItem * ApplyItem(int nItemType);
    bool ReleaseItem(xListViewItem * pItem);

protected:
    MAP_CREATE_ITEM_FUN m_mapCreateItem;
};

#define g_ItemFactory CListViewItemFactory::GetInstance()

#endif
