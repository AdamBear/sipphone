#include "wifi_inc.h"

#include "dsklog/log.h"

namespace
{
#ifdef _T49
#define WIFI_ITEM_HIGHT                 98
#define WIFI_SEARCH_ITEM_HIGHT          86
#define SETTING_ITEM_HIGHT              84
#endif
}

WifiSavedDelegate::WifiSavedDelegate()
    : SubpageListDeleagate()
{
}

WifiSavedDelegate::~WifiSavedDelegate()
{
}

void WifiSavedDelegate::OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction)
{
    if (NULL == pItem || !(pItem->inherits("CSettingItem")) || !IsFrameListAttach(pFrameList))
    {
        return;
    }

    if (LIST_ITEM_WIFI_BTN_CLICK == nAction)
    {
        emit ItemDetailAction(pItem);
        return;
    }
#ifdef _T49
    else if (LIST_ITEM_DRAG_START == nAction)
    {
        emit ItemDragDropStart(pItem);
        return;
    }
    else if (LIST_ITEM_DRAG_DROP == nAction)
    {
        SETTINGUI_INFO("release item[%d]", pItem);
        emit ItemDragDropEnd(pItem);
        return;
    }
#endif

    return SubpageListDeleagate::OnItemAction(pFrameList, pItem, nAction);
}

#ifdef _T49
void WifiSavedDelegate::OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount)
{
    HideAllItem();
    if (NULL != m_pFramelist && m_vecItemData.size() >= nDataIndex)
    {
        //重新设置父类
        int iTotalItem = m_pFramelist->GetTotalItemCount();
        for (int i = 0; i < iTotalItem; ++i)
        {
            CListItemPtr pItem = m_pFramelist->GetItemByIndex(i);
            if (NULL != pItem)
            {
                CSettingItem * pSetItem = (CSettingItem *)pItem;
                if (NULL != pSetItem->ContentWgt())
                {
                    pSetItem->ContentWgt()->setParent(m_pItemParent);
                }
            }
        }

        m_pFramelist->ClearContent(false);

        m_pFramelist->SetTotalDataCount(m_vecItemData.size());

        int iPageItemCount = m_pFramelist->GetMaxPageItemCount();
        int iAddCount = 1;
        //重新添加数据
        for (int i = nDataIndex, j = 0; i < nDataIndex + nDataCount && i < m_vecItemData.size()
                && j < iPageItemCount; i++, j++)
        {
            QWidgetPair itemData = m_vecItemData[i];
            CSettingItem * pItem =  g_SettingItemManger->CreateItemByType(itemData.eType, m_pItemParent,
                                    itemData);
            if (pItem == NULL)
            {
                continue;
            }
            pItem->setObjectName(QString("CSettingItem%1").arg(i));
            if (SETTING_ITEM_WIFI == itemData.eType)
            {
                pItem->setFixedHeight(WIFI_ITEM_HIGHT);
            }
            else if (WIRELESS_ITEM_SEARCH == itemData.eType)
            {
                pItem->setFixedHeight(WIFI_SEARCH_ITEM_HIGHT);
            }
            else// if ( == itemData.eType)
            {
                pItem->setFixedHeight(SETTING_ITEM_HIGHT);
            }
#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
#ifdef QT_TOUCH_SUPPORT
            pItem->SetChangeStyleOnPress(itemData.bChangeStyleOnPress);
#endif
            pItem->SetChangeStyleOnSelect(itemData.bChangeStyleOnSelect);
#endif

            if (i == m_iConfigedIndex && !pItem->IsConfiged())
            {
                pItem->SetConfiged(true);
            }

            pItem->show();

            //设置按下背景，按下背景有的行要，有的行不要，framelist实现不了，所以要定制，在item上实现
            if (iAddCount == 1)
            {
                pItem->SetClickBg(m_strItemClickTopBG);
            }
            else if (iAddCount == iPageItemCount)
            {
                pItem->SetClickBg(m_strItemClickBottomBG);
            }
            else
            {
                pItem->SetClickBg(m_strItemClickMidBG);
            }

            m_pFramelist->AddItem(pItem);
            iAddCount++;
            pItem->CListItem::SetAction(this);
        }

        m_pFramelist->SetTotalDataCount(m_vecItemData.size());

        if (m_iCurrentIndex >= nDataIndex
                && m_iCurrentIndex < nDataIndex + m_pFramelist->GetTotalItemCount())
        {
            m_pFramelist->SetItemFocusByIndex(m_pFramelist->GetIndexByDataIndex(m_iCurrentIndex));
        }
        else
        {
            m_pFramelist->SelectItemByDataIndex(INVALID_ITEM_INDEX);
            m_pFramelist->setFocus();
        }
    }
}
#endif
