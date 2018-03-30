#include <QString>
#include <algorithm>
#include "contactgroupdelegate.h"

#include "directorycommon.h"
#include "directorylistaction.h"
#include "dirctrlfactory.h"
#include "directorymgr.h"
#include "translateiddefine.h"
#include "bsftcontactcontroler.h"
#include "commondirctrl.h"
#include "localcontactcontroler.h"
#include "ldapcontactcontroler.h"
#include "btuccontactcontroler.h"
#include "bsftcontactuihelper.h"
#include "localcontactuihelper.h"
#include "groupedittitle.h"
#include "groupnormaltitle.h"
#include "grouptreeitem.h"
#include "groupedittreeitem.h"
#include "moddirectoryui.h"
#include "baseui/treelist.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "keymap.h"
#include "uimanager/rectdefine.h"
#include "messagebox/modmessagebox.h"
#include "presence/include/modpresence.h"
#include "uikernel/languagehelper.h"
#if IF_FEATURE_GBBOOK
#include "genbandcontactcontroler.h"
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
#include "mtswcontactuihelper.h"
#endif


CContactGroupNode::CContactGroupNode(int nId, int nParentId, bool bHadChildren, int nType,
                                     int nGroupId)
    : CGroupTreeNode(nId, nParentId, bHadChildren, nType, nGroupId)
    , m_bAddContact(false)
    , m_bEditable(false)
{
}

CContactGroupNode::~CContactGroupNode(void)
{
}

void CContactGroupNode::SetAddContact(bool bAddContact)
{
    m_bAddContact = bAddContact;
}

void CContactGroupNode::SetEditable(bool bEditable)
{
    m_bEditable = bEditable;
}

#if IF_FEATURE_GBBOOK
void CContactGroupNode::SetShowBtnRing(bool bShow)
{
    m_bShowBtnRing = bShow;
}
#endif

bool CContactGroupNode::IsAddContact() const
{
    return m_bAddContact;
}

bool CContactGroupNode::IsEditable() const
{
    return m_bEditable;
}

#if IF_FEATURE_GBBOOK
bool CContactGroupNode::IsShowBtnRing() const
{
    return m_bShowBtnRing;
}
#endif

CContactGroupDelegate::CContactGroupDelegate()
    : CGroupDelegate(DIR_MOD_TYPE_CONTACT)
    , m_nLastSelId(0)
{
}

CContactGroupDelegate::~CContactGroupDelegate()
{
}

CContactGroupDelegate * CContactGroupDelegate::GetGroupDelegate(CGroupDelegatePtr & pDelegate)
{
    if (pDelegate.IsEmpty() || DIR_MOD_TYPE_CONTACT != pDelegate->GetType())
    {
        return NULL;
    }

    return (CContactGroupDelegate *)pDelegate.Get();
}

int CContactGroupDelegate::GetDefaultDirType() const
{
    return DIRECTORY_TYPE_LOCAL;
}

int CContactGroupDelegate::AdjustGroupId(int nDirType, int nGroupId)
{
    CGroupTreeNodePtr pGroupNode = GetGroupNode(nDirType, nGroupId);

    if (NULL != pGroupNode)
    {
        return nGroupId;
    }

    if (DIRECTORY_TYPE_LOCAL == nDirType)
    {
        return kszAllContactId;
    }
    else if (DIRECTORY_TYPE_REMOTE == nDirType)
    {
        if (CCommonDirCtrl::IsGroupId(REMOTE_DIR, nGroupId))
        {
            return nGroupId;
        }

        return kszRootId;
    }
    else if (DIRECTORY_TYPE_BROADSOFT == nDirType)
    {
        if (CCommonDirCtrl::IsGroupId(BSFT_DIR, nGroupId))
        {
            return nGroupId;
        }

        return kszAllContactId;
    }
#ifdef BROADTOUCH_UC
    else if (DIRECTORY_TYPE_BTUC == nDirType)
    {
        return ALL_BUDDIES_ID;
    }
#endif
#if IF_BUG_PLCMDIR
    else if (DIRECTORY_TYPE_PLCM == nDirType)
    {
        return kszPlcmDirGroupId;
    }
#endif
#if IF_FEATURE_GBBOOK
    else if (DIRECTORY_TYPE_GENBAND == nDirType)
    {
        if (nGroupId == kszGabBookGroupId)
        {
            return nGroupId;
        }
        else
        {
            return kszAllContactId;
        }
    }
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    else if (DIRECTORY_TYPE_METASWICTH == nDirType)
    {
        if (CCommonDirCtrl::IsGroupId(MTSW_DIR, nGroupId))
        {
            return nGroupId;
        }

        return kszAllContactId;
    }
#endif
    else if (DIRECTORY_TYPE_NONE == nDirType)
    {
        return knInvalidRId;
    }

    return nGroupId;
}

void CContactGroupDelegate::AdjustDirType(int & nDirType, int & nGroupId)
{
    if ((DIRECTORY_TYPE_LOCAL != nDirType || kszBlackListGroupId == nGroupId)
            && NULL != GetTreeDataNode(CONTACT_GROUP_ID_ALL_LOCAL_CONTACT))
    {
        nDirType = DIRECTORY_TYPE_LOCAL;
        nGroupId = kszAllContactId;
        return;
    }

    if (DIRECTORY_TYPE_REMOTE != nDirType
            && NULL != GetTreeDataNode(CONTACT_GROUP_ID_NETWORK_PHONE_BOOK))
    {
        nDirType = DIRECTORY_TYPE_REMOTE;
        nGroupId = kszRootId;
        return;
    }

    if (DIRECTORY_TYPE_BROADSOFT != nDirType
            && NULL != GetTreeDataNode(CONTACT_GROUP_ID_NETWORK_DIRECTORY))
    {
        nDirType = DIRECTORY_TYPE_BROADSOFT;
        nGroupId = kszAllContactId;
        return;
    }

#if FEATURE_BROADTOUCH_UC
    if (DIRECTORY_TYPE_BTUC != nDirType && NULL != GetTreeDataNode(CONTACT_GROUP_ID_BUDDIES))
    {
        nDirType = DIRECTORY_TYPE_BTUC;
        nGroupId = ALL_BUDDIES_ID;
        return;
    }
#endif

#if IF_BUG_PLCMDIR
    if (DIRECTORY_TYPE_PLCM != nDirType && NULL != GetTreeDataNode(CONTACT_GROUP_ID_PLCM))
    {
        nDirType = DIRECTORY_TYPE_PLCM;
        nGroupId = kszPlcmDirGroupId;
        return;
    }
#endif

#if IF_FEATURE_GBBOOK
    if ((DIRECTORY_TYPE_GENBAND != nDirType || kszGabBookGroupId != nGroupId)
            && NULL != GetTreeDataNode(CONTACT_GROUP_ID_GAB))
    {
        nDirType = DIRECTORY_TYPE_GENBAND;
        nGroupId = kszGabBookGroupId;
        return;
    }
    if (DIRECTORY_TYPE_GENBAND != nDirType && NULL != GetTreeDataNode(CONTACT_GROUP_ID_PAB))
    {
        nDirType = DIRECTORY_TYPE_GENBAND;
        nGroupId = kszAllContactId;
        return;
    }
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    if (DIRECTORY_TYPE_METASWICTH != nDirType
            && NULL != GetTreeDataNode(CONTACT_GROUP_ID_METASWITCH_DIRECTORY))
    {
        nDirType = DIRECTORY_TYPE_METASWICTH;
        nGroupId = kszAllContactId;
        return;
    }
#endif

#if IF_FEATURE_FAVORITE_DIR
    if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
    {
        if ((DIRECTORY_TYPE_LOCAL != nDirType || kszFavoriteGroupId != nGroupId)
                && NULL != GetTreeDataNode(CONTACT_GROUP_ID_FAVORITE))
        {
            nDirType = DIRECTORY_TYPE_LOCAL;
            nGroupId = kszFavoriteGroupId;
            return;
        }
    }
#endif

    if ((DIRECTORY_TYPE_LOCAL != nDirType || kszBlackListGroupId != nGroupId)
            && NULL != GetTreeDataNode(CONTACT_GROUP_ID_BLACKLIST))
    {
        nDirType = DIRECTORY_TYPE_LOCAL;
        nGroupId = kszBlackListGroupId;
        return;
    }

    nDirType = DIRECTORY_TYPE_NONE;
    nGroupId = knInvalidRId;
}


bool CContactGroupDelegate::ReInitGroupList(int nGroupType, int nRootTreeId, bool bExpand, int nGroupSelectId)
{
    CGroupTreeNodePtr pGroupNode = (CGroupTreeNodePtr)GetTreeDataNode(nRootTreeId);

    if (NULL == pGroupNode)
    {
        return false;
    }
    if (configParser_GetConfigInt(kszBsftDirUpdateEnable) == 0)
    {
        //m_vecTreeId.clear();

        MAP_TREE_DATA_NODE::iterator iter = m_mapTreeNode.begin();

        for (; iter != m_mapTreeNode.end(); ++iter)
        {
            CContactGroupNodePtr pGroupNode = (CContactGroupNodePtr)iter->second;

            if (NULL == pGroupNode || 0 != pGroupNode->GetParentId())
            {
                continue;
            }

            // LDAP、Buddies不在Setting界面显示
            int nType = pGroupNode->GetType();

            if (nGroupType != nType)
            {
                continue;
            }
            m_vecTreeId.push_back(pGroupNode->GetId());
        }

    }
    else
    {
        m_vecTreeId.push_back(nRootTreeId);
    }

    std::sort(m_vecTreeId.begin(), m_vecTreeId.end(), std::less<int>());

    if (bExpand)
    {
        ExpandNode(pGroupNode);

        CGroupTreeNodePtr pSelGroupNode = NULL;

        if (0 != nGroupSelectId)
        {
            pSelGroupNode = GetGroupNode(nGroupType, nGroupSelectId);
        }

        if (NULL != pSelGroupNode)
        {
            m_nUISelectId = pSelGroupNode->GetId();
        }
        else if (0 != nGroupSelectId)
        {
            m_nUISelectId = pGroupNode->GetId();
        }
    }

    return true;
}

