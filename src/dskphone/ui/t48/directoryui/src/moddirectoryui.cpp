#include <QtGui>
#include "moddirectoryui.h"
#include "remoteserversearchcontroler.h"
#include "bsftcontactcontroler.h"
#include "bsftcontactuihelper.h"
#include "localcontactuihelper.h"
#include "delegatefactory.h"
#include "dirctrlfactory.h"
#include "directorymgr.h"
#include "dirdsskeymgr.h"
#include "diruihelperfactory.h"
#include "directorycommon.h"
#include "translateiddefine.h"
#include "baseui/t4xpicpath.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "callmanager/modcallmanager.h"
#include "uimanager/moduimanager.h"
#include "remotecontactcontroler.h"
#include "edkui/edkmapdefine.h"
#include "directoryui/include/moddirectoryuiopenlogic.h"
#include "edkui/modedkui.h"

#if IF_BUG_PLCMDIR
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#endif

#if IF_FEATURE_MUTI_CALLLOG
#include "account/include/modaccount.h"
#endif

void OnEnterCallPage(const yl::string & strUIName)
{
    if (CALLMGR_PAGE_NAME_DIRECTORY == strUIName)
    {
#if IF_BUG_PLCMDIR
        if (configParser_GetConfigInt(kszPlcmPhoneEnable) != 0)
        {
            g_DirectoryMgr.EnterDirectory(DIRECTORY_TYPE_PLCM, kszPlcmDirGroupId);
        }
        else
        {
            g_DirectoryMgr.EnterDirectory(DIRECTORY_TYPE_LOCAL, kszAllContactId);
        }
#else
        g_DirectoryMgr.EnterDirectory(DIRECTORY_TYPE_LOCAL, kszAllContactId);
#endif
    }
    else
    {
#if IF_FEATURE_METASWITCH_CALLLOG
        if (LocalLog_IsEnable())
        {
            g_DirectoryMgr.EnterDirectory(CALL_LOG_TYPE_LOCAL, kszAllContactId);
        }
        else
        {
            g_DirectoryMgr.EnterDirectory(CALL_LOG_TYPE_MTSW, kszAllContactId);
        }
#else
        g_DirectoryMgr.EnterDirectory(CALL_LOG_TYPE_LOCAL, kszAllContactId);
#endif
    }
}

void OnPreLeftCallPage(const yl::string & strUIName)
{
    callmanager_PutPage(strUIName);
}

void OnLeftCallPage(const yl::string & strUIName, CCallManagerBaseDlg * pDlg)
{
    if (CALLMGR_PAGE_NAME_DIRECTORY == strUIName)
    {
        UIManager_PutPrivateDlg(DLG_CDlgDirectory);
    }
    else if (CALLMGR_PAGE_NAME_CALL_LOG == strUIName)
    {
        UIManager_PutPrivateDlg(DLG_CDlgCallLog);
    }
}

void OnCallPageEvent(const yl::string & strUIName, CallManagerEventType eventType)
{
    if (CallManagerEvent_Show != eventType)
    {
        return;
    }

    if (CALLMGR_PAGE_NAME_DIRECTORY == strUIName)
    {
        g_DirectoryMgr.SwitchLastDirectory(DIRECTORY_TYPE_LOCAL, kszAllContactId);
    }
    else if (CALLMGR_PAGE_NAME_CALL_LOG == strUIName)
    {
        g_DirectoryMgr.SwitchLastDirectory(CALL_LOG_TYPE_LOCAL, kszAllContactId);
    }
}

