#include "settinguiadapter.h"
#include "basesettinguidelegate.h"
#include "dlgsettingui.h"
#include "setuidatadefine.h"
#include "imeedit/ximeedit.h"
#include "common/common_data_define.h"
#include "keymap.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "xwindow/xListView.h"
#include "xwindow/xViewHandler.h"
#include "xwindow/xWindowManager.h"
#include "xwindow/xcombobox.h"

#include "settinguiwizardmanager.h"
#include "settinguidialogmanager.h"
#include "settingui/include/settinguidialogmanager.h"
#include "settingui/include/pageiddefine.h"

// item class
#include "baseui/include/listviewitemfactory.h"
#include "baseui/include/detaillistitem.h"
#include "baseui/include/detaillistitembase.h"
#include "baseui/include/menulistitem.h"
#include "baseui/include/editlistitem.h"
#include "baseui/include/comboboxlistitem.h"
#include "baseui/include/radiolistitem.h"
#include "btchannelcontrollitem.h"
#include "baseui/include/inputmethod.h"

// item data class
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguilogic/src/settinguilineedititemdata.h"
#include "settinguilogic/src/settinguimenuitemdata.h"
#include "settinguibtchannelcontrollitemdata.h"
// tranc
#include "settinguilogic/include/setitemdatatype2baseitemtypefactory.h"
// setting item data 和 base item数据转换
#include "itemdatavaluetransfactory.h"
#include "itemdatavaluetrans.h"
// setting debug macro file
#include "settinguilogic/include/settingdefine.h"
#include "dsklog/log.h"
#include "settingui/src/uicommonfun.h"

namespace
{
#if IF_EDIT_COMBOBOX_SHOW_TWO_LINE && IF_PAGEDATA_SUPPORT_ITEM_PROPERTY
#define DEFAULT_PAGE_ITEM_COUNT 1
#define DEFAULT_ITEM_HEIGHT     38
#endif
}

CSettingUIAdapter::CSettingUIAdapter()
    : m_pSettingUIDelegate(NULL)
    , m_iPreviousFocusIndex(-1)
#if IF_EDIT_COMBOBOX_SHOW_TWO_LINE && IF_PAGEDATA_SUPPORT_ITEM_PROPERTY
    , m_nOldItemHeight(0)
#endif
{
}

CSettingUIAdapter::~CSettingUIAdapter()
{
    m_pSettingUIDelegate = NULL;
}

void CSettingUIAdapter::BindDelegate(CSettingUIDelegateBase * pDelegate)
{
    if (NULL == pDelegate)
    {
        return;
    }

    m_pSettingUIDelegate = pDelegate;
}

void CSettingUIAdapter::UnBindDelegate()
{
    if (NULL != m_pSettingUIDelegate)
    {
#if IF_EDIT_COMBOBOX_SHOW_TWO_LINE
        OnChangeListViewParam(false);
#endif
        m_pSettingUIDelegate = NULL;
    }
}

void CSettingUIAdapter::InitListViewLayout(xListView * pListView)
{
    if (NULL == pListView || NULL == m_pSettingUIDelegate)
    {
        return;
    }

    // 设置Title
    RefreshTitle();

    const yl::string & strHint = m_pSettingUIDelegate->GetEmptyHint();

    if (strHint.empty())
    {
        pListView->SetHint(_LANG(TRID_NONE));
    }
    else
    {
        pListView->SetHint(strHint);
    }

    int nTotalData = m_pSettingUIDelegate->GetTotalDataCount();
    pListView->SetTotalDataCount(nTotalData);


#if IF_EDIT_COMBOBOX_SHOW_TWO_LINE
    OnChangeListViewParam(true);
#else
    // 设置一页Item数量
    pListView->SetPageItemCount(m_pSettingUIDelegate->GetPageItemCount());
#endif

#if IF_SETTING_DISPLAY_DEBUG
    SETTINGUI_INFO("----------- [CSettingUIAdapter::InitListViewLayout] tot[%d], list[%p] ====="
                   , nTotalData, pListView);
#endif
}

void CSettingUIAdapter::RefreshListView()
{
    AdjustFirstShowItemIndex();

    xListViewAdapter::RefreshListView();

    RefreshTitle();

    RefreshSoftkey();
}