int CContactGroupDelegate::GetDefaultGroupId(int nDirType, int nGroupId)
{
    if (DIRECTORY_TYPE_REMOTE == nDirType)
    {
        return kszRootId;
    }
    else if (DIRECTORY_TYPE_BROADSOFT == nDirType)
    {
        if (nGroupId >= kszAllContactId && nGroupId <= kszCustomId)
        {
            return nGroupId;
        }

        if (kszCustomId == Dir_GetParentIdById(nGroupId, BSFT_DIR))
        {
            return kszCustomId;
        }

        return kszAllContactId;
    }
#if IF_BUG_PLCMDIR
    else if (DIRECTORY_TYPE_PLCM == nDirType)
    {
        return kszPlcmDirGroupId;
    }
#endif
#if IF_FEATURE_GBBOOK
    else if (DIRECTORY_TYPE_GENBAND == nDirType)
    {
        if (nGroupId == kszGabBookGroupId)
        {
            return nGroupId;
        }
        else
        {
            return kszAllContactId;
        }
    }
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    else if (DIRECTORY_TYPE_METASWICTH == nDirType)
    {
        if (nGroupId >= kszContactList && nGroupId <= kszMLHG)
        {
            return nGroupId;
        }

        return kszAllContactId;
    }
#endif

    return nGroupId;
}

int CContactGroupDelegate::GetCurrentGroupId(int nContactType, int nOriGroupId)
{
    if (DIRECTORY_TYPE_REMOTE == nContactType)
    {
        return nOriGroupId;
    }

    CContactGroupNodePtr pGroupNode = (CContactGroupNodePtr)GetTreeDataNode(m_nUISelectId);

    if (NULL == pGroupNode || nContactType != pGroupNode->GetType())
    {
        return GetDefaultGroupId(nContactType, 0);
    }
    else if (DIRECTORY_TYPE_BROADSOFT == nContactType && kszCustomId == pGroupNode->GetGroupId())
    {
        return nOriGroupId;
    }

    return pGroupNode->GetGroupId();
}

void CContactGroupDelegate::Init()
{
    m_bSetting = false;
    m_nUISelectId = 0;

#if IF_BUG_PLCMDIR
    InitGroupId(CONTACT_GROUP_TYPE_PLCM, CONTACT_GROUP_ID_PLCM_BEGIN, CONTACT_GROUP_ID_PLCM_END);
#endif
    InitGroupId(CONTACT_GROUP_TYPE_LOCAL, CONTACT_GROUP_ID_LOCAL_BEGIN, CONTACT_GROUP_ID_LOCAL_END);
    InitGroupId(CONTACT_GROUP_TYPE_BLUETOOTH, CONTACT_GROUP_ID_BLUETOOTH_BEGIN,
                CONTACT_GROUP_ID_BLUETOOTH_END);
    InitGroupId(CONTACT_GROUP_TYPE_REMOTE, CONTACT_GROUP_ID_REMOTE_BEGIN, CONTACT_GROUP_ID_REMOTE_END);
    InitGroupId(CONTACT_GROUP_TYPE_LDAP, CONTACT_GROUP_ID_LDAP_BEGIN, CONTACT_GROUP_ID_LDAP_END);
    InitGroupId(CONTACT_GROUP_TYPE_BROADSOFT, CONTACT_GROUP_ID_BROADSOFT_BEGIN,
                CONTACT_GROUP_ID_BROADSOFT_END);
    InitGroupId(CONTACT_GROUP_TYPE_BUDDIES, CONTACT_GROUP_ID_BTUC_BEGIN, CONTACT_GROUP_ID_BTUC_END);
    InitGroupId(CONTACT_GROUP_TYPE_BLACKLIST, CONTACT_GROUP_ID_BLACKLIST_BEGIN,
                CONTACT_GROUP_ID_BLACKLIST_END);
#if IF_FEATURE_GBBOOK
    InitGroupId(CONTACT_GROUP_TYPE_GAB, CONTACT_GROUP_ID_GAB_BEGIN, CONTACT_GROUP_ID_GAB_END);
    InitGroupId(CONTACT_GROUP_TYPE_PAB, CONTACT_GROUP_ID_PAB_BEGIN, CONTACT_GROUP_ID_PAB_END);
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    InitGroupId(CONTACT_GROUP_TYPE_METASWITCH, CONTACT_GROUP_ID_METASWITCH_BEGIN,
                CONTACT_GROUP_ID_METASWITCH_END);
#endif

#if IF_FEATURE_FAVORITE_DIR
    if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
    {
        InitGroupId(CONTACT_GROUP_TYPE_FAVORITE, CONTACT_GROUP_ID_FAVORITE_BEGIN,
                    CONTACT_GROUP_ID_FAVORITE_END);
    }
#endif

#if IF_BUG_PLCMDIR
    InitPlcmGroup();
#endif
    InitLocalGroup();
    InitBluetoothGroup();
    InitRemoteGroup();
    InitLDAPGroup();
    InitBroadSoftGroup();
    InitBTUCGroup();
    InitBlackListGroup();
#if IF_FEATURE_GBBOOK
    InitGabGroup();

    InitPabGroup();
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    InitMetaSwitchGroup();
#endif
#if IF_FEATURE_FAVORITE_DIR
    if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
    {
        InitFavoriteGroup();
    }
#endif

    InitGroupList();
}

void CContactGroupDelegate::SetReadOnly(bool bReadOnly)
{
    if (NULL == m_pFrameList)
    {
        return;
    }

    CFrameListTitlePtr pTitle = m_pFrameList->GetFrameListTitle();
    CGroupNormalTitlePtr pGroupTitle = NULL;

    if (NULL != pTitle && CGroupNormalTitle::IsGroupNormalTitle(pTitle))
    {
        pGroupTitle = (CGroupNormalTitlePtr)pTitle;
    }

    if (NULL != pGroupTitle)
    {
        pGroupTitle->SetReadOnly(bReadOnly);
    }
}

LRESULT CContactGroupDelegate::OnMessage(msgObject & refMessage)
{
    LRESULT retValue = FALSE;

    switch (refMessage.message)
    {
    case LOCAL_DIRECTORY_RELOAD:
        {
            RefreshLocalGroup(kszAllContactId);
            MessageBox_RemoveMessageBox(&g_DirectoryMgr, MessageBox_Cancel, DIR_ACTION_LOCAL_DEL_GROUP);
            retValue = TRUE;
        }
        break;
    case REMOTE_PHONEBOOK_RELOAD:
        {
            RefreshRemoteGroup();
        }
        break;
    case LDAP_STATUS_CHANGED:
        {
            RefreshLDAPGroup();
        }
        break;
    case BROADSOFT_PHONEBOOK_RELOAD:
        {
            RefreshBroadSoftGroup();
            retValue = TRUE;
        }
        break;
    case BLUETOOTH_CONTACT_RELOAD:
        {
            RefreshBluetoothGroup();
        }
        break;
    case BROADSOFT_BUDDYLIST_GROUP_CHANGE:
        {
            RefreshBTUCGroup();
            retValue = TRUE;
        }
        break;
#if IF_FEATURE_GBBOOK
    case GENBAND_BOOK_RELOAD:
        {
            RefreshPABGroup(kszAllContactId);
            retValue = TRUE;
        }
        break;
    case GENBAND_BOOK_REQ_RESULT:
        {
            if (refMessage.wParam == PAB_OPERATION_REMOVE_GROUP
                    || refMessage.wParam == PAB_OPERATION_ADD_GROUP
                    || refMessage.wParam == PAB_OPERATION_MODIFY_GROUP
                    || refMessage.wParam == PAB_OPERATION_GET_GROUPLIST)
            {
                RefreshPABGroup(kszAllContactId);
                retValue = TRUE;
            }
        }
        break;
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    case METASWITCH_PHONEBOOK_RELOAD:
        {
            RefreshMetaSwitchGroup();
            retValue = TRUE;
        }
        break;
#endif
    default:
        break;
    }
    return retValue;
}

void CContactGroupDelegate::InitLocalGroup()
{
    ResetGroupTreeId(CONTACT_GROUP_TYPE_LOCAL);

    // 没有显示权限，就不加到群组中
    if (!CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_LOCAL))
    {
        return ;
    }

    // 初始化普通群组
    YLVector<int> vecGroupId;

    if (!Dir_GetConfigMenuIdList(vecGroupId, LOCAL_DIR))
    {
        return;
    }

    int nSize = vecGroupId.size();
    bool bHadChildren = ((nSize > 1) ? true : false);

    // 初始化根节点
    yl::string strGroupName;

    if (!CCommonDirCtrl::GetDisplayName(LOCAL_DIR, kszAllContactId, strGroupName))
    {
        return;
    }

    int nRootId = ApplyGroupTreeId(CONTACT_GROUP_TYPE_LOCAL);
    CContactGroupNodePtr pGroupNode = new CContactGroupNode(nRootId, 0, bHadChildren,
            DIRECTORY_TYPE_LOCAL, kszAllContactId);

    if (NULL == pGroupNode)
    {
        return;
    }

    strGroupName = LANG_TRANSLATE(strGroupName.c_str()).toUtf8().data();
    pGroupNode->SetName(strGroupName);
    pGroupNode->SetAddContact(true);
    m_mapTreeNode[pGroupNode->GetId()] = pGroupNode;

    for (int i = 0; i < nSize; ++i)
    {
        int nGroupId = vecGroupId[i];

        if (kszAllContactId == nGroupId)
        {
            continue;
        }

        if (!CCommonDirCtrl::GetDisplayName(LOCAL_DIR, nGroupId, strGroupName))
        {
            continue;
        }

        int nGroupTreeId = ApplyGroupTreeId(CONTACT_GROUP_TYPE_LOCAL);
        pGroupNode = new CContactGroupNode(nGroupTreeId, nRootId, false, DIRECTORY_TYPE_LOCAL, nGroupId);

        if (NULL != pGroupNode)
        {
            pGroupNode->SetEditable(true);
#if IF_FEATURE_GBBOOK
            pGroupNode->SetShowBtnRing(true);
#endif
            pGroupNode->SetName(strGroupName);
            pGroupNode->SetAddContact(true);
            m_mapTreeNode[pGroupNode->GetId()] = pGroupNode;
        }
    }
}

