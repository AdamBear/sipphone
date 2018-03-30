#include "bluetoothdelegate.h"
#include "bluetoothitem.h"
#include "settinganimationitem.h"

#include "dsklog/log.h"

BluetoothDelegate::BluetoothDelegate()
    : SubpageListDeleagate()
{
}

BluetoothDelegate::~BluetoothDelegate()
{
}

void BluetoothDelegate::OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction)
{
    if (NULL == pItem || !(pItem->inherits("CSettingItem")) || !IsFrameListAttach(pFrameList))
    {
        return;
    }

    if (nAction == LIST_ITEM_BTN_CLICK)
    {
        emit ItemDetailAction(pItem);
        return;
    }

    return SubpageListDeleagate::OnItemAction(pFrameList, pItem, nAction);
}

void BluetoothDelegate::SetItemDataByDataIndex(QWidgetPair qPair, int iDateIdx,
        int eItemType /*= SETTING_ITEM_BLUETOOTH*/)
{
    SETTINGUI_INFO("[BluetoothDelegate::SetItemDataByDataIndex] iDataIdx[%d], size[%d] "
                   , iDateIdx, m_vecItem.size());
    if (iDateIdx >= 0 && iDateIdx < m_vecItem.size())
    {
        CListItemPtr pNowItem = m_pFramelist->GetItemByDataIndex(iDateIdx);
        if (NULL != m_vecItem[iDateIdx] && m_vecItem[iDateIdx]->inherits("CSettingBluetoothItem"))
        {
            CSettingBluetoothItem * pBlueItem = static_cast<CSettingBluetoothItem *>(m_vecItem[iDateIdx]);
            if (NULL != pBlueItem)
            {
                pBlueItem->SetItemData(qPair);
            }
        }
        else if (NULL != pNowItem && pNowItem->inherits("CSettingAnimationItem"))
        {
            CSettingAnimationItem * pSearchItem = static_cast<CSettingAnimationItem *>(pNowItem);
            if (NULL != pSearchItem)
            {
                pSearchItem->SetItemData(qPair);
            }
        }
    }
}