void CSettingUIAdapter::AdjustFirstShowItemIndex()
{
    if (NULL == m_pListView || NULL == m_pSettingUIDelegate || 0 == m_pSettingUIDelegate->GetTotalDataCount())
    {
        return;
    }

    int nFirstViewIndex = m_pSettingUIDelegate->GetPageDataIndex();
    int nFocusIndex = m_pSettingUIDelegate->GetFocusIndex();

    //焦点项 > 总项
    if (nFocusIndex >= m_pSettingUIDelegate->GetTotalDataCount() || nFocusIndex <= 0)
    {
        SetFocusIndex(0);
        SetPageDataIndex(0);
        return;
    }

    // 焦点项 < 起始项
    if (nFocusIndex < nFirstViewIndex || nFirstViewIndex != GetPageDataIndex())
    {
        SetPageDataIndex(nFirstViewIndex);
        //根据起始项更新焦点项
        nFirstViewIndex = m_pSettingUIDelegate->GetPageDataIndex();
    }

    // 若焦点项超过1页显示Item量，需要先刷新到足够显示Focus项的位置
    int nLastViewIndex = nFirstViewIndex + m_pListView->GetPageItemCount();
    if (nFocusIndex >= nLastViewIndex)
    {
        SetPageDataIndex(nFocusIndex);
        //根据起始项更新焦点项
        nFirstViewIndex = m_pSettingUIDelegate->GetPageDataIndex();
    }

    int nOldListViewFocusIndex = GetFocusIndex();
    int nNewListViewFocusIndex = nFocusIndex - nFirstViewIndex;
    if (nOldListViewFocusIndex != nNewListViewFocusIndex)
    {
        SetFocusIndex(nNewListViewFocusIndex);
    }

    xListViewStylePtr pStyle = m_pListView->GetStyle();
    if (NULL == pStyle || pStyle->IsScrollByPage())
    {
        return;
    }

    //处理 起始项 + 页数据量 > 总数(起始项前移，焦点项后移)
    int nTotalIndex = m_pSettingUIDelegate->GetTotalDataCount();
    int nPageIndex = m_pListView->GetPageItemCount();
    if (nFirstViewIndex + nPageIndex - 1 >= nTotalIndex)
    {
        int nOff = nFirstViewIndex - (nTotalIndex - nPageIndex);
        int nNewFirstIndex = nFirstViewIndex - nOff;
        int nRealOff = (nNewFirstIndex > 0) ? nOff : (nNewFirstIndex + nOff);
        nNewFirstIndex = (nNewFirstIndex > 0) ? nNewFirstIndex : 0;
        SetFocusIndex(GetFocusIndex() + nRealOff);
        SetPageDataIndex(nNewFirstIndex);
    }
}


xListViewItem * CSettingUIAdapter::PrepareItem(xListView * pListView, int nDataIndex,
        xListViewItem * pItem)
{
#if IF_SETTING_DISPLAY_DEBUG
    SETTINGUI_INFO("----------- [CSettingUIAdapter::PrepareItem] create item nDataIndex[%d], list[%p] ====="
                   , nDataIndex, pListView);
#endif
    if (NULL == pListView || pListView != m_pListView || NULL == m_pSettingUIDelegate)
    {
        return NULL;
    }

    CSettingUIItemDataBasePtr pItemInfo = m_pSettingUIDelegate->GetItemDataByDataIndex(nDataIndex);
    if (NULL == pItemInfo)
    {
        return NULL;
    }
    int nItemType = g_ItemDataTypeTransFactory.GetItemDataTypeTrans(pItemInfo->GetItemType());
    if (0 == nItemType)
    {
        nItemType = g_ItemDataTypeTransFactory.GetDefaultItemDataTypeTrans();
    }
#if IF_SETTING_DISPLAY_DEBUG
    SETTINGUI_INFO("----------- [CSettingUIAdapter::PrepareItem] itemType[%d], icontext[%d] ====="
                   , nItemType, CIconTextListItem::GetIconTextListType());
#endif

    xListViewItem* pNewItem = NULL;
    if (NULL != pItem && pItem->GetType() == nItemType)
    {
        pNewItem = pItem;
    }
    else
    {
        pNewItem = g_ItemFactory.ApplyItem(nItemType);
    }

    if (NULL != pNewItem)
    {
        pNewItem->SetAction(this);
        CEditListItemPtr pEditItem = CEditListItem::GetEditListItem(pNewItem);
        if (NULL != pEditItem)
        {
            pEditItem->SetValue(yl::string(""));
            pEditItem->DisconnectIMEChanged(this, static_cast<slotMethod>(&CSettingUIAdapter::OnIMEChanged));
            pEditItem->ConnectIMEChanged(this, static_cast<slotMethod>(&CSettingUIAdapter::OnIMEChanged));
        }
    }

    return pNewItem;
}