void CContactGroupDelegate::InitBluetoothGroup()
{
    ResetGroupTreeId(CONTACT_GROUP_TYPE_BLUETOOTH);

    if (!CCommonDirCtrl::IsDirEnable(DIRECTORY_TYPE_BLUETOOTH))
    {
        return;
    }

    int nRootId = ApplyGroupTreeId(CONTACT_GROUP_TYPE_BLUETOOTH);
    CContactGroupNodePtr pGroupNode = new CContactGroupNode(nRootId, 0, false, DIRECTORY_TYPE_BLUETOOTH,
            kszBluethoothGroupId);

    if (NULL == pGroupNode)
    {
        return;
    }

    QString strDefaultName = LANG_TRANSLATE(TRID_MOBILE_CONTACTS);

    pGroupNode->SetName(strDefaultName.toUtf8().data());

    m_mapTreeNode[pGroupNode->GetId()] = pGroupNode;
}

void CContactGroupDelegate::InitRemoteGroup()
{
    ResetGroupTreeId(CONTACT_GROUP_TYPE_REMOTE);

    if (!CCommonDirCtrl::IsDirEnable(DIRECTORY_TYPE_REMOTE))
    {
        return;
    }

    // 没有显示权限，就不加到群组中
    if (!CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_REMOTE))
    {
        return ;
    }

    int nRootId = ApplyGroupTreeId(CONTACT_GROUP_TYPE_REMOTE);
    CContactGroupNodePtr pGroupNode = new CContactGroupNode(nRootId, 0, false, DIRECTORY_TYPE_REMOTE,
            kszRootId);

    if (NULL == pGroupNode)
    {
        return;
    }

    const yl::string & strDisplayName = RemoteDir_GetRemoteDisplayName();

    if (strDisplayName.empty())
    {
        QString strDefaultName = LANG_TRANSLATE(TRID_REMOTE_PHONEBOOK);

        pGroupNode->SetName(strDefaultName.toUtf8().data());
    }
    else
    {
        pGroupNode->SetName(strDisplayName);
    }

    m_mapTreeNode[pGroupNode->GetId()] = pGroupNode;
}

void CContactGroupDelegate::InitLDAPGroup()
{
    ResetGroupTreeId(CONTACT_GROUP_TYPE_LDAP);

    // 没有配置，就不加到群组中
    if (!LDAP_IsEnable())
    {
        return;
    }

    int nRootId = ApplyGroupTreeId(CONTACT_GROUP_TYPE_LDAP);
    CContactGroupNodePtr pGroupNode = new CContactGroupNode(nRootId, 0, false, DIRECTORY_TYPE_LDAP,
            kszAllContactId);

    if (NULL == pGroupNode)
    {
        return;
    }

    QString strLDAP;

    DM_GetLdapName(strLDAP);
    pGroupNode->SetName(strLDAP.toUtf8().data());

    m_mapTreeNode[pGroupNode->GetId()] = pGroupNode;
}

void CContactGroupDelegate::InitBroadSoftGroup()
{
    ResetGroupTreeId(CONTACT_GROUP_TYPE_BROADSOFT);


    // 没有配置，就不加到群组中
    if (!CCommonDirCtrl::IsDirEnable(DIRECTORY_TYPE_BROADSOFT))
    {
        return;
    }

    // 没有显示权限，就不加到群组中
    if (!CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_BROADSOFT))
    {
        return;
    }

    YLVector<int> vecGroupId;

    if (!Dir_GetConfigMenuIdList(vecGroupId, BSFT_DIR))
    {
        return;
    }

    int nSize = vecGroupId.size();
    bool bHadChildren = ((nSize > 1) ? true : false);
    int nRootId = 0;
    CContactGroupNodePtr pGroupNode;
    if (configParser_GetConfigInt(kszBsftDirUpdateEnable) == 1)
    {
        nRootId = ApplyGroupTreeId(CONTACT_GROUP_TYPE_BROADSOFT);
        pGroupNode = new CContactGroupNode(nRootId, 0, bHadChildren,
                                           DIRECTORY_TYPE_BROADSOFT, kszAllContactId);

        if (NULL == pGroupNode)
        {
            return;
        }

        QString strDefaultName = LANG_TRANSLATE(TRID_NETWORK_DIRECTORY);

        pGroupNode->SetName(strDefaultName.toUtf8().data());
        m_mapTreeNode[pGroupNode->GetId()] = pGroupNode;
    }

    for (int i = 0; i < nSize; ++i)
    {
        int nGroupId = vecGroupId[i];

        if (kszAllContactId == nGroupId)
        {
            continue;
        }

        yl::string strGroupName;

        if (!CCommonDirCtrl::GetDisplayName(BSFT_DIR, nGroupId, strGroupName))
        {
            continue;
        }

        int nGroupTreeId = ApplyGroupTreeId(CONTACT_GROUP_TYPE_BROADSOFT);

        pGroupNode = new CContactGroupNode(nGroupTreeId, nRootId, false, DIRECTORY_TYPE_BROADSOFT,
                                           nGroupId);

        if (NULL != pGroupNode)
        {
            if (strGroupName.empty())
            {
                QString strDisplayName;

                CBSFTContactUIHelper::GetGroupDisplayName(nGroupId, strGroupName, strDisplayName);

                strGroupName = strDisplayName.toUtf8().data();
            }

            pGroupNode->SetName(strGroupName);

            if (kszPersonalId == nGroupId)
            {
                pGroupNode->SetAddContact(true);
            }

            m_mapTreeNode[pGroupNode->GetId()] = pGroupNode;
        }
    }
}

void CContactGroupDelegate::InitBTUCGroup()
{
    ResetGroupTreeId(CONTACT_GROUP_TYPE_BUDDIES);

    if (!CBTUCContactControler::IsConfigure())
    {
        return;
    }

    // 没有显示权限，就不加到群组中
    if (!CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_BTUC))
    {
        return ;
    }

#if FEATURE_BROADTOUCH_UC
    bool bHadChildren = false;
    GroupList listGroup;

    if (Presence_GetGroupList(PRESENCE_UC, listGroup) && listGroup.size() > 1)
    {
        bHadChildren = true;
    }

    int nRootId = ApplyGroupTreeId(CONTACT_GROUP_TYPE_BUDDIES);
    CContactGroupNodePtr pGroupNode = new CContactGroupNode(nRootId, 0, bHadChildren,
            DIRECTORY_TYPE_BTUC, ALL_BUDDIES_ID);

    if (NULL == pGroupNode)
    {
        return;
    }

    QString strDefaultName = LANG_TRANSLATE(TRID_ALL_BUDDIES);

    pGroupNode->SetName(strDefaultName.toUtf8().data());
    m_mapTreeNode[pGroupNode->GetId()] = pGroupNode;

    GroupList::iterator iter = listGroup.begin();

    for (; iter != listGroup.end(); ++iter)
    {
        GroupItem & itemGroup = *iter;
        yl::string strGroupName = itemGroup.m_strDisplayName;

        if (ALL_BUDDIES_ID == itemGroup.m_keyId)
        {
            continue;
        }

        if (FAVOURITE_ID == itemGroup.m_keyId)
        {
            strGroupName = LANG_TRANSLATE(strGroupName.c_str()).toUtf8().data();
        }

        int nGroupTreeId = ApplyGroupTreeId(CONTACT_GROUP_TYPE_BUDDIES);
        pGroupNode = new CContactGroupNode(nGroupTreeId, nRootId, false, DIRECTORY_TYPE_BTUC,
                                           itemGroup.m_keyId);

        if (NULL != pGroupNode)
        {
            pGroupNode->SetName(strGroupName);
            m_mapTreeNode[pGroupNode->GetId()] = pGroupNode;
        }
    }
#endif
}

void CContactGroupDelegate::InitBlackListGroup()
{
    ResetGroupTreeId(CONTACT_GROUP_TYPE_BLACKLIST);

    // 没有显示权限，就不加到群组中
    if (!CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_LOCAL, kszBlackListGroupId))
    {
        return ;
    }

    // 初始化根节点
    yl::string strBlackList = "";

    if (!CCommonDirCtrl::GetDisplayName(LOCAL_DIR, kszBlackListGroupId, strBlackList))
    {
        return;
    }

    int nRootId = ApplyGroupTreeId(CONTACT_GROUP_TYPE_BLACKLIST);
    CContactGroupNodePtr pGroupNode = new CContactGroupNode(nRootId, 0, false, DIRECTORY_TYPE_LOCAL,
            kszBlackListGroupId);

    if (NULL == pGroupNode)
    {
        return;
    }

    pGroupNode->SetName(LANG_TRANSLATE(strBlackList.c_str()).toUtf8().data());
    pGroupNode->SetAddContact(true);
    m_mapTreeNode[pGroupNode->GetId()] = pGroupNode;
}

#if IF_BUG_PLCMDIR
void CContactGroupDelegate::InitPlcmGroup()
{
    ResetGroupTreeId(CONTACT_GROUP_TYPE_PLCM);

    if (configParser_GetConfigInt(kszPlcmPhoneEnable) != 1)
    {
        return;
    }

    // 没有显示权限，就不加到群组中
    if (!CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_PLCM))
    {
        return ;
    }

    int nRootId = ApplyGroupTreeId(CONTACT_GROUP_TYPE_PLCM);
    CContactGroupNodePtr pGroupNode = new CContactGroupNode(nRootId, 0, false, DIRECTORY_TYPE_PLCM,
            kszPlcmDirGroupId);

    if (NULL == pGroupNode)
    {
        return;
    }

    QString strDefaultName = LANG_TRANSLATE(TRID_PLCMBOOK);

    pGroupNode->SetName(strDefaultName.toUtf8().data());


    m_mapTreeNode[pGroupNode->GetId()] = pGroupNode;
}
#endif

