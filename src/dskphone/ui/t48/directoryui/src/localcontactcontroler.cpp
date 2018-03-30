#include "localcontactcontroler.h"
#include "translateiddefine.h"
#ifndef DIR_SUPPORT_SINGLE_UIHELPER
#include "localcontactuihelper.h"
#endif
#ifdef DIR_SUPPORT_BATCH_OPERATE
#include "baseui/t4xpicpath.h"
#include "commonunits/commonunits_def.h"
#include <resource/modresource.h>
#endif

#if IF_FEATURE_FAVORITE_DIR
#include "directoryui/include/moddirectoryui.h"
#endif

#include "configparser/modconfigparser.h"
#include <configiddefine.h>

CLocalContactControler::CLocalContactControler()
    : CCommonDirCtrl(DIRECTORY_TYPE_LOCAL, LOCAL_DIR)
{
}

CLocalContactControler::~CLocalContactControler()
{
}

CLocalContactUIHelper * CLocalContactControler::GetLocalUIHelper()
{
#ifdef DIR_SUPPORT_SINGLE_UIHELPER
    return m_pUIHelper;
#else
    return CLocalContactUIHelper::GetUIHelper(m_pUIHelper);
#endif
}

CLocalContactControler * CLocalContactControler::GetControler(CDirectoryControlerPtr & pControler)
{
    if (pControler.IsEmpty() || DIRECTORY_TYPE_LOCAL != pControler->GetType())
    {
        return NULL;
    }

    return (CLocalContactControler *)pControler.Get();
}

LocalDirContactRecord * CLocalContactControler::GetLocalContact(SmartPtr & pDetail)
{
    if (pDetail.IsEmpty() || DIR_TYPE_LOCAL_CONTACT != pDetail->GetType())
    {
        return NULL;
    }

    return (LocalDirContactRecord *)pDetail.Get();
}

bool CLocalContactControler::IsExitContact(yl::string & strName)
{
    return (Dir_IsContactNameExist(strName, LOCAL_DIR) > 0);
}

#ifdef DIR_SUPPORT_BATCH_OPERATE
void CLocalContactControler::GetCopyDefaultContact(ContactGeneralItemData & itemData)
{
#ifdef PHOTO_SHOW
    static yl::string s_strDefaultPhoto;

    if (s_strDefaultPhoto.empty())
    {
        s_strDefaultPhoto = PIC_CONTACTS_IMAGE_DEFAULT;

        res_toHidePath(s_strDefaultPhoto, PHFILE_TYPE_CONTACT_IMAGE);
    }

    itemData.m_strDefaultPhoto = s_strDefaultPhoto;
#endif

    itemData.m_nLine = AUTO_ACCOUNT_IDENTIFY;
    itemData.m_strRing = TRID_AUTO;
}
#endif

bool CLocalContactControler::OnAddGroup()
{
    CLocalContactUIHelper * pLocalUIHelper = GetLocalUIHelper();

    if (NULL == pLocalUIHelper)
    {
        return false;
    }

    ContactGroupData groupData;

    if (!pLocalUIHelper->GetLocalGroupData(groupData))
    {
        DIRUI_ERR("CDirUIHelper::GetLocalGroupData() fail in OnAddGroup!");
        return false;
    }

    DIRUI_INFO("LocalDir_AddGroup(Name:%s Ring:%s)", groupData.m_strDisplayName.c_str(),
               groupData.m_strRing.c_str());

    int nRetCode = AddGroup(groupData, DIR_ACTION_LOCAL_ADD_GROUP_FINISH);

    return (nRetCode > 0);
}

int CLocalContactControler::AddGroup(ContactGroupData & groupData,
                                     int nAction/* = DIR_ACTION_NONE*/)
{
    if (!CheckGroupDataValid(groupData))
    {
        CLocalContactUIHelper * pLocalUIHelper = GetLocalUIHelper();

        if (NULL != pLocalUIHelper)
        {
            pLocalUIHelper->SetInputFocus(ATTR_NAME);
        }
        return DIR_RET_FAIL;
    }

    int nRetCode = LocalDir_AddGroup(groupData);

    CheckChangeGroupSuccess(nRetCode, nAction);

    return nRetCode;
}

bool CLocalContactControler::OnSaveGroup(int nGroupId)
{
    CLocalContactUIHelper * pLocalUIHelper = GetLocalUIHelper();

    if (NULL == pLocalUIHelper)
    {
        return false;
    }

    ContactGroupData groupData;

    if (!pLocalUIHelper->GetLocalGroupData(groupData))
    {
        DIRUI_ERR("CDirUIHelper::GetLocalGroupData() fail in OnSaveGroup!");
        return false;
    }

    return SaveGroup(nGroupId, groupData, DIR_ACTION_LOCAL_EDIT_GROUP_FINISH);
}

bool CLocalContactControler::SaveGroup(int nGroupId, ContactGroupData & groupData,
                                       int nAction/* = DIR_ACTION_NONE*/)
{
    if (!CheckGroupDataValid(groupData))
    {
        CLocalContactUIHelper * pLocalUIHelper = GetLocalUIHelper();

        if (NULL != pLocalUIHelper)
        {
            pLocalUIHelper->SetInputFocus(ATTR_NAME);
        }

        return false;
    }

    DIRUI_INFO("LocalDir_UpdateGroup(GroupId:%d Name:%s Ring:%s)",
               nGroupId, groupData.m_strDisplayName.c_str(), groupData.m_strRing.c_str());

    int nRetCode = LocalDir_UpdateGroup(nGroupId, groupData);

    return CheckChangeGroupSuccess(nRetCode, nAction);
}

bool CLocalContactControler::GetGroupData(int nGroupId, ContactGroupData & groupData)
{
    SmartPtr pDetail = Dir_GetDetailById(nGroupId, m_eLogicType);

    if (pDetail.IsEmpty() || DIR_TYPE_GROUP != pDetail->GetType())
    {
        return false;
    }

    DirGroupRecord * pGroupData = (DirGroupRecord *)pDetail.Get();

    groupData.m_strDisplayName = pGroupData->GetDisplayName();
    groupData.m_strRing = pGroupData->m_strRing;

    return true;
}

bool CLocalContactControler::OnDelGroup(int nGroupId)
{
    return CDirUIHelper::PopSureMsgBox(TRID_WARNING, TRID_IF_DELETE_SELECT_GROUP,
                                       DIR_ACTION_LOCAL_DEL_GROUP, nGroupId);
}

