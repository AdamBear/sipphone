#include "browseadapter.h"
#include "directorycontroler.h"
#include "directoryuicommon.h"
#include "bsftcontactcontroler.h"
#include "bsftcontactuihelper.h"
#include "btuccontactcontroler.h"
#include "remotecontactcontroler.h"
#include "remoteuihelper.h"
#include "directorymgr.h"
#include "dlgdirectory.h"
#include "baseui/include/menulistitem.h"
#include "languagehelper/modlangtranslate.h"
#include "uimanager/moduimanager.h"

#include "contacts/directory/include/directorymessagedefine.h"
#include "xwindow/xListView.h"
#include "xwindow/xlistviewitem.h"


CBrowseAdapter::CBrowseAdapter()
    : CDirectoryAdapter(DIR_UI_STATE_BROWSER)
    , m_nPageType(BROWSER_PAGE_TYPE_NONE)
    , m_nGroupId(knInvalidRId)
{
}

CBrowseAdapter::~CBrowseAdapter()
{
}

CBrowseAdapter * CBrowseAdapter::GetBrowserAdapter(CDirectoryAdapterPtr & pAdapter)
{
    if (pAdapter.IsEmpty() || DIR_UI_STATE_BROWSER != pAdapter->GetState())
    {
        return NULL;
    }

    return static_cast<CBrowseAdapter *>(pAdapter.Get());
}

void CBrowseAdapter::ClearHistoryPage()
{
    m_vecPage.clear();
}

bool CBrowseAdapter::IsMenuPage() const
{
    return (BROWSER_PAGE_TYPE_MENU == m_nPageType);
}

#if IF_DIR_UI_SWITCH_GROUP_IN_BROWSE
bool CBrowseAdapter::IsGroupSwitchPage() const
{
    return (BROWSER_PAGE_TYPE_GROUP_SWITCH == m_nPageType);
}
#endif

bool CBrowseAdapter::IsNormalPage() const
{
    return (BROWSER_PAGE_TYPE_NORMAL == m_nPageType);
}

bool CBrowseAdapter::IsPageHidden(int nPageType)
{
    int nSize = m_vecPage.size();

    for (int i = 0; i < nSize; ++i)
    {
        ST_BROWSER_PAGE_INFO & stPage = m_vecPage[i];

        if (nPageType == stPage.m_nType)
        {
            return true;
        }
    }

    return false;
}

int CBrowseAdapter::GetGroupId() const
{
    return m_nGroupId;
}

bool CBrowseAdapter::PopupPageType(int & nTotalCount, int & nPageDataIndex, int & nFocusIndex)
{
    int nSize = m_vecPage.size();

    if (nSize <= 0)
    {
        return false;
    }

    ST_BROWSER_PAGE_INFO & pageInfo = m_vecPage[nSize - 1];

    m_nGroupId = pageInfo.m_nGroupId;
    m_nPageType = pageInfo.m_nType;
    nTotalCount = pageInfo.m_nTotalCount;
    nPageDataIndex = pageInfo.m_nPageDataIndex;
    nFocusIndex = pageInfo.m_nFocusIndex;

    m_vecPage.removeAt(nSize - 1);
    return true;
}

bool CBrowseAdapter::PopupToMenu(int & nTotalCount, int & nPageDataIndex,
                                 int & nFocusIndex, bool bCheckCondition /*= false*/)
{
    if (BROWSER_PAGE_TYPE_MENU == m_nPageType)
    {
        return true;
    }

    CDirUIHelper* pUIHelper = CDirUIHelper::GetDirUIHelper(m_pUIHelper);
    if (NULL == pUIHelper)
    {
        return false;
    }

    int nLoop = 0;

    while (m_vecPage.size() > 0)
    {
        DEAD_LOOP_BREAK(nLoop)

        nTotalCount = 0;
        nPageDataIndex = 0;
        nFocusIndex = 0;

        if (!PopupPageType(nTotalCount, nPageDataIndex, nFocusIndex))
        {
            return false;
        }

        if (BROWSER_PAGE_TYPE_MENU == m_nPageType
                || (bCheckCondition && pUIHelper->IsBackMenuPageOver(m_nPageType, m_nGroupId)))
        {
            return true;
        }
    }

    return false;
}

