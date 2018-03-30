#include "accountadapter.h"
#include "diruihelper.h"
#include "commondirctrl.h"
#include "languagehelper/modlangtranslate.h"
#include "xwindow/xListView.h"
#include "account/include/modaccount.h"
#include "baseui/include/detaillistitem.h"
#include "baseui/include/menulistitem.h"
#include "baseui/include/listviewitemfactory.h"
#include "directorymgr.h"

CAccountAdapter::CAccountAdapter()
    : m_bRegisterAccount(true)
{
    m_nState = DIR_UI_STATE_CHOOSE_ACCOUNT;
}

CAccountAdapter::~CAccountAdapter()
{
}

CAccountAdapter* CAccountAdapter::GetAccountAdapter(CDirectoryAdapterPtr& pAdapter)
{
    if (pAdapter.IsEmpty() || DIR_UI_STATE_CHOOSE_ACCOUNT != pAdapter->GetState())
    {
        return NULL;
    }

    return static_cast<CAccountAdapter *>(pAdapter.Get());
}

void CAccountAdapter::EnterAccountChoose(bool bRegisterAccount)
{
    m_bRegisterAccount = bRegisterAccount;

    RefreshUI();

    FocsuDefaultAccount();
}

bool CAccountAdapter::ProcessKeyPress(int nKey)
{
    if (PHONE_KEY_OK == nKey && OnSelect())
    {
        return true;
    }

    return CDetailAdapter::ProcessKeyPress(nKey);
}

bool CAccountAdapter::ProcessLineKeyPress(int nKey)
{
    return false;
}

bool CAccountAdapter::ProcessAction(int nAction)
{
    if (ST_OK == nAction && OnSelect())
    {
        return true;
    }

    return CDetailAdapter::ProcessAction(nAction);
}

bool CAccountAdapter::OnSelect()
{
    if (NULL == m_pListView)
    {
        return false;
    }

    xListViewItem* pFocusItem = m_pListView->GetFocusItem();
    if (NULL == pFocusItem)
    {
        return false;
    }

    int nID = GetItemAccountID(pFocusItem);
    if (knInvalidRId == nID)
    {
        return false;
    }

    CDirectoryControlerPtr pControler = GetControler();

    if (pControler.IsEmpty())
    {
        return false;
    }

    int nFoucsIndex = pFocusItem->GetDataIndex();

    return pControler->OnAction(DIR_ACTION_COMMON_SELECT, nFoucsIndex, nID);

    return true;
}

int CAccountAdapter::GetItemAccountID(xListViewItem* pItem)
{
    if (NULL == pItem)
    {
        return knInvalidRId;
    }

    CDetailListItem* pDetailItem = CDetailListItem::GetDetailListItem(pItem);
    if (NULL != pDetailItem)
    {
        return pDetailItem->GetData();
    }

    CMenuListItem* pMenuItem = CMenuListItem::GetMenuListItem(pItem);
    if (NULL != pMenuItem)
    {
        return pMenuItem->GetId();
    }

    return knInvalidRId;
}

int CAccountAdapter::GetItemType()
{
    int nItemType = CDetailListItem::GetDetailListItemType();
    if (!m_bRegisterAccount)
    {
        nItemType = CMenuListItem::GetMenuItemType();
    }

    return nItemType;
}

void CAccountAdapter::InitListViewLayout(xListView * pListView)
{
    m_vecAccountId.clear();

    if (m_bRegisterAccount)
    {
        ListAccounts listAccount = acc_GetAvailableAccountList();
        ListAccounts::const_iterator iter = listAccount.begin();
        for (; iter != listAccount.end(); iter++)
        {
            int iAccount = *iter;
            m_vecAccountId.push_back(iAccount);
        }
    }
    else
    {
        CDirectoryControlerPtr pControl = GetControler();
        CCommonDirCtrl* pCommonCtrl = CCommonDirCtrl::GetCommonControler(pControl);
        if (NULL != pCommonCtrl)
        {
            pCommonCtrl->GetMenuGroupId(m_vecAccountId);
        }
    }

    pListView->SetTotalDataCount(m_vecAccountId.size());
}

xListViewItem* CAccountAdapter::PrepareItem(xListView * pListView, int nDataIndex, xListViewItem * pItem)
{
    if (NULL == pListView || pListView != m_pListView || nDataIndex >= m_vecAccountId.size())
    {
        return NULL;
    }

    int nItemType = GetItemType();

    if (NULL != pItem && nItemType == pItem->GetType())
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

bool CAccountAdapter::RefreshItem(xListView * pListView, xListViewItem * pItem)
{
    if (m_bRegisterAccount)
    {
        return RefrehDetailItem(pListView, pItem);
    }

    return RefreshMenuItem(pListView, pItem);
}

bool CAccountAdapter::RefreshMenuItem(xListView * pListView, xListViewItem * pItem)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return false;
    }

    CMenuListItem* pMenuItem = CMenuListItem::GetMenuListItem(pItem);
    if (NULL == pMenuItem)
    {
        return false;
    }

    int nDataIndex = pMenuItem->GetDataIndex();
    if (nDataIndex < 0 || nDataIndex >= m_vecAccountId.size())
    {
        return false;
    }

    CDirectoryControlerPtr pControl = GetControler();
    if (pControl.IsEmpty())
    {
        return false;
    }

    yl::string strDispname = "";
    DIR_TYPE eLogicType = CCommonDirCtrl::GetLogicType(pControl->GetType());
    CCommonDirCtrl::GetDisplayName(eLogicType, m_vecAccountId[nDataIndex], strDispname);
    pMenuItem->SetShowIndex(true);
    pMenuItem->SetMenu(strDispname);
    pMenuItem->SetId(m_vecAccountId[nDataIndex]);

    return true;
}

