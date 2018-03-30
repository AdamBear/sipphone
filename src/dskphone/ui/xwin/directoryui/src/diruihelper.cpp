#include "diruihelper.h"
#include "commondirctrl.h"
#include "directorycommon.h"
#include "directoryuicommon.h"
#include "translateiddefine.h"
#include "directorymgr.h"
#include "dirmsgboxmgr.h"
#include "translateiddefine.h"
#include "bsftcontactcontroler.h"
#include "localcontactcontroler.h"
#include "remotecontactcontroler.h"
#include "browseadapter.h"
#include "searchadapter.h"
#include "moddirectoryui.h"
#include "baseui/include/comboboxlistitem.h"
#include "baseui/include/detaillistitem.h"
#include "baseui/include/editlistitem.h"
#include "baseui/include/menulistitem.h"
#include "languagehelper/modlangtranslate.h"
#include "contactlistitem.h"
#include "dlgdirectorybase.h"
#include "account/include/modaccount.h"
#include "configparser/modconfigparser.h"
#include "talk/emergency/include/modemergency.h"
#include "talk/digitmap/include/moddigitmap.h"
#include "talk/talklogic/include/modtalklogic.h"
#include <baseui/include/baseuidefine.h>
#include <xwindow/xwindowdefine.h>

namespace
{
#define MAX_LIST_PAGE_COUNT 3
#define MAX_ITEM_PAGE_COUNT 1
#define MSG_BOX_SHOW_TIME 2000
#define SHORT_TIME 1500
}


int CDirUIHelper::s_nBackUIPageIndex = 0;
int CDirUIHelper::s_nBackUIFocusIndex = 0;
int CDirUIHelper::s_nDialAccountId = AUTO_ACCOUNT_IDENTIFY;
yl::string CDirUIHelper::s_strDialFocusNumber = "";


CDirUIHelper::CDirUIHelper(int nType)
    : m_nType(nType)
    , m_pListView(NULL)
{
#if IF_HIDE_TITLE_BUT_EDIT
    m_bShowTitle = false;
#else
    m_bShowTitle = true;
#endif
}

CDirUIHelper::~CDirUIHelper()
{
    UnInit();
}

void CDirUIHelper::Init()
{
    m_pListView = NULL;
}

void CDirUIHelper::UnInit()
{
    m_pListView = NULL;
}

void CDirUIHelper::SetListView(xListView * pListView)
{
    m_pListView = pListView;
}

int CDirUIHelper::GetType() const
{
    return m_nType;
}

void CDirUIHelper::SetShowTitle(bool bShowTitle)
{
    m_bShowTitle = bShowTitle;
}

bool CDirUIHelper::IsShowTitle()
{
    return m_bShowTitle;
}

bool CDirUIHelper::GetTitlePageInfo(yl::string& strPageInfo)
{
    return false;
}

CDirUIHelper * CDirUIHelper::GetDirUIHelper(IDirUIHelperPtr pUIHelper)
{
    return static_cast<CDirUIHelper *>(pUIHelper.Get());
}