#if IF_FEATURE_GBBOOK
void CContactGroupDelegate::InitGabGroup()
{
    ResetGroupTreeId(CONTACT_GROUP_TYPE_GAB);

    if (configParser_GetConfigInt(kszGbGabBookEnable) != 1
            || configParser_GetConfigInt(kszGbSoupEnable) != 1)
    {
        return;
    }

    // 没有显示权限，就不加到群组中
    if (!CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_GENBAND))
    {
        return ;
    }

    int nRootId = ApplyGroupTreeId(CONTACT_GROUP_TYPE_GAB);
    CContactGroupNodePtr pGroupNode = new CContactGroupNode(nRootId, 0, false, DIRECTORY_TYPE_GENBAND,
            kszGabBookGroupId);

    if (NULL == pGroupNode)
    {
        return;
    }

    QString strDefaultName = LANG_TRANSLATE(TRID_GB_GABBOOK);

    pGroupNode->SetName(strDefaultName.toUtf8().data());
    m_mapTreeNode[pGroupNode->GetId()] = pGroupNode;
}

void CContactGroupDelegate::InitPabGroup()
{
    ResetGroupTreeId(CONTACT_GROUP_TYPE_PAB);

    if (configParser_GetConfigInt(kszGbPabBookEnable) != 1
            || configParser_GetConfigInt(kszGbSoupEnable) != 1)
    {
        return;
    }

    // 没有显示权限，就不加到群组中
    if (!CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_GENBAND))
    {
        return ;
    }

    YLVector<int> vecGroupId;
    ContactListData pData;
    if (Dir_GetContactListByGroupId(&pData, kszPabBookGroupId, GB_DIR))
    {
        vecGroupId = pData.m_listId;
    }

    int nSize = vecGroupId.size();
    bool bHadChildren = ((nSize > 0) ? true : false);

    int nRootId = ApplyGroupTreeId(CONTACT_GROUP_TYPE_PAB);
    CContactGroupNodePtr pGroupNode = new CContactGroupNode(nRootId, 0, bHadChildren,
            DIRECTORY_TYPE_GENBAND, kszAllContactId);

    if (NULL == pGroupNode)
    {
        return;
    }

    QString strDefaultName = LANG_TRANSLATE(TRID_GB_PABBOOK);
    pGroupNode->SetName(strDefaultName.toUtf8().data());
    pGroupNode->SetAddContact(false);
    m_mapTreeNode[pGroupNode->GetId()] = pGroupNode;

    for (int i = 0; i < nSize; ++i)
    {
        int nGroupId = vecGroupId[i];

        yl::string strGroupName;
        if (!CCommonDirCtrl::GetDisplayName(GB_DIR, nGroupId, strGroupName))
        {
            continue;
        }

        int nGroupTreeId = ApplyGroupTreeId(CONTACT_GROUP_TYPE_PAB);
        pGroupNode = new CContactGroupNode(nGroupTreeId, nRootId, false, DIRECTORY_TYPE_GENBAND, nGroupId);

        if (NULL != pGroupNode)
        {
            pGroupNode->SetEditable(nGroupId == kszGbNAGroupId ? false : true);
            pGroupNode->SetShowBtnRing(false);
            // na group  需要翻译
            if (nGroupId == kszGbNAGroupId)
            {
                QString strTmpName = LANG_TRANSLATE(TRID_NO_GROUP);
                pGroupNode->SetName(strTmpName.toUtf8().data());
            }
            else
            {
                pGroupNode->SetName(strGroupName);
            }
            pGroupNode->SetAddContact(true);
            m_mapTreeNode[pGroupNode->GetId()] = pGroupNode;
        }
    }
}
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
void CContactGroupDelegate::InitMetaSwitchGroup()
{
    ResetGroupTreeId(CONTACT_GROUP_TYPE_METASWITCH);

    // 没有配置，就不加到群组中
    if (!CCommonDirCtrl::IsDirEnable(DIRECTORY_TYPE_METASWICTH))
    {
        return;
    }

//  // 没有显示权限，就不加到群组中
    if (!CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_METASWICTH))
    {
        return;
    }

    YLVector<int> vecGroupId;

    if (!Dir_GetConfigMenuIdList(vecGroupId, MTSW_DIR))
    {
        return;
    }

    int nSize = vecGroupId.size();
    bool bHadChildren = ((nSize > 1) ? true : false);
    int nRootId = ApplyGroupTreeId(CONTACT_GROUP_TYPE_METASWITCH);
    CContactGroupNodePtr pGroupNode = new CContactGroupNode(nRootId, 0, bHadChildren,
            DIRECTORY_TYPE_METASWICTH, nSize == 1 ? vecGroupId[0] : kszAllContactId);

    if (NULL == pGroupNode)
    {
        return;
    }

    QString strDefaultName = LANG_TRANSLATE(TRID_NETWORK_CONTACTS);

    pGroupNode->SetName(strDefaultName.toUtf8().data());
    m_mapTreeNode[pGroupNode->GetId()] = pGroupNode;

    // 只有一个组不需要执行添加操作，统一显示在最上层位置
    if (nSize <= 1)
    {
        return;
    }

    for (int i = 0; i < nSize; ++i)
    {
        int nGroupId = vecGroupId[i];

        if (kszAllContactId == nGroupId)
        {
            continue;
        }

        yl::string strGroupName;

        if (!CCommonDirCtrl::GetDisplayName(MTSW_DIR, nGroupId, strGroupName))
        {
            continue;
        }

        int nGroupTreeId = ApplyGroupTreeId(CONTACT_GROUP_TYPE_METASWITCH);

        pGroupNode = new CContactGroupNode(nGroupTreeId, nRootId, false, DIRECTORY_TYPE_METASWICTH,
                                           nGroupId);

        if (NULL != pGroupNode)
        {
            if (strGroupName.empty())
            {
                QString strDisplayName;

                CMTSWContactUIHelper::GetMetaSwitchDisplayName(nGroupId, strGroupName, strDisplayName);

                strGroupName = strDisplayName.toUtf8().data();
            }

            pGroupNode->SetName(strGroupName);



            m_mapTreeNode[pGroupNode->GetId()] = pGroupNode;
        }
    }
}
#endif
#if IF_FEATURE_FAVORITE_DIR
void CContactGroupDelegate::InitFavoriteGroup()
{
    if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) != 1)
    {
        return;
    }

    ResetGroupTreeId(CONTACT_GROUP_TYPE_FAVORITE);

    // 没有显示权限，就不加到群组中
    if (!CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_LOCAL, kszFavoriteGroupId))
    {
        return ;
    }

    // 初始化根节点
    yl::string strFavorite = "";

    if (!CCommonDirCtrl::GetDisplayName(LOCAL_DIR, kszFavoriteGroupId, strFavorite))
    {
        return;
    }

    int nRootId = ApplyGroupTreeId(CONTACT_GROUP_TYPE_FAVORITE);
    CContactGroupNodePtr pGroupNode = new CContactGroupNode(nRootId, 0, false, DIRECTORY_TYPE_LOCAL,
            kszFavoriteGroupId);

    if (NULL == pGroupNode)
    {
        return;
    }

    pGroupNode->SetName(LANG_TRANSLATE(strFavorite.c_str()).toUtf8().data());
    pGroupNode->SetAddContact(true);
    m_mapTreeNode[pGroupNode->GetId()] = pGroupNode;
}
#endif

void CContactGroupDelegate::InitGroupList()
{
    m_vecTreeId.clear();

    MAP_TREE_DATA_NODE::iterator iter = m_mapTreeNode.begin();

    for (; iter != m_mapTreeNode.end(); ++iter)
    {
        CContactGroupNodePtr pGroupNode = (CContactGroupNodePtr)iter->second;

        if (NULL == pGroupNode || 0 != pGroupNode->GetParentId())
        {
            continue;
        }

        if (!m_bSetting)
        {
            m_vecTreeId.push_back(pGroupNode->GetId());
            continue;
        }

        // LDAP、Buddies不在Setting界面显示
        int nType = pGroupNode->GetType();

        if (DIRECTORY_TYPE_LDAP == nType || DIRECTORY_TYPE_BTUC == nType
                || DIRECTORY_TYPE_BROADSOFT == nType)
        {
            continue;
        }

#if IF_FEATURE_FAVORITE_DIR
        if (kszFavoriteGroupId == pGroupNode->GetGroupId())
        {
            continue;
        }
#endif

#if IF_FEATURE_GBBOOK
        if (DIRECTORY_TYPE_GENBAND == nType && pGroupNode->GetGroupId() == kszGabBookGroupId)
        {
            continue;
        }
#endif

        m_vecTreeId.push_back(pGroupNode->GetId());
    }
}

