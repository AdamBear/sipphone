#include "directoryadapter.h"
#include "dirctrlfactory.h"
#include "diruihelperfactory.h"
#include "adapterfactory.h"
#include "diruihelper.h"
#include "directorymgr.h"
#include "dirmsgboxmgr.h"
#include "dlgdirectorybase.h"
#include "commonunits/modcommonunits.h"
#include "baseui/include/listviewitemfactory.h"
#include "baseui/include/comboboxlistitem.h"
#include "baseui/include/editlistitem.h"
#include "baseui/include/modbaseui.h"
#include "languagehelper/modlangtranslate.h"
#include "uimanager/moduimanager.h"
#include "xwindow/xListView.h"
#include "xwindow/xWindowManager.h"

#include "dsskey/include/moddsskey.h"
#include "dsskeyui/moddsskeyui.h"
#include "moddirectoryui.h"

namespace
{
#if IF_EDIT_COMBOBOX_SHOW_TWO_LINE
#define SMALL_SCREEN_PAGE_ITEM_COUNT 1
#define SMALL_SCREEN_PAGE_ITEM_Height 38
#endif

#if IF_HIDE_TITLE_BUT_EDIT
#define EDIT_PAGE_ITEM_COUNT 3
#define EDIT_PAGE_ITEM_HEIGHT 12
#define EDIT_PAGE_LISTVIEW_HEIGHT 38
#define NORMAL_PAGE_LISTVIEW_HEIGHT 52
#endif
}



CDirectoryAdapter::CDirectoryAdapter(int nState)
    : m_nState(nState)
    , m_bRefreshTitle(false)
    , m_bRefreshSoftKey(false)
    , m_bInCache(false)
#if IF_EDIT_COMBOBOX_SHOW_TWO_LINE || IF_HIDE_TITLE_BUT_EDIT
    , m_nOldPageItemCount(0)
    , m_nOldItemHeight(0)
#endif
{

}

CDirectoryAdapter::~CDirectoryAdapter()
{
    DIRUI_INFO("~CDirectoryAdapter %p", this);
    UnInit();
}

CDirectoryControlerPtr CDirectoryAdapter::GetControler()
{
    return m_pControler;
}

IDirUIHelperPtr CDirectoryAdapter::GetUIHelper()
{
    return m_pUIHelper;
}

// 初始化
bool CDirectoryAdapter::Init(int nDirType, int nGroupId/* = knInvalidRId*/)
{
    DIRUI_INFO("CDirectoryAdapter::Init(DirType:%d GroupId:%d)", nDirType, nGroupId);

    ResetIndex();

    CDirectoryControlerPtr pControler = GetControler();

    if (nDirType == GetDirType() && !pControler.IsEmpty())
    {
        pControler->SetGroupId(nGroupId);

        IDirUIHelperPtr pDirUIHelper = pControler->GetUIHelper();

        if (pDirUIHelper.IsEmpty() || m_pUIHelper.IsEmpty())
        {
            m_pUIHelper = g_DirUIHelperFactory.GetDirUIHelper(nDirType);

            pDirUIHelper = GetUIHelper();

            CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);

            if (NULL == pUIHelper)
            {
                return false;
            }

            pControler->Init(pDirUIHelper);
            pUIHelper->Init();
            pUIHelper->SetListView(GetListView());
        }

        pControler->EnterState(m_nState);
        return true;
    }

    UnInit();

    m_pControler = g_DirCtrlFactory.GetControler(nDirType);

    pControler = GetControler();

    if (pControler.IsEmpty())
    {
        return false;
    }

    pControler->SetGroupId(nGroupId);

    m_pUIHelper = g_DirUIHelperFactory.GetDirUIHelper(nDirType);

    IDirUIHelperPtr pDirUIHelper = GetUIHelper();
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);

    if (NULL == pUIHelper)
    {
        return false;
    }

    pControler->Init(pDirUIHelper);
    pUIHelper->Init();
    pUIHelper->SetListView(GetListView());

    pControler->EnterState(m_nState);

    return true;
}

