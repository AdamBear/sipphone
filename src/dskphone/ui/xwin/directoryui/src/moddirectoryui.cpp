#include "moddirectoryui.h"
#include "keymap.h"
#include "bsftcontactcontroler.h"
#include "bsftcontactuihelper.h"
#include "localcontactuihelper.h"
#include "adapterfactory.h"
#include "dirctrlfactory.h"
#include "directorymgr.h"
#include "dirdsskeymgr.h"
#include "dirmsgboxmgr.h"
#include "diruihelperfactory.h"
#include "directorycommon.h"
#include "translateiddefine.h"
#include <directorydefine.h>
#if FEATURE_BROADTOUCH_UC
#include "btuclistitem.h"
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
#include "iconmenulistitem.h"
#endif
#include "callloglistitem.h"
#include "contactlistitem.h"
#if IF_DIR_SUPPORT_FAVORITE_ITEM
#include "favoritecontactlistitem.h"
#endif
#if IS_COLOR
#include "namenumbercallloglistitem.h"
#endif
#include "baseui/include/listviewitemfactory.h"
#include "contacts/ldaplogic/include/modldap.h"
#include "languagehelper/modlangtranslate.h"
#include "keyguard/include/modkeyguard.h"
#include "uimanager/moduimanager.h"


// 初始化Directory模块
void DM_InitData()
{
    g_DirectoryMgr.Init();
    g_DirDssKeyMgr.Init();
    g_DirMsgBoxMgr.Init();

#if FEATURE_BROADTOUCH_UC
    g_ItemFactory.RegisterItem(CBTUCListItem::GetBTUCItemType(), &CBTUCListItem::CreateBTUCListItem);
#endif
    g_ItemFactory.RegisterItem(CCallLogListItem::GetCallLogListItemType(),
                               &CCallLogListItem::CreateCallLogListItem);
    g_ItemFactory.RegisterItem(CContactListItem::GetContactListItemType(),
                               &CContactListItem::CreateContactListItem);
#if IF_FEATURE_METASWITCH_DIRECTORY
    g_ItemFactory.RegisterItem(CIconMenuListItem::GetIconMenuItemType(),
                               &CIconMenuListItem::CreateIconMenuListItem);
#endif
#if IF_DIR_SUPPORT_FAVORITE_ITEM
    g_ItemFactory.RegisterItem(CFavoriteContactListItem::GetFavoriteContactListItemType(),
                               &CFavoriteContactListItem::CreateFavoriteContactListItem);
#endif
#if IS_COLOR
    g_ItemFactory.RegisterItem(CNameNumberCallLogListItem::GetNameNumberCallLogListItemType(),
                               &CNameNumberCallLogListItem::CreateNameNumberCallLogListItem);
#endif
}

// 释放Directory资源
void DM_UnInitData()
{
    g_DirectoryMgr.UnInit();
    g_DirMsgBoxMgr.UnInit();
    g_AdapterFactory.Clear();
    g_DirCtrlFactory.Clear();
    g_DirUIHelperFactory.Clear();
}

// 退出Directory模块
void DM_ExitDirectory()
{
    g_DirectoryMgr.Reset();
    g_AdapterFactory.Clear();
    g_DirCtrlFactory.Clear();
    g_DirUIHelperFactory.Clear();
}

bool DM_EnterDirectoryUI(int nType, int nGroupId/* = 0*/)
{
    //如果不允许进入联系人模块则直接返回
    if (!keyGuard_IsEnableModuld(KGM_MODULE_DIRECTORY))
    {
        return false;
    }

    // http://10.2.1.199/Bug.php?BugID=93558
    // 已经存在联系人界面，则不进入
    if (UIManager_IsWndExist(eWT_Directory))
    {
        DIRUI_WARN("UIManager_IsWndExist(eWT_Directory) return true in DM_EnterDirectoryUI().");
        return false;
    }

    return g_DirectoryMgr.EnterDirectory(nType, nGroupId);
}

bool DM_EnterSearch(int nDirType, int nGroupId/* = kszAllContactId*/)
{
    return g_DirectoryMgr.EnterSearch(nDirType, nGroupId, PHONE_KEY_NONE);
}