void CDirUIHelper::EnterUI(int nDirType, int nUIState, int nAction, int nId)
{
    DIRUI_INFO("EnterUI(DirType:%d State:%d Action:%d Id:%d)", nDirType, nUIState, nAction, nId);

    if (DIR_UI_STATE_BROWSER == nUIState)
    {
        CDirectoryAdapterPtr pActiveAdapter = g_DirectoryMgr.GetActiveAdapter();
        if (pActiveAdapter.IsEmpty() || pActiveAdapter->GetState() != nUIState)
        {
            g_DirectoryMgr.EnterBrowser(nDirType, nId);
            return;
        }

        g_DirectoryMgr.EnterGroup(nDirType, nId);
        return;
    }
    else if (DIR_UI_STATE_SEARCH == nUIState)
    {
        g_DirectoryMgr.EnterSearch(nDirType, nId, PHONE_KEY_NONE);
    }
    else if (DIR_UI_STATE_DETAIL == nUIState)
    {
        CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();

        if (pControler.IsEmpty() || pControler->GetType() != nDirType)
        {
            CDirectoryControler * pDirControler = pControler.Get();

            DIRUI_ERR("Enter Detail fail, %p %d %d",
                      pDirControler, (NULL != pDirControler ? pDirControler->GetType() : DIRECTORY_TYPE_NONE), nDirType);

            return;
        }

        int nGroupId = pControler->GetGroupId();

        g_DirectoryMgr.EnterDetail(nAction, nDirType, nGroupId, nId);

        return;
    }
    else if (DIR_UI_STATE_ADDITION == nUIState)
    {
        g_DirectoryMgr.EnterDetail(nAction, nDirType, nId, knInvalidRId);
        return;
    }
    else if (DIR_UI_STATE_CHG_PWD == nUIState)
    {
        g_DirectoryMgr.EnterBsftPwd();
        return;
    }
    else if (DIR_UI_STATE_CHOOSE_PHOTO == nUIState)
    {
        g_DirectoryMgr.EnterPhotoChoose(nDirType, nId);
        return;
    }
#if IF_FEATURE_34162
    else if (DIR_UI_STATE_LDAP_CHG_PWD == nUIState)
    {
        g_DirectoryMgr.EnterLDAPPwd();
    }
#endif
#if IF_FEATURE_GBBOOK
    else if (DIR_UI_STATE_CHOOSE_ACCOUNT == nUIState)
    {
        g_DirectoryMgr.EnterAccountChoose(nDirType, nId);
    }
#endif
    DIRUI_ERR("EnterUI(DirType:%d State:%d Action:%d Id:%d) fail", nDirType, nUIState, nAction, nId);
}

void CDirUIHelper::ExitCurrentUI(int nUIState, bool bReloadData/* = true*/)
{
    CDirectoryAdapterPtr pActiveAdapter = g_DirectoryMgr.GetActiveAdapter();

    if (pActiveAdapter.IsEmpty() || pActiveAdapter->GetState() != nUIState)
    {
        return;
    }

    int nRefreshType = DIR_UI_REFRESH_TYPE_ALL;

    if (!bReloadData)
    {
        nRefreshType = nRefreshType & (DIR_UI_REFRESH_TYPE_ALL ^ DIR_UI_REFRESH_TYPE_LIST_VIEW);
    }

    g_DirectoryMgr.LeaveCurrentState(nRefreshType);
}

void CDirUIHelper::ExitAllUI(int nDirType)
{
    CDirectoryAdapterPtr pActiveAdapter = g_DirectoryMgr.GetActiveAdapter();

    while (!pActiveAdapter.IsEmpty() && pActiveAdapter->GetDirType() == nDirType)
    {
        g_DirectoryMgr.LeaveCurrentState(DIR_UI_REFRESH_TYPE_NONE);

        pActiveAdapter = g_DirectoryMgr.GetActiveAdapter();
    }

    if (!pActiveAdapter.IsEmpty())
    {
        pActiveAdapter->RefreshUI();
    }
}

void CDirUIHelper::ReturnRootGroup(int nDirType, bool bCheckCondition /*= false*/)
{
    if (DIRECTORY_TYPE_BROADSOFT == nDirType)
    {
        CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();
        CBrowseAdapter * pBrowserAdapter = CBrowseAdapter::GetBrowserAdapter(pAdapter);

        if (NULL != pBrowserAdapter)
        {
            pBrowserAdapter->OnBackToMenu(bCheckCondition);
        }
    }
}

bool CDirUIHelper::CheckState(int nCheckState, int nCurState)
{
    if (DIR_UI_STATE_ADDITION == nCheckState && DIR_UI_STATE_DETAIL == nCurState)
    {
        return true;
    }

    return (nCheckState == nCurState);
}