// 恢复初始化
void CDirectoryAdapter::UnInit()
{
    DIRUI_DEBUG("begin CDirectoryAdapter::UnInit");

    if (NULL != m_pListView)
    {
        m_pListView->ClearContent();
        m_pListView->SetAdapter(NULL);
    }

    if (!m_pControler.IsEmpty())
    {
        m_pControler->LeaveState(m_nState);
        m_pControler.Reset();
    }

    IDirUIHelperPtr pDirUIHelper = GetUIHelper();
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);

    if (NULL != pUIHelper)
    {
        pUIHelper->UnInit();
    }

    m_pUIHelper.Reset();

    DIRUI_DEBUG("end CDirectoryAdapter::UnInit");
}

void CDirectoryAdapter::InitListView()
{
    IDirUIHelperPtr pDirUIHelper = GetUIHelper();
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);

    if (NULL != pUIHelper)
    {
        pUIHelper->SetListView(m_pListView);
    }
}

void CDirectoryAdapter::UnInitListView()
{
    xListViewAdapter::UnInitListView();

    IDirUIHelperPtr pDirUIHelper = GetUIHelper();
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);

    if (NULL != pUIHelper)
    {
        pUIHelper->SetListView(NULL);
    }
}

void CDirectoryAdapter::RefreshUI(int nRefreshType/* = DIR_UI_REFRESH_TYPE_ALL*/)
{
    m_bRefreshTitle = false;
    m_bRefreshSoftKey = false;

    if (DIR_UI_REFRESH_TYPE_LIST_VIEW == (nRefreshType & DIR_UI_REFRESH_TYPE_LIST_VIEW))
    {
        RefreshListView();
    }

    if (!m_bRefreshTitle)
    {
        RefreshTitle();
    }

    if (!m_bRefreshSoftKey)
    {
        RefreshSoftKey();
    }
}

void CDirectoryAdapter::RefreshTitle()
{
    CBaseDialog * pTopDlg = UIManager_GetTopWindow();

    if (NULL == pTopDlg || eWT_Directory != pTopDlg->GetWindowType())
    {
        return;
    }

    CDlgDirectoryBase * pDlg = static_cast<CDlgDirectoryBase *>(pTopDlg);

    pDlg->RefreshTitle();

    m_bRefreshTitle = true;
}

void CDirectoryAdapter::OnReactive()
{
    SetCache(false);
}

bool CDirectoryAdapter::IsShowPageInfo()
{
    return false;
}

yl::string CDirectoryAdapter::GetPageInfo()
{
    yl::string strInfo = "";

    IDirUIHelperPtr pDirUIHelper = GetUIHelper();
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);
    if (NULL != pUIHelper && pUIHelper->GetTitlePageInfo(strInfo))
    {
        return strInfo;
    }

    int nDataIndex = 0;
    int nTotalCount = 0;

    if (NULL != m_pListView)
    {
        nDataIndex = m_pListView->GetFocusItemDataIndex();
        nTotalCount = m_pListView->GetTotalDataCount();
    }

    if (nTotalCount <= 0)
    {
        nDataIndex = -1;
        nTotalCount = 0;
    }

    char szBuffer[64];

    _snprintf(szBuffer, sizeof(szBuffer), "%d/%d", nDataIndex + 1, nTotalCount);
    szBuffer[sizeof(szBuffer) - 1] = '\0';

    strInfo = szBuffer;

    return strInfo;
}

void CDirectoryAdapter::RefreshTitlePageInfo()
{
    if (NULL == m_pListView || !IsShowPageInfo())
    {
        return;
    }

    CBaseDialog * pTopDlg = UIManager_GetTopWindow();

    if (NULL == pTopDlg || eWT_Directory != pTopDlg->GetWindowType())
    {
        return;
    }

    CDlgDirectoryBase * pDlg = static_cast<CDlgDirectoryBase *>(pTopDlg);

    yl::string strInfo = GetPageInfo();

    pDlg->SetTitlePageInfo(strInfo);
}