bool DM_IsInCallLogBrowser()
{
    CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();
    int nDirType = g_DirectoryMgr.GetActiveDirType();

    if (!pAdapter.IsEmpty() && DIR_UI_STATE_BROWSER == pAdapter->GetState()
            && (CALL_LOG_TYPE_LOCAL == nDirType
#if IF_FEATURE_METASWITCH_DIRECTORY
                || CALL_LOG_TYPE_MTSW == nDirType
#endif
                || CALL_LOG_TYPE_BROADSOFT == nDirType))
    {
        return true;
    }

    return false;
}

bool DM_IsDirDssKey(int nDssKey)
{
    return g_DirDssKeyMgr.IsDirDssKey(nDssKey);
}

void DM_GetDsskeyBsftGroupList(VEC_CONTACT_ID & vecGroupId)
{
    CBSFTContactControler::GetAllGroup(vecGroupId);
}

void DM_GetGroupDisplayName(DIR_TYPE eDirType, int nGroupId, yl::string & strDisplayName)
{
    if (!CCommonDirCtrl::GetDisplayName(eDirType, nGroupId, strDisplayName))
    {
        return;
    }

    if (LOCAL_DIR == eDirType)
    {
        CLocalContactUIHelper::GetGroupDisplayName(nGroupId, strDisplayName);
    }
    else if (BSFT_DIR == eDirType)
    {
        CBSFTContactUIHelper::GetGroupDisplayName(nGroupId, strDisplayName);
    }
}

void DM_GetLdapName(yl::string& strName)
{
    strName = LDAP_GetName();

    if (strName.empty())
    {
        strName = _LANG(TRID_LDAP);
    }
}

#if IF_FEATURE_METASWITCH_DIRECTORY
bool DM_AddContact(yl::string strName, yl::string strNumber, int nAction, int nDirType)
{
    CBaseDialog* pDlgDir = UIManager_GetPrivateDlg(DLG_CDlgDirectory);
    if (NULL != pDlgDir)
    {
        return false;
    }

    LIST_ATTRIBUTE lsAttr;
    ST_CONTACT_ATTRIBUTE stAttr;

    if (DIRECTORY_TYPE_LOCAL == nDirType)
    {
        stAttr.m_strName = DIR_ATTR_NAME;
        stAttr.m_strValue = strName;
        lsAttr.push_back(stAttr);

        stAttr.m_strName = DIR_ATTR_NUMBER;
        stAttr.m_strValue = strNumber;
        lsAttr.push_back(stAttr);
    }
    else if (DIRECTORY_TYPE_METASWICTH == nDirType)
    {
        yl::string::size_type iPos = strName.find(',');
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

        stAttr.m_strName = DIR_ATTR_MTSW_FIRST_NAME;
        stAttr.m_strValue = strFirstName;
        lsAttr.push_back(stAttr);
        stAttr.m_strName = DIR_ATTR_MTSW_LAST_NAME;
        stAttr.m_strValue = strLastName;
        lsAttr.push_back(stAttr);
        stAttr.m_strName = DIR_ATTR_MTSW_HOMENUMBER;
        stAttr.m_strValue = strNumber;
        lsAttr.push_back(stAttr);
    }
    else
    {
        return false;
    }


    CDirectoryControler::SetAttributeList(nDirType, lsAttr);

    g_DirectoryMgr.EnterDetail(nAction, nDirType, knInvalidRId, knInvalidRId);

    return true;
}
#endif

#if IF_FEATURE_FAVORITE_DIR
bool DM_AddFavoriteContact(int nGroupId/* = 0*/)
{
    //如果不允许进入联系人模块则直接返回
    if (!keyGuard_IsEnableModuld(KGM_MODULE_DIRECTORY))
    {
        return false;
    }

    if (UIManager_IsWndExist(eWT_Directory))
    {
        DIRUI_WARN("UIManager_IsWndExist(eWT_Directory) return true in DM_EnterDirectoryUI().");
        return false;
    }

    g_DirectoryMgr.EnterDetail(DIR_ACTION_LOCAL_DSSKEY_ADD_CONTACT, DIRECTORY_TYPE_LOCAL, nGroupId, knInvalidRId);
    return true;
}
#endif

#if DIR_SUPPORT_SELECT_CONTACT
void DM_CancelSelectDirectory()
{
    g_DirectoryMgr.CancelSelectDirectory();
}

void DM_SetSelectDirectoryCallBackFun(DIR_SELECT_CONTACT_FUN pFunSelectContact)
{
    g_DirectoryMgr.SetContactSelector(pFunSelectContact);
}

#endif

