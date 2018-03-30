#include "detailadapter.h"
#include "detaillayout.h"
#include "directorymgr.h"
#include "diruihelper.h"
#include "dlgdirectory.h"
#include "baseui/include/detaillistitem.h"
#include "commonunits/modcommonunits.h"
#include "languagehelper/modlangtranslate.h"
#include "uimanager/moduimanager.h"

#include "dsskey/include/moddsskey.h"
#include "dsskeyui/moddsskeyui.h"
#include "moddirectoryui.h"

CDetailAdapter::CDetailAdapter()
    : CDirectoryAdapter(DIR_UI_STATE_DETAIL)
    , m_pDetailLayout(NULL)
{
}

CDetailAdapter::~CDetailAdapter()
{
}

CDetailAdapter * CDetailAdapter::GetDetailAdapter(CDirectoryAdapterPtr & pAdapter)
{
    if (pAdapter.IsEmpty() || DIR_UI_STATE_DETAIL != pAdapter->GetState())
    {
        return NULL;
    }

    return static_cast<CDetailAdapter *>(pAdapter.Get());
}

void CDetailAdapter::UnInit()
{
    if (NULL != m_pDetailLayout)
    {
        m_pDetailLayout = NULL;
    }

#if IF_EDIT_COMBOBOX_SHOW_TWO_LINE || IF_HIDE_TITLE_BUT_EDIT
    OnChangeListViewParam(false);
#endif

    CDirectoryAdapter::UnInit();
}

bool CDetailAdapter::ProcessKeyPress(int nKey)
{
    IDirUIHelperPtr pUIHelper = GetUIHelper();
    CDirUIHelper * pDirUIHelper = CDirUIHelper::GetDirUIHelper(pUIHelper);

    if (NULL != pDirUIHelper
            && DIR_ACTION_COMMON_SEND == pDirUIHelper->GetActionByKeyPress(m_nState, nKey)
            && ProcessDetailDial())
    {
        return true;
    }

    return CDirectoryAdapter::ProcessKeyPress(nKey);
}

bool CDetailAdapter::ProcessDigitKeyPress(int nKey)
{
    if (!commonUnits_IsSendDigitKey(nKey))
    {
        return false;
    }

    return ProcessDetailDial();
}

bool CDetailAdapter::ProcessLineKeyPress(int nKey)
{
    if (nKey < PHONE_KEY_LINE1 || nKey > PHONE_KEY_LINE10 || NULL != m_pDetailLayout)
    {
        return false;
    }

    int nDsskeyId = dsskeyUI_GetDsskeyIDByKeycode(nKey);
    int nAccountId = dsskey_GetDsskeyLine(nDsskeyId);

    return ProcessDetailDial(nAccountId);
}

bool CDetailAdapter::ProcessAction(int nAction)
{
    if (ST_SEND == nAction)
    {
        return ProcessDetailDial();
    }

    return CDirectoryAdapter::ProcessAction(nAction);
}

bool CDetailAdapter::ProcessDetailDial(int nAccountId /*= AUTO_ACCOUNT_IDENTIFY*/)
{
    IDirUIHelperPtr pUIHelper = GetUIHelper();
    CDirUIHelper * pDirUIHelper = CDirUIHelper::GetDirUIHelper(pUIHelper);

    if (NULL != pDirUIHelper && pDirUIHelper->ProcessDetailDial(m_nState, nAccountId))
    {
        return true;
    }

    return false;
}

void CDetailAdapter::EnterDetail(int nAction)
{
    IDirUIHelperPtr pDirUIHelper = GetUIHelper();
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);

    if (NULL != pUIHelper)
    {
        pUIHelper->OnEnterDetail(nAction);
    }

#if IF_EDIT_COMBOBOX_SHOW_TWO_LINE || IF_HIDE_TITLE_BUT_EDIT
    if (NULL != m_pDetailLayout)
    {
        OnChangeListViewParam(true);
    }
#endif

    RefreshUI();
}

void CDetailAdapter::SetDetailLayout(CDetailLayout * pDetailLayout)
{
    m_pDetailLayout = pDetailLayout;
}

CDetailLayout * CDetailAdapter::GetDetailLayout() const
{
    return m_pDetailLayout;
}

int CDetailAdapter::GetTotalItemCount()
{
    if (NULL != m_pDetailLayout)
    {
        return m_pDetailLayout->GetItemCount();
    }
    else if (!m_pControler.IsEmpty())
    {
        return m_pControler->GetDetailItemAmount();
    }

    return 0;
}

void CDetailAdapter::InitListViewLayout(xListView * pListView)
{
    if (NULL == pListView || m_pControler.IsEmpty())
    {
        return;
    }

    int nTotalCount = GetTotalItemCount();

    pListView->SetTotalDataCount(nTotalCount);

    if (NULL != m_pDetailLayout)
    {
        pListView->SetItemAttach();
#if IF_EDIT_COMBOBOX_SHOW_TWO_LINE || IF_HIDE_TITLE_BUT_EDIT
        OnChangeListViewParam(true);
#endif
    }
}

bool CDetailAdapter::PrecheckDataIndex(int & nDataIndex)
{
    return CheckPageIndex(GetTotalItemCount(), nDataIndex);
}

xListViewItem * CDetailAdapter::PrepareItem(xListView * pListView, int nDataIndex,
        xListViewItem * pItem)
{
    if (NULL != m_pDetailLayout)
    {
        CDetailListItemBasePtr pDetailItem = m_pDetailLayout->GetDetailListItem(nDataIndex);

        if (NULL != pDetailItem)
        {
            pDetailItem->SetAction(this);
        }

        return pDetailItem;
    }

    return CDirectoryAdapter::PrepareItem(pListView, nDataIndex, pItem);
}

bool CDetailAdapter::RefreshItem(xListView * pListView, xListViewItem * pItem)
{
    if (NULL != m_pDetailLayout)
    {
        return true;
    }

    CDetailListItemPtr pDetailItem = CDetailListItem::GetDetailListItem(pItem);

    if (NULL == pDetailItem)
    {
        return false;
    }

    CDirectoryControlerPtr pControler = GetControler();

    if (pControler.IsEmpty())
    {
        return false;
    }

    ST_DETAIL_DATA stDetail;

    if (!pControler->GetDetailData(pDetailItem->GetDataIndex(), stDetail))
    {
        return false;
    }

    pDetailItem->SetTitle(stDetail.stAttr.m_strName);
    pDetailItem->SetValue(stDetail.stAttr.m_strValue);
    pDetailItem->SetItemData(stDetail.nType);

    return true;
}

bool CDetailAdapter::GetTitle(yl::string & strTitle)
{
    IDirUIHelperPtr pDirUIHelper = GetUIHelper();
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);

    if (NULL == pUIHelper)
    {
        return false;
    }

    pUIHelper->GetDetailTitle(strTitle);

    return true;
}

bool CDetailAdapter::GetStateSoftKey(xListViewItem * pFocusItem, CArraySoftKeyBarData & vecSoftKey)
{
    CDetailListItemPtr pDetailItem = CDetailListItem::GetDetailListItem(pFocusItem);

    if (NULL != pDetailItem && DETAIL_DATA_TYPE_NUMBER == pDetailItem->GetData())
    {
        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_DIAL);
        vecSoftKey[3].m_iType = ST_SEND;
    }

    return false;
}
