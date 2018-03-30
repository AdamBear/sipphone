#include "setuidlgbasecache.h"
#include "basesettinguidelegate.h"

#include "baseui/include/detaillistitem.h"
#include "baseui/include/detaillistitembase.h"
#include "baseui/include/editlistitem.h"
#include "baseui/include/comboboxlistitem.h"

CSetUIDlgBaseCache * CSetUIDlgBaseCache::s_SetUIDlgBaseCache = NULL;

CSetUIDlgBaseCache * CSetUIDlgBaseCache::GetDlgCache()
{
    if (NULL == s_SetUIDlgBaseCache)
    {
        s_SetUIDlgBaseCache = new CSetUIDlgBaseCache();
    }

    return s_SetUIDlgBaseCache;
}

CSetUIDlgBaseCache::CSetUIDlgBaseCache()
{
}

CSetUIDlgBaseCache::~CSetUIDlgBaseCache()
{
}

bool CSetUIDlgBaseCache::DialogCache(CSettingUIDelegateBase * pSetDelegate, xListView * pListView)
{
    if (NULL == pListView || NULL == pSetDelegate)
    {
        return false;
    }

    // 记录焦点项
    pSetDelegate->SetFocusIndex(pListView->GetFocusItemDataIndex());
    // 记录Item信息
    int nTotal = pListView->GetTotalDataCount();
    for (int i = 0; i < nTotal; ++i)
    {
        xListViewItemPtr pItem = pListView->GetItemByDataIndex(i);
        if (NULL != pItem)
        {
            if (CDetailListItem::GetDetailListItemType() == pItem->GetType()
                    || CEditListItem::GetEditListItemType() == pItem->GetType()
                    || CComboBoxListItem::GetComboBoxListItemType() == pItem->GetType())
            {
                CDetailListItemBasePtr pBaseItem = (CDetailListItemBasePtr)pItem;
                if (NULL != pBaseItem)
                {
                    pSetDelegate->SetItemDataValueByDataIndex(i, pBaseItem->GetValue());
                }
            }
        }
    }

    return true;
}

