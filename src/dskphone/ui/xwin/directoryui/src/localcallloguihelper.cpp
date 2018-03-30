#include "localcallloguihelper.h"
#include "directorymgr.h"
#include "directoryuicommon.h"
#include "directoryuicommon.h"
#include "translateiddefine.h"
#include "localcalllogcontroler.h"
#include "localcontactcontroler.h"
#include "callloglistitem.h"
#if IS_COLOR
#include "namenumbercallloglistitem.h"
#endif
#include "account/include/modaccount.h"
#include "languagehelper/modlangtranslate.h"
#include "baseui/include/baseuicommon.h"
#include "baseui/include/detaillistitem.h"
#if IF_DIR_UI_SHOW_CALLLOG_GROUP_LIST
#include "baseui/include/menulistitem.h"
#endif
#include "xwindow/xListView.h"
#include "commonunits/commonunits_def.h"
#include "talk/talklogic/include/talklogic_common.h"
#include <resource/modresource.h>


CLocalCallLogUIHelper::CLocalCallLogUIHelper()
    : CCommonUIHelper(CALL_LOG_TYPE_LOCAL)
{
}

CLocalCallLogUIHelper::~CLocalCallLogUIHelper()
{
}

CLocalCallLogUIHelper * CLocalCallLogUIHelper::GetUIHelper(IDirUIHelperPtr & pDirUIHelper)
{
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);

    if (NULL == pUIHelper || CALL_LOG_TYPE_LOCAL == pUIHelper->GetType())
    {
        return NULL;
    }

    return static_cast<CLocalCallLogUIHelper *>(pUIHelper);
}

int CLocalCallLogUIHelper::GetBrowsePageType(int nGroupId)
{
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

int CLocalCallLogUIHelper::GetListItemType(int nState, int nDataIndex)
{
    if (DIR_UI_STATE_BROWSER == nState)
    {
#if IF_DIR_UI_SHOW_CALLLOG_GROUP_LIST
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

#if IF_DIR_UI_SHOW_CALLLOG_GROUP_LIST
void CLocalCallLogUIHelper::GetMenuTitle(yl::string & strTitle)
{
    strTitle = _LANG(TRID_LOCAL_CALLLOG);
}
#endif

bool CLocalCallLogUIHelper::GetDetailTitle(yl::string & strTitle)
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CLocalCallLogControler * pLocalCallLog = CLocalCallLogControler::GetControler(pControler);

    if (NULL == pLocalCallLog)
    {
        return false;
    }

    SmartPtr pDetail = pLocalCallLog->GetCurrentDetail();
    LocalLogItemRecord * pCallLog = pLocalCallLog->GetLocalCallLog(pDetail);

    if (NULL == pCallLog)
    {
        return false;
    }

#if IF_BUG_27560
    yl::string strName = pCallLog->GetDisplayNameToUI();
#else
    yl::string strName = pCallLog->GetDisplayName();
#endif

    if (strName.empty())
    {
#if IF_BUG_27560
        strName = pCallLog->GetNumberToUI();
#else
        strName = pCallLog->GetNumber();
#endif
    }

    TransBsftFACText(strName, strTitle);

    return true;
}

bool CLocalCallLogUIHelper::GetSoftKey(xListViewItem * pFocusItem, int nUIState,
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

        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_DIAL);
        vecSoftKey[3].m_iType = ST_SEND;

        return true;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CLocalCallLogControler * pLocalCallLog = CLocalCallLogControler::GetControler(pControler);

    if (NULL == pLocalCallLog)
    {
        return false;
    }

    if (DIR_UI_STATE_CHOOSE_NUMBER == nUIState)
    {
        if (pLocalCallLog->IsChooseNumberToSend())
        {
            vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_DIAL);
        }
        else
        {
            vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_EDIT);
        }
        vecSoftKey[3].m_iType = ST_SEND;
        return true;
    }

    int nGroupId = pLocalCallLog->GetGroupId();

#if IF_DIR_UI_SHOW_CALLLOG_GROUP_LIST
    if (DIR_UI_STATE_BROWSER == nUIState && knInvalidRId == nGroupId)
    {
        vecSoftKey[2].m_strSoftkeyTitle = "";
        vecSoftKey[2].m_iType = ST_EMPTY;

        return true;
    }
#endif

    if (CDirectoryControler::IsDirCanEdit(DIRECTORY_TYPE_LOCAL, nGroupId))
    {
        vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_DELETE);
        vecSoftKey[1].m_iType = ST_DELETE;
    }

    GetContactSoftKey(pFocusItem, nUIState, nGroupId, vecSoftKey);

    return true;
}