bool CSettingUIAdapter::RefreshItem(xListView * pListView, xListViewItem * pItem)
{
#if IF_SETTING_DISPLAY_DEBUG
    static int ii = 0;
    SETTINGUI_INFO("----------- [CSettingUIAdapter::RefreshItem] refresh item[%d] =====", ii++);
#endif
    if (NULL == pListView || pListView != m_pListView
            || NULL == m_pSettingUIDelegate || NULL == pItem)
    {
        return false;
    }

    int nDataIndex = pItem->GetDataIndex();

    // 设置Item数据
    CSettingUIItemDataBasePtr pItemInfo = m_pSettingUIDelegate->GetItemDataByDataIndex(nDataIndex);
    if (NULL == pItemInfo)
    {
        return false;
    }

    SetItemData(pItem, pItemInfo, nDataIndex);

    return true;
}

void CSettingUIAdapter::OnCleanContent(xListView * pListView)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return;
    }

    RefreshSoftkey();
}

void CSettingUIAdapter::OnItemAction(xListView * pListView, xListViewItem * pItem, int nAction)
{
    xListViewAdapter::OnItemAction(pListView, pItem, nAction);

    if (NULL == pListView || NULL == pItem || pListView != m_pListView || NULL == m_pSettingUIDelegate)
    {
        return;
    }

    int nItemDataIndex = pItem->GetDataIndex();

    int nPreFocusIndex = m_iPreviousFocusIndex;
    bool bTheSameIndex = (nPreFocusIndex == nItemDataIndex);
#if IF_SETTING_FOCUS
    SETTINGUI_INFO("--- ################# [CSettingUIAdapter::OnItemAction] action[%d], item[%p] ======================&&&&&&&&&&&&&&"
                   , nAction, pItem);
    SETTINGUI_INFO("--- ################# [CSettingUIAdapter::OnItemAction] pre[%d], cur[%d], setuiadapter[%p] ======================&&&&&&&&&&&&&&"
                   , nPreFocusIndex, nItemDataIndex, this);
#endif

    // 更新前一项焦点序号
    if (NULL != pItem
            && (LIST_VIEW_ITEM_ACTION_FOCUS == nAction || LIST_VIEW_ITEM_ACTION_SELECT == nAction))
    {
        m_iPreviousFocusIndex = nItemDataIndex;
    }
    // 焦点切换
    if (LIST_VIEW_ITEM_ACTION_FOCUS == nAction
            && (!bTheSameIndex || pItem->hasFocus() || pItem->containFocus()))
    {
        if (!bTheSameIndex)
        {
            // 更新焦点项
            m_pSettingUIDelegate->SetFocusIndex(nItemDataIndex);
            // 保存前一项数据到内存中
            m_pSettingUIDelegate->SetItemDataValueByDataIndex(m_iPreviousFocusIndex
                    , GetItemValueByDataIndex(m_iPreviousFocusIndex));
            m_pSettingUIDelegate->OnAction(SET_ACTION_TYPE_FOCUS, nItemDataIndex);
            // focus操作
            UpdateIndex();
            m_pSettingUIDelegate->SetPageDataIndex(GetPageDataIndex());
            RefreshSoftkey();
        }
    }

    // 选中项
    if (LIST_VIEW_ITEM_ACTION_SELECT == nAction)
    {
        if (!bTheSameIndex)
        {
            // 更新焦点项
            m_pSettingUIDelegate->SetFocusIndex(nItemDataIndex);
        }

        UpdataItemValue(pItem);

        // 保存当前项数据到内存中
        m_pSettingUIDelegate->SetItemDataValueByDataIndex(nItemDataIndex
                , GetItemValueByDataIndex(nItemDataIndex));
        // select操作
        m_pSettingUIDelegate->OnAction(SET_ACTION_TYPE_CLICK, nItemDataIndex);

    }
}