bool CBrowseAdapter::ProcessAction(int nAction)
{
    if (ST_BACK == nAction)
    {
        return ProcessPageBack();
    }
    else if (ST_SEARCH == nAction)
    {
        CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(m_pUIHelper);
        CDirectoryControlerPtr pControler = GetControler();

        if (NULL == m_pListView || NULL == pUIHelper || pControler.IsEmpty())
        {
            return false;
        }

        int nGroupId = GetGroupId();

        if (IsMenuPage())
        {
            nGroupId = pUIHelper->GetLisItemId(m_pListView->GetFocusItem());
        }

        int nDirType = m_pControler->GetSearchDirType(nGroupId);

        g_DirectoryMgr.EnterSearch(nDirType, nGroupId, PHONE_KEY_NONE);

        return true;
    }

    return CDirectoryAdapter::ProcessAction(nAction);
}

bool CBrowseAdapter::ProcessPageBack()
{
    int nTotalCount = 0;
    int nPageDataIndex = 0;
    int nFocusIndex = 0;

    if (!PopupPageType(nTotalCount, nPageDataIndex, nFocusIndex))
    {
        return false;
    }

    SetTotalCount(nTotalCount);
    AdjustMenuIndex(nPageDataIndex, nFocusIndex);

    CDirectoryControlerPtr pControler = GetControler();

    if (!pControler.IsEmpty())
    {
        pControler->SetGroupId(m_nGroupId);
    }

    UpdateUI(nPageDataIndex, nFocusIndex);

    return true;
}

bool CBrowseAdapter::Init(int nDirType, int nGroupId/* = knInvalidRId*/)
{
    if (!CDirectoryAdapter::Init(nDirType, nGroupId))
    {
        return false;
    }

    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(m_pUIHelper);
    if (NULL == pUIHelper)
    {
        DIRUI_ERR("Empty UIHelper in CBrowseAdapter::Init()");
        m_nPageType = BROWSER_PAGE_TYPE_NONE;
        m_nGroupId = nGroupId;
        return true;
    }

    pUIHelper->CheckGroupID(nGroupId);

    m_nGroupId = nGroupId;

    m_nPageType = pUIHelper->GetBrowsePageType(m_nGroupId);

    CDirectoryControlerPtr pControler = GetControler();
    if (!pControler.IsEmpty())
    {
        pControler->SetGroupId(nGroupId);
    }

    return true;
}

void CBrowseAdapter::OnReactive()
{
    if (!m_pControler.IsEmpty())
    {
        m_pControler->SetGroupId(m_nGroupId);
    }

    CDirectoryAdapter::OnReactive();
}

// 处理普通按键
bool CBrowseAdapter::ProcessKeyPress(int nKey)
{
    IDirUIHelperPtr pDirUIHelper = GetUIHelper();
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);

    if (NULL == pUIHelper)
    {
        return false;
    }

    if (PHONE_KEY_CANCEL == nKey)
    {
        if (DIR_ACTION_COMMON_BACK == pUIHelper->GetActionByKeyPress(m_nState, nKey))
        {
            return ProcessPageBack();
        }

        return OnBackToMenu();
    }
    else if (PHONE_KEY_REDIAL == nKey)
    {
        int nType = pUIHelper->GetType();
        if (CALL_LOG_TYPE_LOCAL == nType
#if IF_FEATURE_METASWITCH_CALLLOG
                || CALL_LOG_TYPE_MTSW == nType
#endif
                || CALL_LOG_TYPE_BROADSOFT == nType)
        {
            return ProcessRedialKey();
        }
    }
#if IF_DIR_UI_SWITCH_GROUP_IN_BROWSE
    else if ((PHONE_KEY_LEFT == nKey || PHONE_KEY_RIGHT == nKey)
             && IsGroupSwitchPage())
    {
        int nGroupIndex = 0;
        int nGroupId = knInvalidRId;

        if (!pUIHelper->GetBrowseAdjoinGroup((PHONE_KEY_LEFT == nKey), nGroupId, nGroupIndex))
        {
            return true;
        }

        OnChangeGroup(nGroupId, true);

        return true;
    }
#endif

    return CDirectoryAdapter::ProcessKeyPress(nKey);
}