void CDirectoryAdapter::RefreshSoftKey()
{
    if (NULL == m_pListView)
    {
        return;
    }

    CBaseDialog * pTopDlg = UIManager_GetTopWindow();

    if (NULL == pTopDlg || eWT_Directory != pTopDlg->GetWindowType())
    {
        return;
    }

    CDlgDirectoryBase * pDlg = static_cast<CDlgDirectoryBase *>(pTopDlg);

    pDlg->RefreshSoftkey();

    m_bRefreshSoftKey = true;
}

bool CDirectoryAdapter::GetTitle(yl::string & strTitle)
{
    return false;
}

bool CDirectoryAdapter::GetStateSoftKey(xListViewItem * pFocusItem,
                                        CArraySoftKeyBarData & vecSoftKey)
{
    return false;
}

int CDirectoryAdapter::GetState() const
{
    return m_nState;
}

int CDirectoryAdapter::GetDirType()
{
    if (m_pControler.IsEmpty())
    {
        return DIRECTORY_TYPE_NONE;
    }

    return m_pControler->GetType();
}

int CDirectoryAdapter::GetGroupId()
{
    if (m_pControler.IsEmpty())
    {
        return knInvalidRId;
    }

    return m_pControler->GetGroupId();
}

void CDirectoryAdapter::SetCache(bool bCache)
{
    m_bInCache = bCache;
}

bool CDirectoryAdapter::IsInCache() const
{
    return m_bInCache;
}

xListViewItem * CDirectoryAdapter::PrepareItem(xListView * pListView, int nDataIndex,
        xListViewItem * pItem)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return NULL;
    }

    IDirUIHelperPtr pDirUIHelper = GetUIHelper();
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);

    if (NULL == pUIHelper)
    {
        return NULL;
    }

    int nItemType = pUIHelper->GetListItemType(m_nState, nDataIndex);

    if (NULL != pItem && pItem->GetType() == nItemType)
    {
        return pItem;
    }

    xListViewItem * pNewItem = g_ItemFactory.ApplyItem(nItemType);

    if (NULL != pNewItem)
    {
        pNewItem->SetAction(this);
    }

    return pNewItem;
}

bool CDirectoryAdapter::RefreshItem(xListView * pListView, xListViewItem * pItem)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return false;
    }

    IDirUIHelperPtr pDirUIHelper = GetUIHelper();
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);

    if (NULL == pUIHelper)
    {
        return false;
    }

    return pUIHelper->RefreshItem(m_nState, pItem);
}

void CDirectoryAdapter::OnCleanContent(xListView * pListView)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return;
    }

    if (!m_bInCache)
    {
        ResetIndex();
    }

    OnFocusChanged();
}

void CDirectoryAdapter::OnFocusChanged()
{
    RefreshTitlePageInfo();
    RefreshSoftKey();
}

void CDirectoryAdapter::OnItemAction(xListView * pListView, xListViewItem * pItem, int nAction)
{
    xListViewAdapter::OnItemAction(pListView, pItem, nAction);

    DIRUI_INFO("CDirectoryAdapter::OnItemAction(xListView:%p Item:%p Action:%d)",
               pListView, pItem, nAction);

    if (NULL == pListView || pListView != m_pListView || NULL == pItem)
    {
        return;
    }

    CDirectoryControlerPtr pControler = GetControler();
    IDirUIHelperPtr pDirUIHelper = GetUIHelper();
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);

    if (pControler.IsEmpty() || NULL == pUIHelper)
    {
        return;
    }

    if (LIST_VIEW_ITEM_ACTION_FOCUS == nAction)
    {
        UpdateIndex();

        OnFocusChanged();

        return;
    }

    int nItemId = pUIHelper->GetLisItemId(pItem);

    if (knInvalidRId == nItemId)
    {
        return;
    }

    int nCtrlAction = pUIHelper->GetActionByItem(nAction, nItemId);

    if (DIR_ACTION_NONE != nCtrlAction)
    {
        pControler->OnAction(nCtrlAction, pItem->GetDataIndex(), nItemId);
    }
}