bool CLocalContactControler::OnClearGroup()
{
    return CDirUIHelper::PopSureMsgBox(TRID_WARNING, TRID_IF_DELETE_ALL_GROUP,
                                       DIR_ACTION_LOCAL_DEL_ALL_GROUP, knInvalidRId);
}

bool CLocalContactControler::OnAddContact()
{
    CLocalContactUIHelper * pLocalUIHelper = GetLocalUIHelper();

    if (NULL == pLocalUIHelper)
    {
        return false;
    }

    int nGroupId = GetGroupId();
    ContactGeneralItemData itemData;

    if (!pLocalUIHelper->GetLocalContactData(nGroupId, itemData))
    {
        DIRUI_ERR("CDirUIHelp::GetLocalContactData() fail in OnAddContact!");
        return false;
    }

#if IF_FEATURE_FAVORITE_DIR
    if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
    {
        yl::string strFavoriteIndex;
        pLocalUIHelper->GetLocalContactFavorite(strFavoriteIndex);
        if (!CheckFavoriteIndexValid(strFavoriteIndex))
        {
            DIRUI_ERR("CDirUIHelp::GetLocalContactFavorite() Index Empty");
            return false;
        }
    }
#endif

    if (!CheckContactValid(itemData))
    {
        return false;
    }

    if (knInvalidRId == nGroupId)
    {
        DIRUI_ERR("Invalid GroupId in OnAddContact");
        return false;
    }

    DIRUI_INFO("LocalDir_AddLocalContact(Name:%s Line:%d Office:%s Mobile:%s Other:%s Ring:%s Group:%d)",
               itemData.m_strDisplayName.c_str(), itemData.m_nLine, itemData.m_strOfficePhoneNumber.c_str(),
               itemData.m_strMobilePhoneNumber.c_str(), itemData.m_strOtherPhoneNumber.c_str(),
               itemData.m_strRing.c_str(), nGroupId);

#if IF_FEATURE_FAVORITE_DIR
    if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1
            && CheckFavoriteExits(itemData.m_nFavoriteIndex))
    {
        return false;
    }
#endif

    int nRetCode = LocalDir_AddLocalContact(itemData, nGroupId);

    DIRUI_INFO("LocalDir_AddLocalContact return %d", nRetCode);

    return CheckChangeContactSuccess(nGroupId, nRetCode, DIR_ACTION_LOCAL_ADD_CONTACT_FINISH);
}

bool CLocalContactControler::OnSaveContact()
{
    CLocalContactUIHelper * pLocalUIHelper = GetLocalUIHelper();

    if (NULL == pLocalUIHelper)
    {
        return false;
    }

    if (knInvalidRId == m_nContactId)
    {
        DIRUI_ERR("Invalid ContactId in OnSaveContact");
        return false;
    }

    SmartPtr pDetail = GetDetail(m_nContactId);
    LocalDirContactRecord * pRecord = GetLocalContact(pDetail);

    if (NULL == pRecord)
    {
        DIRUI_ERR("Can't find Contact when OnSaveContact(%d)", m_nContactId);
        return false;
    }

    int nGroupId = pRecord->m_nParentId;
    ContactGeneralItemData itemData;

    itemData.m_strRing = pRecord->m_strRing;
#ifdef PHOTO_SHOW
    itemData.m_strDefaultPhoto = pRecord->m_strDefaultPhoto;
#endif

    if (!pLocalUIHelper->GetLocalContactData(nGroupId, itemData))
    {
        DIRUI_ERR("CDirUIHelp::GetLocalContactData() fail in OnSaveContact!");
        return false;
    }

#if IF_FEATURE_FAVORITE_DIR
    if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
    {
        yl::string strFavoriteIndex;
        pLocalUIHelper->GetLocalContactFavorite(strFavoriteIndex);
        if (!CheckFavoriteIndexValid(strFavoriteIndex))
        {
            DIRUI_ERR("CDirUIHelp::GetLocalContactFavorite() Index Empty");
            return false;
        }
    }
#endif

    if (knInvalidRId == nGroupId)
    {
        DIRUI_ERR("Invalid GroupId in OnSaveContact");
        return false;
    }

    if (!CheckContactValid(itemData))
    {
        return false;
    }

    DIRUI_INFO("LocalDir_UpdateContact(Name:%s Line:%d Office:%s Mobile:%s Other:%s Ring:%s Group:%d)",
               itemData.m_strDisplayName.c_str(), itemData.m_nLine, itemData.m_strOfficePhoneNumber.c_str(),
               itemData.m_strMobilePhoneNumber.c_str(), itemData.m_strOtherPhoneNumber.c_str(),
               itemData.m_strRing.c_str(), nGroupId);
#if IF_FEATURE_FAVORITE_DIR
    if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1
            && CheckFavoriteExits(itemData.m_nFavoriteIndex, m_nContactId))
    {
        return false;
    }
#endif

    int nRetCode = LocalDir_UpdateContact(itemData, nGroupId, m_nContactId);

    DIRUI_INFO("LocalDir_UpdateContact return %d", nRetCode);

    return CheckChangeContactSuccess(nGroupId, nRetCode, DIR_ACTION_LOCAL_EDIT_CONTACT_FINISH);
}

bool CLocalContactControler::OnDelContact(int nContactId)
{
    return CDirUIHelper::PopSureMsgBox(TRID_WARNING, TRID_IF_DELETE_SELECTED_CONTACT_ITEM,
                                       DIR_ACTION_LOCAL_DEL_CONTACT, nContactId);
}

bool CLocalContactControler::OnClearContact()
{
    return CDirUIHelper::PopSureMsgBox(TRID_WARNING, TRID_IF_DELETE_ALL_CONTACT_ITEMS,
                                       DIR_ACTION_LOCAL_DEL_ALL_CONTACT, knInvalidRId);
}

bool CLocalContactControler::OnMoveToContact(int nContactId)
{
    return CDirUIHelper::PopSureMsgBox(TRID_WARNING, TRID_IF_MOVE_SELECTED_TO_CONTACT,
                                       DIR_ACTION_LOCAL_MOVE_TO_CONTACTS, nContactId);
}

bool CLocalContactControler::OnMoveToBlacklist(int nContactId)
{
    return CDirUIHelper::PopSureMsgBox(TRID_WARNING, TRID_IF_MOVE_SELECTED_TO_BLACK_LIST,
                                       DIR_ACTION_LOCAL_MOVE_TO_BLACKLIST, nContactId);
}