bool CBrowseAdapter::ProcessRedialKey()
{
    if (NULL == m_pListView)
    {
        return;
    }

    IDirUIHelperPtr pDirUIHelper = GetUIHelper();
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);
    if (NULL == pUIHelper)
    {
        return false;
    }

    xListViewItemPtr pFocusItem = m_pListView->GetFocusItem();
    if (NULL != pFocusItem && pUIHelper->IsFocusItemCanDial(pFocusItem))
    {
        int nItemID = pUIHelper->GetLisItemId(pFocusItem);
        if (knInvalidRId != nItemID)
        {
            CDirectoryControlerPtr pControler = GetControler();
            if (!pControler.IsEmpty())
            {
                return pControler->DialContact(nItemID);
            }
        }
    }

    return false;
}

// 处理数字键
bool CBrowseAdapter::ProcessDigitKeyPress(int nKey)
{
    if (IsMenuPage())
    {
        if (nKey > PHONE_KEY_0 && nKey <= PHONE_KEY_9)
        {
            return EnterGroupByIndex(nKey - PHONE_KEY_0 - 1);
        }
        else
        {
            return false;
        }
    }

    CDirectoryControlerPtr pControler = GetControler();

    if (pControler.IsEmpty())
    {
        return false;
    }

    if (nKey > PHONE_KEY_0 && nKey <= PHONE_KEY_9 && pControler->IsCanEnterGroupByKey(nKey)
            && EnterGroupByIndex(nKey - PHONE_KEY_0 - 1))
    {
        return true;
    }

    if (CDirectoryAdapter::ProcessDigitKeyPress(nKey))
    {
        return true;
    }

    if (pControler->IsCanEnterSearchByKey(nKey))
    {
        g_DirectoryMgr.EnterSearch(pControler->GetType(), pControler->GetGroupId(), nKey);

        return true;
    }

    if (pControler->EnterGroupByHotKey(nKey))
    {
        return true;
    }

    return false;
}

// 处理LineKey
bool CBrowseAdapter::ProcessLineKeyPress(int nKey)
{
    if (IsMenuPage())
    {
        return false;
    }

    return CDirectoryAdapter::ProcessLineKeyPress(nKey);
}

bool CBrowseAdapter::EnterGroup(int nDirType, int nGroupId)
{
    if (NULL == m_pListView || nDirType != GetDirType())
    {
        return false;
    }

    if (BROWSER_PAGE_TYPE_NONE != m_nPageType)
    {
        ST_BROWSER_PAGE_INFO pageInfo;

        pageInfo.m_nGroupId = m_nGroupId;
        pageInfo.m_nType = m_nPageType;
        pageInfo.m_nTotalCount = m_nTotalCount;
        pageInfo.m_nPageDataIndex = m_nPageDataIndex;
        pageInfo.m_nFocusIndex = m_nFocusIndex;

        m_vecPage.push_back(pageInfo);
    }

    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(m_pUIHelper);

    if (NULL != pUIHelper)
    {
        m_nPageType = pUIHelper->GetBrowsePageType(nGroupId);
    }
    else
    {
        DIRUI_WARN("CBrowseAdapter::EnterGroup(%d %d) UIHelper empty", nDirType, nGroupId);
        m_nPageType = BROWSER_PAGE_TYPE_NONE;
    }

    OnChangeGroup(nGroupId);

    return true;
}

bool CBrowseAdapter::EnterGroupByIndex(int nIndex)
{
    if (NULL == m_pListView)
    {
        return false;
    }

    int nTotalCount = m_pListView->GetTotalDataCount();

    if (nIndex < 0 || nIndex >= nTotalCount)
    {
        return false;
    }

    xListViewItemPtr pItem = m_pListView->GetItemByDataIndex(nIndex);

    if (NULL != pItem)
    {
        if (!pItem->IsItemFocus())
        {
            pItem->FocusItem(true);
            UpdateIndex();
        }

        OnItemAction(m_pListView, pItem, LIST_VIEW_ITEM_ACTION_SELECT);
        return true;
    }

    SetPageDataIndex(nIndex);
    SetFocusIndex(nIndex - m_nPageDataIndex);
    RefreshListView();

    pItem = m_pListView->GetFocusItem();

    if (NULL != pItem)
    {
        OnItemAction(m_pListView, pItem, LIST_VIEW_ITEM_ACTION_SELECT);
        return true;
    }

    return false;
}