void CContactGroupDelegate::RefreshLocalGroup(int nSelGroupId)
{
    // 默认节点
    if (0 == nSelGroupId)
    {
        CContactGroupNodePtr pGroupNode = (CContactGroupNodePtr)GetTreeDataNode(
                                              CONTACT_GROUP_ID_ALL_LOCAL_CONTACT);

        if (NULL != pGroupNode)
        {
            nSelGroupId = pGroupNode->GetGroupId();
        }
    }

    // 初始化节点
    ClearGroup(CONTACT_GROUP_ID_LOCAL_BEGIN, CONTACT_GROUP_ID_LOCAL_END);
    InitLocalGroup();

    // 初始化界面节点ID
    CContactGroupNodePtr pGroupNode = (CContactGroupNodePtr)GetTreeDataNode(
                                          CONTACT_GROUP_ID_ALL_LOCAL_CONTACT);

    if (NULL == pGroupNode)
    {
        return;
    }

    m_vecTreeId.push_back((int)CONTACT_GROUP_ID_ALL_LOCAL_CONTACT);

    std::sort(m_vecTreeId.begin(), m_vecTreeId.end(), std::less<int>());

    ExpandNode(pGroupNode);

    // 设定选中项
    CContactGroupNodePtr pSelectGroupNode = (CContactGroupNodePtr)GetGroupNode(DIRECTORY_TYPE_LOCAL,
                                            nSelGroupId);

    if (NULL != pSelectGroupNode)
    {
        m_nUISelectId = pSelectGroupNode->GetId();
    }
    else
    {
        m_nUISelectId = pGroupNode->GetId();
    }

    // 刷新列表
    ReLocateCurrentPage();
    ReSelectGroup();
}

void CContactGroupDelegate::RefreshBluetoothGroup()
{
    ClearGroup(CONTACT_GROUP_ID_BLUETOOTH_BEGIN, CONTACT_GROUP_ID_BLUETOOTH_END);
    InitBluetoothGroup();

    bool bGroupChange = false;

    if (!ReInitGroupList(DIRECTORY_TYPE_BLUETOOTH, CONTACT_GROUP_ID_BLUETOOTH_CONTACT, false, 0)
            && CONTACT_GROUP_ID_BLUETOOTH_CONTACT == m_nUISelectId)
    {
        bGroupChange = true;
        m_nUISelectId = CONTACT_GROUP_ID_ALL_LOCAL_CONTACT;
    }
    else if (CONTACT_GROUP_ID_BLUETOOTH_CONTACT == m_nUISelectId)
    {
        bGroupChange = true;
    }

    // 刷新列表
    ReLocateCurrentPage();

    if (bGroupChange)
    {
        ReSelectGroup();
    }
}

void CContactGroupDelegate::RefreshRemoteGroup()
{
    ClearGroup(CONTACT_GROUP_ID_REMOTE_BEGIN, CONTACT_GROUP_ID_REMOTE_END);
    InitRemoteGroup();

    bool bGroupChange = false;

    if (!ReInitGroupList(DIRECTORY_TYPE_REMOTE, CONTACT_GROUP_ID_NETWORK_PHONE_BOOK, false, 0)
            && CONTACT_GROUP_ID_NETWORK_PHONE_BOOK == m_nUISelectId)
    {
        bGroupChange = true;
        m_nUISelectId = CONTACT_GROUP_ID_ALL_LOCAL_CONTACT;
    }
    else if (CONTACT_GROUP_ID_NETWORK_PHONE_BOOK == m_nUISelectId)
    {
        bGroupChange = true;
    }

    // 刷新列表
    ReLocateCurrentPage();

    if (bGroupChange)
    {
        ReSelectGroup();
    }
}

void CContactGroupDelegate::RefreshLDAPGroup()
{
    ClearGroup(CONTACT_GROUP_ID_LDAP_BEGIN, CONTACT_GROUP_ID_LDAP_END);
    InitLDAPGroup();

    bool bGroupChange = false;

    if (!ReInitGroupList(DIRECTORY_TYPE_LDAP, CONTACT_GROUP_ID_LDAP_ROOT, false, 0)
            && CONTACT_GROUP_ID_LDAP_ROOT == m_nUISelectId)
    {
        bGroupChange = true;
        m_nUISelectId = CONTACT_GROUP_ID_ALL_LOCAL_CONTACT;
    }

    // 刷新列表
    ReLocateCurrentPage();

    if (bGroupChange)
    {
        ReSelectGroup();
    }
}

void CContactGroupDelegate::RefreshBroadSoftGroup()
{
    // 获取刷新前的界面信息
    bool bExpand = IsExpand(CONTACT_GROUP_ID_NETWORK_DIRECTORY);
    int nGroupSelectId = GetGroupSelectId(DIRECTORY_TYPE_BROADSOFT);
    bool bGroupChange = false;

    // 初始化节点
    ClearGroup(CONTACT_GROUP_ID_BROADSOFT_BEGIN, CONTACT_GROUP_ID_BROADSOFT_END);
    InitBroadSoftGroup();

    // 初始化界面节点ID
    if (!m_bSetting)
    {
        if (!ReInitGroupList(DIRECTORY_TYPE_BROADSOFT, CONTACT_GROUP_ID_NETWORK_DIRECTORY, bExpand,
                             nGroupSelectId)
                && (m_nUISelectId >= CONTACT_GROUP_ID_BROADSOFT_BEGIN
                    && m_nUISelectId <= CONTACT_GROUP_ID_BROADSOFT_END))
        {
            m_nUISelectId = CONTACT_GROUP_ID_ALL_LOCAL_CONTACT;
            bGroupChange = true;
        }
        else if (m_nUISelectId >= CONTACT_GROUP_ID_BROADSOFT_BEGIN
                 && m_nUISelectId <= CONTACT_GROUP_ID_BROADSOFT_END)
        {
            bGroupChange = true;
        }
    }
    else if (m_nUISelectId >= CONTACT_GROUP_ID_BROADSOFT_BEGIN
             && m_nUISelectId <= CONTACT_GROUP_ID_BROADSOFT_END)
    {
        m_nUISelectId = CONTACT_GROUP_ID_ALL_LOCAL_CONTACT;
        bGroupChange = true;
    }

    // 刷新列表
    ReLocateCurrentPage();

    // init update title
    CDirectoryControlerPtr pActiveControler = g_DirectoryMgr.GetActiveControler();
    CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pActiveControler);
    if (pBSFTContact != NULL)
    {
        CBSFTContactUIHelper * pBSFTUIHelper = pBSFTContact->GetBSFTUIHelper();
        if (pBSFTUIHelper != NULL)
        {
            pBSFTUIHelper->OnAttachFrameList();
        }
    }


    if (bGroupChange)
    {
        ReSelectGroup();
    }
}

void CContactGroupDelegate::RefreshBTUCGroup()
{
    // 获取刷新前的界面信息
    bool bExpand = IsExpand(CONTACT_GROUP_ID_BUDDIES);
    int nGroupSelectId = GetGroupSelectId(DIRECTORY_TYPE_BTUC);

    // 初始化节点
    ClearGroup(CONTACT_GROUP_ID_BTUC_BEGIN, CONTACT_GROUP_ID_BTUC_END);
    InitBTUCGroup();

    // 初始化界面节点ID
    bool bGroupChange = false;

    if (!ReInitGroupList(DIRECTORY_TYPE_BTUC, CONTACT_GROUP_ID_BUDDIES, bExpand, nGroupSelectId)
            && (m_nUISelectId >= CONTACT_GROUP_ID_BTUC_BEGIN && m_nUISelectId <= CONTACT_GROUP_ID_BTUC_END))
    {
        bGroupChange = true;
        m_nUISelectId = CONTACT_GROUP_ID_ALL_LOCAL_CONTACT;
    }
    else if (0 != nGroupSelectId)
    {
        CContactGroupNodePtr pGroupNode = (CContactGroupNodePtr)GetTreeDataNode(m_nUISelectId);

        if (NULL != pGroupNode && nGroupSelectId != pGroupNode->GetGroupId())
        {
            bGroupChange = true;
        }
    }

    // 刷新列表
    ReLocateCurrentPage();

    if (bGroupChange)
    {
        ReSelectGroup();
    }
}

#if IF_FEATURE_GBBOOK
void CContactGroupDelegate::RefreshPABGroup(int nSelGroupId)
{
    // 默认节点
    if (0 == nSelGroupId)
    {
        CContactGroupNodePtr pGroupNode = (CContactGroupNodePtr)GetTreeDataNode(
                                              CONTACT_GROUP_ID_ALL_LOCAL_CONTACT);

        if (NULL != pGroupNode)
        {
            nSelGroupId = pGroupNode->GetGroupId();
        }
    }

    // 初始化节点
    ClearGroup(CONTACT_GROUP_ID_PAB_BEGIN, CONTACT_GROUP_ID_PAB_END);
    InitPabGroup();

    // 初始化界面节点ID
    CContactGroupNodePtr pGroupNode = (CContactGroupNodePtr)GetTreeDataNode(CONTACT_GROUP_ID_PAB);

    if (NULL == pGroupNode)
    {
        return;
    }

    m_vecTreeId.push_back((int)CONTACT_GROUP_ID_PAB);

    std::sort(m_vecTreeId.begin(), m_vecTreeId.end(), std::less<int>());

    ExpandNode(pGroupNode);

    // 设定选中项
    CContactGroupNodePtr pSelectGroupNode = (CContactGroupNodePtr)GetGroupNode(DIRECTORY_TYPE_GENBAND,
                                            nSelGroupId);

    if (NULL != pSelectGroupNode)
    {
        m_nUISelectId = pSelectGroupNode->GetId();
    }
    else
    {
        m_nUISelectId = pGroupNode->GetId();
    }

    // 刷新列表
    ReLocateCurrentPage();
    ReSelectGroup();
}
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
void CContactGroupDelegate::RefreshMetaSwitchGroup()
{
    // 获取刷新前的界面信息
    bool bExpand = IsExpand(CONTACT_GROUP_ID_METASWITCH_DIRECTORY);
    int nGroupSelectId = GetGroupSelectId(DIRECTORY_TYPE_METASWICTH);

    // 初始化节点
    ClearGroup(CONTACT_GROUP_ID_METASWITCH_BEGIN, CONTACT_GROUP_ID_METASWITCH_END);
    InitMetaSwitchGroup();

    // 初始化界面节点ID
    if (!m_bSetting)
    {
        if (!ReInitGroupList(DIRECTORY_TYPE_METASWICTH, CONTACT_GROUP_ID_METASWITCH_DIRECTORY, bExpand,
                             nGroupSelectId)
                && (m_nUISelectId >= CONTACT_GROUP_ID_METASWITCH_BEGIN
                    && m_nUISelectId <= CONTACT_GROUP_ID_METASWITCH_END))
        {
            m_nUISelectId = CONTACT_GROUP_ID_ALL_LOCAL_CONTACT;
        }
    }
    else if (m_nUISelectId >= CONTACT_GROUP_ID_METASWITCH_BEGIN
             && m_nUISelectId <= CONTACT_GROUP_ID_METASWITCH_END)
    {
        m_nUISelectId = CONTACT_GROUP_ID_METASWITCH_DIRECTORY;
    }

    // 刷新列表
    ReLocateCurrentPage();
    ReSelectGroup();
}
#endif