void CSettingUIAdapter::RefreshSoftkey()
{
    if (NULL == m_pListView)
    {
        return;
    }


    CDlgSettingUI * pDlg = (CDlgSettingUI *)m_pListView->getRootView();
    if (NULL == pDlg)
    {
        return;
    }

    CArraySoftKeyBarData vecSoftKey;
    // 初始化
    for (int i = 0; i < SOFTKEY_NUM; ++i)
    {
        CSoftKeyBarData objSoftkeyData;

        objSoftkeyData.m_iType = ST_EMPTY;
        objSoftkeyData.m_strSoftkeyTitle = "";

        vecSoftKey.push_back(objSoftkeyData);
    }

    GetPageXmlSoftkey(vecSoftKey);
    GetCommonSoftkey(vecSoftKey);

    GetPrivateSoftkey(vecSoftKey);

    if (NULL != m_pSettingUIDelegate && !m_pSettingUIDelegate->IsPageEditAble()
            && SOFTKEY_NUM == vecSoftKey.size())
    {
        for (int i = 0; i < SOFTKEY_NUM; ++i)
        {
            int nType = vecSoftKey[i].m_iType;
            if (!m_pSettingUIDelegate->GetShowStatus())
            {
                if (ST_BACK != nType
#ifdef IF_SETTING_WIZARD
                        && ST_PREVIOUS != nType
#endif
                        && ST_NEXT != nType)
                {
                    vecSoftKey[i].m_iType = ST_EMPTY;
                    vecSoftKey[i].m_strSoftkeyTitle = "";
                }
            }
            else if (!m_pSettingUIDelegate->IsPageEditAble())
            {
                if (ST_BACK != nType && ST_ENTER != nType && ST_VIEW != nType
#ifdef IF_SETTING_WIZARD
                        && ST_PREVIOUS != nType
#endif
                        && ST_NEXT != nType)
                {
                    vecSoftKey[i].m_iType = ST_EMPTY;
                    vecSoftKey[i].m_strSoftkeyTitle = "";
                }
            }
        }
    }

    pDlg->SetSoftKey(vecSoftKey);
}

void CSettingUIAdapter::RefreshTitle()
{
    yl::string strTitle = "";
    if (NULL != m_pSettingUIDelegate)
    {
        strTitle = m_pSettingUIDelegate->GetTitleName();
    }

    CSettingDialogBase * pDlgSetting = g_pSettingUIDialogManager->GetTopActiveDialog();

    if (NULL == pDlgSetting)
    {
        return;
    }

    pDlgSetting->RefreshTitle(strTitle);
}

void CSettingUIAdapter::Init()
{
}

void CSettingUIAdapter::GetPageXmlSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
    if (NULL != m_pSettingUIDelegate)
    {
        m_pSettingUIDelegate->GetPageXmlSoftkey(vecSoftKey);
    }
}

void CSettingUIAdapter::GetCommonSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
    if (vecSoftKey.size() != SOFTKEY_NUM)
    {
        return;
    }

#ifdef IF_SETTING_WIZARD
    if (NULL != g_pSettingUIWizardManager && g_pSettingUIWizardManager->IsInWizard())
    {
        return;
    }
#endif

    xListViewItemPtr pItem = m_pListView->GetFocusItem();

    CMenuListItem * pMenuItem = CMenuListItem::GetMenuListItem(pItem);
    if (NULL != pMenuItem)
    {
        vecSoftKey[3].m_iType = ST_ENTER;
        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_ENTER);
        return;
    }
}

void CSettingUIAdapter::GetPrivateSoftkey(CArraySoftKeyBarData & vecSoftkey)
{
    if (NULL != m_pSettingUIDelegate)
    {
        m_pSettingUIDelegate->GetPagePrivateSoftkey(vecSoftkey);
    }
}

#if IF_XLISTVIEW_CUSTOM_SWITCH_FOCUS
int CSettingUIAdapter::GetFocusDataIndex(int nDataIndex, bool bNext)
{
    if (NULL == m_pSettingUIDelegate || NULL == m_pListView)
    {
        return nDataIndex;
    }

    int nResultIndex = nDataIndex;

    int iScroller = bNext ? 1 : -1;
    int nTotal = m_pSettingUIDelegate->GetTotalDataCount();
    if (nTotal > 0)
    {
        do
        {
            // 边界处理
            if (nDataIndex == nTotal)
            {
                nDataIndex = 0;
            }
            else if (nDataIndex < 0)
            {
                nDataIndex = nTotal - 1;
            }

            // 判断
            CSettingUIItemDataBasePtr pBaseItemData = m_pSettingUIDelegate->GetItemDataByDataIndex(nDataIndex);
            if (NULL != pBaseItemData && pBaseItemData->IsShow())
            {
                nResultIndex = nDataIndex;
                break;
            }

            // 计时器更新
            nDataIndex += iScroller;
        }
        while (nDataIndex != nResultIndex);
    }

    return nResultIndex;
}
#endif

