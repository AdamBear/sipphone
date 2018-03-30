#include "wifi_inc.h"

#include "dsklog/log.h"

namespace
{
#ifdef _T49
#define WIFI_ITEM_HIGHT                 98
#define WIFI_SEARCH_ITEM_HIGHT          86
#define SETTING_ITEM_HIGHT              84
// item width
#define ITEM_WIDTH                      838
#else
#define SETTING_ITEM_HIGHT              48
#endif
}

Q_DECLARE_METATYPE(WifiAddData);

WifiDelegate::WifiDelegate()
    : SubpageListDeleagate()
#ifdef _T49
    , m_iContentHeight(0)
    , m_iCurOff(0)
    , m_bTestMode(false)
#else
    , m_iCurDataIndex(0)
#endif // _T49
{
}

WifiDelegate::~WifiDelegate()
{
#ifdef _T49
    m_bTestMode = false;
#else
    m_iCurDataIndex = 0;
#endif // _T49
}

void WifiDelegate::OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction)
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

    return SubpageListDeleagate::OnItemAction(pFrameList, pItem, nAction);
}

// 刷新控件状态 : 编辑属性
void WifiDelegate::UpdateItemStatus(bool bEanbled)
{
    QWPVector::iterator iter = m_vecItemData.begin();
    for (; iter != m_vecItemData.end(); ++iter)
    {
        if (iter->second == NULL)
        {
            continue;
        }

        iter->second->setEnabled(bEanbled);
    }
}

void WifiDelegate::OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount)
{
    SETTINGUI_INFO("[WifiDelegate::OnLoadData] dIdx[%d], dCnt[%d], itemCnt[%d]"
                   , nDataIndex, nDataCount, m_vecItemData.size());

    // hide all qwidget
    if (NULL != m_pFramelist)
    {
        QWPVector::iterator iter = m_vecItemData.begin();
        for (; iter != m_vecItemData.end(); ++iter)
        {
            if (iter->second != NULL)
            {
                iter->second->hide();
            }
        }
    }

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
        //m_pFramelist->SetItemAttach();

        int iPageItemCount = m_pFramelist->GetMaxPageItemCount();
        int iAddCount = 1;
        //重新添加数据
        for (int i = nDataIndex, j = 0; i < nDataIndex + nDataCount && i < m_vecItemData.size()
                && j < iPageItemCount; i++, j++)
        {
            QWidgetPair itemData = m_vecItemData[i];
            itemData.iIndex = i;
            CSettingItem * pItem =  g_SettingItemManger->CreateItemByType(itemData.eType, m_pItemParent,
                                    itemData);
            if (NULL == pItem)
            {
                continue;
            }

            pItem->setObjectName(QString("CSettingItem%1").arg(i));

#ifdef _T49
            if (SETTING_ITEM_WIFI == itemData.eType)
            {
                pItem->setFixedHeight(WIFI_ITEM_HIGHT);
            }
            else if (WIRELESS_ITEM_SEARCH == itemData.eType)
            {
                pItem->setFixedHeight(WIFI_SEARCH_ITEM_HIGHT);
            }
            else// if ( == itemData.eType)
#endif
            {
                pItem->setFixedHeight(SETTING_ITEM_HIGHT);
            }
#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
#ifdef QT_TOUCH_SUPPORT
            pItem->SetChangeStyleOnPress(itemData.bChangeStyleOnPress);
#endif
            pItem->SetChangeStyleOnSelect(itemData.bChangeStyleOnSelect);
#endif
            //翻页时纠正选中项
            if (pItem->IsFocus() && pItem->GetDataIndex() != m_iCurrentIndex)
            {
                pItem->FocusItem(false);
            }

            if (i != m_iConfigedIndex && pItem->IsConfiged())
            {
                pItem->SetConfiged(false);
            }
            else if (i == m_iConfigedIndex && !pItem->IsConfiged())
            {
                pItem->SetConfiged(true);
            }

            pItem->show();

            //设置按下背景，按下背景有的行要，有的行不要，framelist实现不了，所以要定制，在item上实现
#ifdef _T49
            if (m_bBgColor)
            {
                pItem->SetClickBgBGColor(true, m_clickBgColor);
            }
            else
#endif
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
#ifdef _T49
#ifdef QT_SLIP_GESTURE_SUPPORT
        if (0 != m_iContentHeight)
        {
            CSlipFrameListArea & framelistArea = m_pFramelist->GetSlipArea();
            framelistArea.SetContentHeight(m_iContentHeight);
        }
#endif
#endif // _T49
    }

    SETTINGUI_INFO("[WifiDelegate::OnLoadData] end");
}

//刷新列表，重新加载数据
void WifiDelegate::UpdateList()
{
    SubpageListDeleagate::UpdateList();
#ifdef _T49
#ifdef QT_SLIP_GESTURE_SUPPORT
    if (0 != m_iCurOff)
    {
        m_pFramelist->GetSlipArea().RefreshArea(m_iCurOff);
    }
#endif
#else

    if (NULL != m_pFramelist)
    {
        m_pFramelist->setFocus(Qt::ActiveWindowFocusReason);//页面捕获去焦点，焦点始终给list

        if (0 != m_iCurDataIndex)
        {
            m_pFramelist->RefreshData(m_iCurDataIndex);
        }
    }
#endif
}

