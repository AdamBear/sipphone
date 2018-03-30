#include "commonuihelper.h"
#include "directorymgr.h"
#include "commondirctrl.h"
#include "translateiddefine.h"
#include "callloglistitem.h"
#if IS_COLOR
#include "namenumbercallloglistitem.h"
#endif
#include "baseui/include/menulistitem.h"
#include "baseui/include/detaillistitem.h"

#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "talk/talklogic/include/talklogic_common.h"
#include "languagehelper/modlangtranslate.h"
#include "directoryuicommon.h"
#include "dirctrlfactory.h"
#include "xwindow/xListView.h"


CCommonUIHelper::CCommonUIHelper(int nType)
    : CDirUIHelper(nType)
{
}

CCommonUIHelper::~CCommonUIHelper()
{
}

int CCommonUIHelper::GetBrowsePageType(int nGroupId)
{
    if (knInvalidRId == nGroupId || kszRootId == nGroupId)
    {
        return BROWSER_PAGE_TYPE_MENU;
    }

#if IF_DIR_UI_SWITCH_GROUP_IN_BROWSE
    CDirectoryControlerPtr pControler = g_DirCtrlFactory.FindControler(m_nType);

    if (!pControler.IsEmpty() && kszRootId == pControler->GetParGroupId(nGroupId))
    {
        return BROWSER_PAGE_TYPE_GROUP_SWITCH;
    }
#endif

    return BROWSER_PAGE_TYPE_NORMAL;
}

bool CCommonUIHelper::GetBrowseAdjoinGroup(bool bPrevious, int & nGroupId, int & nGroupIndex)
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CCommonDirCtrl * pCommonDirCtrl = CCommonDirCtrl::GetCommonControler(pControler);

    if (NULL == pCommonDirCtrl)
    {
        return false;
    }

    VEC_CONTACT_ID vecGroupId;

    if (!pCommonDirCtrl->GetBrowseGroupList(vecGroupId))
    {
        return false;
    }

    int nCurGroupId = pCommonDirCtrl->GetGroupId();

    nGroupId = GetAdjoinGroupId(vecGroupId, bPrevious, nCurGroupId, nGroupIndex);

    return knInvalidRId != nGroupId;
}

bool CCommonUIHelper::IsInRootGroup()
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();

    if (pControler.IsEmpty())
    {
        return false;
    }

    int nGroupId = pControler->GetGroupId();

    if (knInvalidRId == nGroupId || kszRootId == nGroupId)
    {
        return true;
    }

    return false;
}

int CCommonUIHelper::GetListItemType(int nState, int nDataIndex)
{
    if (DIR_UI_STATE_BROWSER == nState)
    {
        if (IsInRootGroup())
        {
            return CMenuListItem::GetMenuItemType();
        }

        return CContactListItem::GetContactListItemType();
    }

    return CDirUIHelper::GetListItemType(nState, nDataIndex);
}

bool CCommonUIHelper::RefreshItem(int nState, xListViewItem * pItem)
{
    if (CDirUIHelper::RefreshItem(nState, pItem))
    {
        return true;
    }

    if (NULL == pItem)
    {
        return false;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CCommonDirCtrl * pCommonControler = CCommonDirCtrl::GetCommonControler(pControler);

    if (NULL == pCommonControler)
    {
        return false;
    }

    int nId = knInvalidRId;
    SmartPtr pDetail = pCommonControler->GetDetailByIndex(nId, pItem->GetDataIndex());

    if (pDetail.IsEmpty())
    {
        DIRUI_ERR("CCommonUIHelper::RefreshItem() fail, DataIndex:%d", pItem->GetDataIndex());
        return false;
    }

    return UpdateListItem(pItem, nId, pDetail);
}

bool CCommonUIHelper::GetGroupTitleList(int & nGroupIndex, VEC_GROUP_TITLE & vecTitle)
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CCommonDirCtrl * pCommonDirCtrl = CCommonDirCtrl::GetCommonControler(pControler);

    if (NULL == pCommonDirCtrl)
    {
        return false;
    }

    VEC_CONTACT_ID vecGroupId;

    if (!pCommonDirCtrl->GetBrowseGroupList(vecGroupId))
    {
        return false;
    }

    int nCurGroupId = pCommonDirCtrl->GetGroupId();
    int nSize = vecGroupId.size();

    for (int i = 0; i < nSize; ++i)
    {
        int nGroupId = vecGroupId[i];
        yl::string strTitle;

        if (!GetNormalTitle(strTitle, nGroupId))
        {
            continue;
        }

        vecTitle.push_back(strTitle);

        if (nGroupId == nCurGroupId)
        {
            nGroupIndex = i;
        }
    }

    return true;
}

// 默认每个组都进行翻译(目前用于Local CallLog和Network CallLog)
bool CCommonUIHelper::GetNormalTitle(yl::string & strTitle, int nGroupId/* = knInvalidRId*/)
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CCommonDirCtrl * pCommonDirCtrl = CCommonDirCtrl::GetCommonControler(pControler);

    if (NULL == pCommonDirCtrl)
    {
        return false;
    }

    yl::string strGroupName;

    if (knInvalidRId == nGroupId)
    {
        nGroupId = pCommonDirCtrl->GetGroupId();
    }

    if (pCommonDirCtrl->GetGroupName(nGroupId, strGroupName))
    {
        strTitle = _LANG(strGroupName.c_str());
        return true;
    }

    return false;
}

