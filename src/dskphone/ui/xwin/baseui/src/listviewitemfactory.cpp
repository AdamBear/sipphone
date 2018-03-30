#include "ETLLib/ETLLib.hpp"
#include "listviewitemfactory.h"
#include "baseui/include/baseuicommon.h"


#if IF_MULTI_WORKSTATION_ENBALE
IMPLEMENT_MULTI_WORKSTATION_INSTANCE(CListViewItemFactory)
#else
IMPLEMENT_WORKSTATION_INSTANCE(CListViewItemFactory)
#endif

CListViewItemFactory::CListViewItemFactory()
{
}

CListViewItemFactory::~CListViewItemFactory()
{
    m_mapCreateItem.clear();
}

void CListViewItemFactory::RegisterItem(int nItemType, FUN_CREATE_ITEM pFunCreateItem)
{
    if (NULL == pFunCreateItem)
    {
        return;
    }

    chASSERTx(m_mapCreateItem.find(nItemType) == m_mapCreateItem.end(),
              "repeat register same type item:%0x", nItemType);

    m_mapCreateItem[nItemType] = pFunCreateItem;
}

xListViewItem * CListViewItemFactory::ApplyItem(int nItemType)
{
    MAP_CREATE_ITEM_FUN::iterator iter = m_mapCreateItem.find(nItemType);
    FUN_CREATE_ITEM pFunCreateItem = NULL;

    if (iter != m_mapCreateItem.end())
    {
        pFunCreateItem = iter->second;
    }

    if (NULL == pFunCreateItem)
    {
        return NULL;
    }

    return (*pFunCreateItem)();
}

bool CListViewItemFactory::ReleaseItem(xListViewItem * pItem)
{
    if (NULL == pItem)
    {
        return false;
    }

    pItem->close();

    return true;
}