void DM_SetCallManagerInfo()
{
    // callmanager中history页面初始化
    CCallMGRItemInfo itemHistory;

    itemHistory.strDisplayName = LANG_TRANSLATE(TRID_CMG_CALLLOG).toUtf8().data();
    itemHistory.strIcon = PIC_SOFTKEY_CALLLOG;
    itemHistory.strSelectBKImg = PIC_BTN_ITEM_CLICK_LEFT;

    callmanager_SetPageInfo(CALLMGR_PAGE_NAME_CALL_LOG,
                            0,
                            itemHistory,
                            &OnCallPageEvent,
                            &OnEnterCallPage,
                            &OnPreLeftCallPage,
                            &OnLeftCallPage);

    // callmanager中directory页面初始化
    CCallMGRItemInfo itemDirectory;

    itemDirectory.strDisplayName = LANG_TRANSLATE(TRID_DIRECTORY).toUtf8().data();
    itemDirectory.strIcon = PIC_SOFTKEY_CALLMANAGER_DIR;
    itemDirectory.strSelectBKImg = PIC_BTN_ITEM_CLICK_MID;

    callmanager_SetPageInfo(CALLMGR_PAGE_NAME_DIRECTORY,
                            0,
                            itemDirectory,
                            &OnCallPageEvent,
                            &OnEnterCallPage,
                            &OnPreLeftCallPage,
                            &OnLeftCallPage);
}

// 初始化Directory模块
void DM_InitData()
{
    g_DirectoryMgr.Init();
    g_DirDssKeyMgr.Init();
}

// 退出Directory模块
void DM_ExitDirectory()
{
    g_DirectoryMgr.UnInit();
    g_DirDelegateFactory.Clear();
    g_DirCtrlFactory.Clear();
    g_DirUIHelperFactory.Clear();
}

BOOL DM_EnterLocalMenu()
{
    return DM_EnterDirectoryUI(DIRECTORY_TYPE_LOCAL, kszAllContactId);
}

bool DM_IsSearchDirectoryExist()
{
    return UIManager_IsWndExist(eWT_Search_Directory);
}

#if DIR_SUPPORT_SELECT_CONTACT
bool DM_EnterDirectoryUI(int nType, int nGroupId/* = 0*/, bool bSelectState/* = false*/)
#else
bool DM_EnterDirectoryUI(int nType, int nGroupId/* = 0*/)
#endif
{
#if DIR_SUPPORT_SELECT_CONTACT
    if (!bSelectState)
    {
        DM_CancelSelectDirectory();
    }
#endif

    // http://10.2.1.199/Bug.php?BugID=93558
    // 已经存在联系人界面，则不进入
    if (UIManager_IsWndExist(eWT_Directory))
    {
        DIRUI_WARN("UIManager_IsWndExist(eWT_Directory) return true in DM_EnterDirectoryUI().");
        return false;
    }

    if (DIRECTORY_TYPE_REMOTE == nType && CRemoteContactControler::IsServerSearchGroup(nGroupId))
    {
        g_DirectoryMgr.EnterSearch(DIRECTORY_TYPE_REMOTE_SEARCH, nGroupId, PHONE_KEY_NONE);

        return true;
    }

#if IF_FEATURE_MUTI_CALLLOG
    if (BsftLog_IsMutilLogEnable()
            && CALL_LOG_TYPE_BROADSOFT == nType)
    {
        CALL_TYPE eCallType = CT_CALLALL;
        if (nGroupId == kszPlacedLogId)
        {
            eCallType = CT_OUTGOING;
        }
        else if (nGroupId == kszMissLogId)
        {
            eCallType = CT_MISSCALL;
        }
//         int nAccountID = -1;
//         if (eCallType == CT_MISSCALL)
//         {
//             nAccountID = LocalLog_GetLastChildMissAccountID();
//         }
//         else if (eCallType == CT_OUTGOING)
//         {
//             yl::string strNumber = "";
//             LocalLog_GetLastOutCallNumAndAccountID(strNumber, nAccountID);
//         }
//         int nAvailableNum = acc_GetAvailableAccountCount();
//         if (nAvailableNum > 1 && nAccountID < 0)
//         {
//             nGroupId = kszRootId;
//         }
//         else
//         {
//             int nID = nAvailableNum == 1 ? acc_GetUsableAccount() : 0;
//             if (nAccountID >= 0)
//             {
//                 nID = nAccountID;
//             }
//             nGroupId = BsftLog_GetGroupIDByAccountID(nID, eCallType);
//         }
        return g_DirectoryMgr.EnterDirectory(nType, nGroupId, eCallType);
    }
#endif
    if (DIRECTORY_TYPE_BROADSOFT == nType && nGroupId == 0 && configParser_GetConfigInt(kszBsftDirUpdateEnable) == 0)
    {
        nGroupId = 2;
    }

    return g_DirectoryMgr.EnterDirectory(nType, nGroupId);
}

