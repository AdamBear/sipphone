#include "bsftcallloguihelper.h"
#include "bsftcalllogcontroler.h"
#include "bsftcontactcontroler.h"
#include "directorycommon.h"
#include "directorymgr.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#if IS_COLOR
#include "namenumbercallloglistitem.h"
#endif
#if IF_DIR_UI_SHOW_CALLLOG_GROUP_LIST || IF_FEATURE_MUTI_CALLLOG
#include "baseui/include/menulistitem.h"
#endif
#include "callloglistitem.h"
#include "xwindow/xListView.h"
#include "account/include/modaccount.h"


CBSFTCallLogUIHelper::CBSFTCallLogUIHelper()
    : CCommonUIHelper(CALL_LOG_TYPE_BROADSOFT)
{
}

CBSFTCallLogUIHelper::~CBSFTCallLogUIHelper()
{
}

CBSFTCallLogUIHelper * CBSFTCallLogUIHelper::GetUIHelper(IDirUIHelperPtr & pDirUIHelper)
{
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);

    if (NULL == pUIHelper || CALL_LOG_TYPE_BROADSOFT != pUIHelper->GetType())
    {
        return NULL;
    }

    return static_cast<CBSFTCallLogUIHelper *>(pUIHelper);
}

int CBSFTCallLogUIHelper::GetBrowsePageType(int nGroupId)
{
#if IF_FEATURE_MUTI_CALLLOG
    if (kszRootId == nGroupId || knInvalidRId == nGroupId)
    {
        return BROWSER_PAGE_TYPE_MENU;
    }
    else if (BsftLog_IsMutilLogEnable() && BsftLog_IsAccountGroup(nGroupId))
    {
        return BROWSER_PAGE_TYPE_NORMAL;
    }
#endif

#if IF_DIR_UI_SWITCH_GROUP_IN_BROWSE
    return BROWSER_PAGE_TYPE_GROUP_SWITCH;
#else
#if IF_DIR_UI_SHOW_CALLLOG_GROUP_LIST
    if (knInvalidRId == nGroupId)
    {
        return BROWSER_PAGE_TYPE_MENU;
    }
#endif

    return BROWSER_PAGE_TYPE_NORMAL;
#endif
}

int CBSFTCallLogUIHelper::GetListItemType(int nState, int nDataIndex)
{
    if (DIR_UI_STATE_BROWSER == nState)
    {
#if IF_DIR_UI_SHOW_CALLLOG_GROUP_LIST || IF_FEATURE_MUTI_CALLLOG
        if (IsInRootGroup())
        {
            return CMenuListItem::GetMenuItemType();
        }
#endif

#if !IF_FEATURE_CALLLIST_SHOW_METHOD && IS_COLOR
        if (IsCallLogShowNameAndNumber(nDataIndex))
        {
            return CNameNumberCallLogListItem::GetNameNumberCallLogListItemType();
        }
#endif

        return CCallLogListItem::GetCallLogListItemType();
    }

    return CDirUIHelper::GetListItemType(nState, nDataIndex);
}

bool CBSFTCallLogUIHelper::GetOption(int nUIState, VEC_OPTION_ITEM_DATA & vecOption)
{
    if (DIR_UI_STATE_BROWSER != nUIState)
    {
        return false;
    }

    ST_OPTION_ITEM_DATA stOption;

    PushDetailOption(stOption, vecOption);

    if (CBSFTContactControler::IsExitPersonal()
            && CDirectoryControler::IsDirCanEdit(DIRECTORY_TYPE_BROADSOFT, kszPersonalId))
    {
        stOption.m_nAction = ST_COPY_TO_PERSONAL;
        stOption.m_strTitle = _LANG(TRID_ADD_TO_PERSONAL);
        vecOption.push_back(stOption);
    }

#if DIR_SUPPORT_NET_CALLLOG_DELETE
    if (IsCallLogDeleteable())
    {
        stOption.m_nAction = ST_DELETEALL;
        stOption.m_strTitle = TRID_DELETE_ALL;
        vecOption.push_back(stOption);
    }
#endif

    return true;
}

