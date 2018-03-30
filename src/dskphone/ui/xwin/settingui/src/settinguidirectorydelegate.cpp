#include "settinguidirectorydelegate.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "translateiddefine.h"
#include "xwindow/xlistviewitem.h"
#include "languagehelper/modlangtranslate.h"
#include "uimanager/dialogviewcontroller.h"
#include "contacts/directory/include/moddirinterface.h"
#include "contacts/ldaplogic/include/modldap.h"
#include "directoryui/include/moddirectoryui.h"
#include "setuidatadefine.h"
#include "presence/include/presence_inc.h"
#include "settinguimanager.h"
#include "settingui/include/pageiddefine.h"
#include "uicallbackfun.h"

namespace
{
#if IF_BUG_PLCMDIR
const char * kszPlcmPhone = "plcmdirectory";
#endif
const char * kszDirLocal = "localdirectory";
const char * kszDirBluetooth = "DirectoryBluetooth";
const char * kszDirRemote = "remotedirectory";
#ifdef IF_FEATURE_BROADSOFT_CONTACT
const char * kszDirBsft = "broadsoftdirectory";
#endif
#ifdef IF_FEATURE_PSTN
const char * kszDirFavorite = "favoritedirectory";
#endif
const char * kszDirLDAP = "ldap";
#ifdef IF_FEATURE_SHOW_BLACKLIST
const char * kszBlacklist = "blacklist";
#endif
#if IF_FEATURE_GBBOOK
// 远程联系人菜单的id名.
static const char kszMenuIdNameGenbandGab[] = "genbandgab_phonebook";
static const char kszMenuIdNameGenbandPab[] = "genbandpab_phonebook";
#endif
#if IF_FEATURE_FAVORITE_DIR
static const char kszMenuIdNameLocalFavorite[] = "local_favorite";
#endif
#if FEATURE_BROADTOUCH_UC
static const char kszMenuCAUC[] = "uc_contact_list";
#endif
#if IF_FEATURE_METASWITCH
static const char kszMenuCAMetaswith[] = "OpenDirectoryPage(metaswitch_contact_list.xml)";
#endif
}

CSettingUIDirectoryDelegate::CSettingUIDirectoryDelegate()
    : m_bNeedBack(false)
#if IF_SUPPORT_SELECT_CONTACT_INFO
    , m_pFunSelectContact(NULL)
#endif
{

}

CSettingUIDirectoryDelegate::~CSettingUIDirectoryDelegate()
{

}

CSettingUIDelegateBase * CSettingUIDirectoryDelegate::CreateDirectoryDelegate()
{
    CSettingUIDirectoryDelegate * pDelegate = new CSettingUIDirectoryDelegate;

    return pDelegate;
}

CSettingUIPageDataBase * CSettingUIDirectoryDelegate::LoadPageData(bool bReLoad/* = false*/)
{
    CSettingUIPageDataBase * pPageData = CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (NULL == pPageData || !bReLoad)
    {
        return pPageData;
    }

#if IF_BUG_PLCMDIR
    if (0 != configParser_GetConfigInt(kszPlcmPhoneEnable))
    {
        AddMenuItem(pPageData, kszPlcmPhone, _LANG(TRID_PLCM_PHONEBOOK), kszPlcmPhone);
    }
#endif

    AddMenuItem(pPageData, kszDirLocal, _LANG(TRID_LOCAL_DIRECTORY), kszDirLocal);

    if (Dir_IsDirConfiged(BLUETOOTH_DIR))
    {
        AddMenuItem(pPageData, kszDirBluetooth, _LANG(TRID_MOBILE_CONTACTS), kszDirBluetooth);
    }

    if (Dir_IsDirConfiged(REMOTE_DIR))
    {
        yl::string strName = RemoteDir_GetRemoteDisplayName();

        if (strName.empty())
        {
            AddMenuItem(pPageData, kszDirRemote, _LANG(TRID_REMOTE_PHONEBOOK), kszDirRemote);
        }
        else
        {
            AddMenuItem(pPageData, kszDirRemote, strName, kszDirRemote);
        }
    }

#ifdef IF_FEATURE_BROADSOFT_CONTACT
    if (BsftDir_IsEnable())
    {
        AddMenuItem(pPageData, kszDirBsft, _LANG(TRID_NETWORK_DIRECTORIES), kszDirBsft);
    }
#endif

#ifdef IF_FEATURE_PSTN
    AddMenuItem(pPageData, kszDirFavorite, _LANG(TRID_FAVOURITE), kszDirFavorite);
#endif

    // T2X新增LDAP开关
    if (LDAP_IsEnable())
    {
        yl::string strLDAP;
        DM_GetLdapName(strLDAP);
        AddMenuItem(pPageData, kszDirLDAP, strLDAP, kszDirLDAP);
    }

#if FEATURE_BROADTOUCH_UC
    AddMenuItem(pPageData, kszMenuCAUC, _LANG(TRID_UC_FAVORITE), kszMenuCAUC);
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
    AddMenuItem(pPageData, kszMenuCAMetaswith, _LANG(TRID_NETWORK_CONTACTS), kszMenuCAMetaswith);
#endif

#ifdef IF_FEATURE_SHOW_BLACKLIST
    AddMenuItem(pPageData, kszBlacklist, _LANG(TRID_BLACKLIST), kszBlacklist);
#endif

#if IF_FEATURE_GBBOOK
    AddMenuItem(pPageData, kszMenuIdNameGenbandGab, _LANG(TRID_GB_GABBOOK), kszMenuIdNameGenbandGab);
    AddMenuItem(pPageData, kszMenuIdNameGenbandPab, _LANG(TRID_GB_PABBOOK), kszMenuIdNameGenbandPab);
#endif

#if IF_FEATURE_FAVORITE_DIR
    if (1 == configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch))
    {
        AddMenuItem(pPageData, kszMenuIdNameLocalFavorite, _LANG(TRID_FAVOURITE), kszMenuIdNameLocalFavorite);
    }