void WifiDelegate::RecordOffSet()
{
    CListItem * pItem =  m_pFramelist->GetItemByIndex(0);
    if (NULL == pItem)
    {
        return;
    }

    m_iCurDataIndex = pItem->GetDataIndex();
    SETTINGUI_INFO("[WifiDelegate::RecordOffSet()] cur data idx[%d]", m_iCurDataIndex);
}

void WifiDelegate::ClearOffSet()
{
    //m_iCurrentIndex = 0;
    m_iCurDataIndex = 0;
}

#ifdef _T49
//设置列表基本属性
void WifiDelegate::InitSettingListStyle()
{
    SubpageListDeleagate::InitSettingListStyle();
}

void WifiDelegate::SetTestMode(bool bTestMode)
{
    m_bTestMode = bTestMode;
}

#ifdef QT_SLIP_GESTURE_SUPPORT
int WifiDelegate::GetRecordOffSet()
{
    m_iCurOff;
}

void WifiDelegate::SetRecordOffSet(int iOffSet)
{
    if (iOffSet <= 0)
    {
        m_iCurOff = iOffSet;
    }
}

void WifiDelegate::RecordOffSet()
{
    m_iCurOff = m_pFramelist->GetSlipArea().GetOffset();
}

void WifiDelegate::ClearOffSet()
{
    m_iCurOff = 0;
}
#endif

#ifdef FRESH_WINDOW_WITH_MIN_CHANGED
//刷新列表，重新加载数据
void WifiDelegate::UpdateList()
{
    SubpageListDeleagate::UpdateList();
#ifdef QT_SLIP_GESTURE_SUPPORT
    if (0 != m_iCurOff)
    {
        m_pFramelist->GetSlipArea().RefreshArea(m_iCurOff);
    }
#endif
}
#endif

#ifdef QT_SLIP_GESTURE_SUPPORT
void WifiDelegate::SetContentHeight(int iContentHeight)
{
    SETTINGUI_INFO("[WifiDelegate::SetContentHeight][%d]"
                   , iContentHeight);
    m_iContentHeight = iContentHeight;
}
#endif

#ifdef QT_SLIP_GESTURE_SUPPORT
/****************************** ISlipAreaDelegate ************************/
// 初始化滑动相关信息
void WifiDelegate::InitSlipInfo(CFrameListBase * pFrameList)
{
}

// 根据位置获取列表项
CListItem * WifiDelegate::PrepareItem(CFrameListBase * pFrameList, int nDataIndex)
{
    SETTINGUI_INFO("[WifiDelegate::PrepareItem]DataIndex[%d] Count[%d]"
                   , nDataIndex, m_vecItemData.size());

    if (m_vecItemData.size() <= nDataIndex)
    {
        return NULL;
    }

    QWidgetPair itemData = m_vecItemData[nDataIndex];
    CSettingItem * pItem =  g_SettingItemManger->CreateItemByType(itemData.eType, m_pItemParent,
                            itemData);
    if (pItem == NULL)
    {
        return NULL;
    }
#ifdef TEMP_SLIP_SOLUTION
    if (SETTING_ITEM_NORMAL == itemData.eType)
    {
        qCheckBox * pChk = new qCheckBox();
        if (NULL != pChk)
        {
            pChk->SetData(itemData.bAddMark);
            pItem->SetContentWgt(pChk);
            pItem->ContentWgt()->setParent(pItem);

            CDlgWifiBase * pWifiBase = qobject_cast<CDlgWifiBase *>(UIManager_GetPrivateDlg(DLG_CDlgWifiBase));
            if (NULL != pWifiBase)
            {
                connect(pChk->GetRadioButtonOn(), SIGNAL(toggled(bool)), pWifiBase,
                        SLOT(OnSwitchDeviceChange(bool)));
            }
        }
    }
#endif

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

    //设置按下背景，按下背景有的行要，有的行不要，framelist实现不了，所以要定制，在item上实现
    if (m_bBgColor)
    {
        pItem->SetClickBgBGColor(true, m_clickBgColor);
    }

    pItem->CListItem::SetAction(this);
    pItem->SetDataIndex(nDataIndex);

    SETTINGUI_INFO("[WifiDelegate::PrepareItem] end");

    return pItem;
}

// 刷新列表项数据
bool WifiDelegate::RefreshItem(CFrameListBase * pFrameList, CListItem * pItem)
{
    SETTINGUI_INFO("[WifiDelegate::RefreshItem]");
    if (NULL == pItem || NULL == pFrameList)
    {
        return false;
    }

    return true;
}

// 滑动区域发生变化
void WifiDelegate::OnSlipAreaChanged(CFrameListBase * pFrameList, int nOffset)
{
    SETTINGUI_INFO("[WifiDelegate::OnSlipAreaChanged offSet[%d]] m_offset[%d]"
                   , nOffset, m_pFramelist->GetSlipArea().GetOffset());
    //m_nOffset = nOffset;
}

