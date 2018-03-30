#if IF_FEATURE_METASWITCH_CALLLOG
#include "mtswcallloguihelper.h"
#include "mtswcalllogcontroler.h"
#include "directorymgr.h"
#include "callloglistitem.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "settingui/include/modsettingui.h"
#include "xwindow/xListView.h"
#if IS_COLOR
#include "namenumbercallloglistitem.h"
#endif

namespace
{
const char* const   kszPageMTSWCommportal = "mtswcommportal";
}

CMTSWCallLogUIHelper::CMTSWCallLogUIHelper()
    : CCommonUIHelper(CALL_LOG_TYPE_MTSW)
{
}

CMTSWCallLogUIHelper::~CMTSWCallLogUIHelper()
{
}

CMTSWCallLogUIHelper* CMTSWCallLogUIHelper::GetUIHelper(IDirUIHelperPtr& pDirUIHelper)
{
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);
    if (NULL == pUIHelper || CALL_LOG_TYPE_MTSW != pUIHelper->GetType())
    {
        return NULL;
    }
    return static_cast<CMTSWCallLogUIHelper *>(pUIHelper);
}

IMTSWCallLogUIHelper* CMTSWCallLogUIHelper::GetMTSWCalllogUIHelper(IDirUIHelperPtr & pDirUIHelper)
{
    CMTSWCallLogUIHelper * pMTSWUIHelper = CMTSWCallLogUIHelper::GetUIHelper(pDirUIHelper);

    return (IMTSWCallLogUIHelper *)pMTSWUIHelper;
}

void CMTSWCallLogUIHelper::OnLoginResultCallBack(bool bLoginSucceed)
{

}

void CMTSWCallLogUIHelper::EnterMTSWLoginPage()
{
    if (NULL != m_pListView)
    {
        m_pListView->ClearAllItem();
    }

    SettingUI_EnterPage(kszPageMTSWCommportal, (void*)CMTSWCallLogUIHelper::OnLoginResultCallBack);
}

int CMTSWCallLogUIHelper::GetBrowsePageType(int nGroupId)
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

int CMTSWCallLogUIHelper::GetListItemType(int nState, int nDataIndex)
{
    if (DIR_UI_STATE_BROWSER == nState)
    {
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

bool CMTSWCallLogUIHelper::GetOption(int nUIState, VEC_OPTION_ITEM_DATA& vecOption)
{
    ST_OPTION_ITEM_DATA stOption;

    stOption.m_nAction = ST_DETAIL;
    stOption.m_strTitle = TRID_DETAIL;
    vecOption.push_back(stOption);

    stOption.m_nAction = ST_MTSW_ADD_TO_LOCAL;
    stOption.m_strTitle = TRID_ADD_TO_PHONEBOOK;
    vecOption.push_back(stOption);

    if (MtswDir_IsEnable())
    {
        stOption.m_nAction = ST_ADD;
        stOption.m_strTitle = TRID_ADD_TO_MTSW_CONTACT;
        vecOption.push_back(stOption);
    }

    return true;
}

void CMTSWCallLogUIHelper::GetMenuTitle(yl::string& strTitle)
{
    strTitle = _LANG(TRID_MTSW_CALLLOG);
}

bool CMTSWCallLogUIHelper::GetNormalTitle(yl::string& strTitle, int nGroupId/* = knInvalidRId*/)
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CMTSWCallLogControler* pMTSWCallLog = CMTSWCallLogControler::GetControler(pControler);
    if (NULL == pMTSWCallLog)
    {
        return false;
    }

    yl::string strGroupName = "";

    if (knInvalidRId == nGroupId)
    {
        nGroupId = pMTSWCallLog->GetGroupId();
    }

    if (pMTSWCallLog->GetGroupName(nGroupId, strGroupName))
    {
        strTitle = _LANG(strGroupName.c_str());
        return true;
    }

    return false;
}

bool CMTSWCallLogUIHelper::GetSoftKey(xListViewItem* pFocusItem, int nUIState,
                                      CArraySoftKeyBarData& vecSoftKey)
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
    CMTSWCallLogControler* pMTSWCallLog = CMTSWCallLogControler::GetControler(pControler);
    if (NULL == pMTSWCallLog)
    {
        return false;
    }

    if (CDirectoryControler::IsDirCanEdit(m_nType, pMTSWCallLog->GetGroupId()))
    {
        vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_OPTION);
        vecSoftKey[2].m_iType = ST_OPTION;
    }

    vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_DIAL);
    vecSoftKey[3].m_iType = ST_SEND;

    return true;
}

int CMTSWCallLogUIHelper::GetActionByUIAction(int nUIState, int nUIAction)
{
    if (ST_ADD == nUIAction)
    {
        return DIR_ACTION_MTSW_ENTER_ADD_CONTACTLISTL;
    }
    else if (ST_EDIT == nUIAction)
    {
        return DIR_ACTION_LCLOG_EDIT;
    }
    else if (ST_MTSW_ADD_TO_LOCAL == nUIAction)
    {
        return DIR_ACTION_LOCAL_ENTER_COPY_TO_CONTACTS;
    }

    return CCommonUIHelper::GetActionByUIAction(nUIState, nUIAction);
}

bool CMTSWCallLogUIHelper::ProcessDetailDial(int nUIState, int nAccountId/* = AUTO_ACCOUNT_IDENTIFY*/)
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CMTSWCallLogControler* pMTSWCallLog = CMTSWCallLogControler::GetControler(pControler);

    if (NULL == pMTSWCallLog)
    {
        return false;
    }

    pMTSWCallLog->DialCallLog(nAccountId);
    return true;
}

#if !IF_DIR_UI_SHOW_CALLLOG_GROUP_LIST
void CMTSWCallLogUIHelper::CheckGroupID(int& nGroupID)
{
    if (knInvalidRId == nGroupID)
    {
        nGroupID = kszAllContactId;
    }
}
#endif

bool CMTSWCallLogUIHelper::UpdateListItem(xListViewItem* pListItem, int nId, SmartPtr& pDetail)
{
    MTSWCallLogRecord* pCallLog = CMTSWCallLogControler::GetMTSWCallLog(pDetail);
    if (NULL == pCallLog)
    {
        return false;
    }

    time_t tmTick = TransBsftCallLogTime(pCallLog->m_strBornTick.c_str(), true);
    yl::string strBornTick = Timet2String(tmTick, "", true, false, false);

    yl::string strName = "";
    yl::string strNumber = "";
    GetCallLogTextToShow(strName, strNumber, pCallLog->GetDisplayName(), pCallLog->GetNumber());
    if (strName.empty())
    {
        strName = TRID_ANONYMOUS;
    }

    return UpdateCallLogItem(pListItem, nId, pCallLog->GetLogType(), strName, strNumber, strBornTick);
}

#endif //#if IF_FEATURE_METASWITCH_CALLLOG