void CSettingUIAdapter::SetItemData(xListViewItem * pItem, CSettingUIItemDataBase * pItemData,
                                    int nDataIndex)
{
    if (NULL == pItem || NULL == pItemData)
    {
        return;
    }

#if IF_SETTING_ITEM_DEBUG
    SETTINGUI_INFO("---------------- [CSettingUIAdapter::SetItemData] dis[%s] value is [%s], index[%d] =============="
                   , pItemData->GetDisplayName().c_str(), pItemData->GetValue().c_str(), nDataIndex);
#endif

    CItemDataValueTransBase * pTrans = g_ItemDataValueTransFactory.GetItemDataValueTrans(
                                           pItemData->GetItemType());
    if (NULL != pTrans)
    {
        pTrans->TransItemDataValue(pItemData, pItem);
    }
}

yl::string CSettingUIAdapter::GetItemValueByDataIndex(int nDataIndex)
{
    xListViewPtr pListView = GetListView();
    if (NULL == pListView)
    {
        return "";
    }
    xListViewItem * pItem = pListView->GetItemByDataIndex(nDataIndex);
    if (NULL == pItem)
    {
        return "";
    }

    yl::string strValue = "";
    // label item
    CDetailListItemPtr pDetailItem = CDetailListItem::GetDetailListItem(pItem);
    if (NULL != pDetailItem)
    {
        strValue = pDetailItem->GetValue();
    }
    // combobox item
    CComboBoxListItemPtr pComboBoxItem = CComboBoxListItem::GetComboBoxListItem(pItem);
    if (NULL != pComboBoxItem)
    {
        strValue = pComboBoxItem->GetValue();
    }
    // input item
    CEditListItemPtr pEditItem = CEditListItem::GetEditListItem(pItem);
    if (NULL != pEditItem)
    {
        strValue = pEditItem->GetValue();
    }
    // bt channel controll item
#ifdef IF_SUPPORT_BLUETOOTH
    CBTChannelControllItem * pBtChannelItem = CBTChannelControllItem::GetBTChannelControllItem(pItem);
    if (NULL != pBtChannelItem)
    {
        strValue = pBtChannelItem->GetValue();
    }
#endif

#if IF_SETTING_ITEM_DEBUG
    SETTINGUI_INFO("---------------- [CSettingUIAdapter::GetItemValue] value is [%s] =============="
                   , strValue.c_str());
#endif

    return strValue;
}

void CSettingUIAdapter::UpdataItemValue(xListViewItem * pItem)
{
    if (NULL == pItem)
    {
        return ;
    }

    int nIndex = pItem->GetDataIndex();
    // 通话中不能设置
    if (m_pSettingUIDelegate->IsExistTalking())
    {
        bool bCanUpdate = false;
        if (0 != m_pSettingUIDelegate->GetItemActionByDataIndex(nIndex).compare(NS_SET::kszActionItemSave))
        {
            bCanUpdate = true;
        }

        if (!bCanUpdate)
        {
            return;
        }
    }

    //控件更新相应值RadioItem，待补充
}

void CSettingUIAdapter::SetItemTextFontSize(xListViewItem * pItem, int nTextFontSize)
{
    if (NULL == pItem)
    {
        return;
    }
}

void CSettingUIAdapter::SetComboBoxStyle(xListViewItem * pItem, const yl::string & strStyle)
{
    if (NULL == pItem && strStyle.empty())
    {
        return;
    }

    CComboBoxListItemPtr pCustomItem = CComboBoxListItem::GetComboBoxListItem(pItem);
    if (NULL != pCustomItem)
    {
        xComboBox * pCmb = pCustomItem->GetComboBox();
        if (NULL != pCmb)
        {
            pCmb->setStyle(strStyle);
        }
    }
}