bool CLocalContactControler::OnCopyToContact(bool bOverwrite)
{
    CLocalContactUIHelper * pLocalUIHelper = GetLocalUIHelper();

    if (NULL == pLocalUIHelper)
    {
        return false;
    }

    int nGroupId = GetGroupId();
    ContactGeneralItemData itemData;

    if (!pLocalUIHelper->GetLocalContactData(nGroupId, itemData))
    {
        DIRUI_ERR("CDirUIHelp::GetLocalContactData() fail in OnCopyToContact!");
        return false;
    }
#if IF_FEATURE_FAVORITE_DIR
    if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
    {
        yl::string strFavoriteIndex;
        pLocalUIHelper->GetLocalContactFavorite(strFavoriteIndex);
        if (!CheckFavoriteIndexValid(strFavoriteIndex))
        {
            DIRUI_ERR("CDirUIHelp::GetLocalContactFavorite() Index Empty");
            return false;
        }
    }
#endif

    if (!CheckContactValid(itemData))
    {
        return false;
    }

    if (knInvalidRId == nGroupId)
    {
        DIRUI_ERR("Invalid GroupId(%s) in OnCopyToContact", itemData.m_strGroupName.c_str());
        return false;
    }

    int nRetCode = DIR_RET_SUCC;
    int nContactId = Dir_IsContactNameExist(itemData.m_strDisplayName, LOCAL_DIR);

    if (-1 == nContactId)
    {
        nRetCode = LocalDir_AddLocalContact(itemData, nGroupId);
    }
    else
    {
        if (!bOverwrite)
        {
            return CDirUIHelper::PopSureMsgBox(TRID_WARNING, TRID_IF_OVERWRITE_THE_ORIGINAL_CONTACT,
                                               DIR_ACTION_LOCAL_COPY_TO_CONTACTS_OVERWRITE, nContactId);
        }

        CheckContactCopyNumber(nContactId, itemData);

        nRetCode = LocalDir_UpdateContact(itemData, nGroupId, nContactId);
    }

    return CheckChangeContactSuccess(nGroupId, nRetCode, DIR_ACTION_LOCAL_COPY_CONTACT_FINISH);
}

bool CLocalContactControler::OnCopyToBlacklist(bool bOverwrite)
{
    CLocalContactUIHelper * pLocalUIHelper = GetLocalUIHelper();

    if (NULL == pLocalUIHelper)
    {
        return false;
    }

    int nGroupId = kszBlackListGroupId;
    ContactGeneralItemData itemData;

    if (!pLocalUIHelper->GetLocalContactData(nGroupId, itemData))
    {
        DIRUI_ERR("CDirUIHelp::GetLocalContactData() fail in OnCopyToBlacklist!");
        return false;
    }

    if (!CheckContactValid(itemData))
    {
        return false;
    }

    if (!GetGroupName(nGroupId, itemData.m_strGroupName))
    {
        DIRUI_ERR("GetGroupName(kszBlackListGroupId) fail in OnCopyToBlacklist!");
        return false;
    }

    int nRetCode = DIR_RET_SUCC;
    int nContactId = Dir_IsContactNameExist(itemData.m_strDisplayName, LOCAL_DIR);

    if (-1 == nContactId)
    {
        nRetCode = LocalDir_AddLocalContact(itemData, nGroupId);
    }
    else
    {
        if (!bOverwrite)
        {
            return CDirUIHelper::PopSureMsgBox(TRID_WARNING, TRID_IF_OVERWRITE_THE_ORIGINAL_CONTACT,
                                               DIR_ACTION_LOCAL_COPY_TO_BLACKLIST_OVERWRITE, nContactId);
        }

        CheckContactCopyNumber(nContactId, itemData);

        nRetCode = LocalDir_UpdateContact(itemData, nGroupId, nContactId);
    }

    return CheckChangeContactSuccess(nGroupId, nRetCode, DIR_ACTION_LOCAL_COPY_CONTACT_FINISH);
}

