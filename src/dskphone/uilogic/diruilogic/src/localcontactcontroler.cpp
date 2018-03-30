#include "localcontactcontroler.h"
#include "translateiddefine.h"
#include "ilocalcontactuihelper.h"
#include "diruiadapter.h"
#ifdef DIR_SUPPORT_BATCH_OPERATE
#include "baseui/t4xpicpath.h"
#include "commonunits/commonunits_def.h"
#include <resource/modresource.h>
#endif
#if IF_FEATURE_FAVORITE_DIR
#include "uilogiccommon/include/uilogiclanghelper.h"
#endif


CLocalContactControler::CLocalContactControler()
    : CCommonDirCtrl(DIRECTORY_TYPE_LOCAL, LOCAL_DIR)
{
}

CLocalContactControler::~CLocalContactControler()
{
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

bool CLocalContactControler::IsGroupData(SmartPtr & pDetail)
{
    if (pDetail.IsEmpty())
    {
        return false;
    }

    if (DIR_TYPE_LOCAL_GROUP == pDetail->GetType())
    {
        return true;
    }

    return CCommonDirCtrl::IsGroupData(pDetail);
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
    ILocalContactUIHelper * pLocalUIHelper = g_DirUIAdapter.GetLocalContactUIHelper(m_pUIHelper);

    if (NULL == pLocalUIHelper)
    {
        return false;
    }

    ContactGroupData groupData;

    if (!pLocalUIHelper->GetLocalGroupData(groupData))
    {
        DIRUI_ERR("ILocalContactUIHelper::GetLocalGroupData() fail in OnAddGroup!");
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
        return DIR_RET_FAIL;
    }

    int nRetCode = LocalDir_AddGroup(groupData);

    CheckChangeGroupSuccess(nRetCode, nAction);

    return nRetCode;
}

bool CLocalContactControler::OnSaveGroup(int nGroupId)
{
    ILocalContactUIHelper * pLocalUIHelper = g_DirUIAdapter.GetLocalContactUIHelper(m_pUIHelper);

    if (NULL == pLocalUIHelper)
    {
        return false;
    }

    ContactGroupData groupData;

    if (!pLocalUIHelper->GetLocalGroupData(groupData))
    {
        DIRUI_ERR("ILocalContactUIHelper::GetLocalGroupData() fail in OnSaveGroup!");
        return false;
    }

    return SaveGroup(nGroupId, groupData, DIR_ACTION_LOCAL_EDIT_GROUP_FINISH);
}

bool CLocalContactControler::SaveGroup(int nGroupId, ContactGroupData & groupData,
                                       int nAction/* = DIR_ACTION_NONE*/)
{
    if (!CheckGroupDataValid(groupData))
    {
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
    return g_DirUIAdapter.PopSureMsgBox(TRID_WARNING, TRID_IF_DELETE_SELECT_GROUP,
                                        DIR_ACTION_LOCAL_DEL_GROUP, nGroupId);
}

bool CLocalContactControler::OnClearGroup()
{
    return g_DirUIAdapter.PopSureMsgBox(TRID_WARNING, TRID_IF_DELETE_ALL_GROUP,
                                        DIR_ACTION_LOCAL_DEL_ALL_GROUP, knInvalidRId);
}

bool CLocalContactControler::OnAddContact()
{
    ILocalContactUIHelper * pLocalUIHelper = g_DirUIAdapter.GetLocalContactUIHelper(m_pUIHelper);

    if (NULL == pLocalUIHelper)
    {
        return false;
    }

#if IF_FEATURE_FAVORITE_DIR
    if (!CheckFavoriteIndexValid())
    {
        return false;
    }
#endif

    int nGroupId = GetGroupId();
    ContactGeneralItemData itemData;

    if (!pLocalUIHelper->GetLocalContactData(nGroupId, itemData))
    {
        DIRUI_ERR("CDirUIHelp::GetLocalContactData() fail in OnAddContact!");
        return false;
    }

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

    int nRetCode = LocalDir_AddLocalContact(itemData, nGroupId);

    DIRUI_INFO("LocalDir_AddLocalContact return %d", nRetCode);

    return CheckChangeContactSuccess(nGroupId, nRetCode, DIR_ACTION_LOCAL_ADD_CONTACT_FINISH);
}

bool CLocalContactControler::OnSaveContact()
{
    ILocalContactUIHelper * pLocalUIHelper = g_DirUIAdapter.GetLocalContactUIHelper(m_pUIHelper);

    if (NULL == pLocalUIHelper)
    {
        return false;
    }

    if (knInvalidRId == m_nContactId)
    {
        DIRUI_ERR("Invalid ContactId in OnSaveContact");
        return false;
    }

#if IF_FEATURE_FAVORITE_DIR
    if (!CheckFavoriteIndexValid())
    {
        return false;
    }
#endif

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
    itemData.m_strAutoDivert = pRecord->m_strDivert;
#if IF_FEATURE_FAVORITE_DIR
    itemData.m_nFavoriteIndex = pRecord->m_nFavoriteIndex;
#endif

    if (!pLocalUIHelper->GetLocalContactData(nGroupId, itemData))
    {
        DIRUI_ERR("CDirUIHelp::GetLocalContactData() fail in OnSaveContact!");
        return false;
    }

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

    int nRetCode = LocalDir_UpdateContact(itemData, nGroupId, m_nContactId);

    DIRUI_INFO("LocalDir_UpdateContact return %d", nRetCode);

    return CheckChangeContactSuccess(nGroupId, nRetCode, DIR_ACTION_LOCAL_EDIT_CONTACT_FINISH);
}

bool CLocalContactControler::OnDelContact(int nContactId)
{
    return g_DirUIAdapter.PopSureMsgBox(TRID_WARNING, TRID_IF_DELETE_SELECTED_CONTACT_ITEM,
                                        DIR_ACTION_LOCAL_DEL_CONTACT, nContactId);
}

bool CLocalContactControler::OnClearContact()
{
    return g_DirUIAdapter.PopSureMsgBox(TRID_WARNING, TRID_IF_DELETE_ALL_CONTACT_ITEMS,
                                        DIR_ACTION_LOCAL_DEL_ALL_CONTACT, knInvalidRId);
}

bool CLocalContactControler::OnMoveToContact(int nContactId)
{
    return g_DirUIAdapter.PopSureMsgBox(TRID_WARNING, TRID_IF_MOVE_SELECTED_TO_CONTACT,
                                        DIR_ACTION_LOCAL_MOVE_TO_CONTACTS, nContactId);
}

bool CLocalContactControler::OnMoveToBlacklist(int nContactId)
{
    return g_DirUIAdapter.PopSureMsgBox(TRID_WARNING, TRID_IF_MOVE_SELECTED_TO_BLACK_LIST,
                                        DIR_ACTION_LOCAL_MOVE_TO_BLACKLIST, nContactId);
}

bool CLocalContactControler::OnCopyToContact(bool bOverwrite)
{
    ILocalContactUIHelper * pLocalUIHelper = g_DirUIAdapter.GetLocalContactUIHelper(m_pUIHelper);

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
            return g_DirUIAdapter.PopSureMsgBox(TRID_WARNING, TRID_IF_OVERWRITE_THE_ORIGINAL_CONTACT,
                                                DIR_ACTION_LOCAL_COPY_TO_CONTACTS_OVERWRITE, nContactId);
        }

        CheckContactCopyNumber(nContactId, itemData);

        nRetCode = LocalDir_UpdateContact(itemData, nGroupId, nContactId);
    }

    return CheckChangeContactSuccess(nGroupId, nRetCode, DIR_ACTION_LOCAL_COPY_CONTACT_FINISH);
}