bool CCommonUIHelper::GetDetailTitle(yl::string & strTitle)
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CCommonDirCtrl * pCommonDirCtrl = CCommonDirCtrl::GetCommonControler(pControler);

    if (NULL == pCommonDirCtrl)
    {
        return false;
    }

    SmartPtr pDetail = pCommonDirCtrl->GetCurrentDetail();

    if (pDetail.IsEmpty())
    {
        return false;
    }

    strTitle = pDetail->GetDisplayName();

    return true;
}

int CCommonUIHelper::GetLisItemId(xListViewItem * pItem)
{
    CMenuListItemPtr pMenuItem = CMenuListItem::GetMenuListItem(pItem);

    if (NULL != pMenuItem)
    {
        return pMenuItem->GetId();
    }

    CCallLogListItemPtr pCallLogItem = CCallLogListItem::GetCallLogListItem(pItem);
    if (NULL != pCallLogItem)
    {
        return pCallLogItem->GetId();
    }

#if IS_COLOR
    CNameNumberCallLogListItem* pNameNumberItem = CNameNumberCallLogListItem::GetNameNumberCallLogListItem(pItem);
    if (NULL != pNameNumberItem)
    {
        return pNameNumberItem->GetId();
    }
#endif

    return CDirUIHelper::GetLisItemId(pItem);
}

bool CCommonUIHelper::IsFocusItemCanDial(xListViewItem * pItem)
{
    CCallLogListItemPtr pCallLogItem = CCallLogListItem::GetCallLogListItem(pItem);
    if (NULL != pCallLogItem)
    {
        return true;
    }

#if IS_COLOR
    CNameNumberCallLogListItem* pNameNumberItem = CNameNumberCallLogListItem::GetNameNumberCallLogListItem(pItem);
    if (NULL != pNameNumberItem)
    {
        return true;
    }
#endif

    return CDirUIHelper::IsFocusItemCanDial(pItem);
}

int CCommonUIHelper::GetActionByItem(int nItemAction, int nItemId)
{
    if (LIST_VIEW_ITEM_ACTION_SELECT == nItemAction)
    {
        CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
        CCommonDirCtrl * pCommonDirCtrl = CCommonDirCtrl::GetCommonControler(pControler);

        if (NULL != pCommonDirCtrl)
        {
            if (pCommonDirCtrl->IsGroupId(nItemId))
            {
                return DIR_ACTION_COMMON_SELECT;
            }
            else
            {
                return DIR_ACTION_COMMON_SEND;
            }
        }
    }

    return CDirUIHelper::GetActionByItem(nItemAction, nItemId);
}

int CCommonUIHelper::GetCallLogActionByKey(int nUIState, int nKey)
{
    if (PHONE_KEY_OK == nKey)
    {
        if (DIR_UI_STATE_BROWSER == nUIState)
        {
#if IF_DIR_UI_SHOW_CALLLOG_GROUP_LIST
            if (IsInRootGroup())
            {
                return DIR_ACTION_COMMON_SELECT;
            }
#endif

            if (NULL != m_pListView && NULL != m_pListView->GetFocusItem())
            {
                return DIR_ACTION_COMMON_DETAIL;
            }

            return DIR_ACTION_NONE;
        }
        else if (DIR_UI_STATE_DETAIL == nUIState)
        {
            return DIR_ACTION_COMMON_SEND;
        }
    }

    if (1 == configParser_GetConfigInt(kszCallHistoryByOffHook) && (PHONE_KEY_HANDSET_OFF_HOOK == nKey
            || PHONE_KEY_HANDFREE == nKey || PHONE_KEY_EHS_OPEN == nKey)
#if IF_DIR_UI_SHOW_CALLLOG_GROUP_LIST
            && (!(DIR_UI_STATE_BROWSER == nUIState && IsInRootGroup()))
#endif
       )
    {
        return DIR_ACTION_COMMON_SEND;
    }

    return DIR_ACTION_NONE;
}

int CCommonUIHelper::GetCallLogId(xListViewItem * pItem)
{
    CCallLogListItemPtr pCallLogItem = CCallLogListItem::GetCallLogListItem(pItem);
    if (NULL != pCallLogItem)
    {
        return pCallLogItem->GetId();
    }

#if IS_COLOR
    CNameNumberCallLogListItem* pNameNumberItem = CNameNumberCallLogListItem::GetNameNumberCallLogListItem(pItem);
    if (NULL != pNameNumberItem)
    {
        return pNameNumberItem->GetId();
    }
#endif

    return knInvalidRId;
}