#if IF_DIR_UI_SHOW_CALLLOG_GROUP_LIST || IF_FEATURE_MUTI_CALLLOG
void CBSFTCallLogUIHelper::GetMenuTitle(yl::string & strTitle)
{
#if IF_FEATURE_MUTI_CALLLOG && IF_DIR_UI_SHOW_CALLLOG_GROUP_LIST
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CBSFTCallLogControler * pBsftCallLogControler = CBSFTCallLogControler::GetControler(pControler);
    if (NULL == pBsftCallLogControler)
    {
        return;
    }

    int nID = pBsftCallLogControler->GetAccountID();
    if (-1 != nID && CCommonDirCtrl::GetDisplayName(BSFT_LOG, nID, strTitle))
    {
        //账号选择界面点击后的类型选择界面
        return;
    }
#endif

    //类型选择界面(不经过账号选择)
    strTitle = _LANG(TRID_NETWORK_CALLLOG);
}
#endif

bool CBSFTCallLogUIHelper::GetSoftKey(xListViewItem * pFocusItem, int nUIState,
                                      CArraySoftKeyBarData & vecSoftKey)
{
    if (NULL == pFocusItem)
    {
        return false;
    }

    if (DIR_UI_STATE_DETAIL == nUIState)
    {
        vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_EDIT);
        vecSoftKey[1].m_iType = ST_EDIT;

#if DIR_SUPPORT_NET_CALLLOG_DELETE
        if (IsCallLogDeleteable())
        {
            vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_DELETE);
            vecSoftKey[2].m_iType = ST_DELETE;
        }
#endif

        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_DIAL);
        vecSoftKey[3].m_iType = ST_SEND;

        return true;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CBSFTCallLogControler * pBsftCallLogControler = CBSFTCallLogControler::GetControler(pControler);

    if (NULL == pBsftCallLogControler)
    {
        return false;
    }

    int nGroupId = pBsftCallLogControler->GetGroupId();

#if IF_DIR_UI_SHOW_CALLLOG_GROUP_LIST || IF_FEATURE_MUTI_CALLLOG
    if (DIR_UI_STATE_BROWSER == nUIState
            && (knInvalidRId == nGroupId || kszRootId == nGroupId))
    {
        vecSoftKey[2].m_strSoftkeyTitle = "";
        vecSoftKey[2].m_iType = ST_EMPTY;

        return true;
    }
#endif

#if DIR_SUPPORT_NET_CALLLOG_DELETE
    if (DIR_UI_STATE_BROWSER == nUIState && IsCallLogDeleteable())
    {
        vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_DELETE);
        vecSoftKey[1].m_iType = ST_DELETE;
    }
#endif

    GetContactSoftKey(pFocusItem, nUIState, nGroupId, vecSoftKey);

    return true;
}

int CBSFTCallLogUIHelper::GetActionByKeyPress(int nUIState, int nKey)
{
#if IF_FEATURE_MUTI_CALLLOG
    if (DIR_UI_STATE_BROWSER == nUIState && PHONE_KEY_OK == nKey)
    {
        CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
        CBSFTCallLogControler * pBSFTCallLogControler = CBSFTCallLogControler::GetControler(pControler);

        if (NULL != pBSFTCallLogControler && pBSFTCallLogControler->IsGroupId(GetFocusId()))
        {
            return DIR_ACTION_COMMON_SELECT;
        }
    }
#endif

    return GetCallLogActionByKey(nUIState, nKey);
}

int CBSFTCallLogUIHelper::GetActionByUIAction(int nUIState, int nUIAction)
{
    if (ST_COPY_TO_PERSONAL == nUIAction)
    {
        return DIR_ACTION_BSFT_ENTER_ADD_TO_PERSONAL;
    }
    else if (ST_EDIT == nUIAction)
    {
        return DIR_ACTION_BSFTLOG_EDIT;
    }
#if DIR_SUPPORT_NET_CALLLOG_DELETE
    else if (ST_DELETE == nUIAction)
    {
        return DIR_ACTION_BSFTLOG_DELETE;
    }
    else if (ST_DELETEALL == nUIAction)
    {
        return DIR_ACTION_BSFTLOG_CLEAR;
    }
#endif

    return CCommonUIHelper::GetActionByUIAction(nUIState, nUIAction);
}

int CBSFTCallLogUIHelper::GetLisItemId(xListViewItem * pItem)
{
    int nCallLogId = GetCallLogId(pItem);

    if (knInvalidRId != nCallLogId)
    {
        return nCallLogId;
    }

    return CCommonUIHelper::GetLisItemId(pItem);
}