bool CLocalContactControler::OnAction(int nAction, int nDataIndex, int nId)
{
    DIRUI_INFO("CLocalContactControler::OnAction(Action:%d Index:%d Id:%d)",
               nAction, nDataIndex, nId);

    if (CCommonDirCtrl::OnAction(nAction, nDataIndex, nId))
    {
        return true;
    }

    switch (nAction)
    {
    case DIR_ACTION_LOCAL_ENTER_ADD_GROUP:
        {
            VEC_CONTACT_ID vecId;

            if (!Dir_GetConfigMenuIdList(vecId, m_eLogicType))
            {
                return false;
            }

            if (vecId.size() >= DIRECTORY_MAX_GROUP_SIZE)
            {
                CDirUIHelper::ShowMsgBox(TRID_GROUP_IS_FULL);
                return false;
            }

            CDirUIHelper::EnterUI(m_nType, DIR_UI_STATE_ADDITION, nAction, knInvalidRId);
            return true;
        }
        break;
    case DIR_ACTION_LOCAL_ENTER_ADD_CONTACT:
        {
            if (kszBlackListGroupId == GetGroupId())
            {
                if (Dir_GetContactCount(kszBlackListGroupId, m_eLogicType) >= DIRECTORY_MAX_BLACKLIST_SIZE)
                {
                    CDirUIHelper::ShowMsgBox(TRID_BLACKLIST_IS_FULL);
                    return false;
                }
            }
            else
            {
                if (Dir_GetContactCount(kszAllContactId, m_eLogicType) >= DIRECTORY_MAX_CONTACT_SIZE)
                {
                    CDirUIHelper::ShowMsgBox(TRID_CONTACT_FULL);
                    return false;
                }
            }

            CDirUIHelper::EnterUI(m_nType, DIR_UI_STATE_ADDITION, nAction, GetGroupId());
            return true;
        }
        break;
    case DIR_ACTION_LOCAL_ADD_GROUP:
        {
            return OnAddGroup();
        }
        break;
    case DIR_ACTION_LOCAL_EDIT_GROUP:
        {
            return OnSaveGroup(m_nContactId);
        }
        break;
    case DIR_ACTION_LOCAL_DEL_GROUP:
        {
            return OnDelGroup(nId);
        }
        break;
    case DIR_ACTION_LOCAL_DEL_ALL_GROUP:
        {
            return OnClearGroup();
        }
        break;
    case DIR_ACTION_LOCAL_ADD_CONTACT:
        {
            return OnAddContact();
        }
        break;
    case DIR_ACTION_LOCAL_DEL_CONTACT:
        {
            return OnDelContact(nId);
        }
        break;
    case DIR_ACTION_LOCAL_DEL_ALL_CONTACT:
        {
            return OnClearContact();
        }
        break;
    case DIR_ACTION_LOCAL_EDIT_CONTACT:
        {
            return OnSaveContact();
        }
        break;
    case DIR_ACTION_LOCAL_MOVE_TO_CONTACTS:
        {
            return OnMoveToContact(nId);
        }
        break;
    case DIR_ACTION_LOCAL_MOVE_TO_BLACKLIST:
        {
            return OnMoveToBlacklist(nId);
        }
        break;
    case DIR_ACTION_LOCAL_COPY_TO_CONTACTS:
        {
            return OnCopyToContact(false);
        }
        break;
    case DIR_ACTION_LOCAL_COPY_TO_BLACKLIST:
        {
            return OnCopyToBlacklist(false);
        }
        break;
#if IF_FEATURE_FAVORITE_DIR
    case DIR_ACTION_LOCAL_COPY_TO_FAVORITE:
        {
            if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
            {
                return OnCopyToFavorite(nId);
            }
        }
        break;
    case DIR_ACTION_LOCAL_REMOVE_FROM_FAVORITE:
        {
            if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
            {
                return OnRemoveFromFavorite(nId);
            }
        }
        break;
#endif
#ifdef DIR_SUPPORT_BATCH_OPERATE
    case DIR_ACTION_LOCAL_BATCH_DEL_CONTACT:
        {
            if (CDirUIHelper::IsSelectAllContact())
            {
                return CDirUIHelper::PopSureMsgBox(TRID_WARNING, TRID_IF_DELETE_ALL_CONTACT_ITEMS, nAction, nId);
            }
            else
            {
                return CDirUIHelper::PopSureMsgBox(TRID_WARNING, TRID_IF_DELETE_SELECTED_CONTACT_ITEM, nAction,
                                                   nId);
            }
        }
        break;
    case DIR_ACTION_LOCAL_BATCH_MOVE_CONTACT:
        {
            bool bIncludeRoot = false;
            int nGroupId = GetGroupId();

            if (kszBlackListGroupId == nGroupId)
            {
                bIncludeRoot = true;
            }

            return CDirUIHelper::EnterSelectGroup(DIRECTORY_TYPE_LOCAL, nGroupId, bIncludeRoot, nAction);
        }
        break;
    case DIR_ACTION_LOCAL_BATCH_MOVE_TO_BLACKLIST:
        {
            return CDirUIHelper::PopSureMsgBox(TRID_WARNING, TRID_IF_MOVE_SELECTED_TO_BLACK_LIST, nAction, nId);
        }
        break;
#endif
    default:
        {
            DIRUI_ERR("Action:%d is not be Processed in OnAction", nAction);
            return false;
        }
        break;
    }

    return false;
}