void CContactGroupDelegate::RefreshTreeListItem(int nCount, int nIndex)
{
    CTreeList * pTreeList = (CTreeList *)m_pFrameList;

    if (NULL == pTreeList)
    {
        return;
    }

    if (m_bSetting && !(m_pFrameList->IsCanReuseItem(CGroupEditTreeItem::GetGroupEditTreeItemType())))
    {
        pTreeList->ClearContent();
    }

    int nMaxPageItemCount = pTreeList->GetMaxPageItemCount();

    if (0 == nCount)
    {
        nCount = nMaxPageItemCount;
    }

    int nSize = m_vecTreeId.size();

    if (nCount > nSize)
    {
        nCount = nSize;
    }

    if (nMaxPageItemCount > 0)
    {
        if (nIndex >= nSize)
        {
            nIndex = nSize - (nSize % nMaxPageItemCount);
            nCount = nSize - nIndex;
        }
    }
    else
    {
        nIndex = 0;
    }

    m_nPageDataIndex = nIndex;
    pTreeList->SetTotalDataCount(nSize);

    int nItemIndex = 0;
    int nSelectIndex = INVALID_ITEM_INDEX;
    VEC_TREE_ID::iterator iter = m_vecTreeId.begin();

    if (nIndex > 0)
    {
        std::advance(iter, nIndex);
    }

    for (; nItemIndex < nCount && iter != m_vecTreeId.end(); ++iter, ++nItemIndex)
    {
        int nGroupId = *iter;
        CContactGroupNodePtr pGroupNode = (CContactGroupNodePtr)GetTreeDataNode(*iter);

        if (NULL == pGroupNode)
        {
            if (m_bSetting && 0 == nGroupId)
            {
                RefreshAddNewItem(nItemIndex);
                nSelectIndex = nItemIndex;
            }
            continue;
        }

        bool bRefresh = false;

        if (m_bSetting)
        {
            bRefresh = RefreshEditItem(nItemIndex, pGroupNode);
        }
        else
        {
            bRefresh = RefreshNormalItem(nItemIndex, pGroupNode);
        }

        if (bRefresh && pGroupNode->GetId() == m_nUISelectId)
        {
            nSelectIndex = nItemIndex;
        }
    }

    int nItemCount = pTreeList->GetTotalItemCount();

    for (int i = nItemIndex; i < nItemCount; ++i)
    {
        bool bRefresh = false;

        if ((i + 1) == nItemCount)
        {
            bRefresh = true;
        }

        pTreeList->DelItemByIndex(nItemIndex, bRefresh);
    }

    pTreeList->SetItemFocusByIndex(nSelectIndex);
}

void CContactGroupDelegate::RefreshAddNewItem(int nItemIndex)
{
    CTreeList * pTreeList = (CTreeList *)m_pFrameList;

    if (NULL == pTreeList)
    {
        return;
    }

    bool bAdd = false;
    CGroupEditTreeItemPtr pGroupEditTreeItem = (CGroupEditTreeItemPtr)pTreeList->GetItemByIndex(
                nItemIndex);

    if (NULL == pGroupEditTreeItem)
    {
        pGroupEditTreeItem = new CGroupEditTreeItem;

        if (NULL == pGroupEditTreeItem)
        {
            return;
        }
        pGroupEditTreeItem->setObjectName(QString("GroupEditTreeItem%1").arg(nItemIndex));
        bAdd = true;
    }

    pGroupEditTreeItem->ClearContact();

    // CListItem
    pGroupEditTreeItem->SetDataIndex(m_nPageDataIndex + nItemIndex);

    // CTreeItem
    pGroupEditTreeItem->SetHadChildren(false);

    // 目前只有两级，可以这样判断
    pGroupEditTreeItem->SetExpand(false);

    pGroupEditTreeItem->SetLevel(TREE_ITEM_LEVEL_SECOND);
    pGroupEditTreeItem->SetId(0);
    pGroupEditTreeItem->SetAction(this);

    // CGroupTreeItem
    QString strDefaultGroup = LANG_TRANSLATE(TRID_NEW_GROUP);

    pGroupEditTreeItem->SetGroupName(strDefaultGroup);

    // CGroupEditTreeItem
    pGroupEditTreeItem->SetEditable(true);
    pGroupEditTreeItem->SetEditing(true);

    if (bAdd && !pTreeList->AddItem(pGroupEditTreeItem))
    {
        pGroupEditTreeItem->Release();
    }
}

bool CContactGroupDelegate::RefreshNormalItem(int nItemIndex, CContactGroupNodePtr pGroupNode)
{
    CTreeList * pTreeList = (CTreeList *)m_pFrameList;

    if (NULL == pTreeList || NULL == pGroupNode)
    {
        return false;
    }

    bool bAdd = false;
    CGroupTreeItemPtr pGroupTreeItem = (CGroupTreeItemPtr)pTreeList->GetItemByIndex(nItemIndex);

    if (NULL == pGroupTreeItem)
    {
        pGroupTreeItem = new CGroupTreeItem;

        if (NULL == pGroupTreeItem)
        {
            return false;
        }
        pGroupTreeItem->setObjectName(QString("GroupTreeItem%1").arg(nItemIndex));
        bAdd = true;
    }

    if (!UpdateGroupItem(nItemIndex, pGroupNode, pGroupTreeItem))
    {
        if (bAdd)
        {
            pGroupTreeItem->Release();
        }

        return false;
    }

    if (bAdd && !pTreeList->AddItem(pGroupTreeItem))
    {
        pGroupTreeItem->Release();

        return false;
    }

    return true;
}

bool CContactGroupDelegate::RefreshEditItem(int nItemIndex, CContactGroupNodePtr pGroupNode)
{
    CTreeList * pTreeList = (CTreeList *)m_pFrameList;

    if (NULL == pTreeList || NULL == pGroupNode)
    {
        return false;
    }

    bool bAdd = false;
    CGroupEditTreeItemPtr pGroupEditTreeItem = (CGroupEditTreeItemPtr)pTreeList->GetItemByIndex(
                nItemIndex);

    if (NULL == pGroupEditTreeItem)
    {
        pGroupEditTreeItem = new CGroupEditTreeItem;

        if (NULL == pGroupEditTreeItem)
        {
            return false;
        }
        pGroupEditTreeItem->setObjectName(QString("GroupEditTreeItem%1").arg(nItemIndex));
        bAdd = true;
    }

    int nId = pGroupEditTreeItem->GetId();

    if (!UpdateGroupItem(nItemIndex, pGroupNode, pGroupEditTreeItem))
    {
        if (bAdd)
        {
            pGroupEditTreeItem->Release();
        }

        return false;
    }

    // CGroupEditTreeItem
    pGroupEditTreeItem->SetEditable(pGroupNode->IsEditable());

#if IF_FEATURE_GBBOOK
    pGroupEditTreeItem->SetShowBtnRing(pGroupNode->IsShowBtnRing());
#endif

    if (pGroupEditTreeItem->GetId() != nId)
    {
        pGroupEditTreeItem->SetEditing(false);
    }

    if (bAdd && !pTreeList->AddItem(pGroupEditTreeItem))
    {
        pGroupEditTreeItem->Release();

        return false;
    }

    return true;
}

bool CContactGroupDelegate::UpdateGroupItem(int nItemIndex, CContactGroupNodePtr pGroupNode,
        CGroupTreeItem * pGroupTreeItem)
{
    if (NULL == pGroupNode || NULL == pGroupTreeItem)
    {
        return false;
    }

    pGroupTreeItem->ClearContact();

    // CListItem
    pGroupTreeItem->SetDataIndex(m_nPageDataIndex + nItemIndex);

    // CTreeItem
    pGroupTreeItem->SetHadChildren(pGroupNode->IsHadChildren());

    // 目前只有两级，可以这样判断
    pGroupTreeItem->SetExpand(pGroupNode->IsExpanded());

    if (0 != pGroupNode->GetParentId())
    {
        pGroupTreeItem->SetLevel(TREE_ITEM_LEVEL_SECOND);
    }
    else
    {
        pGroupTreeItem->SetLevel(TREE_ITEM_LEVEL_FIRST);
    }

    pGroupTreeItem->SetId(pGroupNode->GetId());
    pGroupTreeItem->SetAction(this);

    // CGroupTreeItem
    pGroupTreeItem->SetGroupName(pGroupNode->GetName());

    return true;
}