#if IF_EDIT_COMBOBOX_SHOW_TWO_LINE && IF_PAGEDATA_SUPPORT_ITEM_PROPERTY
void CSettingUIAdapter::OnChangeListViewParam(bool bInit)
{
    if (NULL == m_pListView || NULL == m_pSettingUIDelegate
            || m_pSettingUIDelegate->GetTotalDataCount() <= 0)
    {
        return;
    }

    xListViewStylePtr pStyle = m_pListView->GetStyle();
    if (NULL == pStyle)
    {
        return;
    }

    if (bInit)
    {
        bool bShowSmallScreenItem = true;
        CSettingUIItemDataBase* pBaseItemData = m_pSettingUIDelegate->GetItemDataByDataIndex(0);
        if (NULL == CSettingUIComboBoxItemData::GetComboBoxDataItem(pBaseItemData)
                && NULL == CSettingUILineEditItemData::GetLineEditDataItem(pBaseItemData))
        {
            bShowSmallScreenItem = false;
        }

        CSettingUIPageDataBase* pPageData = m_pSettingUIDelegate->GetPageData();
        if (NULL == pPageData)
        {
            return;
        }

        int nOldPageItemCount = pStyle->GetPageItemCount();
        int nPageItemCount = pPageData->GetPageItemCount();
        if (nPageItemCount <= 0)
        {
            nPageItemCount = DEFAULT_PAGE_ITEM_COUNT;
        }

        if (!bShowSmallScreenItem || nPageItemCount == nOldPageItemCount)
        {
            return;
        }

        int nItemHeight = pPageData->GetItemHeight();
        if (nItemHeight <= 0)
        {
            nItemHeight = DEFAULT_ITEM_HEIGHT;
        }
        m_nOldItemHeight = pStyle->GetItemHeight();
        pStyle->SetPageItemCount(nPageItemCount);
        pStyle->SetItemHeight(nItemHeight);
    }
    else if (m_nOldItemHeight > 0)
    {
        pStyle->SetPageItemCount(m_pSettingUIDelegate->GetPageItemCount());
        pStyle->SetItemHeight(m_nOldItemHeight);
        m_nOldItemHeight = 0;
    }
}
#endif

bool CSettingUIAdapter::OnNumberKeyPress(int nKey)
{
    if (NULL == m_pListView || NULL == m_pSettingUIDelegate)
    {
        return false;
    }

    int nTotalNum = GetTotalCount();
    int nIndex = (PHONE_KEY_0 == nKey) ? (nTotalNum - 1) : (nKey - PHONE_KEY_0 - 1);

    if (nIndex < 0 || nIndex >= nTotalNum || nTotalNum <= 0
            || NULL != CEditListItem::GetEditListItem(m_pListView->GetFocusItem()))
    {
        return false;
    }

    CSettingUIItemDataBasePtr pItemData = m_pSettingUIDelegate->GetItemDataByDataIndex(nIndex);
    if (NULL == pItemData || !pItemData->IsShowIndex())
    {
        return false;
    }

    int nPreFocusIndex = GetFocusIndex();
    m_pSettingUIDelegate->SetItemDataValueByDataIndex(nPreFocusIndex,
            GetItemValueByDataIndex(nPreFocusIndex));

    xListViewItemPtr pItem = m_pListView->GetItemByDataIndex(nIndex);
    if (NULL == pItem)
    {
        SetPageDataIndex(nIndex);
        m_pListView->RefreshData(nIndex);
    }

    m_pListView->FocusItemByDataIndex(nIndex);

    UpdateIndex();

    m_pSettingUIDelegate->SetPageDataIndex(GetPageDataIndex());

    if (PHONE_KEY_0 == nKey)
    {
        return true;
    }

    return m_pSettingUIDelegate->OnAction(SET_ACTION_TYPE_CLICK, nIndex);
}

void CSettingUIAdapter::OnIMEChanged(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData)
{
    if (NULL == m_pSettingUIDelegate || NULL == m_pListView)
    {
        return;
    }

    xEditTextView* pEdit = (xEditTextView*)wParam;
    CEditListItemPtr pFocusItem = CEditListItem::GetEditListItem(m_pListView->GetFocusItem());
    if (NULL == pFocusItem || NULL == pEdit || pEdit != pFocusItem->GetEditText())
    {
        return;
    }

    int nFocusIndex = pFocusItem->GetDataIndex();
    CSettingUILineEditItemDataPtr pItemData = CSettingUILineEditItemData::GetLineEditDataItem(
                m_pSettingUIDelegate->GetItemDataByDataIndex(nFocusIndex));
    if (NULL != pItemData)
    {
        yl::string strIMEName = pEdit->GetCurrentIme();
        if (!strIMEName.empty())
        {
            pItemData->SetDefaultIME(strIMEName, false);
        }
    }
}