bool CLocalContactControler::ExecAction(int nAction, int nDataIndex, int nId)
{
    DIRUI_INFO("CLocalContactControler::ExecAction(Action:%d Index:%d Id:%d)",
               nAction, nDataIndex, nId);

    switch (nAction)
    {
    case DIR_ACTION_LOCAL_DEL_GROUP:
        {
            DeleteGroup(nId);
        }
        break;
    case DIR_ACTION_LOCAL_DEL_ALL_GROUP:
        {
            ClearAllGroup();
        }
        break;
    case DIR_ACTION_LOCAL_DEL_CONTACT:
        {
            DeleteContact(nId);
        }
        break;
    case DIR_ACTION_LOCAL_DEL_ALL_CONTACT:
        {
            ClearAllContact();
        }
        break;
    case DIR_ACTION_LOCAL_MOVE_TO_CONTACTS:
        {
            MoveContact(nId, GetGroupId(), kszAllContactId);
        }
        break;
    case DIR_ACTION_LOCAL_MOVE_TO_BLACKLIST:
        {
            MoveContact(nId, GetGroupId(), kszBlackListGroupId);
        }
        break;
    case DIR_ACTION_LOCAL_COPY_TO_CONTACTS_OVERWRITE:
        {
            OnCopyToContact(true);
        }
        break;
    case DIR_ACTION_LOCAL_COPY_TO_BLACKLIST_OVERWRITE:
        {
            OnCopyToContact(true);
        }
        break;
    case DIR_ACTION_LOCAL_ADD_GROUP_FINISH:
    case DIR_ACTION_LOCAL_EDIT_GROUP_FINISH:
    case DIR_ACTION_LOCAL_COPY_CONTACT_FINISH:
        {
            int nCheckUIState = DIR_UI_STATE_ADDITION;
            int nUIState = GetState();

            if (DIR_ACTION_LOCAL_EDIT_GROUP_FINISH == nAction)
            {
                nCheckUIState = DIR_UI_STATE_DETAIL;
            }

            if (nCheckUIState != nUIState)
            {
                break;
            }

            CDirUIHelper::ExitCurrentUI(nUIState);
        }
        break;
    case DIR_ACTION_LOCAL_ADD_CONTACT_FINISH:
        {
            int nUIState = GetState();

            if (DIR_UI_STATE_ADDITION != nUIState)
            {
                break;
            }

#if IF_FEATURE_FAVORITE_DIR
            bool IsFromDsskeyAdd = false;
            if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
            {
                CLocalContactUIHelper * pLocalUIHelper = GetLocalUIHelper();
                if (NULL != pLocalUIHelper && pLocalUIHelper->IsFromDsskeyAdd())
                {
                    IsFromDsskeyAdd = true;
                }
            }
#endif
            CDirUIHelper::ExitCurrentUI(nUIState);
            RefreshSearchUI();
#if IF_FEATURE_FAVORITE_DIR
            if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1
                    && IsFromDsskeyAdd)
            {
                DM_ReturnToIdle();
            }
#endif
        }
        break;
    case DIR_ACTION_LOCAL_EDIT_CONTACT_FINISH:
        {
            int nContactId = m_nContactId;
            int nGroupId = GetGroupId();
            int nUIState = GetState();

            if (DIR_UI_STATE_DETAIL != nUIState)
            {
                break;
            }

            CDirUIHelper::ExitCurrentUI(nUIState);

            if (!RefreshSearchUI() && kszAllContactId != nGroupId)
            {
                SmartPtr pDetail = Dir_GetDetailById(nContactId, m_eLogicType);

                if (pDetail.IsEmpty() || pDetail->m_nParentId != nGroupId)
                {
                    CDirUIHelper::RefreshUIAfterDeleted(true);
                }
            }
        }
        break;
    case DIR_ACTION_LOCAL_MOVE_CONTACT_FINISH:
        {
            if (!RefreshSearchUI())
            {
                CDirUIHelper::RefreshUIAfterDeleted();
            }
        }
        break;
#if IF_FEATURE_FAVORITE_DIR
    case DIR_ACTION_LOCAL_COPY_TO_FAVORITE:
        {
            if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
            {
                CopyContactToFavorite(nId);
            }
        }
        break;
    case DIR_ACTION_LOCAL_REMOVE_FROM_FAVORITE:
        {
            if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
            {
                RemoveContactFromFavorite(nId);
            }
        }
        break;
#endif
#ifdef DIR_SUPPORT_BATCH_OPERATE
    case DIR_ACTION_LOCAL_BATCH_DEL_CONTACT:
        {
            if (CDirUIHelper::IsSelectAllContact())
            {
                LocalDir_ClearContact(GetGroupId());
            }
            else
            {
                VEC_CONTACT_ID vecContactId;

                if (!CDirUIHelper::GetSelectContactId(vecContactId) || vecContactId.size() <= 0)
                {
                    return false;
                }

                if (!LocalDir_RemoveContacts(vecContactId, GetGroupId()))
                {
                    return false;
                }
            }

            CDirUIHelper::RefreshUI(true, true);
            return true;
        }
        break;
    case DIR_ACTION_LOCAL_BATCH_MOVE_CONTACT:
        {
            int nGroupId = nId;

            if (knInvalidRId == nGroupId)
            {
                return false;
            }

            VEC_CONTACT_ID vecContactId;

            if (!CDirUIHelper::GetSelectContactId(vecContactId) || vecContactId.size() <= 0)
            {
                return false;
            }

            DIRECTORY_RET_CODE retCode = LocalDir_MoveToGroup(vecContactId, nGroupId, GetGroupId());

            CDirUIHelper::RefreshUI(true, true);

            if (DIR_RET_SUCC == retCode)
            {
                if (kszBlackListGroupId == nGroupId)
                {
                    CDirUIHelper::ShowMsgBox(TRID_MOVE_BLACKLIST_SUCCESS);
                }
                else
                {
                    CDirUIHelper::ShowMsgBox(TRID_MOVE_CONTACTS_SUCCESS);
                }
            }
            else if (DIR_RET_GROUP_SIZE_UNEXPECTED == retCode)
            {
                if (kszBlackListGroupId == nGroupId)
                {
                    CDirUIHelper::ShowMsgBox(TRID_ONLY_30_CONTACTS_ALLOWED);
                }
                else
                {
                    CDirUIHelper::ShowMsgBox(TRID_ONLY_1000_CONTACTS_ALLOWED);
                }
            }
            else
            {
                CDirUIHelper::ShowErrorMsg(retCode);
            }

            return true;
        }
        break;
    case DIR_ACTION_LOCAL_BATCH_MOVE_TO_BLACKLIST:
        {
            VEC_CONTACT_ID vecContactId;

            if (!CDirUIHelper::GetSelectContactId(vecContactId) || vecContactId.size() <= 0)
            {
                break;
            }

            DIRECTORY_RET_CODE retCode = LocalDir_MoveToGroup(vecContactId, kszBlackListGroupId, GetGroupId());

            CDirUIHelper::RefreshUI(true, true);

            if (DIR_RET_SUCC == retCode)
            {
                CDirUIHelper::ShowMsgBox(TRID_MOVE_BLACKLIST_SUCCESS);
            }
            else if (DIR_RET_GROUP_SIZE_UNEXPECTED == retCode)
            {
                CDirUIHelper::ShowMsgBox(TRID_ONLY_30_CONTACTS_ALLOWED);
            }
            else
            {
                CDirUIHelper::ShowErrorMsg(retCode);
            }

            return true;
        }
        break;
#endif
    default:
        {
            DIRUI_ERR("Action:%d is not be Processed in ExecAction", nAction);
            return false;
        }
        break;
    }

    return true;
}

bool CLocalContactControler::DialContact(int nContactId, int nAccountId/* = AUTO_ACCOUNT_IDENTIFY*/)
{
    SmartPtr pDetail = GetDetail(nContactId);
    LocalDirContactRecord * pRecord = GetLocalContact(pDetail);

    if (NULL == pRecord)
    {
        DIRUI_ERR("Can't find Contact when DialContact(%d)", nContactId);
        return false;
    }

    m_nContactId = nContactId;

    if (AUTO_ACCOUNT_IDENTIFY == nAccountId)
    {
        nAccountId = pRecord->m_nLine;
    }

    ST_CONTACT_ATTRIBUTE stAttr;
    LIST_ATTRIBUTE lsNumber;

    if (pRecord->GetNumberByName(kszOfficeNumber, stAttr.m_strValue) && !stAttr.m_strValue.empty())
    {
        stAttr.m_strName = TRID_OFFICE_NUMBER;
        lsNumber.push_back(stAttr);
    }

    if (pRecord->GetNumberByName(kszMobileNumber, stAttr.m_strValue) && !stAttr.m_strValue.empty())
    {
        stAttr.m_strName = TRID_MOBILE_NUMBER;
        lsNumber.push_back(stAttr);
    }

    if (pRecord->GetNumberByName(kszOtherNumber, stAttr.m_strValue) && !stAttr.m_strValue.empty())
    {
        stAttr.m_strName = TRID_OTHER_NUMBER;
        lsNumber.push_back(stAttr);
    }

    CDirUIHelper::Dial(nAccountId, pDetail->GetDisplayName(), lsNumber, this, pRecord);

    return true;
}

#ifdef DIR_SUPPORT_SWITCH_DETAIL
bool CLocalContactControler::GetContactImage(int nId, yl::string & strImage) const
{
    SmartPtr pDetail = Dir_GetDetailById(nId, m_eLogicType);
    LocalDirContactRecord * pRecord = GetLocalContact(pDetail);

    if (NULL == pRecord)
    {
        DIRUI_ERR("Can't find Contact when GetContactImage(%d)", nId);
        return false;
    }

    strImage = pRecord->m_strDefaultPhoto;

    res_toRealPath(strImage, PHFILE_TYPE_CONTACT_IMAGE);

    return true;
}
#endif