bool CDirectoryAdapter::CheckPageIndex(int nTotalCount, int & nDataIndex)
{
    if (NULL == m_pListView)
    {
        nDataIndex = 0;
        return true;
    }

    if (m_pListView->GetTotalDataCount() == nTotalCount)
    {
        return true;
    }

    int nPageItemCount = m_pListView->GetPageItemCount();

    if (nPageItemCount <= 0)
    {
        return true;
    }

    // 只有一页数据时
    if (nTotalCount <= nPageItemCount)
    {
        if (nDataIndex > 0)
        {
            nDataIndex = 0;
            return false;
        }

        return true;
    }

    // 超过一页数据
    if (nPageItemCount > 0 && (nDataIndex + nPageItemCount) > nTotalCount)
    {
        xListViewStylePtr pListViewStyle = m_pListView->GetStyle();
        int nTmpDataIndex = 0;

        if (NULL != pListViewStyle && pListViewStyle->IsScrollByPage())
        {
            int nRemainder = nTotalCount % nPageItemCount;

            if (0 == nRemainder)
            {
                nTmpDataIndex = nTotalCount - nPageItemCount;
            }
            else
            {
                nTmpDataIndex = nTotalCount - nRemainder;
            }
        }
        else
        {
            nTmpDataIndex = nTotalCount - nPageItemCount;
        }

        if (nTmpDataIndex != nDataIndex)
        {
            nDataIndex = nTmpDataIndex;
            return false;
        }
    }

    return true;
}

bool CDirectoryAdapter::ProcessKeyPress(int nKey)
{
    IDirUIHelperPtr pDirUIHelper = GetUIHelper();
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);

    if (NULL == pUIHelper)
    {
        return false;
    }

    return ProcessCtrlAction(pUIHelper, pUIHelper->GetActionByKeyPress(m_nState, nKey));
}

bool CDirectoryAdapter::ProcessDigitKeyPress(int nKey)
{
    if (NULL != m_pListView && commonUnits_IsSendDigitKey(nKey))
    {
        xListViewItemPtr pFocusItem = m_pListView->GetFocusItem();

        if (NULL == pFocusItem)
        {
            return false;
        }

        IDirUIHelperPtr pUIHelper = GetUIHelper();
        CDirUIHelper * pDirUIHelper = CDirUIHelper::GetDirUIHelper(pUIHelper);

        if (NULL != pDirUIHelper && pDirUIHelper->IsFocusItemCanDial(pFocusItem))
        {
            OnItemAction(m_pListView, pFocusItem, LIST_VIEW_ITEM_ACTION_SELECT);

            return true;
        }
    }

    return false;
}

bool CDirectoryAdapter::ProcessLineKeyPress(int nKey)
{
    if (nKey < PHONE_KEY_LINE1 || nKey > PHONE_KEY_LINE10
            || DIR_UI_STATE_NONE == m_nState || NULL == m_pListView)
    {
        return false;
    }

    IDirUIHelperPtr pDirUIHelper = GetUIHelper();
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);
    if (NULL == pUIHelper)
    {
        return false;
    }

    //联系人/历史记录型控件才能呼出
    int nFocusId = pUIHelper->GetLisItemId(m_pListView->GetFocusItem());

    if (knInvalidRId != nFocusId)
    {
        CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
        if (!pControler.IsEmpty())
        {
            int nDsskeyId = dsskeyUI_GetDsskeyIDByKeycode(nKey);
            int nAccountId = dsskey_GetDsskeyLine(nDsskeyId);
            pControler->DialContact(nFocusId, nAccountId);
            return true;
        }
    }

    return false;
}