bool CBrowseAdapter::OnChangeGroup(int nGroupId, bool bRefresh/* = true*/)
{
    CDirectoryControlerPtr pControler = GetControler();

    if (pControler.IsEmpty())
    {
        return false;
    }

    int nCurGroupId = pControler->GetGroupId();

    m_nGroupId = nGroupId;
    pControler->SetGroupId(nGroupId);

    if (nCurGroupId != nGroupId)
    {
        ResetIndex();
    }

    if (IsMenuPage())
    {
        pControler->OnEnterMenu();
    }
    else if (!pControler->PreLoadContact(nGroupId))
    {
        if (NULL != m_pListView)
        {
            m_pListView->ClearContent();
        }
        return false;
    }

    if (bRefresh)
    {
        RefreshUI();
    }

    return true;
}

bool CBrowseAdapter::OnBack()
{
    return ProcessPageBack();
}

bool CBrowseAdapter::OnBackToMenu(bool bCheckCondition /*= false*/)
{
    if (IsMenuPage() || !IsPageHidden(BROWSER_PAGE_TYPE_MENU))
    {
        return false;
    }

    int nTotalCount = 0;
    int nPageDataIndex = 0;
    int nFocusIndex = 0;

    if (!PopupToMenu(nTotalCount, nPageDataIndex, nFocusIndex))
    {
        return false;
    }

    CDirectoryControlerPtr pControler = GetControler();

    if (!pControler.IsEmpty())
    {
        pControler->SetGroupId(m_nGroupId);
    }

    SetTotalCount(nTotalCount);

    AdjustMenuIndex(nPageDataIndex, nFocusIndex);

    UpdateUI(nPageDataIndex, nFocusIndex);

    return true;
}

void CBrowseAdapter::AdjustMenuIndex(int & nPageIndex, int & nFocusIndex)
{
    if (!IsMenuPage())
    {
        return;
    }

    int nIndex = -1;
    CDirectoryControlerPtr pControler = GetControler();
    CCommonDirCtrl * pCommonDirCtrl = CCommonDirCtrl::GetCommonControler(pControler);

    if (NULL != pCommonDirCtrl)
    {
        nIndex = pCommonDirCtrl->GetGroupIndexById(m_nGroupId);
    }
#if FEATURE_BROADTOUCH_UC
    else
    {
        CBTUCContactControler * pBTUCContact = CBTUCContactControler::GetControler(pControler);

        if (NULL != pBTUCContact)
        {
            nIndex = pBTUCContact->GetGroupIndexById(m_nGroupId);
        }
    }
#endif

    if (-1 == nIndex)
    {
        return;
    }

    nPageIndex = nIndex;
    nFocusIndex = nIndex;
}

bool CBrowseAdapter::IsShowPageInfo()
{
    return IsNormalPage();
}

bool CBrowseAdapter::GetTitle(yl::string & strTitle)
{
    IDirUIHelperPtr pDirUIHelper = GetUIHelper();
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);

    if (NULL == pUIHelper)
    {
        return false;
    }

    if (IsMenuPage())
    {
        pUIHelper->GetMenuTitle(strTitle);
    }
#if IF_DIR_UI_SWITCH_GROUP_IN_BROWSE
    else if (IsGroupSwitchPage())
    {
        return false;
    }
#endif
    else
    {
        pUIHelper->GetNormalTitle(strTitle);
    }

    return true;
}

bool CBrowseAdapter::GetStateSoftKey(xListViewItem * pFocusItem, CArraySoftKeyBarData & vecSoftKey)
{
    CMenuListItemPtr pMenuItem = CMenuListItem::GetMenuListItem(pFocusItem);

    if (NULL != pMenuItem)
    {
        vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_SEARCH);
        vecSoftKey[2].m_iType = ST_SEARCH;

        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_ENTER);
        vecSoftKey[3].m_iType = ST_ENTER;
    }

    return false;
}

void CBrowseAdapter::UpdateUI(int nPageDataIndex, int nFocusIndex)
{
    SetPageDataIndex(nPageDataIndex);
    SetFocusIndex(nFocusIndex);

    RefreshUI();
}

void CBrowseAdapter::InitListViewLayout(xListView * pListView)
{
    if (NULL == pListView || m_pControler.IsEmpty())
    {
        return;
    }

    int nTotalCount = m_pControler->GetBrowseItemAmount();

    pListView->SetTotalDataCount(nTotalCount);
}

bool CBrowseAdapter::PrecheckDataIndex(int & nDataIndex)
{
    if (m_pControler.IsEmpty())
    {
        return false;
    }

    int nTotalCount = m_pControler->GetBrowseItemAmount();

    return CheckPageIndex(nTotalCount, nDataIndex);
}