LRESULT CLocalContactControler::OnMessage(msgObject & refMessage)
{
    DIRUI_INFO("CLocalContactControler::OnMessage() msg:%d wp:%d lp:%d",
               refMessage.message, refMessage.wParam, refMessage.lParam);

    if (LOCAL_SEARCH_RESULT != refMessage.message && (refMessage.message < LOCAL_DIRECTORY_MESSAGE_BEGIN
            || refMessage.message > LOCAL_DIRECTORY_MESSAGE_END))
    {
        return FALSE;
    }

    CLocalContactUIHelper * pLocalUIHelper = GetLocalUIHelper();

    if (NULL == pLocalUIHelper)
    {
        return FALSE;
    }

    LRESULT retValue = FALSE;

    switch (refMessage.message)
    {
    case LOCAL_DIRECTORY_RELOAD:
        {
            CDirUIHelper::ExitAllUI(m_nType);

            retValue = TRUE;
        }
        break;
    case DIRECTORY_SHORTCUT_RELOAD:
        break;
    case LOCAL_DIRECTORY_RELOAD_CONFIG:
        break;
    case LOCAL_SEARCH_RESULT:
        {
            OnSearchResult(refMessage);
            retValue = TRUE;
        }
        break;
    default:
        break;
    }

    return retValue;
}

bool CLocalContactControler::DeleteGroup(int nGroupId)
{
    if (!LocalDir_RemoveGroup(nGroupId))
    {
        DIRUI_INFO("LocalDir_RemoveGroup(%d) fail!", nGroupId);

        CDirUIHelper::ShowMsgBox(TRID_DELETE_FIALED);
        return false;
    }

    CDirUIHelper::RefreshUI(true, true);
    return true;
}

bool CLocalContactControler::ClearAllGroup()
{
    LocalDir_ClearGroup();

    CDirUIHelper::RefreshUI(true, true);
    return true;
}

bool CLocalContactControler::DeleteContact(int nContactId)
{
    VectorId vecId;

    vecId.push_back(nContactId);

#if IF_FEATURE_FAVORITE_DIR
    if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
    {
        bool bSuc;
        if (GetGroupId() == kszFavoriteGroupId)
        {
            bSuc = LocalDir_RemoveContactsIdFromFavorite(vecId, GetGroupId());
        }
        else
        {
            bSuc = LocalDir_RemoveContacts(vecId, GetGroupId());
        }
        if (!bSuc)
        {
            DIRUI_ERR("LocalDir_RemoveContacts(ContactId:%d GroupId:%d) fail!", nContactId, GetGroupId());
        }
    }
    else
#endif
    {
        if (!LocalDir_RemoveContacts(vecId, GetGroupId()))
        {
            DIRUI_ERR("LocalDir_RemoveContacts(ContactId:%d GroupId:%d) fail!", nContactId, GetGroupId());
        }
    }

    CDirUIHelper::RefreshUIAfterDeleted();

    return true;
}

bool CLocalContactControler::ClearAllContact()
{
    LocalDir_ClearContact(GetGroupId());

    CDirUIHelper::RefreshUI(true, true);
    return true;
}

bool CLocalContactControler::MoveContact(int nContactId, int nSrcGroupId, int nTarGroupId)
{
    if (nSrcGroupId == nTarGroupId)
    {
        DIRUI_ERR("MoveContact in same group, ContactId:%d SrcGroupId:%d TarGroupId:%d",
                  nContactId, nSrcGroupId, nTarGroupId);
        return false;
    }

    SmartPtr pDetail = Dir_GetDetailById(nContactId, m_eLogicType);
    LocalDirContactRecord * pRecord = GetLocalContact(pDetail);

    if (NULL == pRecord)
    {
        DIRUI_ERR("Dir_GetDetailById(%d) fail in MoveToContact", nContactId);
        return false;
    }

    ContactGeneralItemData itemData;

    itemData.m_strDisplayName = pRecord->GetDisplayName();
    itemData.m_strRing = pRecord->m_strRing;
    itemData.m_nLine = pRecord->m_nLine;
    itemData.m_strAutoDivert = pRecord->m_strDivert;

#ifdef PHOTO_SHOW
    itemData.m_strDefaultPhoto = pRecord->m_strDefaultPhoto;
    itemData.m_nSelectedPhoto = pRecord->m_nSelectedPhoto;
#endif

    GetGroupName(nTarGroupId, itemData.m_strGroupName);
    pRecord->GetNumberByName(kszOfficeNumber, itemData.m_strOfficePhoneNumber);
    pRecord->GetNumberByName(kszMobileNumber, itemData.m_strMobilePhoneNumber);
    pRecord->GetNumberByName(kszOtherNumber, itemData.m_strOtherPhoneNumber);

#if IF_FEATURE_FAVORITE_DIR
    if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
    {
        if (nTarGroupId == kszBlackListGroupId)
        {
            itemData.m_nFavoriteIndex = -1;
        }
    }
#endif

    int nRetCode = LocalDir_UpdateContact(itemData, nTarGroupId, nContactId);

    return CheckChangeContactSuccess(nTarGroupId, nRetCode, DIR_ACTION_LOCAL_MOVE_CONTACT_FINISH);
}

bool CLocalContactControler::RefreshSearchUI()
{
    if (IsSearchState())
    {
        if (PreSearchContact())
        {
            CDirUIHelper::RefreshUI(true, true);
        }

        return true;
    }

    return false;
}

bool CLocalContactControler::CheckContactCopyNumber(int nContactId,
        ContactGeneralItemData & itemData)
{
    SmartPtr pDetail = Dir_GetDetailById(nContactId, m_eLogicType);
    LocalDirContactRecord * pRecord = GetLocalContact(pDetail);

    if (NULL == pRecord)
    {
        DIRUI_ERR("GetLocalContact fail in CheckContactCopyNumber(%d, %s)",
                  nContactId, itemData.m_strDisplayName.c_str());
        return false;
    }

    if (itemData.m_strOfficePhoneNumber.empty())
    {
        pRecord->GetNumberByName(kszOfficeNumber, itemData.m_strOfficePhoneNumber);
    }

    if (itemData.m_strMobilePhoneNumber.empty())
    {
        pRecord->GetNumberByName(kszMobileNumber, itemData.m_strMobilePhoneNumber);
    }

    if (itemData.m_strOtherPhoneNumber.empty())
    {
        pRecord->GetNumberByName(kszOtherNumber, itemData.m_strOtherPhoneNumber);
    }

    return true;
}