#if DIR_SUPPORT_SELECT_CONTACT
void DM_EnterSelectDirectoryUI(DIR_SELECT_CONTACT_FUN pFunSelectContact,
                               int nType /*= DIRECTORY_TYPE_LOCAL*/)
{
    g_DirectoryMgr.SetContactSelector(pFunSelectContact);
    DM_EnterDirectoryUI(nType, 0, true);
}

void DM_CancelSelectDirectory()
{
    g_DirectoryMgr.CancelSelectDirectory();
}
#endif

void DM_RetryChgPwdRuest(int nType, const yl::string & strPwd)
{
    g_DirectoryMgr.RetryChgPwdRuest(nType, strPwd);
}

yl::string DM_GetContactImageName(const yl::string & strResName)
{
    if (strResName == CONTACT_PHOTO_FILE_DEFAULT)
    {
        return TRID_PHOTO_DEFAULT;
    }
    else if (strResName == CONTACT_PHOTO_FILE_FRIEND)
    {
        return TRID_PHOTO_FRIEND;
    }
    else if (strResName == CONTACT_PHOTO_FILE_FAMILY)
    {
        return TRID_PHOTO_FAMILY;
    }
    else if (strResName == CONTACT_PHOTO_FILE_CUSTOMER)
    {
        return TRID_PHOTO_CUSTOMER;
    }

    return strResName;
}

void DM_GetDsskeyBsftGroupList(VEC_CONTACT_ID & vecGroupId)
{
    CBSFTContactControler::GetAllGroup(vecGroupId);
}

void DM_GetGroupDisplayName(DIR_TYPE eDirType, int nGroupId, QString & strDisplayName)
{
    yl::string strGroupName;

    if (!CCommonDirCtrl::GetDisplayName(eDirType, nGroupId, strGroupName))
    {
        return;
    }

    if (LOCAL_DIR == eDirType)
    {
        CLocalContactUIHelper::GetGroupDisplayName(nGroupId, strGroupName, strDisplayName);
    }
    else if (BSFT_DIR == eDirType)
    {
        CBSFTContactUIHelper::GetGroupDisplayName(nGroupId, strGroupName, strDisplayName);
    }
    else
    {
        strDisplayName = toQString(strGroupName, true);
    }
}

void DM_GetGroupDisplayName(DIR_TYPE eDirType, int nGroupId, yl::string & strDisplayName)
{
    QString strGroupName;
    DM_GetGroupDisplayName(eDirType, nGroupId, strGroupName);
    strDisplayName = fromQString(strGroupName);
}

void DM_GetLdapName(QString & strName)
{
    yl::string strLdapName = LDAP_GetName();

    if (strLdapName.empty())
    {
        strName = LANG_TRANSLATE(TRID_LDAP);
    }
    else
    {
        strName = toQString(strLdapName, true);
    }
}

void DM_EnterAddPersonalContact(const yl::string & strName, const yl::string & strNumber)
{
    LIST_ATTRIBUTE lsAttr;
    ST_CONTACT_ATTRIBUTE stAttr;

    stAttr.m_strName = ATTR_NAME;
    stAttr.m_strValue = strName;
    lsAttr.push_back(stAttr);

    stAttr.m_strName = ATTR_NUMBER;
    stAttr.m_strValue = strNumber;
    lsAttr.push_back(stAttr);

    CDirUIHelper::SetAttributeList(DIRECTORY_TYPE_BROADSOFT, lsAttr);

    CDirUIHelper::EnterUI(DIRECTORY_TYPE_BROADSOFT, DIR_UI_STATE_ADDITION,
                          DIR_ACTION_BSFT_ENTER_ADD_TO_PERSONAL, kszPersonalId);
}