void CContactGroupDelegate::OnItemAction(CFrameListBase * pFrameList, CListItem * pItem,
        int nAction)
{
    CTreeList * pTreeList = (CTreeList *)pFrameList;
    CGroupTreeItemPtr pTreeItem = (CGroupTreeItemPtr)pItem;

    if (NULL == pTreeList || NULL == pTreeItem || NULL == pFrameList || pFrameList != m_pFrameList)
    {
        return;
    }

    CGroupDelegatePtr pGroupDelegate = g_DirectoryMgr.GetGroupDelegate();

    if (pGroupDelegate.Get() != this)
    {
        CTreeListDelegate::OnItemAction(pFrameList, pItem, nAction);
        return;
    }

    int nTreeId = pTreeItem->GetId();
    CContactGroupNodePtr pGroupNode = (CContactGroupNodePtr)GetTreeDataNode(nTreeId);

    if (NULL == pGroupNode)
    {
        // 添加群组操作
        if (0 == nTreeId)
        {
            if (LIST_ITEM_ACTION_GROUP_MODIFY == nAction)
            {
                const QString & strGroupName = pTreeItem->GetGroupName();
#if IF_FEATURE_GBBOOK
                if (m_AddGroupDirType == DIRECTORY_TYPE_GENBAND)
                {
                    OnFinishAddGenbandGroup(strGroupName);
                }
                else
                {
                    OnFinishAddLocalGroup(strGroupName);
                }

#else
                OnFinishAddLocalGroup(strGroupName);
#endif
            }
            else if (LIST_ITEM_ACTION_GROUP_DEL == nAction)
            {
                OnDelGroup(pGroupNode, (CGroupEditTreeItem *)pTreeItem);
            }
        }
        return;
    }

    switch (nAction)
    {
    case LIST_ITEM_ACTION_SELECT:
        {
            int nDirType = pGroupNode->GetType();
            int nGroupId = pGroupNode->GetGroupId();

            if (g_DirectoryMgr.IsGroupShow(nDirType, nGroupId)
                    || g_DirectoryMgr.OnGroupClick(nDirType, nGroupId))
            {
                CTreeListDelegate::OnItemAction(pFrameList, pItem, nAction);
            }
            else
            {
                pTreeList->SetItemFocusByIndex(INVALID_ITEM_INDEX);

                int nSelIndex = pTreeList->GetIndexById(m_nUISelectId);

                if (INVALID_ITEM_INDEX != nSelIndex)
                {
                    pTreeList->SetItemFocusByIndex(nSelIndex);
                }

                // LDAP进入搜索界面
                if (DIRECTORY_TYPE_LDAP == nDirType)
                {
                    g_DirectoryMgr.EnterSearch(DIRECTORY_TYPE_LDAP, knInvalidRId, PHONE_KEY_NONE);
                }
            }
        }
        break;
    case LIST_ITEM_ACTION_GROUP_DEL:
        {
            OnDelGroup(pGroupNode, (CGroupEditTreeItem *)pTreeItem);
        }
        break;
    case LIST_ITEM_ACTION_GROUP_MODIFY:
        {
            OnModifyGroup(pGroupNode, (CGroupEditTreeItem *)pTreeItem);
        }
        break;
    case LIST_ITEM_ACTION_GROUP_SET_RING:
        {
            OnSetGroupRing(pGroupNode, (CGroupEditTreeItem *)pTreeItem);
        }
        break;

    default:
        break;
    }
}

void CContactGroupDelegate::OnTitleAction(CFrameList * pFrameList, CFrameListTitle * pTitle,
        int nAction)
{
    switch (nAction)
    {
    case LIST_TITLE_ACTION_DIR_SEARCH:
        {
            g_DirectoryMgr.OnSearchClick();
        }
        break;
    case LIST_TITLE_ACTION_DIR_ADD_CONTACT:
        {
            VEC_CONTACT_GROUP_NODE vecGroupNode;

            GetAdditiveGroup(vecGroupNode, false, DIRECTORY_TYPE_NONE);

            if (!vecGroupNode.empty())
            {
                g_DirectoryMgr.OnNewContactClick();
            }
        }
        break;
    case LIST_TITLE_ACTION_DIR_SETTING:
        {
            g_DirectoryMgr.OnSettingClick();
        }
        break;
    case LIST_TITLE_ACTION_DIR_ADD_GROUP:
        {
#if IF_FEATURE_GBBOOK
            CContactGroupNodePtr pGroupNode = (CContactGroupNodePtr)GetTreeDataNode(m_nUISelectId);
            if (pGroupNode != NULL)
            {
                int nDirType = pGroupNode->GetType();
                if (nDirType == DIRECTORY_TYPE_GENBAND)
                {
                    m_AddGroupDirType = DIRECTORY_TYPE_GENBAND;
                    OnAddGenbandGroup();
                    break;
                }
            }
            m_AddGroupDirType = DIRECTORY_TYPE_LOCAL;
#endif
            if (IsDirTypeValid(DIRECTORY_TYPE_LOCAL))
            {
                // 只能添加到本地
                OnAddLocalGroup();
            }
        }
        break;
    default:
        break;
    }
}