bool CLocalContactControler::CheckGroupDataValid(const ContactGroupData & groupData)
{
    if (IsHadInvalidChar(groupData.m_strDisplayName.c_str()))
    {
        CDirUIHelper::ShowMsgBox(TRID_DISPLAYNAME_CANNNOT_CONTAIN_ILLEGAL_CHAR_AS);
        return false;
    }

    if (groupData.m_strDisplayName.empty())
    {
        CDirUIHelper::ShowMsgBox(TRID_DISPLAY_NAME_CANNOT_BE_EMPTY);
        return false;
    }

    if (0 == commonAPI_StringConvert(groupData.m_strDisplayName,
                                     STRING_CONVERT_TO_LOWER).compare("contacts"))
    {
        CDirUIHelper::ShowMsgBox(TRID_GROUP_NAME_CANNOT_BE_THE_WORD_CONTACTS);
        return false;
    }

    return true;
}

bool CLocalContactControler::CheckChangeGroupSuccess(int nRetCode, int nAction)
{
    if (DIR_RET_GROUP_SIZE_UNEXPECTED == nRetCode)
    {
        CDirUIHelper::ShowMsgBox(TRID_GROUP_IS_FULL);
        return false;
    }
    else if (DIR_RET_GROUP_DUPLICATE == nRetCode)
    {
        CDirUIHelper::ShowMsgBox(TRID_GROUP_ALREAD_EXIST);
        return false;
    }

    CDirUIHelper::ShowMsgBox(TRID_SAVING_FILES, NULL, nAction);
    return true;
}

bool CLocalContactControler::CheckContactDataValid(const ContactGeneralItemData & itemData)
{
    if (IsHadInvalidChar(itemData.m_strDisplayName.c_str()) || itemData.m_strDisplayName.empty())
    {
        return false;
    }

    return true;
}

bool CLocalContactControler::CheckContactValid(const ContactGeneralItemData & itemData)
{
    if (IsHadInvalidChar(itemData.m_strDisplayName.c_str()))
    {
        CDirUIHelper::ShowMsgBox(TRID_DISPLAYNAME_CANNNOT_CONTAIN_ILLEGAL_CHAR_AS);

        CLocalContactUIHelper * pLocalUIHelper = GetLocalUIHelper();

        if (NULL != pLocalUIHelper)
        {
            pLocalUIHelper->SetInputFocus(ATTR_NAME);
        }

        return false;
    }

    if (itemData.m_strDisplayName.empty())
    {
        CDirUIHelper::ShowMsgBox(TRID_DISPLAY_NAME_CANNOT_BE_EMPTY);

        CLocalContactUIHelper * pLocalUIHelper = GetLocalUIHelper();

        if (NULL != pLocalUIHelper)
        {
            pLocalUIHelper->SetInputFocus(ATTR_NAME);
        }

        return false;
    }

    return true;
}

#if IF_FEATURE_FAVORITE_DIR
bool CLocalContactControler::CheckFavoriteIndexValid(yl::string & strIndex)
{
    DIRUI_INFO("CLocalContactControler::CheckFavoriteIndexValid [%s]", strIndex.c_str());
    bool bIsValid = true;
    if (!strIndex.empty())
    {
        if (!commonAPI_IsDigital(strIndex))
        {
            bIsValid = false;
        }
        else
        {
            bIsValid = atoi(strIndex.c_str()) > 0;
        }

        if (!bIsValid)
        {
            CDirUIHelper::ShowMsgBox(TRID_FAVORITE_INDEX_VALID);

            CLocalContactUIHelper * pLocalUIHelper = GetLocalUIHelper();

            if (NULL != pLocalUIHelper)
            {
                pLocalUIHelper->SetInputFocus(ATTR_FAVORITE_INDEX);
            }
        }
    }
    return bIsValid;
}
#endif

bool CLocalContactControler::CheckChangeContactSuccess(int nGroupId, int nRetCode,
        int nAction/* = DIR_ACTION_NONE*/)
{
    if (DIR_RET_GROUP_SIZE_UNEXPECTED == nRetCode)
    {
        if (kszBlackListGroupId == nGroupId)
        {
            CDirUIHelper::ShowMsgBox(TRID_BLACKLIST_IS_FULL);
        }
        else
        {
            CDirUIHelper::ShowMsgBox(TRID_CONTACT_FULL);
        }

        return false;
    }
    else if (DIR_RET_CONTACT_EXIST == nRetCode)
    {
        CDirUIHelper::ShowMsgBox(TRID_CONTACT_EXIST);

        return false;
    }
#if IF_FEATURE_FAVORITE_DIR
    else if (DIR_RET_FAVORITE_INDEX_MAX == nRetCode)
    {
        CDirUIHelper::ShowMsgBox(TRID_FAVORITE_INDEX_NAX);

        return false;
    }
    else if (DIR_RET_FAVORITE_INDEX_EXIST == nRetCode)
    {
        CDirUIHelper::ShowMsgBox(TRID_FAVORITE_INDEX_EXIST);

        return false;
    }
#endif


    CDirUIHelper::ShowMsgBox(TRID_SAVING_FILES, NULL, nAction);
    return true;
}

bool CLocalContactControler::IsHadInvalidChar(const char * pszStr)
{
    // 删除对名字字段的限制 BUG_ID : 92208
    return false;
}

int CLocalContactControler::GetContactAccountLine()
{
    SmartPtr pDetail = GetDetail(m_nContactId);
    LocalDirContactRecord * pRecord = GetLocalContact(pDetail);

    if (NULL == pRecord)
    {
        DIRUI_ERR("Can't find Contact when GetContactAccountLine() ContactId:%d", m_nContactId);
        return AUTO_ACCOUNT_IDENTIFY;
    }

    return pRecord->m_nLine;
}