bool CDirectoryAdapter::ProcessAction(int nAction)
{
    IDirUIHelperPtr pDirUIHelper = GetUIHelper();
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);

    if (NULL == pUIHelper)
    {
        return false;
    }

    if (ST_OPTION == nAction)
    {
        return OnShowOption();
    }

    return ProcessCtrlAction(pUIHelper, pUIHelper->GetActionByUIAction(m_nState, nAction));
}

bool CDirectoryAdapter::ProcessCtrlAction(CDirUIHelper * pUIHelper, int nAction)
{
    if (NULL == m_pListView || NULL == pUIHelper || DIR_ACTION_NONE == nAction)
    {
        return false;
    }

    CDirectoryControlerPtr pControler = GetControler();

    if (pControler.IsEmpty())
    {
        return false;
    }

    int nFoucsIndex = m_pListView->GetFocusItemDataIndex();
    int nFocusId = pUIHelper->GetLisItemId(m_pListView->GetFocusItem());

    return pControler->OnAction(nAction, nFoucsIndex, nFocusId);
}

bool CDirectoryAdapter::OnShowOption()
{
    IDirUIHelperPtr pDirUIHelper = GetUIHelper();
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);

    if (NULL == pUIHelper)
    {
        return false;
    }

    VEC_OPTION_ITEM_DATA vecOption;

    if (!pUIHelper->GetOption(m_nState, vecOption) || 0 == vecOption.size())
    {
        return false;
    }

    BaseUI_ShowOptionMenu(&g_DirMsgBoxMgr, vecOption);

    return true;
}

#if IF_EDIT_COMBOBOX_SHOW_TWO_LINE || IF_HIDE_TITLE_BUT_EDIT
void CDirectoryAdapter::OnChangeListViewParam(bool bEnter)
{
    if (NULL == m_pListView)
    {
        return;
    }

    xListViewStylePtr pStyle = m_pListView->GetStyle();
    if (NULL == pStyle)
    {
        return;
    }

    if (bEnter)
    {
        int nPageItemCount = pStyle->GetPageItemCount();
#if IF_EDIT_COMBOBOX_SHOW_TWO_LINE
        if (SMALL_SCREEN_PAGE_ITEM_COUNT != nPageItemCount)
        {
            m_nOldPageItemCount = nPageItemCount;
            m_nOldItemHeight = pStyle->GetItemHeight();
            pStyle->SetPageItemCount(SMALL_SCREEN_PAGE_ITEM_COUNT);
            pStyle->SetItemHeight(SMALL_SCREEN_PAGE_ITEM_Height);
        }
#endif

#if IF_HIDE_TITLE_BUT_EDIT
        if (EDIT_PAGE_ITEM_COUNT != nPageItemCount)
        {
            m_nOldPageItemCount = nPageItemCount;
            m_nOldItemHeight = pStyle->GetItemHeight();
            pStyle->SetPageItemCount(EDIT_PAGE_ITEM_COUNT);
            pStyle->SetItemHeight(EDIT_PAGE_ITEM_HEIGHT);

            m_pListView->layout_height = EDIT_PAGE_LISTVIEW_HEIGHT;

            IDirUIHelperPtr pDirUIHelper = GetUIHelper();
            CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);
            if (NULL != pUIHelper)
            {
                pUIHelper->SetShowTitle(true);
            }
        }
#endif
    }
    else if (m_nOldItemHeight > 0 && m_nOldPageItemCount > 0)
    {
        pStyle->SetPageItemCount(m_nOldPageItemCount);
        pStyle->SetItemHeight(m_nOldItemHeight);
        m_nOldPageItemCount = 0;
        m_nOldItemHeight = 0;
#if IF_HIDE_TITLE_BUT_EDIT
        m_pListView->layout_height = NORMAL_PAGE_LISTVIEW_HEIGHT;
        IDirUIHelperPtr pDirUIHelper = GetUIHelper();
        CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);
        if (NULL != pUIHelper)
        {
            pUIHelper->SetShowTitle(false);
        }
#endif
    }
}
#endif