void CContactGroupDelegate::OnDelGroup(CContactGroupNodePtr pGroupNode,
                                       CGroupEditTreeItem * pTreeItem)
{
    if (NULL == pTreeItem)
    {
        return;
    }

    if (NULL == pGroupNode)
    {
        pTreeItem->SetEditing(false);

        // 创建新群组时点击删除
        VEC_TREE_ID::iterator iter = std::find(m_vecTreeId.begin(), m_vecTreeId.end(), 0);

        if (iter != m_vecTreeId.end())
        {
            m_vecTreeId.erase(iter);

            m_nUISelectId = m_nLastSelId;
            ReLocateCurrentPage();
            ReSelectGroup();
        }

        return;
    }

    if (!pTreeItem->IsFocus())
    {
        pTreeItem->SelectItem(true);
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CLocalContactControler * pLocalContact = CLocalContactControler::GetControler(pControler);
    int nGroupTreeId = pGroupNode->GetId();

    if (NULL != pLocalContact)
    {
        pLocalContact->OnAction(DIR_ACTION_LOCAL_DEL_GROUP, -1, nGroupTreeId);
    }
#if IF_FEATURE_GBBOOK
    else
    {
        CGenbandContactControler * pGBContact = CGenbandContactControler::GetControler(pControler);
        if (pGBContact != NULL)
        {
            pGBContact->OnAction(DIR_ACTION_GENBAND_DEL_GROUP, -1, nGroupTreeId);
        }
    }
#endif
}

void CContactGroupDelegate::DelGroup(int nGroupTreeId)
{
    CContactGroupNodePtr pGroupNode = (CContactGroupNodePtr)GetTreeDataNode(nGroupTreeId);

    if (NULL == pGroupNode)
    {
        return;
    }

    int nDirType = pGroupNode->GetType();

#if IF_FEATURE_GBBOOK
    int nGroupId = pGroupNode->GetGroupId();
    int nDefaultGroupId = GetDefaultRecentGroupId(nDirType, nGroupTreeId);

    if (DIRECTORY_TYPE_LOCAL == nDirType)
    {
        CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
        CLocalContactControler * pLocalContact = CLocalContactControler::GetControler(pControler);

        if (NULL != pLocalContact && pLocalContact->ExecAction(DIR_ACTION_LOCAL_DEL_GROUP, -1, nGroupId))
        {
            RefreshLocalGroup(nDefaultGroupId);
        }
    }
    else if (DIRECTORY_TYPE_GENBAND ==  nDirType)
    {
        CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
        CGenbandContactControler * pGBContact = CGenbandContactControler::GetControler(pControler);

        if (NULL != pGBContact)
        {
            //等到结果返回在刷新界面
            pGBContact->ExecAction(DIR_ACTION_GENBAND_DEL_GROUP, -1, nGroupId);
        }
    }
#else
    if (DIRECTORY_TYPE_LOCAL != nDirType)
    {
        return;
    }

    int nGroupId = pGroupNode->GetGroupId();
    int nDefaultGroupId = GetDefaultRecentGroupId(nDirType, nGroupTreeId);

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CLocalContactControler * pLocalContact = CLocalContactControler::GetControler(pControler);

    if (NULL != pLocalContact && pLocalContact->ExecAction(DIR_ACTION_LOCAL_DEL_GROUP, -1, nGroupId))
    {
        RefreshLocalGroup(nDefaultGroupId);
    }
#endif
}

bool CContactGroupDelegate::IsCanAddGroup()
{
#if IF_FEATURE_GBBOOK
    return (NULL != GetTreeDataNode(CONTACT_GROUP_ID_ALL_LOCAL_CONTACT))
           || (NULL != GetTreeDataNode(CONTACT_GROUP_ID_PAB));
#else
    return (NULL != GetTreeDataNode(CONTACT_GROUP_ID_ALL_LOCAL_CONTACT));
#endif
}

void CContactGroupDelegate::OnModifyGroup(CContactGroupNodePtr pGroupNode,
        CGroupEditTreeItem * pTreeItem)
{
    // 修改按钮和完成修改按钮复用
    if (NULL == pGroupNode || NULL == pTreeItem || (!pGroupNode->IsEditable()))
    {
        return;
    }

    bool bEditing = pTreeItem->IsEditing();

    if (!bEditing)
    {
        if (!pTreeItem->IsFocus())
        {
            pTreeItem->SelectItem(true);
        }
        pTreeItem->SetEditing(true);
        return;
    }

#if IF_FEATURE_GBBOOK
    // 保存群组
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CLocalContactControler * pLocalContact = CLocalContactControler::GetControler(pControler);
    CGenbandContactControler * pGBContact = CGenbandContactControler::GetControler(pControler);

    if (NULL != pLocalContact)
    {
        int nGroupId = pGroupNode->GetGroupId();
        ContactGroupData groupData;

        if (!pLocalContact->GetGroupData(nGroupId, groupData))
        {
            return;
        }

        groupData.m_strDisplayName = pTreeItem->GetGroupName().toUtf8().data();

        if (pLocalContact->SaveGroup(nGroupId, groupData))
        {
            pGroupNode->SetName(groupData.m_strDisplayName);
            pTreeItem->SetEditing(false);
        }
        else
        {
            pTreeItem->SetEditing(true);
        }
    }
    else if (NULL != pGBContact)
    {
        int nGroupId = pGroupNode->GetGroupId();

        yl::string strDisplayName = pTreeItem->GetGroupName().toUtf8().data();

        if (pGBContact->SaveGroup(nGroupId, strDisplayName))
        {
            pGroupNode->SetName(strDisplayName);
            pTreeItem->SetEditing(false);
        }
        else
        {
            pTreeItem->SetEditing(true);
        }
    }
#else
    // 保存群组
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CLocalContactControler * pLocalContact = CLocalContactControler::GetControler(pControler);

    if (NULL == pLocalContact)
    {
        return;
    }

    int nGroupId = pGroupNode->GetGroupId();
    ContactGroupData groupData;

    if (!pLocalContact->GetGroupData(nGroupId, groupData))
    {
        return;
    }

    groupData.m_strDisplayName = pTreeItem->GetGroupName().toUtf8().data();

    if (pLocalContact->SaveGroup(nGroupId, groupData))
    {
        pGroupNode->SetName(groupData.m_strDisplayName);
        pTreeItem->SetEditing(false);
    }
    else
    {
        pTreeItem->SetEditing(true);
    }
#endif
    RefreshFrameList();
}

void CContactGroupDelegate::OnSetGroupRing(CContactGroupNodePtr pGroupNode,
        CGroupEditTreeItem * pTreeItem)
{
    if (NULL == pGroupNode || NULL == pTreeItem)
    {
        return;
    }

    if (!pTreeItem->IsFocus())
    {
        pTreeItem->FocusItem(true);
    }

    g_DirectoryMgr.EnterRingSetting(pGroupNode->GetGroupId());
}

void CContactGroupDelegate::OnAddLocalGroup()
{
    CDirectoryControlerPtr pControler = g_DirCtrlFactory.GetControler(DIRECTORY_TYPE_LOCAL);
    CLocalContactControler * pLocalContact = CLocalContactControler::GetControler(pControler);

    if (NULL == pLocalContact)
    {
        return;
    }

    VEC_CONTACT_ID vecGroupId;

    if (!pLocalContact->GetMenuGroupId(vecGroupId))
    {
        return;
    }

    if (vecGroupId.size() >= DIRECTORY_MAX_GROUP_SIZE)
    {
        CDirUIHelper::ShowMsgBox(TRID_GROUP_IS_FULL);
        return;
    }

    // 防止连接多次调用
    VEC_TREE_ID::iterator iter = std::find(m_vecTreeId.begin(), m_vecTreeId.end(), 0);

    if (iter != m_vecTreeId.end())
    {
        return;
    }

    CContactGroupNodePtr pGroupNode = (CContactGroupNodePtr)GetTreeDataNode(
                                          CONTACT_GROUP_ID_ALL_LOCAL_CONTACT);

    if (NULL == pGroupNode)
    {
        return;
    }

    // 切换至本地联系人组
    if (!pGroupNode->IsExpanded())
    {
        ExpandNode(pGroupNode);
    }

    // 加入新的节点
    iter = std::find(m_vecTreeId.begin(), m_vecTreeId.end(), pGroupNode->GetId());

    if (iter == m_vecTreeId.end())
    {
        return;
    }

    ++iter;
    m_vecTreeId.insert(iter, 0);

    m_nLastSelId = m_nUISelectId;
    m_nUISelectId = 0;

    // 刷新界面
    ReLocateCurrentPage();
    ReSelectGroup();
}

#if IF_FEATURE_GBBOOK
void CContactGroupDelegate::OnAddGenbandGroup()
{
    CDirectoryControlerPtr pControler = g_DirCtrlFactory.GetControler(DIRECTORY_TYPE_GENBAND);
    CGenbandContactControler * pGBContact = CGenbandContactControler::GetControler(pControler);

    if (NULL == pGBContact)
    {
        return;
    }

    VEC_CONTACT_ID vecGroupId;

    ContactListData pData;
    if (Dir_GetContactListByGroupId(&pData, kszPabBookGroupId, GB_DIR))
    {
        vecGroupId = pData.m_listId;
    }

    if (vecGroupId.size() >= DIRECTORY_MAX_GROUP_SIZE)
    {
        CDirUIHelper::ShowMsgBox(TRID_GROUP_IS_FULL);
        return;
    }

    // 防止连接多次调用
    VEC_TREE_ID::iterator iter = std::find(m_vecTreeId.begin(), m_vecTreeId.end(), 0);

    if (iter != m_vecTreeId.end())
    {
        return;
    }

    CContactGroupNodePtr pGroupNode = (CContactGroupNodePtr)GetTreeDataNode(CONTACT_GROUP_ID_PAB);

    if (NULL == pGroupNode)
    {
        return;
    }

    // 切换至本地联系人组
    if (!pGroupNode->IsExpanded())
    {
        ExpandNode(pGroupNode);
    }

    // 加入新的节点
    iter = std::find(m_vecTreeId.begin(), m_vecTreeId.end(), pGroupNode->GetId());

    if (iter == m_vecTreeId.end())
    {
        return;
    }

    ++iter;
    m_vecTreeId.insert(iter, 0);

    m_nLastSelId = m_nUISelectId;
    m_nUISelectId = 0;

    // 刷新界面
    ReLocateCurrentPage();
    ReSelectGroup();
}
#endif

void CContactGroupDelegate::OnFinishAddLocalGroup(const QString & strGroupName)
{
    CDirectoryControlerPtr pControler = g_DirCtrlFactory.GetControler(DIRECTORY_TYPE_LOCAL);
    CLocalContactControler * pLocalContact = CLocalContactControler::GetControler(pControler);

    if (NULL == pLocalContact)
    {
        return;
    }

    ContactGroupData itemData;

    itemData.m_strDisplayName = strGroupName.toUtf8().data();

    itemData.m_strRing = "";

    int nGroupId = pLocalContact->AddGroup(itemData);

    DIRUI_INFO("AddGroup return %d", nGroupId);

    if (nGroupId > 0)
    {
        // 删除临时加进去的“0”节点ID
        VEC_TREE_ID::iterator iter = std::find(m_vecTreeId.begin(), m_vecTreeId.end(), 0);

        if (iter != m_vecTreeId.end())
        {
            m_vecTreeId.erase(iter);
        }

        RefreshLocalGroup(nGroupId);
    }
}

#if IF_FEATURE_GBBOOK
void CContactGroupDelegate::OnFinishAddGenbandGroup(const QString & strGroupName)
{
    CDirectoryControlerPtr pControler = g_DirCtrlFactory.GetControler(DIRECTORY_TYPE_GENBAND);
    CGenbandContactControler * pGBContact = CGenbandContactControler::GetControler(pControler);

    if (NULL == pGBContact)
    {
        return;
    }

    yl::string strDisplayName = strGroupName.toUtf8().data();

    int nGroupId = pGBContact->AddGroup(strDisplayName);

    DIRUI_INFO("AddGroup return GB %d", nGroupId);

    if (nGroupId > 0)
    {
        // 删除临时加进去的“0”节点ID
        VEC_TREE_ID::iterator iter = std::find(m_vecTreeId.begin(), m_vecTreeId.end(), 0);

        if (iter != m_vecTreeId.end())
        {
            m_vecTreeId.erase(iter);
        }

        //RefreshGroup(nGroupId);
    }
}
#endif

int CContactGroupDelegate::UpdateLocalGroupRing(int nGroupId, const yl::string & strRing)
{
    CContactGroupNodePtr pGroupNode = (CContactGroupNodePtr)GetGroupNode(DIRECTORY_TYPE_LOCAL,
                                      nGroupId);

    if (NULL == pGroupNode)
    {
        return DIR_RET_FAIL;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CLocalContactControler * pLocalContact = CLocalContactControler::GetControler(pControler);

    if (NULL == pLocalContact)
    {
        return DIR_RET_FAIL;
    }

    ContactGroupData groupData;

    if (!pLocalContact->GetGroupData(nGroupId, groupData))
    {
        return DIR_RET_FAIL;
    }

    groupData.m_strRing = strRing;

    return LocalDir_UpdateGroup(nGroupId, groupData);
}

bool CContactGroupDelegate::GetLocalGroupRing(int nGroupId, yl::string & strRing)
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CLocalContactControler * pLocalContact = CLocalContactControler::GetControler(pControler);

    if (NULL == pLocalContact)
    {
        return false;
    }

    ContactGroupData groupData;

    if (!pLocalContact->GetGroupData(nGroupId, groupData))
    {
        return false;
    }

    strRing = groupData.m_strRing;
    return true;
}

void CContactGroupDelegate::GetAdditiveGroup(VEC_CONTACT_GROUP_NODE & veCContactGroupNode,
        bool bSameType, int nContactType)
{
    veCContactGroupNode.clear();

    MAP_TREE_DATA_NODE::iterator iter = m_mapTreeNode.begin();

    for (; iter != m_mapTreeNode.end(); ++iter)
    {
        CContactGroupNodePtr pGroupNode = (CContactGroupNodePtr)iter->second;

        if (NULL == pGroupNode)
        {
            continue;
        }

        if (bSameType && pGroupNode->GetType() != nContactType)
        {
            continue;
        }

#if IF_FEATURE_GBBOOK
        if (nContactType != DIRECTORY_TYPE_GENBAND && pGroupNode->GetType() == DIRECTORY_TYPE_GENBAND)
        {
            continue;
        }
#endif

        if (pGroupNode->IsAddContact())
        {
            veCContactGroupNode.push_back(pGroupNode);
        }
    }

    std::sort(veCContactGroupNode.begin(), veCContactGroupNode.end(), CompareGroupNode);
}

bool CContactGroupDelegate::CompareGroupNode(CContactGroupNodePtr pGroupNode1,
        CContactGroupNodePtr pGroupNode2)
{
    if (NULL == pGroupNode1 || NULL == pGroupNode2)
    {
        return false;
    }

    return pGroupNode1->GetId() < pGroupNode2->GetId();
}

#if IF_FEATURE_GBBOOK
void CContactGroupDelegate::SetGABGroupIsSelect()
{
    CGroupTreeNodePtr pSelGroupNode = GetGroupNode(DIRECTORY_TYPE_GENBAND, kszGabBookGroupId);
    if (NULL != pSelGroupNode)
    {
        m_nUISelectId = pSelGroupNode->GetId();
    }
}
#endif