#if IF_FEATURE_CALLLIST_SHOW_METHOD
bool CCommonUIHelper::GetCallLogDisplayInfo(yl::string & strText, const yl::string & strName,
        const yl::string & strNumber, const yl::string & strSipServer)
{
    if (strName.empty() && strNumber.empty())
    {
        return false;
    }

    strText = "";

    int nDisplayMethod = configParser_GetConfigInt(kszCalllogShowNumEnable);
    yl::string strSeparator = "/";

    switch (nDisplayMethod)
    {
    case CL_SHOW_ONLY_NUMBER:
        {
            strText = strNumber.empty() ? strName : strNumber;
        }
        break;
    case CL_SHOW_NAME_AND_NUMBER:
        {
            if (strName.empty())
            {
                strText = strNumber;
            }
            else
            {
                strText = strNumber.empty() ? strName : (strName + strSeparator + strNumber);
            }
        }
        break;
    case CL_SHOW_NUMBER_AND_NAME:
        {
            if (strNumber.empty())
            {
                strText = strName;
            }
            else
            {
                strText = strName.empty() ? strNumber : (strNumber + strSeparator + strName);
            }
        }
        break;
    case CL_SHOW_FULL_CONTACT_INFO:
        {
            //local record full contact: "name" <sip:number@server>;
            //bsft record full contact: "name" <sip:number>;
            if (!strName.empty())
            {
                strText = "\"" + strName + "\" ";
            }
            strText += "<sip:" + strNumber;
            if (!strSipServer.empty())
            {
                strText += "@" + strSipServer;
            }
            strText += ">";
        }
        break;
    default:
        {
            strText = strName.empty() ? strNumber : strName;
        }
        break;
    }

    return true;
}
#endif

void CCommonUIHelper::GetCallLogTextToShow(yl::string& strDstName, yl::string& strDstNumber,
        const yl::string& strSrcName, const yl::string& strSrcNumber)
{
    if (strSrcName.empty() && strSrcNumber.empty())
    {
        return;
    }

    strDstNumber = "";

    int nDisplayMethod = configParser_GetConfigInt(kszCalllogShowNumEnable);

    switch (nDisplayMethod)
    {
    case CL_NORMAL_SHOW_ONLY_NAME:
        {
            strDstName = strSrcName.empty() ? strSrcNumber : strSrcName;
        }
        break;
    case CL_NORMAL_SHOW_ONLY_NUMBER:
        {
            strDstName = strSrcNumber.empty() ? strSrcName : strSrcNumber;
        }
        break;
    case CL_NORMAL_SHOW_NAME_AND_NUMBER:
        {
            if (strSrcNumber.empty() || strSrcName.empty())
            {
                strDstName = strSrcName.empty() ? strSrcNumber : strSrcName;
                strDstNumber = "";
            }
            else
            {
#if IS_COLOR
                strDstName = strSrcName;
                strDstNumber = strSrcNumber;
#else
                strDstName = commonAPI_FormatString("%s(%s)", strSrcName.c_str(), strSrcNumber.c_str());
                strDstNumber = "";
#endif
            }
        }
        break;
    default:
        {
            strDstName = strSrcName.empty() ? strSrcNumber : strSrcName;
        }
        break;
    }
}

bool CCommonUIHelper::UpdateCallLogItem(xListViewItem * pItem, int nId, int nCallType,
                                        const yl::string & strName, const yl::string & strNumber,
                                        const yl::string & strBornTick)
{
    CCallLogListItemPtr pCallLogItem = CCallLogListItem::GetCallLogListItem(pItem);
    if (NULL != pCallLogItem)
    {
        pCallLogItem->SetId(nId);
        pCallLogItem->SetName(strName);
        pCallLogItem->SetInfo(strBornTick);
        pCallLogItem->SetCallType(nCallType);
        return true;
    }

#if IS_COLOR
    CNameNumberCallLogListItem* pNameNumberCalllog = CNameNumberCallLogListItem::GetNameNumberCallLogListItem(pItem);
    if (NULL != pNameNumberCalllog)
    {
        pNameNumberCalllog->SetId(nId);
        pNameNumberCalllog->SetName(strName);
        pNameNumberCalllog->SetInfo(strBornTick);
        pNameNumberCalllog->SetCallType(nCallType);
        pNameNumberCalllog->SetNumber(strNumber);
        return true;
    }
#endif

    return false;
}

#if IS_COLOR
bool CCommonUIHelper::IsCallLogShowNameAndNumber(int nDataIndex)
{
    if (CL_NORMAL_SHOW_NAME_AND_NUMBER != configParser_GetConfigInt(kszCalllogShowNumEnable))
    {
        return false;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CCommonDirCtrl * pCommonControler = CCommonDirCtrl::GetCommonControler(pControler);
    if (NULL == pCommonControler)
    {
        return false;
    }

    int nId = knInvalidRId;
    SmartPtr pDetail = pCommonControler->GetDetailByIndex(nId, nDataIndex);
    BaseContactRecord* pRecord = (BaseContactRecord*)pDetail.Get();
    if (NULL == pRecord)
    {
        return false;
    }

    yl::string strName = pRecord->GetDisplayName();
    yl::string strNumber = "";

    if (!pRecord->GetNumberByIndex(0, strNumber) || strName.empty() || strNumber.empty())
    {
        return false;
    }

    return true;
}
#endif