bool CLocalContactControler::IsContactChanged()
{
    CLocalContactUIHelper * pLocalUIHelper = GetLocalUIHelper();

    if (NULL == pLocalUIHelper)
    {
        return false;
    }

    SmartPtr pDetail = GetDetail(m_nContactId);
    LocalDirContactRecord * pRecord = GetLocalContact(pDetail);

    if (NULL == pRecord)
    {
        DIRUI_ERR("Can't find Contact when IsContactChanged() ContactId:%d", m_nContactId);
        return false;
    }

    int nGroupId = pRecord->m_nParentId;
    ContactGeneralItemData itemData;

    if (!pLocalUIHelper->GetLocalContactData(nGroupId, itemData))
    {
        DIRUI_ERR("CDirUIHelp::GetLocalContactData() fail in OnSaveContact!");
        return false;
    }

    if (nGroupId != pRecord->m_nParentId)
    {
        DIRUI_INFO("contact change, Input GroupId:%d ParentId:%d", nGroupId, pRecord->m_nParentId);
        return true;
    }

    if (itemData.m_strDisplayName != pRecord->GetDisplayName())
    {
        DIRUI_INFO("contact changed, Input Name:%s Name:%s",
                   itemData.m_strDisplayName.c_str(), pRecord->GetDisplayName().c_str());
        return true;
    }

    yl::string strOfficeNo;

    pRecord->GetNumberByName(kszOfficeNumber, strOfficeNo);

    if (itemData.m_strOfficePhoneNumber != strOfficeNo)
    {
        DIRUI_INFO("Contact changed, Input OfficeNo:%s OfficeNo:%s",
                   itemData.m_strOfficePhoneNumber.c_str(), strOfficeNo.c_str());
        return true;
    }

    yl::string strMobileNo;

    pRecord->GetNumberByName(kszMobileNumber, strMobileNo);

    if (itemData.m_strMobilePhoneNumber != strMobileNo)
    {
        DIRUI_INFO("Contact changed, Input MobileNo:%s MobileNo:%s",
                   itemData.m_strMobilePhoneNumber.c_str(), strMobileNo.c_str());
        return true;
    }

    yl::string strOtherNo;

    pRecord->GetNumberByName(kszOtherNumber, strOtherNo);

    if (itemData.m_strOtherPhoneNumber != strOtherNo)
    {
        DIRUI_INFO("Contact changed, Input OtherNo:%s OtherNo:%s",
                   itemData.m_strOtherPhoneNumber.c_str(), strOtherNo.c_str());
        return true;
    }

    if (itemData.m_nLine != pRecord->m_nLine)
    {
        DIRUI_INFO("Contact changed, Input Line:%d Line:%d",
                   itemData.m_nLine, pRecord->m_nLine);
        return true;
    }

    if (kszBlackListGroupId == nGroupId)
    {
        return false;
    }

    if (!pLocalUIHelper->IsSameContactRing(itemData.m_strRing, pRecord->m_strRing))
    {
        DIRUI_INFO("Contact changed, Input Ring:%s Ring:%s",
                   itemData.m_strRing.c_str(), pRecord->m_strRing.c_str());
        return true;
    }

#ifdef PHOTO_SHOW
    if (!pLocalUIHelper->IsSameContactPhoto(itemData.m_strDefaultPhoto, pRecord->m_strDefaultPhoto))
    {
        DIRUI_INFO("Contact changed, Input Photo:%s Photo:%s",
                   itemData.m_strDefaultPhoto.c_str(), pRecord->m_strDefaultPhoto.c_str());
        return true;
    }
#endif

#if IF_FEATURE_FAVORITE_DIR
    if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1
            && itemData.m_nFavoriteIndex != pRecord->m_nFavoriteIndex)
    {
        return true;
    }
#endif

    return false;
}
#if IF_FEATURE_FAVORITE_DIR
bool CLocalContactControler::OnCopyToFavorite(int nContactId)
{
    int nIndex = LocalDir_GetFavoriteIndex();
    yl::string strIndex = commonAPI_FormatString("%d ?", nIndex);
    return CDirUIHelper::PopSureMsgBox(TRID_WARNING, TRID_LOCAL_COPY_TO_FAVORITE,
                                       DIR_ACTION_LOCAL_COPY_TO_FAVORITE, nContactId, NULL, strIndex.c_str());
}

bool CLocalContactControler::OnRemoveFromFavorite(int nContactId)
{
    return CDirUIHelper::PopSureMsgBox(TRID_WARNING, TRID_LOCAL_REMOVE_FROM_FAVORITE,
                                       DIR_ACTION_LOCAL_REMOVE_FROM_FAVORITE, nContactId);
}

bool CLocalContactControler::CopyContactToFavorite(int nContactId)
{
    SmartPtr pDetail = Dir_GetDetailById(nContactId, m_eLogicType);
    LocalDirContactRecord * pRecord = GetLocalContact(pDetail);

    if (NULL == pRecord)
    {
        DIRUI_ERR("Dir_GetDetailById(%d) fail in CopyContact", nContactId);
        return false;
    }

    int nRetCode = LocalDir_CopyContactToFavorite(nContactId);

    return CheckChangeContactSuccess(kszFavoriteGroupId, nRetCode,
                                     DIR_ACTION_LOCAL_MOVE_CONTACT_FINISH);
}

bool CLocalContactControler::RemoveContactFromFavorite(int nContactId)
{
    SmartPtr pDetail = Dir_GetDetailById(nContactId, m_eLogicType);
    LocalDirContactRecord * pRecord = GetLocalContact(pDetail);

    if (NULL == pRecord)
    {
        DIRUI_ERR("Dir_GetDetailById(%d) fail in CopyContact", nContactId);
        return false;
    }

    VectorId vecId;
    vecId.push_back(nContactId);
    bool bSuc = LocalDir_RemoveContactsIdFromFavorite(vecId, GetGroupId());

    CheckChangeContactSuccess(kszFavoriteGroupId, bSuc ? DIR_RET_SUCC : DIR_RET_FAIL,
                              DIR_ACTION_LOCAL_MOVE_CONTACT_FINISH);
    CDirUIHelper::RefreshUIAfterDeleted();
    return bSuc;
}

bool CLocalContactControler::CheckFavoriteExits(int nIndex, int nContactID /*= 0*/)
{
    DIRUI_INFO("CLocalContactControler::CheckFavoriteExits : %d", nIndex);
    if (nIndex > 0)
    {
        bool bIndexExist = LocalDir_IsFavoriteIndexExist(nIndex, nContactID);
        if (bIndexExist)
        {
            CDirUIHelper::ShowMsgBox(TRID_FAVORITE_INDEX_EXIST);
            return true;
        }
    }
    return false;
}

#endif