bool CDirUIHelper::DialContact(int nAccountId, const yl::string & strName, LIST_ATTRIBUTE & lsAttr,
                               int nDialAction/* = DIR_DIAL_ACTION_NONE*/)
{
    // 允许显示空号码，因此号码数量需要重新判断
    LIST_ATTRIBUTE lsSolidAttr;
    LIST_ATTRIBUTE::iterator iter = lsAttr.begin();

    for (; iter != lsAttr.end(); ++iter)
    {
        ST_CONTACT_ATTRIBUTE & stAttr = (*iter);

        if (!stAttr.m_strValue.empty())
        {
            lsSolidAttr.push_back(stAttr);
        }
    }

    int nSize = lsSolidAttr.size();

    if (0 == nSize)
    {
        g_DirMsgBoxMgr.ShowMsgBox(TRID_PHONE_NUMBER_IS_EMPTY);
        return false;
    }
    else if (1 == nSize)
    {
        ST_CONTACT_ATTRIBUTE & stAttr = lsSolidAttr.front();

        DialNumber(nAccountId, strName, stAttr.m_strValue, nDialAction);
        return true;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();

    if (pControler.IsEmpty())
    {
        return false;
    }

    s_nDialAccountId = nAccountId;
    s_strDialFocusNumber = "";

    IDirUIHelperPtr pDirUiHelper = g_DirectoryMgr.GetActiveUIHelper();
    CDirUIHelper* pUIHelper = (CDirUIHelper*)pDirUiHelper.Get();
    if (NULL != pUIHelper)
    {
        s_strDialFocusNumber = pUIHelper->GetFocusContactNumber();
    }

    CDirectoryControler::SetAttributeList(pControler->GetType(), lsAttr);

    g_DirectoryMgr.EnterNumberChoose(pControler->GetType(), pControler->GetGroupId());
    return true;
}

bool CDirUIHelper::DialNumber(int nAccountId, const yl::string & strName,
                              const yl::string & strNumber, int nDialAction/* = DIR_DIAL_ACTION_NONE*/)
{
    if (strNumber.empty())
    {
        g_DirMsgBoxMgr.ShowMsgBox(TRID_PHONE_NUMBER_IS_EMPTY);
        return false;
    }

#ifdef IF_BT_SUPPORT_PHONE
    if (talklogic_IsFoucsInBluetoothDialSession())
    {
        DIRUI_INFO("CDirUIHelper::Dial, Mobile CallOut Number=[%s] Name=[%s]", strNumber.c_str(),
                   strName.c_str());

        talklogic_CallMobileOut(strNumber, strName);

        ResetDialAccountId();

        return true;
    }
#endif

#if DIR_SUPPORT_SELECT_CONTACT
    if (g_DirectoryMgr.IsInSelecting())
    {
        g_DirectoryMgr.OnSelectContact(strName, strNumber);
        return true;
    }
#endif

    // 如果是-1,则使用联系人配置的账号
    if (-1 != nAccountId && !acc_IsAccountAvailable(nAccountId)
            && !Emergency_IsEmergencyMode(strNumber.c_str()))
    {
        // 如果没有账号，则取当前账号
        nAccountId = acc_GetDefaultAccount();
        // 如果默认账号不存在，则尝试IP直拨账号
        if (configParser_GetConfigInt(kszEnableDirectIPCall) != 0 && -1 == nAccountId)
        {
            nAccountId = acc_GetDirectIPAccountID();
        }

#ifdef IF_FEATURE_PSTN
        // http://10.2.1.199/Bug.php?BugID=93431
        // 默认账号是pstn，当前通话中的是pstn 因为pstn只有一路通话，所以此时账号是无效的
        // pstn这里注释掉，在通话呼出时候会有判定。
        if (!acc_IsPstnAccount(nAccountId))
        {
            if (!acc_IsAccountAvailable(nAccountId))
            {
                g_DirMsgBoxMgr.ShowMsgBox(TRID_INVALID_ACCOUNT);
                return false;
            }
        }
#else
        if (!acc_IsAccountAvailable(nAccountId))
        {
            g_DirMsgBoxMgr.ShowMsgBox(TRID_INVALID_ACCOUNT);
            return false;
        }
#endif
    }
    else if (AUTO_SELECT_LINE_ID == nAccountId)
    {
        nAccountId = acc_GetDefaultAccount();
    }

    ResetDialAccountId();

    bool bExeMatch = true;

    if (DIR_DIAL_ACTION_DIGIT_MAP_HISTORY == (nDialAction & DIR_DIAL_ACTION_DIGIT_MAP_HISTORY))
    {
        bExeMatch = DigitMap_IsConfigEnable(kszDigitMapInHistoryDial, nAccountId);
    }
    else if (DIR_DIAL_ACTION_DIGIT_MAP_DIRECTORY == (nDialAction & DIR_DIAL_ACTION_DIGIT_MAP_DIRECTORY))
    {
        bExeMatch = DigitMap_IsConfigEnable(kszDigitMapInDirDial, nAccountId);
    }

    CallOut_Param tCalloutParam;

    if (!bExeMatch)
    {
        tCalloutParam.tDialPlanMask.RemoveDigitMapFlag();
    }

    DIRUI_INFO("CDirUIHelper::Dial action=[%d] acc=[%d] match=[%d]", nDialAction, nAccountId,
               (bExeMatch ? 1 : 0));

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    if (!pControler.IsEmpty())
    {
        tCalloutParam.eTransferType = pControler->IsBlindTransfer() ? DTT_BLIND_TRANSFER : DTT_CALL_OUT;
        pControler->SetBlindTransfer(false);
    }

#if IF_BUG_27045
    if (DIR_DIAL_ACTION_ADD_AREA_CODE == (DIR_DIAL_ACTION_ADD_AREA_CODE & nDialAction))
    {
        yl::string strNumberNew = strNumber;

        if (_DialRuleController.GetAreaCodeNumber(strNumber, strNumberNew, nAccountId))
        {
            talklogic_CallOut(strNumberNew, strName, nAccountId, &tCalloutParam, true);
            return true;
        }
    }
#endif

    talklogic_CallOut(strNumber, strName, nAccountId, &tCalloutParam, true);

    return true;
}

int CDirUIHelper::GetDialAccountId()
{
    return s_nDialAccountId;
}

void CDirUIHelper::SetDialAccountId(int nID)
{
    s_nDialAccountId = nID;
}

void CDirUIHelper::ResetDialAccountId()
{
    s_nDialAccountId = AUTO_ACCOUNT_IDENTIFY;
}

yl::string CDirUIHelper::GetDialFocusNumber()
{
    return s_strDialFocusNumber;
}

void CDirUIHelper::ResetDialFocusNumber()
{
    s_strDialFocusNumber = "";
}

const yl::string & CDirUIHelper::GetSearchKey()
{
    static yl::string s_strEmpty = "";

    CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();
    CSearchAdapter * pSearchAdapter = CSearchAdapter::GetSearchAdapter(pAdapter);

    if (NULL != pSearchAdapter)
    {
        return pSearchAdapter->GetSearchKey();
    }

    return s_strEmpty;
}

void CDirUIHelper::RefreshUI(int nType, int nParam)
{
    CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();

    if (pAdapter.IsEmpty())
    {
        return;
    }

    if (DIR_UI_REFRESH_PARAM_RESET_FOCUS == (DIR_UI_REFRESH_PARAM_RESET_FOCUS & nParam))
    {
        CSearchAdapter * pSearchAdapter = CSearchAdapter::GetSearchAdapter(pAdapter);

        if (NULL != pSearchAdapter && (INVALID_ITEM_INDEX != pSearchAdapter->GetFocusIndex()
                                       || (NULL != m_pListView && m_pListView->hasFocus())))
        {
            pSearchAdapter->SetPageDataIndex(0);
            pSearchAdapter->SetFocusIndex(0);
        }
        else
        {
            pAdapter->ResetIndex();
        }
    }

    if (DIR_UI_REFRESH_TYPE_LIST_VIEW == nType)
    {
        pAdapter->RefreshListView();

        return;
    }

    pAdapter->RefreshUI(nType);
}

void CDirUIHelper::RefreshUIAfterDeleted(bool bBackUI/* = false*/)
{
    CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();

    if (pAdapter.IsEmpty())
    {
        return;
    }

    xListView * pListView = pAdapter->GetListView();

    if (NULL == pListView)
    {
        return;
    }

    int nPageItemCount = pListView->GetPageItemCount();

    if (nPageItemCount <= 0)
    {
        return;
    }

    int nTotalCount = pAdapter->GetTotalCount();
    int nPageDataIndex = pAdapter->GetPageDataIndex();
    int nFocusIndex = pAdapter->GetFocusIndex();

    if (nFocusIndex > 0 && nFocusIndex >= (nTotalCount - 1))
    {
        // 数据项小于一屏，且是最后一项
        nPageDataIndex = 0;
        --nFocusIndex;
    }
    else if (nPageDataIndex > 0 && (nPageDataIndex + nPageItemCount) > (nTotalCount - 1))
    {
        // 最后一页
        --nPageDataIndex;

        if ((nFocusIndex + 1) < nPageItemCount)
        {
            ++nFocusIndex;
        }
    }

    pAdapter->SetPageDataIndex(nPageDataIndex);
    pAdapter->SetFocusIndex(nFocusIndex);
    pAdapter->RefreshListView();
    if (!bBackUI)
    {
        pAdapter->RefreshUI(DIR_UI_REFRESH_TYPE_TITLE);
    }
}

void CDirUIHelper::FocusDefaultNumber()
{
    if (CheckFocusSearchNum())
    {
        return;
    }

    FocusFirstDetailNumber();
}

void CDirUIHelper::FocusFirstDetailNumber()
{
    if (NULL == m_pListView)
    {
        return;
    }

    int nTotalItemCount = m_pListView->GetTotalItemCount();

    for (int i = 0; i < nTotalItemCount; ++i)
    {
        xListViewItemPtr pItem = m_pListView->GetItemByIndex(i);

        if (NULL == pItem || CDetailListItem::GetDetailListItemType() != pItem->GetType())
        {
            continue;
        }

        CDetailListItemPtr pDetailItem = (CDetailListItemPtr)pItem;

        if (!pDetailItem->GetValue().empty())
        {
            pDetailItem->FocusItem(true);
            return;
        }
    }
}

bool CDirUIHelper::CheckFocusSearchNum(bool bCheckState /*= true */)
{
    if (NULL == m_pListView)
    {
        return false;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    if (pControler.IsEmpty()
            || (bCheckState && DIR_UI_STATE_SEARCH != pControler->GetDetailState()))
    {
        return false;
    }

    yl::string strFocusNumber = CDirUIHelper::GetDialFocusNumber();
    if (strFocusNumber.empty())
    {
        return false;
    }

    int nTotalItemCount = m_pListView->GetTotalItemCount();
    for (int i = 0; i < nTotalItemCount; ++i)
    {
        xListViewItemPtr pItem = m_pListView->GetItemByIndex(i);
        CDetailListItemPtr pDetailItem = CDetailListItem::GetDetailListItem(pItem);
        if (NULL != pDetailItem && pDetailItem->GetValue() == strFocusNumber)
        {
            pDetailItem->FocusItem(true);
            return true;
        }
    }

    return false;
}

yl::string CDirUIHelper::GetFocusContactNumber()
{
    yl::string strNumber = "";
    if (NULL == m_pListView)
    {
        return strNumber;
    }

    xListViewItemPtr pItem = m_pListView->GetFocusItem();
    CContactListItem* pContactItem = CContactListItem::GetContactListItem(pItem);
    if (NULL != pContactItem)
    {
        strNumber = pContactItem->GetInfo();
    }

    return strNumber;
}

int CDirUIHelper::GetBrowsePageType(int nGroupId)
{
    return BROWSER_PAGE_TYPE_NORMAL;
}

bool CDirUIHelper::GetBrowseAdjoinGroup(bool bPrevious, int& nGroupId, int& nGroupIndex)
{
    return false;
}

int CDirUIHelper::GetAdjoinGroupId(VEC_CONTACT_ID& vecGroupId, bool bPrevious, int nGroupId, int& nIndex)
{
    int nSize = vecGroupId.size();

    if (nSize <= 0)
    {
        nIndex = 0;

        return knInvalidRId;
    }

    int i = 0;

    for (; i < nSize; ++i)
    {
        if (vecGroupId[i] == nGroupId)
        {
            break;
        }
    }

    if (i >= nSize)
    {
        nIndex = 0;

        return vecGroupId[0];
    }

    if (bPrevious)
    {
        --i;
    }
    else
    {
        ++i;
    }

    if (i < 0)
    {
        i = nSize - 1;
    }
    else if (i >= nSize)
    {
        i = 0;
    }

    nIndex = i;

    return vecGroupId[i];
}

int CDirUIHelper::GetListItemType(int nState, int nDataIndex)
{
    if (DIR_UI_STATE_CHOOSE_NUMBER == nState || DIR_UI_STATE_DETAIL == nState)
    {
        return CDetailListItem::GetDetailListItemType();
    }

    return CContactListItem::GetContactListItemType();
}

bool CDirUIHelper::RefreshItem(int nState, xListViewItem * pItem)
{
    if (DIR_UI_STATE_CHOOSE_NUMBER != nState)
    {
        return false;
    }

    CDetailListItemPtr pDetailItem = CDetailListItem::GetDetailListItem(pItem);

    if (NULL == pDetailItem)
    {
        DIRUI_ERR("Get detail item fail in choose number state.");
        return false;
    }

    int nDataIndex = pItem->GetDataIndex();
    yl::string strName = "";
    yl::string strNumber = "";

    if (!CDirectoryControler::GetAttributeByIndex(nDataIndex, strName, strNumber))
    {
        DIRUI_ERR("Get attribute fail in choose number state.");
        return false;
    }

    pDetailItem->SetTitle(strName);
    pDetailItem->SetValue(strNumber);
    pDetailItem->SetItemData(DETAIL_DATA_TYPE_NUMBER);

    return true;
}

void CDirUIHelper::GetMenuTitle(yl::string & strTitle)
{

}

bool CDirUIHelper::GetGroupTitleList(int & nGroupIndex, VEC_GROUP_TITLE & vecTitle)
{
    return false;
}

bool CDirUIHelper::GetNormalTitle(yl::string & strTitle, int nGroupId/* = knInvalidRId*/)
{
    return false;
}

void CDirUIHelper::GetSearchTitle(yl::string & strTitle)
{
    strTitle = _LANG(TRID_SEARCH);
}

bool CDirUIHelper::GetDetailTitle(yl::string & strTitle)
{
    return false;
}

// 默认处理联系人的Option
bool CDirUIHelper::GetOption(int nUIState, VEC_OPTION_ITEM_DATA & vecOption)
{
    if (DIR_UI_STATE_DETAIL == nUIState)
    {
        return false;
    }

    ST_OPTION_ITEM_DATA stOption;

    PushDetailOption(stOption, vecOption);

    // 默认可以添加到本地联系人
    if (CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_LOCAL, 0))
    {
        stOption.m_nAction = ST_COPY_TO_LOCAL_GROUP;
        stOption.m_strTitle = _LANG(TRID_ADD_TO_CONTACT);

        vecOption.push_back(stOption);
    }

    // 默认可以添加到黑名单
    if (CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_LOCAL, kszBlackListGroupId))
    {
        stOption.m_nAction = ST_COPY_TO_BLACKLIST;
        stOption.m_strTitle = _LANG(TRID_ADD_TO_BLACK_LIST);

        vecOption.push_back(stOption);
    }

    return true;
}