#endif

    return pPageData;
}

void CSettingUIDirectoryDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
    int nSize = vecSoftKey.size();

    if (nSize > 2)
    {
        CSoftKeyBarData & objSoftKey = vecSoftKey[2];

        objSoftKey.m_iType = ST_SEARCH;
        objSoftKey.m_strSoftkeyTitle = _LANG(TRID_SEARCH);
    }
}

#if IF_SUPPORT_SELECT_CONTACT_INFO
void CSettingUIDirectoryDelegate::UpdateContactInfoCallBack(yl::string strContactName, yl::string strContactNumber)
{
    CSettingUIDelegateBase* pDelegate = g_pSettingUIManager->GetTopDelegate();
    if (NULL == pDelegate || kszPageIDDirectory != pDelegate->GetPageID())
    {
        return;
    }

    CSettingUIDirectoryDelegate* pDirectoryDelegate = static_cast<CSettingUIDirectoryDelegate*>(pDelegate);
    if (NULL == pDirectoryDelegate)
    {
        return;
    }

    pDirectoryDelegate->UpdateContactInfo(strContactName, strContactNumber);
}

void CSettingUIDirectoryDelegate::UpdateContactInfo(yl::string strContactName, yl::string strContactNumber)
{
    if (NULL == m_pFunSelectContact)
    {
        return;
    }

    DIR_SELECT_CONTACT_FUN pFunSelectContact = m_pFunSelectContact;
    pFunSelectContact(strContactName, strContactNumber);
    m_bNeedBack = true;
}

void CSettingUIDirectoryDelegate::InitExtraData(void* pData)
{
    if (NULL == pData)
    {
        return;
    }
    m_pFunSelectContact = (DIR_SELECT_CONTACT_FUN)pData;
}
#endif

bool CSettingUIDirectoryDelegate::OnAction(int eActionType, int iDataIndex)
{
    if (SET_ACTION_TYPE_CLICK == eActionType)
    {
        OnEnterPage(iDataIndex);

        return true;
    }

    return CSettingUIDelegateBase::OnAction(eActionType, iDataIndex);
}

bool CSettingUIDirectoryDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    if (!bLongPress && ST_SEARCH == iType)
    {
        OnEnterSearch(nDataIndex);

        return true;
    }

    return CSettingUIDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}

bool CSettingUIDirectoryDelegate::FocusEvent(bool bFocusIn)
{
    if (m_bNeedBack)
    {
        g_SetUICallBackFun.FallBackToPrevousPage();
    }
    return true;
}

void CSettingUIDirectoryDelegate::OnEnterPage(int nDataIndex)
{
    const yl::string & strID = GetItemIDByDataIndex(nDataIndex);

    if (strID.empty())
    {
        return;
    }

#if IF_SUPPORT_SELECT_CONTACT_INFO
    if (NULL != m_pFunSelectContact)
    {
        DM_SetSelectDirectoryCallBackFun(&CSettingUIDirectoryDelegate::UpdateContactInfoCallBack);
    }
#endif

#if IF_BUG_PLCMDIR
    if (kszPlcmPhone == strID)
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_PLCM, kszPlcmDirGroupId);
        return;
    }