bool CLocalContactControler::OnCopyToBlacklist(bool bOverwrite)
{
    ILocalContactUIHelper * pLocalUIHelper = g_DirUIAdapter.GetLocalContactUIHelper(m_pUIHelper);

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
            return g_DirUIAdapter.PopSureMsgBox(TRID_WARNING, TRID_IF_OVERWRITE_THE_ORIGINAL_CONTACT,
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

    switch (nAction)
    {
    case DIR_ACTION_COMMON_DETAIL:
        {
            if (IsGroupId(nId))
            {
                EnterDetail(nId, nDataIndex, DIR_ACTION_LOCAL_EDIT_GROUP);
            }
            else
            {
                EnterDetail(nId, nDataIndex, DIR_ACTION_LOCAL_EDIT_CONTACT);
            }

            return true;
        }
        break;
    case DIR_ACTION_LOCAL_ENTER_ADD_GROUP:
        {
            VEC_CONTACT_ID vecId;

            if (!Dir_GetConfigMenuIdList(vecId, m_eLogicType))
            {
                return false;
            }

            if (vecId.size() >= DIRECTORY_MAX_GROUP_SIZE)
            {
                g_DirUIAdapter.ShowMsgBox(TRID_GROUP_IS_FULL);
                return false;
            }

            LIST_ATTRIBUTE lsAttr;

            SetAttributeList(DIRECTORY_TYPE_NONE, lsAttr);

            g_DirUIAdapter.EnterUI(m_nType, DIR_UI_STATE_ADDITION, DIR_ACTION_LOCAL_ADD_GROUP, knInvalidRId);
            return true;
        }
        break;
    case DIR_ACTION_LOCAL_ENTER_ADD_CONTACT:
        {
            if (kszBlackListGroupId == GetGroupId())
            {
                if (Dir_GetContactCount(kszBlackListGroupId, m_eLogicType) >= DIRECTORY_MAX_BLACKLIST_SIZE)
                {
                    g_DirUIAdapter.ShowMsgBox(TRID_BLACKLIST_IS_FULL);
                    return false;
                }
            }
            else
            {
                if (Dir_GetContactCount(kszAllContactId, m_eLogicType) >= DIRECTORY_MAX_CONTACT_SIZE)
                {
                    g_DirUIAdapter.ShowMsgBox(TRID_CONTACT_FULL);
                    return false;
                }
            }

            LIST_ATTRIBUTE lsAttr;

            SetAttributeList(DIRECTORY_TYPE_NONE, lsAttr);

            g_DirUIAdapter.EnterUI(m_nType, DIR_UI_STATE_ADDITION, DIR_ACTION_LOCAL_ADD_CONTACT, GetGroupId());
            return true;
        }
        break;
    case DIR_ACTION_LOCAL_ENTER_CHOOSE_PHOTO:
        {
            g_DirUIAdapter.EnterUI(m_nType, DIR_UI_STATE_CHOOSE_PHOTO, nAction, GetContactId());

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
#ifdef IF_FEATURE_FAVORITE_DIR
    case DIR_ACTION_LOCAL_COPY_TO_FAVORITE:
        {
            return OnCopyToFavorite(nId);
        }
        break;
    case DIR_ACTION_LOCAL_REMOVE_FROM_FAVORITE:
        {
            return OnRemoveFromFavorite(nId);
        }
        break;
#endif
#ifdef DIR_SUPPORT_BATCH_OPERATE
    case DIR_ACTION_LOCAL_BATCH_DEL_CONTACT:
        {
            IDirUIHelper * pUIHelper = m_pUIHelper.Get();

            if (NULL != pUIHelper && pUIHelper->IsSelectAllContact())
            {
                return g_DirUIAdapter.PopSureMsgBox(TRID_WARNING, TRID_IF_DELETE_ALL_CONTACT_ITEMS, nAction, nId);
            }
            else
            {
                return g_DirUIAdapter.PopSureMsgBox(TRID_WARNING, TRID_IF_DELETE_SELECTED_CONTACT_ITEM, nAction,
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

            IDirUIHelper * pUIHelper = m_pUIHelper.Get();

            if (NULL == pUIHelper)
            {
                return false;
            }

            return pUIHelper->EnterSelectGroup(DIRECTORY_TYPE_LOCAL, nGroupId, bIncludeRoot, nAction);
        }
        break;
    case DIR_ACTION_LOCAL_BATCH_MOVE_TO_BLACKLIST:
        {
            return g_DirUIAdapter.PopSureMsgBox(TRID_WARNING, TRID_IF_MOVE_SELECTED_TO_BLACK_LIST, nAction,
                                                nId);
        }
        break;
#endif
    default:
        break;
    }

    if (CCommonDirCtrl::OnAction(nAction, nDataIndex, nId))
    {
        return true;
    }

    DIRUI_ERR("Action:%d is not be Processed in OnAction", nAction);
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

            if (!g_DirUIAdapter.CheckState(nCheckUIState, nUIState))
            {
                break;
            }

            g_DirUIAdapter.ExitCurrentUI(nUIState);
        }
        break;
    case DIR_ACTION_LOCAL_ADD_CONTACT_FINISH:
        {
            int nUIState = GetState();

            if (!g_DirUIAdapter.CheckState(DIR_UI_STATE_ADDITION, nUIState))
            {
                break;
            }

            g_DirUIAdapter.ExitCurrentUI(nUIState);
            RefreshSearchUI();
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

            g_DirUIAdapter.ExitCurrentUI(nUIState, false);

            if (!RefreshSearchUI() && kszAllContactId != nGroupId)
            {
                SmartPtr pDetail = Dir_GetDetailById(nContactId, m_eLogicType);

                if (!m_pUIHelper.IsEmpty() && (pDetail.IsEmpty()
                                               || pDetail->m_nParentId != nGroupId))
                {
                    m_pUIHelper->RefreshUIAfterDeleted(true);
                    break;
                }
            }

            if (!m_pUIHelper.IsEmpty())
            {
                m_pUIHelper->RefreshUI(DIR_UI_REFRESH_TYPE_LIST_VIEW, DIR_UI_REFRESH_PARAM_NONE);
            }
        }
        break;
    case DIR_ACTION_LOCAL_MOVE_CONTACT_FINISH:
        {
            IDirUIHelperPtr pUIHelper = m_pUIHelper;

            if (!pUIHelper.IsEmpty() && !RefreshSearchUI())
            {
                pUIHelper->RefreshUIAfterDeleted();
            }
        }
        break;
#ifdef IF_FEATURE_FAVORITE_DIR
    case DIR_ACTION_LOCAL_COPY_TO_FAVORITE:
        {
            CopyContactToFavorite(nId);
        }
        break;
    case DIR_ACTION_LOCAL_REMOVE_FROM_FAVORITE:
        {
            RemoveContactFromFavorite(nId);
        }
        break;
    case DIR_ACTION_LOCAL_EDIT_FAVORITE:
        {
            //进入编辑界面，焦点在favorite
            m_nContactId = nId;
            g_DirUIAdapter.EnterUI(m_nType, DIR_UI_STATE_DETAIL, DIR_ACTION_LOCAL_EDIT_CONTACT, nId);
            ILocalContactUIHelper* pLocalUIHelper = g_DirUIAdapter.GetLocalContactUIHelper(m_pUIHelper);
            if (NULL != pLocalUIHelper)
            {
                pLocalUIHelper->SetInputFocus(DIR_ATTR_FAVORITE_INDEX);
            }

            return true;
        }
        break;
#endif
#ifdef DIR_SUPPORT_BATCH_OPERATE
    case DIR_ACTION_LOCAL_BATCH_DEL_CONTACT:
        {
            IDirUIHelper * pUIHelper = m_pUIHelper.Get();

            if (NULL == pUIHelper)
            {
                return false;
            }

            if (pUIHelper->IsSelectAllContact())
            {
                LocalDir_ClearContact(GetGroupId());
            }
            else
            {
                VEC_CONTACT_ID vecContactId;

                if (!pUIHelper->GetSelectContactId(vecContactId) || vecContactId.size() <= 0)
                {
                    return false;
                }

                if (!LocalDir_RemoveContacts(vecContactId, GetGroupId()))
                {
                    return false;
                }
            }

            pUIHelper->RefreshUI(DIR_UI_REFRESH_TYPE_LIST_VIEW | DIR_UI_REFRESH_TYPE_TITLE,
                                 DIR_UI_REFRESH_PARAM_ALL);
            return true;
        }
        break;
    case DIR_ACTION_LOCAL_BATCH_MOVE_CONTACT:
        {
            IDirUIHelper * pUIHelper = m_pUIHelper.Get();

            if (NULL == pUIHelper)
            {
                return false;
            }

            int nGroupId = nId;

            if (knInvalidRId == nGroupId)
            {
                return false;
            }

            VEC_CONTACT_ID vecContactId;

            if (!pUIHelper->GetSelectContactId(vecContactId) || vecContactId.size() <= 0)
            {
                return false;
            }

            DIRECTORY_RET_CODE retCode = LocalDir_MoveToGroup(vecContactId, nGroupId, GetGroupId());

            pUIHelper->RefreshUI(DIR_UI_REFRESH_TYPE_LIST_VIEW | DIR_UI_REFRESH_TYPE_TITLE,
                                 DIR_UI_REFRESH_PARAM_ALL);

            if (DIR_RET_SUCC == retCode)
            {
                if (kszBlackListGroupId == nGroupId)
                {
                    g_DirUIAdapter.ShowMsgBox(TRID_MOVE_BLACKLIST_SUCCESS);
                }
                else
                {
                    g_DirUIAdapter.ShowMsgBox(TRID_MOVE_CONTACTS_SUCCESS);
                }
            }
            else if (DIR_RET_GROUP_SIZE_UNEXPECTED == retCode)
            {
                if (kszBlackListGroupId == nGroupId)
                {
                    g_DirUIAdapter.ShowMsgBox(TRID_ONLY_30_CONTACTS_ALLOWED);
                }
                else
                {
                    g_DirUIAdapter.ShowMsgBox(TRID_ONLY_1000_CONTACTS_ALLOWED);
                }
            }
            else
            {
                CCommonDirCtrl::ShowErrorMsg(retCode);
            }

            return true;
        }
        break;
    case DIR_ACTION_LOCAL_BATCH_MOVE_TO_BLACKLIST:
        {
            IDirUIHelper * pUIHelper = m_pUIHelper.Get();

            if (NULL == pUIHelper)
            {
                return false;
            }

            VEC_CONTACT_ID vecContactId;

            if (!pUIHelper->GetSelectContactId(vecContactId) || vecContactId.size() <= 0)
            {
                break;
            }

            DIRECTORY_RET_CODE retCode = LocalDir_MoveToGroup(vecContactId, kszBlackListGroupId, GetGroupId());

            pUIHelper->RefreshUI(DIR_UI_REFRESH_TYPE_LIST_VIEW | DIR_UI_REFRESH_TYPE_TITLE,
                                 DIR_UI_REFRESH_PARAM_ALL);

            if (DIR_RET_SUCC == retCode)
            {
                g_DirUIAdapter.ShowMsgBox(TRID_MOVE_BLACKLIST_SUCCESS);
            }
            else if (DIR_RET_GROUP_SIZE_UNEXPECTED == retCode)
            {
                g_DirUIAdapter.ShowMsgBox(TRID_ONLY_30_CONTACTS_ALLOWED);
            }
            else
            {
                CCommonDirCtrl::ShowErrorMsg(retCode);
            }

            return true;
        }
        break;
#endif
    case DIR_ACTION_NAME_INVALID_TIP:
        {
            ILocalContactUIHelper * pLocalUIHelper = g_DirUIAdapter.GetLocalContactUIHelper(m_pUIHelper);

            if (NULL != pLocalUIHelper)
            {
                pLocalUIHelper->SetInputFocus(DIR_ATTR_NAME);
            }
        }
        break;
    case DIR_ACTION_NUMBER_INVALID_TIP:
        {
            ILocalContactUIHelper * pLocalUIHelper = g_DirUIAdapter.GetLocalContactUIHelper(m_pUIHelper);

            if (NULL != pLocalUIHelper)
            {
                pLocalUIHelper->SetInputFocus(DIR_ATTR_NUMBER);
            }
        }
        break;
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

    Dial(nAccountId, pDetail->GetDisplayName(), lsNumber, GetDialAction());

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

    ILocalContactUIHelper * pLocalUIHelper = g_DirUIAdapter.GetLocalContactUIHelper(m_pUIHelper);

    if (NULL == pLocalUIHelper)
    {
        return FALSE;
    }

    LRESULT retValue = FALSE;

    switch (refMessage.message)
    {
    case LOCAL_DIRECTORY_RELOAD:
        {
            g_DirUIAdapter.ExitAllUI(m_nType);

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

        g_DirUIAdapter.ShowMsgBox(TRID_DELETE_FIALED);
        return false;
    }

    if (!m_pUIHelper.IsEmpty())
    {
        m_pUIHelper->RefreshUI(DIR_UI_REFRESH_TYPE_LIST_VIEW | DIR_UI_REFRESH_TYPE_TITLE,
                               DIR_UI_REFRESH_PARAM_ALL);
    }

    return true;
}

bool CLocalContactControler::ClearAllGroup()
{
    LocalDir_ClearGroup();

    if (!m_pUIHelper.IsEmpty())
    {
        m_pUIHelper->RefreshUI(DIR_UI_REFRESH_TYPE_LIST_VIEW | DIR_UI_REFRESH_TYPE_TITLE,
                               DIR_UI_REFRESH_PARAM_ALL);
    }

    return true;
}

bool CLocalContactControler::DeleteContact(int nContactId)
{
    VectorId vecId;

    vecId.push_back(nContactId);

    bool bSuc = false;
    int nGroupID = GetGroupId();
#ifdef IF_FEATURE_FAVORITE_DIR
    if (nGroupID == kszFavoriteGroupId)
    {
        bSuc = LocalDir_RemoveContactsIdFromFavorite(vecId, GetGroupId());
    }
    else
#endif
    {
        bSuc = LocalDir_RemoveContacts(vecId, nGroupID);
    }

    if (!bSuc)
    {
        DIRUI_ERR("LocalDir_RemoveContacts(ContactId:%d GroupId:%d) fail!", nContactId, nGroupID);
    }

    if (!m_pUIHelper.IsEmpty())
    {
        m_pUIHelper->RefreshUIAfterDeleted();
    }

    return true;
}

bool CLocalContactControler::ClearAllContact()
{
    LocalDir_ClearContact(GetGroupId());

    if (!m_pUIHelper.IsEmpty())
    {
        m_pUIHelper->RefreshUI(DIR_UI_REFRESH_TYPE_LIST_VIEW | DIR_UI_REFRESH_TYPE_TITLE,
                               DIR_UI_REFRESH_PARAM_ALL);
    }

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

#ifdef PHOTO_SHOW
    itemData.m_strDefaultPhoto = pRecord->m_strDefaultPhoto;
    itemData.m_nSelectedPhoto = pRecord->m_nSelectedPhoto;
#endif

    GetGroupName(nTarGroupId, itemData.m_strGroupName);
    pRecord->GetNumberByName(kszOfficeNumber, itemData.m_strOfficePhoneNumber);
    pRecord->GetNumberByName(kszMobileNumber, itemData.m_strMobilePhoneNumber);
    pRecord->GetNumberByName(kszOtherNumber, itemData.m_strOtherPhoneNumber);

    itemData.m_strAutoDivert = pRecord->m_strDivert;

#if IF_FEATURE_FAVORITE_DIR
    if (nTarGroupId == kszBlackListGroupId)
    {
        itemData.m_nFavoriteIndex = -1;
    }
#endif

    int nRetCode = LocalDir_UpdateContact(itemData, nTarGroupId, nContactId);

    return CheckChangeContactSuccess(nTarGroupId, nRetCode, DIR_ACTION_LOCAL_MOVE_CONTACT_FINISH);
}

bool CLocalContactControler::RefreshSearchUI()
{
    if (IsSearchState())
    {
        if (PreSearchContact() && !m_pUIHelper.IsEmpty())
        {
            m_pUIHelper->RefreshUI(DIR_UI_REFRESH_TYPE_LIST_VIEW | DIR_UI_REFRESH_TYPE_TITLE,
                                   DIR_UI_REFRESH_PARAM_ALL);
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
        g_DirUIAdapter.ShowMsgBox(TRID_DISPLAYNAME_CANNNOT_CONTAIN_ILLEGAL_CHAR_AS, NULL,
                                  DIR_ACTION_GROUP_NAME_INVALID_TIP);

        ILocalContactUIHelper * pLocalUIHelper = g_DirUIAdapter.GetLocalContactUIHelper(m_pUIHelper);

        if (NULL != pLocalUIHelper)
        {
            pLocalUIHelper->SetInputFocus(DIR_ATTR_NAME);
        }

        return false;
    }

    if (groupData.m_strDisplayName.empty())
    {
        g_DirUIAdapter.ShowMsgBox(TRID_DISPLAY_NAME_CANNOT_BE_EMPTY, NULL,
                                  DIR_ACTION_GROUP_NAME_INVALID_TIP);

        ILocalContactUIHelper * pLocalUIHelper = g_DirUIAdapter.GetLocalContactUIHelper(m_pUIHelper);

        if (NULL != pLocalUIHelper)
        {
            pLocalUIHelper->SetInputFocus(DIR_ATTR_NAME);
        }

        return false;
    }

    if (0 == commonAPI_StringConvert(groupData.m_strDisplayName,
                                     STRING_CONVERT_TO_LOWER).compare("contacts"))
    {
        g_DirUIAdapter.ShowMsgBox(TRID_GROUP_NAME_CANNOT_BE_THE_WORD_CONTACTS, NULL,
                                  DIR_ACTION_GROUP_NAME_INVALID_TIP);
        return false;
    }

    return true;
}

bool CLocalContactControler::CheckChangeGroupSuccess(int nRetCode, int nAction)
{
    if (DIR_RET_GROUP_SIZE_UNEXPECTED == nRetCode)
    {
        g_DirUIAdapter.ShowMsgBox(TRID_GROUP_IS_FULL);
        return false;
    }
    else if (DIR_RET_GROUP_DUPLICATE == nRetCode)
    {
        g_DirUIAdapter.ShowMsgBox(TRID_GROUP_ALREAD_EXIST, NULL, DIR_ACTION_GROUP_NAME_INVALID_TIP);
        return false;
    }

    g_DirUIAdapter.ShowMsgBox(TRID_SAVING_FILES, NULL, nAction);
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

#if IF_FEATURE_FAVORITE_DIR
bool CLocalContactControler::CheckFavoriteIndexValid()
{
    if (!LocalDir_IsFavoriteEnable() || kszBlackListGroupId == GetGroupId())
    {
        return true;
    }

    ILocalContactUIHelper * pLocalUIHelper = g_DirUIAdapter.GetLocalContactUIHelper(m_pUIHelper);

    if (NULL == pLocalUIHelper)
    {
        return false;
    }

    yl::string strIndex = "";

    if (!pLocalUIHelper->GetLocalContactFavorite(strIndex))
    {
        DIRUI_ERR("CDirUIHelp::GetLocalContactFavorite() fail in OnAddContact!");
        return false;
    }

    if (strIndex.empty())
    {
        return true;
    }

    if (!commonAPI_IsDigital(strIndex) || atoi(strIndex.c_str()) <= 0)
    {
        g_DirUIAdapter.ShowMsgBox(TRID_FAVORITE_INDEX_VALID);

        ILocalContactUIHelper * pLocalUIHelper = g_DirUIAdapter.GetLocalContactUIHelper(m_pUIHelper);

        if (NULL != pLocalUIHelper)
        {
            pLocalUIHelper->SetInputFocus(DIR_ATTR_FAVORITE_INDEX);
        }
        return false;
    }

    return true;
}
#endif

bool CLocalContactControler::CheckContactValid(const ContactGeneralItemData & itemData)
{
    if (IsHadInvalidChar(itemData.m_strDisplayName.c_str()))
    {
        g_DirUIAdapter.ShowMsgBox(TRID_DISPLAYNAME_CANNNOT_CONTAIN_ILLEGAL_CHAR_AS, NULL,
                                  DIR_ACTION_NAME_INVALID_TIP);

        ILocalContactUIHelper * pLocalUIHelper = g_DirUIAdapter.GetLocalContactUIHelper(m_pUIHelper);

        if (NULL != pLocalUIHelper)
        {
            pLocalUIHelper->SetInputFocus(DIR_ATTR_NAME);
        }

        return false;
    }

    if (itemData.m_strDisplayName.empty())
    {
        g_DirUIAdapter.ShowMsgBox(TRID_DISPLAY_NAME_CANNOT_BE_EMPTY, NULL, DIR_ACTION_NAME_INVALID_TIP);

        ILocalContactUIHelper * pLocalUIHelper = g_DirUIAdapter.GetLocalContactUIHelper(m_pUIHelper);

        if (NULL != pLocalUIHelper)
        {
            pLocalUIHelper->SetInputFocus(DIR_ATTR_NAME);
        }

        return false;
    }

    return true;
}

bool CLocalContactControler::CheckChangeContactSuccess(int nGroupId, int nRetCode,
        int nAction/* = DIR_ACTION_NONE*/)
{
    if (DIR_RET_GROUP_SIZE_UNEXPECTED == nRetCode)
    {
        if (kszBlackListGroupId == nGroupId)
        {
            g_DirUIAdapter.ShowMsgBox(TRID_BLACKLIST_IS_FULL);
        }
        else
        {
            g_DirUIAdapter.ShowMsgBox(TRID_CONTACT_FULL);
        }

        return false;
    }
    else if (DIR_RET_CONTACT_EXIST == nRetCode)
    {
        g_DirUIAdapter.ShowMsgBox(TRID_T20_TIP_CONTACT_NAME_EXISTED_PART1,
                                  TRID_T20_TIP_CONTACT_NAME_EXISTED_PART2, DIR_ACTION_NAME_INVALID_TIP);

        return false;
    }
#if IF_FEATURE_FAVORITE_DIR
    else if (DIR_RET_FAVORITE_INDEX_MAX == nRetCode || DIR_RET_FAVORITE_INDEX_EXIST == nRetCode)
    {
        yl::string strNote = (DIR_RET_FAVORITE_INDEX_MAX == nRetCode) ? TRID_FAVORITE_INDEX_NAX : TRID_FAVORITE_INDEX_EXIST;
        g_DirUIAdapter.ShowMsgBox(strNote.c_str());

        int nUISattus = GetState();
        if (DIR_UI_STATE_DETAIL == nUISattus || DIR_UI_STATE_ADDITION == nUISattus)
        {
            ILocalContactUIHelper* pLocalUIHelper = g_DirUIAdapter.GetLocalContactUIHelper(m_pUIHelper);
            if (NULL != pLocalUIHelper)
            {
                pLocalUIHelper->SetInputFocus(DIR_ATTR_FAVORITE_INDEX);
            }
        }

        return false;
    }
#endif

    g_DirUIAdapter.ShowMsgBox(TRID_SAVING_FILES, NULL, nAction);
    return true;
}

bool CLocalContactControler::IsHadInvalidChar(const char * pszStr)
{
    if (NULL == pszStr)
    {
        return false;
    }

    // 由于web放开限制了GUI这边也需要放开http://bugfree.yealink.com/Bug.php?BugID=76923
    // 异常字符 "    < > ' " \      "
//  yl::string strExceptionalChar[5] = {"<", ">", "\'", "\"", "\\"};
//  for (int i = 0; i < 5; ++i)
//  {
//      // 判断字符串中是否含有指定的字符
//      if (NULL != strstr(pszStr, strExceptionalChar[i].c_str()))
//      {
//          return true;
//      }
//  }

    return false;
}

int CLocalContactControler::GetBrowseItemAmount()
{
    int nItemAmount = CCommonDirCtrl::GetBrowseItemAmount();

    if (DIR_INVALID_AMOUNT != nItemAmount)
    {
        return nItemAmount;
    }

    return GetGroupChildrenAmount(m_nGroupId, DIR_TYPE_LOCAL_GROUP);
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

#ifdef DIR_SUPPORT_EXIST_SAVE
bool CLocalContactControler::IsContactChanged()
{
    ILocalContactUIHelper * pLocalUIHelper = g_DirUIAdapter.GetLocalContactUIHelper(m_pUIHelper);

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

    if (itemData.m_strAutoDivert != pRecord->m_strDivert)
    {
        DIRUI_INFO("Contact changed, Input AutoDivert:%s AutoDivert:%s",
                   itemData.m_strAutoDivert.c_str(), pRecord->m_strDivert.c_str());
        return true;
    }

#if IF_FEATURE_FAVORITE_DIR
    if (!CheckFavoriteIndexValid() || itemData.m_nFavoriteIndex != pRecord->m_nFavoriteIndex)
    {
        DIRUI_INFO("Contact changed, Input Favorites:%d Favorites:%d",
                   itemData.m_nFavoriteIndex, pRecord->m_nFavoriteIndex);
        return true;
    }
#endif

    return false;
}
#endif

#ifdef IF_FEATURE_FAVORITE_DIR
bool CLocalContactControler::IsFavoriteContact(int nContactId)
{
    return LocalDir_IsContactInFavorite(nContactId);
}

bool CLocalContactControler::OnCopyToFavorite(int nContactId)
{
    ILocalContactUIHelper* pLocalUIHelper = g_DirUIAdapter.GetLocalContactUIHelper(m_pUIHelper);
    if (NULL == pLocalUIHelper)
    {
        return false;
    }

    int nIndex = LocalDir_GetFavoriteIndex();
    yl::string strNote = commonAPI_FormatString("%s %d ?", _UILOGIC_LANG_TRAN(TRID_LOCAL_COPY_TO_FAVORITE).c_str(), nIndex);
    pLocalUIHelper->PopSureEditMsgBox(TRID_WARNING, strNote.c_str(),
                                      DIR_ACTION_LOCAL_COPY_TO_FAVORITE, nContactId);

    return true;
}

bool CLocalContactControler::OnRemoveFromFavorite(int nContactId)
{
    return g_DirUIAdapter.PopSureMsgBox(TRID_WARNING, TRID_LOCAL_REMOVE_FROM_FAVORITE,
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
    int nRetCode = bSuc ? DIR_RET_SUCC : DIR_RET_FAIL;
    return CheckChangeContactSuccess(kszFavoriteGroupId, nRetCode, DIR_ACTION_LOCAL_MOVE_CONTACT_FINISH);
}
#endif