int CDirUIHelper::GetContactCopyAction(int nUIAction)
{
    if (ST_COPY_TO_BLACKLIST == nUIAction)
    {
        return DIR_ACTION_LOCAL_ENTER_COPY_TO_BLACKLIST;
    }
    else if (ST_COPY_TO_LOCAL_GROUP == nUIAction)
    {
        return DIR_ACTION_LOCAL_ENTER_COPY_TO_CONTACTS;
    }

    return DIR_ACTION_NONE;
}

int CDirUIHelper::GetActionByKeyPress(int nUIState, int nKey)
{
    if (NULL == m_pListView)
    {
        return DIR_ACTION_NONE;
    }

#ifndef _WIN32
#warning
#endif
    if (PHONE_KEY_OK == nKey || (/*1 == configParser_GetConfigInt(kszCallDirectoryByOffHook)*/
                /*&&*/ (PHONE_KEY_HANDSET_OFF_HOOK == nKey || PHONE_KEY_HANDFREE == nKey
                        || PHONE_KEY_EHS_OPEN == nKey)))
    {
        xListViewItemPtr pFocusItem = m_pListView->GetFocusItem();

        if (DIR_UI_STATE_DETAIL == nUIState || DIR_UI_STATE_CHOOSE_NUMBER == nUIState)
        {
            CDetailListItemPtr pDetailItem = CDetailListItem::GetDetailListItem(pFocusItem);

            if (NULL == pDetailItem)
            {
                return DIR_ACTION_NONE;
            }

            int nData = pDetailItem->GetData();

            if (DETAIL_DATA_TYPE_NUMBER != nData)
            {
                return DIR_ACTION_NONE;
            }

            return DIR_ACTION_COMMON_SEND;
        }

        if (IsFocusItemCanDial(pFocusItem))
        {
            return DIR_ACTION_COMMON_SEND;
        }
    }

    return DIR_ACTION_NONE;
}