#endif

    if (kszDirLocal == strID)
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_LOCAL, knInvalidRId);
        return;
    }

    if (kszDirBluetooth == strID)
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_BLUETOOTH, kszBluethoothGroupId);
        return;
    }

    if (kszDirRemote == strID)
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_REMOTE, kszRootId);
        return;
    }

#ifdef IF_FEATURE_BROADSOFT_CONTACT
    if (kszDirBsft == strID)
    {
#ifdef IF_FEATURE_DIR_OPT
        DM_EnterDirectoryUI(DIRECTORY_TYPE_BROADSOFT, kszAllContactId);
#else
        // 进入BroadSoft联系人页面.
        DM_EnterDirectoryUI(DIRECTORY_TYPE_BROADSOFT, knInvalidRId);
#endif
        return;
    }
#endif

#ifdef IF_FEATURE_PSTN
    if (kszDirFavorite == strID)
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_LOCAL, kszFavoriteGroupId);
        return;
    }
#endif

    if (kszDirLDAP == strID)
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_LDAP, knInvalidRId);
        return;
    }

#ifdef IF_FEATURE_SHOW_BLACKLIST
    if (kszBlacklist == strID)
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_LOCAL, kszBlackListGroupId);
    }
#endif

#if IF_FEATURE_GBBOOK
    if (kszMenuIdNameGenbandGab == strID)
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_GENBAND, kszGabBookGroupId);
    }

    if (kszMenuIdNameGenbandPab == strID)
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_GENBAND, kszAllContactId);
    }
#endif

#if IF_FEATURE_FAVORITE_DIR
    if (kszMenuIdNameLocalFavorite == strID)
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_LOCAL, kszFavoriteGroupId);
    }
#endif

#if FEATURE_BROADTOUCH_UC
    if (kszMenuCAUC == strID)
    {
#ifdef IF_FEATURE_DIR_OPT
        DM_EnterDirectoryUI(DIRECTORY_TYPE_BTUC, kszAllBuddiesId);
#else
        DM_EnterDirectoryUI(DIRECTORY_TYPE_BTUC, knInvalidRId);
#endif
    }
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
    else if (kszMenuCAMetaswith == strID)
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_METASWICTH, knInvalidRId);
    }
#endif
}

void CSettingUIDirectoryDelegate::OnEnterSearch(int nDataIndex)
{
    const yl::string & strID = GetItemIDByDataIndex(nDataIndex);

    if (strID.empty())
    {
        return;
    }

#if IF_BUG_PLCMDIR
    if (kszPlcmPhone == strID)
    {
        DM_EnterSearch(DIRECTORY_TYPE_PLCM, kszPlcmDirGroupId);
        return;
    }
#endif

    if (kszDirLocal == strID)
    {
        DM_EnterSearch(DIRECTORY_TYPE_LOCAL, knInvalidRId);
        return;
    }

    if (kszDirBluetooth == strID)
    {
        DM_EnterSearch(DIRECTORY_TYPE_BLUETOOTH, kszBluethoothGroupId);
        return;
    }

    if (kszDirRemote == strID)
    {
        DM_EnterSearch(DIRECTORY_TYPE_REMOTE, kszRootId);
        return;
    }

#ifdef IF_FEATURE_BROADSOFT_CONTACT
    if (kszDirBsft == strID)
    {
        DM_EnterSearch(DIRECTORY_TYPE_BROADSOFT, knInvalidRId);
        return;
    }
#endif

#ifdef IF_FEATURE_PSTN
    if (kszDirFavorite == strID)
    {
        DM_EnterSearch(DIRECTORY_TYPE_LOCAL, kszFavoriteGroupId);
        return;
    }
#endif

    if (kszDirLDAP == strID)
    {
        DM_EnterSearch(DIRECTORY_TYPE_LDAP, knInvalidRId);
        return;
    }

#ifdef IF_FEATURE_SHOW_BLACKLIST
    if (kszBlacklist == strID)
    {
        DM_EnterSearch(DIRECTORY_TYPE_LOCAL, kszBlackListGroupId);
    }
#endif

#if FEATURE_BROADTOUCH_UC
    if (kszMenuCAUC == strID)
    {
        DM_EnterSearch(DIRECTORY_TYPE_BTUC, ALL_BUDDIES_ID);
    }
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    else if (kszMenuCAMetaswith == strID)
    {
        DM_EnterSearch(DIRECTORY_TYPE_METASWICTH, knInvalidRId);
    }
#endif
}
