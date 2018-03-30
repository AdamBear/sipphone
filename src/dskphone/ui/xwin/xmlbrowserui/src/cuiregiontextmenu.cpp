#include "cuiregiontextmenu.h"
#include "keymap.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "modxmlbrowserui.h"
#include "xmlbrowseruicommon.h"
#include "baseui/include/menulistitem.h"
#include "baseui/include/listviewitemfactory.h"
#include "xwindow/xListView.h"

#if XML_BROWSER_SUPPORT_UI
////////////////////////////////////////////////////////////////////////////////
CUIRegionTextMenu::CUIRegionTextMenu()
    : CUIRegionBaseListScreen()
{
    m_vecItemData.clear();
}

CUIRegionTextMenu::~CUIRegionTextMenu()
{

}

void CUIRegionTextMenu::Update(void * pMenuData)
{
    if (NULL == pMenuData)
    {
        return;
    }

    XBTextMenuData * pNewData = static_cast<XBTextMenuData *>(pMenuData);

    if (NULL == m_pTxtTitle
            || NULL == m_pListView || NULL == pNewData)
    {
        return;
    }

    m_stBaseData.m_strTitle = pNewData->stAttr.strTitle;
    m_stBaseData.m_bIsWrapTitle = pNewData->stAttr.bWrapTitle;
    m_stBaseData.m_nDefaultIndex = pNewData->stAttr.iDefaultIndex;
    LoadMenuItemData(pNewData);
    m_iQuitTime = pNewData->stAttr.iTimeOut;
    m_bLock = pNewData->stAttr.bLockIn;

#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
    m_vecGlobalSoftkey = pNewData->vecSoftkey;
    const VecSoftkeyParam & vecPrivateSoftkey = GetSoftkeyByIndex(0);
    if (vecPrivateSoftkey.size() > 0)
    {
        SetSoftkey(vecPrivateSoftkey);
    }
    else
#endif
    {
        SetSoftkey(pNewData->vecSoftkey);
    }
}

const VecSoftkeyParam & CUIRegionTextMenu::GetFocusSoftkey()
{
    int nIndex = GetCurrentItemIndex();
    return GetSoftkeyByIndex(nIndex);
}

const VecSoftkeyParam & CUIRegionTextMenu::GetSoftkeyByIndex(int nIndex)
{
    if (nIndex >= 0 && nIndex < m_vecItemData.size())
    {
        return m_vecItemData[nIndex].m_vecSoftkey;
    }
    return sVecSoftParam;
}

void CUIRegionTextMenu::SetDefaultSoftkey(VecSoftkeyParam & softkeyParam)
{
    softkeyParam.clear();

    softkeyParam.push_back(TypeToSoftInfo(XB_ST_EXIT));
    softkeyParam.push_back(TypeToSoftInfo(XB_ST_EMPTY));
    softkeyParam.push_back(TypeToSoftInfo(XB_ST_EMPTY));
    softkeyParam.push_back(TypeToSoftInfo(XB_ST_SELECT));
}

bool CUIRegionTextMenu::LoadMenuItemData(XBTextMenuData * pMenuData)
{
    if (NULL == pMenuData)
    {
        return false;
    }

    TextMenuItemData vecData;
    vecData.m_bIsWrap = pMenuData->stAttr.bWrapList;

    //解析数据

    if (STYLE_NUMBERED == pMenuData->stAttr.eStyle)
    {
        //显示序号
        vecData.m_bShowIndex = true;
    }

    m_vecItemData.clear();
    List_Menu_Item::iterator it = pMenuData->listItem.begin();
    for (/* */; it != pMenuData->listItem.end(); ++it)
    {
        if (NULL != *it)
        {
            vecData.m_strText = (*it)->strPrompt;
#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
            vecData.m_vecSoftkey = (*it)->vecSoftkey;
#endif
            m_vecItemData.push_back(vecData);
        }
    }
    return true;
}

void CUIRegionTextMenu::InitListViewLayout(xListView * pListView)
{
    CUIRegionBaseListScreen::InitListViewLayout(pListView);
    if (NULL == pListView)
    {
        return;
    }

    pListView->SetTotalDataCount(m_vecItemData.size());
}

xListViewItem * CUIRegionTextMenu::PrepareItem(xListView * pListView, int nDataIndex,
        xListViewItem * pItem)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return NULL;
    }

    CMenuListItemPtr pMenuItem = CMenuListItem::GetMenuListItem(pItem);

    if (NULL == pMenuItem)
    {
        pMenuItem = static_cast<CMenuListItemPtr>(g_ItemFactory.ApplyItem(
                        CMenuListItem::GetMenuItemType()));
    }


    return pMenuItem;
}

bool CUIRegionTextMenu::RefreshItem(xListView * pListView, xListViewItem * pItem)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return false;
    }

    CMenuListItemPtr pMenuItem = CMenuListItem::GetMenuListItem(pItem);

    if (NULL == pMenuItem)
    {
        return false;
    }

    pMenuItem->SetAction(this);

    int nDataIndex = pItem->GetDataIndex();

    if (nDataIndex >= 0 && nDataIndex < m_vecItemData.size())
    {
        const TextMenuItemData & stMenuData = m_vecItemData[nDataIndex];
        pMenuItem->SetShowIndex(stMenuData.m_bShowIndex);
        pMenuItem->SetMenu(stMenuData.m_strText);
    }

    return true;
}

bool CUIRegionTextMenu::OnKeyPress(int nKey)
{
    if (CUIRegionBase::OnKeyPress(nKey))
    {
        return true;
    }

    if (NULL == m_pListView)
    {
        return false;
    }

    switch (nKey)
    {
    case PHONE_KEY_1:
    case PHONE_KEY_2:
    case PHONE_KEY_3:
    case PHONE_KEY_4:
    case PHONE_KEY_5:
    case PHONE_KEY_6:
    case PHONE_KEY_7:
    case PHONE_KEY_8:
    case PHONE_KEY_9:
        {
            // 0
            int nIndex = (nKey - PHONE_KEY_1);
            if (nIndex >= m_pListView->GetTotalDataCount())
            {
                break;
            }

            xListViewItemPtr pItem = m_pListView->GetItemByDataIndex(nIndex);
            if (NULL == pItem)
            {
                SetPageDataIndex(nIndex);
                m_pListView->RefreshData(nIndex);
            }
            m_pListView->FocusItemByDataIndex(nIndex);
            UpdateIndex();
            return true;
        }
        break;
    default:
        break;
    }
    return false;
}

#endif