int CDirUIHelper::GetActionByUIAction(int nUIState, int nUIAction)
{
    if (ST_SEND == nUIAction)
    {
        return DIR_ACTION_COMMON_SEND;
    }
    else if (ST_ENTER == nUIAction)
    {
        return DIR_ACTION_COMMON_SELECT;
    }
    else if (ST_DETAIL == nUIAction)
    {
        return DIR_ACTION_COMMON_DETAIL;
    }

    return DIR_ACTION_NONE;
}

void CDirUIHelper::PushDetailOption(ST_OPTION_ITEM_DATA & stOption,
                                    VEC_OPTION_ITEM_DATA & vecOption)
{
    stOption.m_nAction = ST_DETAIL;
    stOption.m_strTitle = _LANG(TRID_DETAIL);

    vecOption.push_back(stOption);
}

bool CDirUIHelper::GetContactSoftKey(xListViewItem * pFocusItem, int nUIState,
                                     int nGroupId, CArraySoftKeyBarData & vecSoftKey)
{
    if (NULL == pFocusItem)
    {
        return false;
    }

    if (CDirectoryControler::IsDirCanEdit(m_nType, nGroupId))
    {
        VEC_OPTION_ITEM_DATA vecOption;

        GetOption(nUIState, vecOption);

        int nOptionCount = vecOption.size();

        if (nOptionCount > 1)
        {
            vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_OPTION);
            vecSoftKey[2].m_iType = ST_OPTION;
        }
        else if (1 == nOptionCount)
        {
            ST_OPTION_ITEM_DATA & stOption = vecOption[0];

            vecSoftKey[2].m_strSoftkeyTitle = stOption.m_strTitle;
            vecSoftKey[2].m_iType = stOption.m_nAction;
        }
    }
    else if (SESSION_PRETRAN == talklogic_GetSessionStateBySessionID(talklogic_GetFocusedSessionID()))
    {
        vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_BLIND_TRANSFER);
        vecSoftKey[2].m_iType = ST_BLIND_TRANSFER;
    }

    vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_DIAL);
    vecSoftKey[3].m_iType = ST_SEND;

    return true;
}