bool CAccountAdapter::RefrehDetailItem(xListView * pListView, xListViewItem * pItem)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return false;
    }

    CDetailListItem* pDetailItem = CDetailListItem::GetDetailListItem(pItem);
    if (NULL == pDetailItem)
    {
        return false;
    }

    int nDataIndex = pDetailItem->GetDataIndex();
    if (nDataIndex < 0 || nDataIndex >= m_vecAccountId.size())
    {
        return false;
    }

    int nAccountId = m_vecAccountId[nDataIndex];
    yl::string strName = commonAPI_FormatString("%d", nAccountId + 1) + "." + acc_GetUsername(nAccountId);
    bool bDefault = (nAccountId == acc_GetDefaultAccount());
    yl::string strStatus = GetAccountStates(acc_GetState(nAccountId), bDefault);
    yl::string strItemData = commonAPI_FormatString("%d", nAccountId);
    pDetailItem->SetShowIndex(false);
    pDetailItem->SetValue(strStatus);
    pDetailItem->SetTitle(strName);
    pDetailItem->SetItemData(strItemData);
    return true;
}

yl::string CAccountAdapter::GetAccountStates(int nStates, bool bDefault)
{
    yl::string strValue = "";
    switch (nStates)
    {
    case LS_UNKNOW:
        strValue = _LANG(TRID_UNKNOWN);
        break;
    case LS_DISABLE:
        strValue = _LANG(TRID_DISABLED);
        break;
    case LS_REGISTERING:
        strValue = _LANG(TRID_REGISTERING);
        break;
    case LS_REGISTERED:
    case LS_DISABLE_REG:
        {
            if (bDefault)
            {
                strValue = _LANG(TRID_SYSTEM_DEFAULT_ACCOUNT);
            }
            else
            {
                strValue = _LANG(TRID_REGISTERED);
            }
        }
        break;
    case LS_REGISTER_FAIL:
        strValue = _LANG(TRID_REGISTER_FAIL);
        break;
    case LS_BE_UNREGISTERED:
        strValue = _LANG(TRID_UNREGISTERED);
        break;
    case LS_SYSTEM_DEFAULT_ACCOUNT:
        strValue = _LANG(TRID_SYSTEM_DEFAULT_ACCOUNT);
        break;
    case LS_INEXISTENCE:
        strValue = _LANG(TRID_INEXISTENCE);
        break;
    case LS_UNREGISTERING:
        strValue = _LANG(TRID_UNREGISTERING);
        break;
    case LS_EXIT:
        strValue = _LANG(TRID_EXIT);
        break;
    default:
        break;
    }

    return strValue;
}

bool CAccountAdapter::GetTitle(yl::string & strTitle)
{
    if (m_bRegisterAccount)
    {
        strTitle = _LANG(TRID_CHOOSE_ACCOUNT);
    }
    else
    {
        strTitle = _LANG(TRID_LINE_SELECT);
    }
    return true;
}

bool CAccountAdapter::GetStateSoftKey(xListViewItem * pFocusItem, CArraySoftKeyBarData & vecSoftKey)
{
    if (NULL != pFocusItem)
    {
        vecSoftKey[3].m_iType = ST_OK;
        vecSoftKey[3].m_strSoftkeyTitle = m_bRegisterAccount ? _LANG(TRID_OK) : _LANG(TRID_ENTER);
    }

    return true;
}

void CAccountAdapter::FocsuDefaultAccount()
{
    if (NULL == m_pListView || m_pListView->GetTotalDataCount() <= 0 || !m_bRegisterAccount)
    {
        return;
    }

    int nDialAccountID = CDirUIHelper::GetDialAccountId();
    for (int i = 0; i < m_pListView->GetTotalDataCount(); ++i)
    {
        xListViewItemPtr pItem = m_pListView->GetItemByDataIndex(i);
        CDetailListItem* pDetailItem = CDetailListItem::GetDetailListItem(pItem);
        if (NULL == pItem)
        {
            continue;
        }

        if (pDetailItem->GetData() == nDialAccountID)
        {
            pDetailItem->FocusItem(true);
            return;
        }
    }

    m_pListView->FocusItemByDataIndex(0);
}