// 实现每个界面打开逻辑的注册
void DM_RegisterOpenUICheckFunc()
{
    EDKUI_SetFuncAllowEnter(EDK_UI_DIRECTORY_REMOTE, DirUILogic_IsRemoteEnable);
    EDKUI_SetFuncAllowEnter(EDK_UI_DIRECTORY_LDAP, LDAP_IsEnable);
    EDKUI_SetFuncAllowEnter(EDK_UI_DIRECTORY_BROADSOFT, DirUILogic_IsBSFTEnable);
    EDKUI_SetFuncAllowEnter(EDK_UI_DIRECTORY_BTUC, DirUILogic_IsButcEnable);
    EDKUI_SetFuncAllowEnter(EDK_UI_DIRECTORY_PLCM, DirUILogic_IsPlcmEnable);
    EDKUI_SetFuncAllowEnter(EDK_UI_LOG_BSFT, DirUILogic_IsLogBSFTEnable);
    EDKUI_SetFuncAllowEnter(EDK_UI_BLACK_LIST, DirUILogic_IsBlacklistEnable);
}

bool DM_AddDirectory(yl::string strName, yl::string strNumber, int nAction, int nDirType)
{
    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();

    int nGroupId = kszAllContactId;
    if (nDirType == DIRECTORY_TYPE_METASWICTH)
    {
        nGroupId = kszContactList;
    }

    if (pDelegate.IsEmpty())
    {
        g_DirectoryMgr.SetCurDirType(nDirType);
        if (!g_DirectoryMgr.CheckContactDelegate(DIR_UI_STATE_ADDITION, nGroupId, false))
        {
            return false;
        }
    }

    LIST_ATTRIBUTE lsAttr;
    ST_CONTACT_ATTRIBUTE stAttr;

    if (nDirType == DIRECTORY_TYPE_LOCAL)
    {
        stAttr.m_strName = ATTR_NAME;
        stAttr.m_strValue = strName;
        lsAttr.push_back(stAttr);

        stAttr.m_strName = ATTR_NUMBER;
        stAttr.m_strValue = strNumber;
        lsAttr.push_back(stAttr);
    }
    else if (nDirType == DIRECTORY_TYPE_METASWICTH)
    {
        int iPos = strName.find(',');
        yl::string strFirstName = "";
        yl::string strLastName = "";

        if (iPos != -1)
        {
            strFirstName = strName.substr(0, iPos);
            strLastName = strName.substr(iPos + 1, strName.length() - iPos);
        }
        else
        {
            strFirstName = strName;
        }

        stAttr.m_strName = ATTR_MTSW_FIRST_NAME;
        stAttr.m_strValue = strFirstName;
        lsAttr.push_back(stAttr);
        stAttr.m_strName = ATTR_MTSW_LAST_NAME;
        stAttr.m_strValue = strLastName;
        lsAttr.push_back(stAttr);
        stAttr.m_strName = ATTR_MTSW_HOMENUMBER;
        stAttr.m_strValue = strNumber;
        lsAttr.push_back(stAttr);
    }
    else
    {
        return false;
    }


    CDirUIHelper::SetAttributeList(nDirType, lsAttr);
    g_DirectoryMgr.EnterAddition(nDirType, knInvalidRId, true);
    //CDirUIHelper::EnterUI(nDirType, DIR_UI_STATE_ADDITION, nAction, knInvalidRId);
    return true;
}

#if IF_FEATURE_FAVORITE_DIR
bool DM_EnterAddLocalContact(int nGroupId/* = 0*/)
{
    g_DirectoryMgr.EnterDirectory(DIRECTORY_TYPE_LOCAL, nGroupId);
    g_DirectoryMgr.EnterAddition(DIRECTORY_TYPE_LOCAL, nGroupId, false, true);
    return true;
}

void DM_ReturnToIdle()
{
    DM_ExitDirectory();
    UIManager_ReturnToIdle();
}
#endif