bool CLocalCallLogUIHelper::GetOption(int nUIState, VEC_OPTION_ITEM_DATA & vecOption)
{
    if (!CCommonUIHelper::GetOption(nUIState, vecOption))
    {
        return false;
    }

    ST_OPTION_ITEM_DATA stOption;

    stOption.m_nAction = ST_DELETEALL;
    stOption.m_strTitle = _LANG(TRID_DELETE_ALL);

    vecOption.push_back(stOption);

    return true;
}

int CLocalCallLogUIHelper::GetActionByKeyPress(int nUIState, int nKey)
{
    return CCommonUIHelper::GetCallLogActionByKey(nUIState, nKey);
}

int CLocalCallLogUIHelper::GetActionByUIAction(int nUIState, int nUIAction)
{
    int nAction = GetContactCopyAction(nUIAction);

    if (DIR_ACTION_NONE != nAction)
    {
        return nAction;
    }

    if (ST_EDIT == nUIAction)
    {
        return DIR_ACTION_LCLOG_EDIT;
    }
    else if (ST_DELETE == nUIAction)
    {
        return DIR_ACTION_LCLOG_DEL_CALL_LOG;
    }
    else if (ST_DELETEALL == nUIAction)
    {
        return DIR_ACTION_LCLOG_DEL_ALL_CALL_LOG;
    }

    return CCommonUIHelper::GetActionByUIAction(nUIState, nUIAction);
}

int CLocalCallLogUIHelper::GetLisItemId(xListViewItem * pItem)
{
    int nCallLogId = GetCallLogId(pItem);

    if (knInvalidRId != nCallLogId)
    {
        return nCallLogId;
    }

    return CCommonUIHelper::GetLisItemId(pItem);
}

bool CLocalCallLogUIHelper::ProcessDetailDial(int nUIState,
        int nAccountId/* = AUTO_ACCOUNT_IDENTIFY*/)
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CLocalCallLogControler * pLocalCallLog = CLocalCallLogControler::GetControler(pControler);

    if (NULL == pLocalCallLog)
    {
        return false;
    }

    if (DIR_UI_STATE_CHOOSE_NUMBER == nUIState)
    {
        if (NULL == m_pListView || !pLocalCallLog->IsChooseNumberToSend())
        {
            return false;
        }

        xListViewItemPtr pFocusItem = m_pListView->GetFocusItem();
        CDetailListItemPtr pDetailItem = CDetailListItem::GetDetailListItem(pFocusItem);

        if (NULL == pDetailItem)
        {
            return false;
        }

        return pLocalCallLog->OnNumberChooseResult(nAccountId, pDetailItem->GetValue());
    }

    pLocalCallLog->DialCallLog(nAccountId);

    return true;
}

#if !IF_DIR_UI_SHOW_CALLLOG_GROUP_LIST
void CLocalCallLogUIHelper::CheckGroupID(int& nGroupID)
{
    if (knInvalidRId == nGroupID)
    {
        nGroupID = kszAllContactId;
    }
}
#endif

bool CLocalCallLogUIHelper::UpdateListItem(xListViewItem * pListItem, int nId, SmartPtr & pDetail)
{
#if IF_DIR_UI_SHOW_CALLLOG_GROUP_LIST
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

    LocalLogItemRecord * pCallLog = CLocalCallLogControler::GetLocalCallLog(pDetail);

    if (NULL == pCallLog)
    {
        return false;
    }

    yl::string strBornTick = Timet2String((time_t)atoi(pCallLog->m_strBornTick.c_str()));

#if IF_BUG_27560
    yl::string strName = pCallLog->GetDisplayNameToUI();
    yl::string strNumber = pCallLog->GetNumberToUI();
#else
    const yl::string & strName = pCallLog->GetDisplayName();
    yl::string strNumber = pCallLog->GetNumber();
#endif
    yl::string strDstName;
    yl::string strDstNumber;

#if IF_FEATURE_CALLLIST_SHOW_METHOD
    GetCallLogDisplayInfo(strDstName, strName, strNumber, pCallLog->m_strRemoteSipServer);
#else
    GetCallLogTextToShow(strDstName, strDstNumber, strName, strNumber);
#endif

    yl::string strTransName;
    yl::string strTransNumber;

    TransBsftFACText(strDstName, strTransName);
    TransBsftFACText(strDstNumber, strTransNumber);

    CALL_TYPE eCallType = pCallLog->GetLogType();

    return UpdateCallLogItem(pListItem, nId, eCallType, strTransName, strTransNumber, strBornTick);
}