int CDirUIHelper::GetLisItemId(xListViewItem * pItem)
{
    return GetContactId(pItem);
}

bool CDirUIHelper::ProcessLineKey(int nKey, int nUIState)
{
    return false;
}

bool CDirUIHelper::ProcessDetailDial(int nUIState, int nAccountId/* = AUTO_ACCOUNT_IDENTIFY*/)
{
    if (NULL == m_pListView)
    {
        return false;
    }

    xListViewItemPtr pFocusItem = m_pListView->GetFocusItem();
    CDetailListItemPtr pDetailItem = CDetailListItem::GetDetailListItem(pFocusItem);

    if (NULL == pDetailItem)
    {
        return false;
    }

    int nData = pDetailItem->GetData();

    if (DETAIL_DATA_TYPE_NUMBER != nData)
    {
        return false;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();

    if (pControler.IsEmpty())
    {
        return false;
    }

    yl::string strName = "";

    if (!pControler->GetContactShowName(strName))
    {
        return false;
    }

    if (DIR_UI_STATE_CHOOSE_NUMBER == nUIState && AUTO_ACCOUNT_IDENTIFY == nAccountId)
    {
        nAccountId = CDirUIHelper::GetDialAccountId();
    }

    yl::string strNumber = pDetailItem->GetValue();

    DialNumber(nAccountId, strName, strNumber, pControler->GetDialAction());

    return true;
}

bool CDirUIHelper::IsFocusItemCanDial(xListViewItem * pItem)
{
    CContactListItemPtr pContactItem = CContactListItem::GetContactListItem(pItem);

    if (NULL != pContactItem)
    {
        return true;
    }

    return false;
}

void CDirUIHelper::OnEnterDetail(int nAction)
{

}

int CDirUIHelper::GetActionByItem(int nItemAction, int nItemId)
{
    if (LIST_VIEW_ITEM_ACTION_SELECT == nItemAction)
    {
        return DIR_ACTION_COMMON_SELECT;
    }

    return DIR_ACTION_NONE;
}

void CDirUIHelper::CheckGroupID(int& nGroupID)
{

}

bool CDirUIHelper::IsBackMenuPageOver(int nPageType, int nGroupID)
{
    return false;
}

int CDirUIHelper::GetContactId(xListViewItem * pItem)
{
    CContactListItem * pContactItem = CContactListItem::GetContactListItem(pItem);

    if (NULL != pContactItem)
    {
        return pContactItem->GetId();
    }

    return knInvalidRId;
}

bool CDirUIHelper::UpdateMenuItem(CMenuListItem * pMenuItem, int nId,
                                  const yl::string & strMenu, bool bShowIndex)
{
    if (NULL == pMenuItem)
    {
        return false;
    }

    pMenuItem->SetId(nId);
    pMenuItem->SetShowIndex(bShowIndex);
    pMenuItem->SetMenu(_LANG(strMenu));

    return true;
}

bool CDirUIHelper::UpdateContactItem(xListViewItem * pItem, int nId,
                                     const yl::string & strName, const yl::string & strNumber,
                                     const yl::string& strPhoto /*= PIC_CONTACT_NORMAL*/)
{
    CContactListItem * pContactItem = CContactListItem::GetContactListItem(pItem);

    if (NULL == pContactItem)
    {
        return false;
    }

    pContactItem->SetId(nId);
    pContactItem->SetName(strName);

    if (strNumber.empty())
    {
        pContactItem->SetInfo(_LANG(TRID_NONE));
    }
    else
    {
        pContactItem->SetInfo(strNumber);
    }

    pContactItem->SetPhoto(strPhoto);

#if IF_TEXTVIEW_SUPPORT_HIGH_LIGHT_TEXT
    if (1 == configParser_GetConfigInt(kszEnableHighLight))
    {
        CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
        if (!pControler.IsEmpty() && pControler->IsSearchState())
        {
            yl::string strSearchKey = pControler->GetSearchKey();
            yl::string strHightLightText = strSearchKey;
            if (!commonAPI_IsDigital(pControler->GetSearchKey()))
            {
                strHightLightText = commonAPI_transT9String(strSearchKey);
            }
            pContactItem->SetHightLightText(strHightLightText);
        }
    }
#endif

    return true;
}