// 不定长列表项场景中，根据偏移量获取偏移量所在Item的位置信息(rcItem)及数据索引(返回值)
int WifiDelegate::GetItemInfo(CFrameListBase * pFrameList, int nOffset, QRect & rcItem)
{
    SETTINGUI_INFO("[WifiDelegate::GetItemInfo offSet[%d], m[%d]]"
                   , nOffset, m_pFramelist->GetSlipArea().GetOffset());
#ifdef FRESH_WINDOW_WITH_MIN_CHANGED
    int iOff = -nOffset;
    int iIdx = 0;
    if (iOff <= 0)
    {
        rcItem.setRect(0, 0, ITEM_WIDTH, SETTING_ITEM_HIGHT);
        return 0;
    }

    int iNumOfCommonItem = IsTestMode() ? 1 : 3;
    if (iOff / 85 < iNumOfCommonItem)
    {
        int iN = iOff / 85;
        iOff -= iN * 85;

        rcItem.setRect(0, -iOff, ITEM_WIDTH, SETTING_ITEM_HIGHT);
        return iN;
    }
    iIdx += iNumOfCommonItem;
    iOff -= 85 * iNumOfCommonItem;
    if (iOff < 87)
    {
        rcItem.setRect(0, -iOff, ITEM_WIDTH, WIFI_SEARCH_ITEM_HIGHT);
        return iIdx;
    }

    iOff -= 87;

    int iN = iOff / 99;
    iOff -= iN * 99;

    rcItem.setRect(0, -iOff, ITEM_WIDTH, WIFI_ITEM_HIGHT);
    return (iIdx + iN + 1);
#else
    return 0;
#endif
}
#endif

/*************************************************************************/
void WifiDelegate::SetItemDataByDataIndex(QWidgetPair qPair, int iDateIdx,
        int eItemType /*= SETTING_ITEM_WIFI*/)
{
    if (iDateIdx >= 0 && iDateIdx < m_vecItemData.size())
    {
        m_vecItemData[iDateIdx] = qPair;

        CListItemPtr pNowItem = m_pFramelist->GetItemByDataIndex(iDateIdx);
        if (NULL != pNowItem && pNowItem->inherits("CSettingWifiItem"))
        {
            CSettingWifiItem * pWifiItem = static_cast<CSettingWifiItem *>(pNowItem);
            if (NULL != pWifiItem)
            {
                pWifiItem->SetItemData(qPair);
            }
        }
        else if (NULL != pNowItem && pNowItem->inherits("WirelessSearchStatisticsItem"))
        {
            WirelessSearchStatisticsItem * pSearchItem = static_cast<WirelessSearchStatisticsItem *>(pNowItem);
            if (NULL != pSearchItem)
            {
                pSearchItem->SetItemData(qPair);
            }
        }

        if (SETTING_ITEM_WIFI == eItemType)
        {
            WifiAddData wifiData = qPair.appendVariantData.value<WifiAddData>();
            if (INT_DISAPPEAR == wifiData.eType)
            {
                wifiData.eType = INT_NONE;
                wifiData.strNote = "";
                m_vecItemData[iDateIdx].appendVariantData.setValue(wifiData);
            }
        }
    }
}

// protected
void WifiDelegate::InitFrameList()
{
}

void WifiDelegate::UnInitFrameList()
{
}

// 获取真正代理指针
IFrameListDelegatePtr WifiDelegate::GetDelegate() const
{
    return (IFrameListDelegatePtr)this;
}
#else

/*************************************************************************/
void WifiDelegate::SetItemDataByDataIndex(QWidgetPair qPair, int iDateIdx,
        int eItemType /*= SETTING_ITEM_WIFI*/)
{
    if (iDateIdx >= 0 && iDateIdx < m_vecItemData.size())
    {
        m_vecItemData[iDateIdx] = qPair;

        CListItemPtr pNowItem = m_pFramelist->GetItemByDataIndex(iDateIdx);
        if (NULL != pNowItem && pNowItem->inherits("CSettingWifiItem"))
        {
            CSettingWifiItem * pWifiItem = static_cast<CSettingWifiItem *>(pNowItem);
            if (NULL != pWifiItem)
            {
                pWifiItem->SetItemData(qPair);
            }
        }
        else if (NULL != pNowItem && pNowItem->inherits("WirelessSearchStatisticsItem"))
        {
            WirelessSearchStatisticsItem * pSearchItem = static_cast<WirelessSearchStatisticsItem *>(pNowItem);
            if (NULL != pSearchItem)
            {
                pSearchItem->SetItemData(qPair);
            }
        }

        if (SETTING_ITEM_WIFI == eItemType)
        {
            WifiAddData wifiData = qPair.appendVariantData.value<WifiAddData>();
            if (INT_DISAPPEAR == wifiData.eType)
            {
                wifiData.eType = INT_NONE;
                wifiData.strNote = "";
                m_vecItemData[iDateIdx].appendVariantData.setValue(wifiData);
            }
        }
    }
}

void WifiDelegate::SetItemData(const QWPVector & vec)
{
    m_vecItemData.clear();
    m_vecItemData = vec;
}
#endif // _T49