bool CBSFTCallLogUIHelper::ProcessDetailDial(int nUIState,
        int nAccountId/* = AUTO_ACCOUNT_IDENTIFY*/)
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CBSFTCallLogControler * pBSFTCallLogControler = CBSFTCallLogControler::GetControler(pControler);

    if (NULL == pBSFTCallLogControler)
    {
        return false;
    }

    pBSFTCallLogControler->DialCallLog(nAccountId);
    return true;
}

void CBSFTCallLogUIHelper::CheckGroupID(int& nGroupID)
{
#if IF_FEATURE_MUTI_CALLLOG
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CBSFTCallLogControler * pBSFTCallLogControl = CBSFTCallLogControler::GetControler(pControler);
    if (NULL != pBSFTCallLogControl && -1 != pBSFTCallLogControl->GetAccountID())
    {
        return;
    }

    if (BsftLog_IsMutilLogEnable())
    {
        CALL_TYPE eCallType = CT_CALLALL;
        int nAccountID = -1;

        if (kszPlacedLogId == nGroupID)
        {
            eCallType = CT_OUTGOING;

            yl::string strNumber = "";
            LocalLog_GetLastOutCallNumAndAccountID(strNumber, nAccountID);
        }
        else if (kszMissLogId == nGroupID)
        {
            eCallType = CT_MISSCALL;
            nAccountID = LocalLog_GetLastChildMissAccountID();
        }

        int nAvailableNum = acc_GetAvailableAccountCount();

        if (nAvailableNum > 1 && nAccountID < 0)
        {
            nGroupID = kszRootId;
            pBSFTCallLogControl->SetCallType(eCallType);
        }
        else
        {
#ifndef WIN32
#warning "需要等逻辑层添加0注册账号时获取呼叫类型组的接口,1个账号待确认"
#endif // !WIN32

#if !IF_DIR_UI_SHOW_CALLLOG_GROUP_LIST
            int nID = nAccountID;
            if (nID < 0)
            {
                nID = (1 == nAvailableNum) ? acc_GetUsableAccount() : 0;
            }
            nGroupID = BsftLog_GetGroupIDByAccountID(nID, eCallType);
#endif
        }
    }
#endif

#if !IF_DIR_UI_SHOW_CALLLOG_GROUP_LIST
    if (knInvalidRId == nGroupID)
    {
        nGroupID = kszAllContactId;
    }
#endif
}

bool CBSFTCallLogUIHelper::UpdateListItem(xListViewItem * pListItem, int nId, SmartPtr & pDetail)
{
#if IF_DIR_UI_SHOW_CALLLOG_GROUP_LIST || IF_FEATURE_MUTI_CALLLOG
    CMenuListItem * pMenuItem = CMenuListItem::GetMenuListItem(pListItem);

    if (NULL != pMenuItem)
    {
        if (pDetail.IsEmpty())
        {
            return false;
        }

        UpdateMenuItem(pMenuItem, nId, pDetail->GetDisplayName(), true);

        return true;
    }
#endif

    BSFTCallLogRecord * pCallLog = CBSFTCallLogControler::GetBSFTCallLog(pDetail);

    if (NULL == pCallLog)
    {
        return false;
    }

    time_t tmTick = TransBsftCallLogTime(pCallLog->m_strBornTick.c_str(), true);
    yl::string strBornTick = Timet2String(tmTick);
    CALL_TYPE eCallType = pCallLog->GetLogType();
    yl::string strCalllogName = pCallLog->GetDisplayName();
    yl::string strCalllogNumber = pCallLog->GetNumber();

    yl::string strName = "";
    yl::string strNumber = "";

#if IF_FEATURE_CALLLIST_SHOW_METHOD
    GetCallLogDisplayInfo(strName, strCalllogName, strCalllogNumber, "");
#else
    GetCallLogTextToShow(strName, strNumber, strCalllogName, strCalllogNumber);
#endif

    return UpdateCallLogItem(pListItem, nId, eCallType, strName, strNumber, strBornTick);
}

#if DIR_SUPPORT_NET_CALLLOG_DELETE
bool CBSFTCallLogUIHelper::IsCallLogDeleteable()
{
    if (BsftLog_LogDeleteEnable()
            && CDirectoryControler::IsDirCanEdit(CALL_LOG_TYPE_BROADSOFT, knInvalidRId))
    {
        return true;
    }

    return false;
}
#endif

#if IF_FEATURE_MUTI_CALLLOG
int CBSFTCallLogUIHelper::GetFocusId()
{
    if (NULL == m_pListView)
    {
        return knInvalidRId;
    }

    return GetLisItemId(m_pListView->GetFocusItem());
}

#endif
