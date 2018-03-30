#include "remotecontactcontroler.h"
#include "translateiddefine.h"
#ifndef DIR_SUPPORT_SINGLE_UIHELPER
#include "remoteuihelper.h"
#endif
#ifdef DIR_SUPPORT_BATCH_OPERATE
#include "localcontactcontroler.h"
#endif
#include <configiddefine.h>
#include "configparser/modconfigparser.h"


CRemoteContactControler::CRemoteContactControler()
    : CCommonDirCtrl(DIRECTORY_TYPE_REMOTE, REMOTE_DIR)
    , m_nHotKeyId(knInvalidRId)
    , m_bDownloading(false)
    , m_bUpdating(false)
    , m_nDownLoadGroupId(knInvalidRId)
{
}

CRemoteContactControler::~CRemoteContactControler()
{
}

CRemoteUIHelper * CRemoteContactControler::GetRemoteUIHelper()
{
#ifdef DIR_SUPPORT_SINGLE_UIHELPER
    return m_pUIHelper;
#else
    return CRemoteUIHelper::GetUIHelper(m_pUIHelper);
#endif
}

ContactRemoteUnit * CRemoteContactControler::GetRemoteContact(SmartPtr & pDetail)
{
    if (pDetail.IsEmpty() || DIR_TYPE_REMOTE_UNIT != pDetail->GetType())
    {
        return NULL;
    }

    return (ContactRemoteUnit *)pDetail.Get();
}

ContactRemoteMenu * CRemoteContactControler::GetRemoteGroup(SmartPtr & pDetail)
{
    if (pDetail.IsEmpty() || DIR_TYPE_REMOTE_MENU != pDetail->GetType())
    {
        return NULL;
    }

    return (ContactRemoteMenu *)pDetail.Get();
}

bool CRemoteContactControler::IsServerSearchGroup(int nGroupId)
{
    int nRootId = RemoteDir_GetRootGroupId(nGroupId);
    SmartPtr pDetail = Dir_GetDetailById(nRootId, REMOTE_DIR);
    ContactRemoteMenu * pRemoteMenu = GetRemoteGroup(pDetail);

    if (NULL != pRemoteMenu)
    {
        return pRemoteMenu->m_bSearchRootGroup;
    }

    return false;
}

CRemoteContactControler * CRemoteContactControler::GetControler(CDirectoryControlerPtr & pControler)
{
    if (pControler.IsEmpty() || DIRECTORY_TYPE_REMOTE != pControler->GetType())
    {
        return NULL;
    }

    return (CRemoteContactControler *)pControler.Get();
}

int CRemoteContactControler::GetHotKeyGroupId(int nKeyCode)
{
    if (nKeyCode >= PHONE_KEY_0 && nKeyCode <= PHONE_KEY_9)
    {
        char cKey = '0' + (nKeyCode - PHONE_KEY_0);

        m_strHotKey = cKey;
    }
    else if (PHONE_KEY_POUND == nKeyCode)
    {
        m_strHotKey = '#';
    }
    else if (PHONE_KEY_STAR == nKeyCode)
    {
        m_strHotKey = '*';
    }
    else if (nKeyCode >= PHONE_KEY_A && nKeyCode <= PHONE_KEY_D)
    {
        char cKey = 'A' + (nKeyCode - PHONE_KEY_A);

        m_strHotKey = cKey;
    }
    else
    {
        m_strHotKey = "";
    }

    m_nHotKeyId = knInvalidRId;

    RemoteDir_GetHotKeyStatus(m_strHotKey, GetGroupId(), m_nHotKeyId);

    return m_nHotKeyId;
}

int CRemoteContactControler::GetDetailType(int nId)
{
    SmartPtr pDetail = Dir_GetDetailById(nId, m_eLogicType);

    if (pDetail.IsEmpty())
    {
        return DIR_TYPE_NONE;
    }

    return pDetail->GetType();
}

int CRemoteContactControler::GetSubMenuIndexbyId(int nMenuId)
{
    ContactListData lsData;

    lsData.m_nCountToGet = 0;
    lsData.m_nIndexFrom = 0;

    Dir_GetContactListByGroupId(&lsData, GetGroupId(), m_eLogicType);

    int nSize = lsData.m_listId.size();

    for (int i = 0; i < nSize; ++i)
    {
        if (nMenuId == lsData.m_listId[i])
        {
            return i;
        }
    }

    return -1;
}

bool CRemoteContactControler::IsHadContactInGroup(int nGroupId)
{
    if (kszRootId == nGroupId)
    {
        return false;
    }

    ContactListData lsData;

    lsData.m_nCountToGet = 1;
    lsData.m_nIndexFrom = 0;

    if (!Dir_GetContactListByGroupId(&lsData, nGroupId, REMOTE_DIR) || 1 != lsData.m_listId.size())
    {
        return false;
    }

    return (DIR_TYPE_REMOTE_UNIT == GetDetailType(lsData.m_listId[0]));
}

bool CRemoteContactControler::GetGroupTitle(int nGroupId, yl::string & strTitle)
{
    SmartPtr pDetail = Dir_GetDetailById(nGroupId, m_eLogicType);

    if (pDetail.IsEmpty())
    {
        return false;
    }

    int nParentId = Dir_GetParentIdById(nGroupId, m_eLogicType);
    int nType = pDetail->GetType();

    if ((nParentId != kszRootId && (DIR_TYPE_REMOTE_MENU == nType || DIR_TYPE_REMOTE_HOTKEY == nType))
            || (IsServerSearchGroup(nGroupId) && RemoteDir_GetRootGroupId(nGroupId) != nGroupId))
    {
        ContactRemoteMenu * pRemoteMenu = (ContactRemoteMenu *)pDetail.Get();

        strTitle = pRemoteMenu->m_strTitle;

        if (IsServerSearchGroup(nGroupId))
        {
            return true;
        }

        if (strTitle.empty())
        {
            strTitle = pDetail->GetDisplayName();
        }
    }
    else
    {
        strTitle = pDetail->GetDisplayName();
    }

    return true;
}

void CRemoteContactControler::UpdateContacts(int nGroupId)
{
    if (m_bUpdating)
    {
        return;
    }

    if (knInvalidRId == nGroupId)
    {
        nGroupId = GetGroupId();

        if (knInvalidRId == nGroupId)
        {
            nGroupId = kszRootId;
        }
    }

    m_nDownLoadGroupId = nGroupId;
    DIRUI_INFO("CRemoteContactControler::UpdateContacts(%d)", m_nDownLoadGroupId);

    int nRetCode = RemoteDir_Update(nGroupId);

    if (DIR_RET_SUCC == nRetCode)
    {
        m_bUpdating = true;

        CDirUIHelper::ShowWaiting(TRID_NOTE, TRID_UPDATING_PLEASE_WAIT, DIR_ACTION_REMOTE_UPDATE);
    }
    else
    {
        CDirUIHelper::ShowErrorMsg(nRetCode);
    }
}

bool CRemoteContactControler::OnBack()
{
    int nCurGroupId = GetGroupId();

    if (knInvalidRId == nCurGroupId || kszRootId == nCurGroupId)
    {
        return false;
    }

    int nParentGroupId = Dir_GetParentIdById(nCurGroupId, m_eLogicType);

    if (knInvalidRId == nParentGroupId)
    {
        nParentGroupId = kszRootId;
    }

    SetGroupId(nParentGroupId);

    return true;
}

bool CRemoteContactControler::PreLoadContact(int nGroupId)
{
    DIRUI_INFO("CRemoteContactControler::PreLoadContact(GroupId:%d HotKeyId:%d)", nGroupId,
               m_nHotKeyId);

    int nOriGroupId = GetGroupId();

    if (!CCommonDirCtrl::PreLoadContact(nGroupId))
    {
        return false;
    }

    FileDownloadStatus eStatus = FILE_DOWNLOAD_STATUS_NONE;

    if (kszRootId == nGroupId)
    {
        eStatus = FILE_DOWNLOADED_OK;
    }
    else if (1 != configParser_GetConfigInt(kszRemoteDirEnterUpdate)
             || (nGroupId != m_nHotKeyId && kszRootId != Dir_GetParentIdById(nGroupId, m_eLogicType)))
    {
        if (nGroupId == m_nHotKeyId)
        {
            eStatus = RemoteDir_GetHotKeyStatus(m_strHotKey, nOriGroupId, m_nHotKeyId);
        }
        else
        {
            SmartPtr pDetail = Dir_GetDetailById(nGroupId, m_eLogicType);
            DirGroupRecord * pMenu = GetGroupRecord(pDetail);

            if (NULL == pMenu)
            {
                DIRUI_ERR("CRemoteContactControler::PreLoadContact(%d) not group", nGroupId);
                return false;
            }

            eStatus = pMenu->m_eStatus;
        }
    }

    if (FILE_DOWNLOADED_OK == eStatus)
    {
        return true;
    }

    m_nDownLoadGroupId = nGroupId;
    m_bDownloading = true;

    if (FILE_DOWNLOADING != eStatus)
    {
        int nRetCode = RemoteDir_ReqLoadContact(nGroupId);

        if (DIR_RET_SUCC != nRetCode)
        {
            CDirUIHelper::ShowErrorMsg(nRetCode);
            return true;
        }
    }

    CDirUIHelper::ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT, DIR_ACTION_REMOTE_DOWNLOAD);
    return true;
}

bool CRemoteContactControler::OnAction(int nAction, int nDataIndex, int nId)
{
    DIRUI_INFO("CRemoteContactControler::OnAction(Action:%d Index:%d Id:%d)",
               nAction, nDataIndex, nId);

    if (DIR_ACTION_COMMON_SELECT == nAction && IsServerSearchGroup(nId))
    {
        CDirUIHelper::EnterUI(DIRECTORY_TYPE_REMOTE_SEARCH, DIR_UI_STATE_SEARCH, nAction, nId);
        return true;
    }

    if (CCommonDirCtrl::OnAction(nAction, nDataIndex, nId))
    {
        return true;
    }

    switch (nAction)
    {
    case DIR_ACTION_REMOTE_UPDATE:
        {
            UpdateContacts(nId);
        }
        break;
    case DIR_ACTION_LOCAL_ENTER_COPY_TO_CONTACTS:
    case DIR_ACTION_LOCAL_ENTER_COPY_TO_BLACKLIST:
        {
            OnCopyContact(nAction, nId);
        }
        break;
#ifdef DIR_SUPPORT_BATCH_OPERATE
    case DIR_ACTION_LOCAL_BATCH_MOVE_CONTACT:
        {
            int nAllContactSize = Dir_GetContactCount(kszAllContactId, LOCAL_DIR);
            VEC_CONTACT_ID vecSelectId;

            if (!CDirUIHelper::GetSelectContactId(vecSelectId))
            {
                return false;
            }

            if ((nAllContactSize + vecSelectId.size()) > DIRECTORY_MAX_CONTACT_SIZE)
            {
                CDirUIHelper::ShowMsgBox(TRID_ONLY_1000_CONTACTS_ALLOWED);
                return false;
            }

            return CDirUIHelper::EnterSelectGroup(DIRECTORY_TYPE_LOCAL, kszBlackListGroupId, true, nAction);
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

    return true;
}

bool CRemoteContactControler::ExecAction(int nAction, int nDataIndex, int nId)
{
    switch (nAction)
    {
#ifdef DIR_SUPPORT_BATCH_OPERATE
    case DIR_ACTION_LOCAL_BATCH_MOVE_TO_BLACKLIST:
        {
            int nBlackListSize = Dir_GetContactCount(kszBlackListGroupId, LOCAL_DIR);
            VEC_CONTACT_ID vecSelectId;

            if (!CDirUIHelper::GetSelectContactId(vecSelectId))
            {
                return false;
            }

            if ((nBlackListSize + vecSelectId.size()) > DIRECTORY_MAX_BLACKLIST_SIZE)
            {
                CDirUIHelper::ShowMsgBox(TRID_ONLY_30_CONTACTS_ALLOWED);
                return false;
            }

            return CopyToLocalContact(kszBlackListGroupId, vecSelectId);
        }
        break;
    case DIR_ACTION_LOCAL_BATCH_MOVE_CONTACT:
        {
            VEC_CONTACT_ID vecSelectId;

            if (!CDirUIHelper::GetSelectContactId(vecSelectId))
            {
                return false;
            }

            return CopyToLocalContact(nId, vecSelectId);
        }
        break;
#endif
    default:
        break;
    }

    return false;
}

bool CRemoteContactControler::CancelAction(int nAction, int nDataIndex, int nId)
{
    if (DIR_ACTION_REMOTE_UPDATE == nAction || DIR_ACTION_REMOTE_DOWNLOAD == nAction)
    {
        if (m_bUpdating || m_bDownloading)
        {
            Dir_CancelLoad(GetGroupId(), m_eLogicType);
        }

        m_bUpdating = false;
        m_bDownloading = false;
        CDirUIHelper::RefreshUI(false, false);
        return true;
    }

    return false;
}

#ifdef DIR_SUPPORT_BATCH_OPERATE
bool CRemoteContactControler::CopyToLocalContact(int nGroupId, VEC_CONTACT_ID & vecContactId)
{
    int nSize = vecContactId.size();
    YLList<ContactGeneralItemData> listData;

    for (int i = 0; i < nSize; ++i)
    {
        int nContactId = vecContactId[i];
        SmartPtr pDetail = Dir_GetDetailById(nContactId, m_eLogicType);
        ContactRemoteUnit * pRemoteUnit = GetRemoteContact(pDetail);

        if (NULL == pRemoteUnit)
        {
            continue;
        }

        ContactGeneralItemData itemData;

        CLocalContactControler::GetCopyDefaultContact(itemData);

        itemData.m_strDisplayName = pRemoteUnit->m_strDisplayName;
        itemData.m_strOfficePhoneNumber = pRemoteUnit->GetNumberByIndex(0);
        itemData.m_strMobilePhoneNumber = pRemoteUnit->GetNumberByIndex(1);
        itemData.m_strOtherPhoneNumber = pRemoteUnit->GetNumberByIndex(2);

        // 去掉左右空格
        itemData.m_strDisplayName = commonAPI_TrimString(itemData.m_strDisplayName, " ");

        if (!CLocalContactControler::CheckContactDataValid(itemData))
        {
            continue;
        }

        listData.push_back(itemData);
    }

    int nRetCode = LocalDir_AddLocalContactList(listData, nGroupId);

    if (nRetCode >= 0)
    {
        if (kszBlackListGroupId == nGroupId)
        {
            CDirUIHelper::ShowMsgBox(TRID_MOVE_BLACKLIST_SUCCESS);
        }
        else
        {
            CDirUIHelper::ShowMsgBox(TRID_MOVE_CONTACTS_SUCCESS);
        }

        CRemoteUIHelper * pUIHelper = GetRemoteUIHelper();

        if (NULL != pUIHelper)
        {
            pUIHelper->OnBatchCopyContactSuccess();
        }
    }
    else if (DIR_RET_GROUP_SIZE_UNEXPECTED == nRetCode)
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
        CDirUIHelper::ShowErrorMsg(nRetCode);
    }

    return true;
}
#endif

void CRemoteContactControler::GetNumberAttr(ContactRemoteUnit * pRecord, LIST_ATTRIBUTE & lsNumber,
        bool bCopy)
{
    if (NULL == pRecord)
    {
        return;
    }

    int nSize = pRecord->m_listNumber.size();
    ST_CONTACT_ATTRIBUTE stAttr;

    if (1 == nSize)
    {
        if (bCopy)
        {
            stAttr.m_strName = ATTR_NUMBER;
        }
        else
        {
            stAttr.m_strName = TRID_NUMBER;
        }
        stAttr.m_strValue = pRecord->GetNumberByIndex(0);

        lsNumber.push_back(stAttr);
    }
    else if (nSize > 1)
    {
        yl::string strNumberName = TRID_NUMBER;
        int nIndex = 1;

        for (int i = 0; i < nSize; ++i)
        {
            const yl::string & strNumber = pRecord->GetNumberByIndex(i);

            if (strNumber.empty())
            {
                continue;
            }

            if (bCopy)
            {
                stAttr.m_strName = ATTR_NUMBER;
            }
            else
            {
                char szBuffer[32] = {0};

                snprintf(szBuffer, sizeof(szBuffer), "%d", nIndex);
                szBuffer[sizeof(szBuffer) - 1] = '\0';

                stAttr.m_strName = strNumberName + szBuffer;
            }

            ++nIndex;
            stAttr.m_strValue = strNumber;

            lsNumber.push_back(stAttr);
        }
    }
}

void CRemoteContactControler::OnCopyContact(int nAction, int nId)
{
    SmartPtr pDetail = Dir_GetDetailById(nId, m_eLogicType);
    ContactRemoteUnit * pRecord = GetRemoteContact(pDetail);

    if (NULL == pRecord)
    {
        DIRUI_ERR("GetRemoteContact fail OnCopyContact(Action:%d Id:%d)", nAction, nId);
        return;
    }

    LIST_ATTRIBUTE lsAttr;
    ST_CONTACT_ATTRIBUTE stAttr;

    stAttr.m_strName = ATTR_NAME;
    stAttr.m_strValue = pRecord->GetDisplayName();
    lsAttr.push_back(stAttr);

    GetNumberAttr(pRecord, lsAttr, true);

    CDirUIHelper::SetAttributeList(m_nType, lsAttr);
    CDirUIHelper::EnterUI(DIRECTORY_TYPE_LOCAL, DIR_UI_STATE_ADDITION, nAction, knInvalidRId);
}

bool CRemoteContactControler::DialContact(int nContactId,
        int nAccountId/*  = AUTO_ACCOUNT_IDENTIFY*/)
{
    SmartPtr pDetail = Dir_GetDetailById(nContactId, m_eLogicType);
    ContactRemoteUnit * pRecord = GetRemoteContact(pDetail);

    if (NULL == pRecord)
    {
        DIRUI_ERR("GetRemoteContact fail DialContact(ContactId:%d)", nContactId);
        return false;
    }

    m_nContactId = nContactId;

    ST_CONTACT_ATTRIBUTE stAttr;
    LIST_ATTRIBUTE lsNumber;

    GetNumberAttr(pRecord, lsNumber, false);

    CDirUIHelper::Dial(nAccountId, pRecord->GetDisplayName(), lsNumber, this, pRecord);

    return true;
}

LRESULT CRemoteContactControler::OnMessage(msgObject & refMessage)
{
    if (LOCAL_SEARCH_RESULT != refMessage.message
            && (refMessage.message < REMOTE_DIRECTORY_MESSAGE_BEGIN
                || refMessage.message > REMOTE_DIRECTORY_MESSAGE_END))
    {
        return FALSE;
    }

    switch (refMessage.message)
    {
    case REMOTE_PHONEBOOK_RELOAD:
    case REMOTE_PHONEBOOK_BEGIN_UPDATE:
        {
            DIRUI_INFO("Receive REMOTE_PHONEBOOK_RELOAD|REMOTE_PHONEBOOK_BEGIN_UPDATE message");

            m_nDownLoadGroupId = knInvalidRId;
            SetGroupId(knInvalidRId);
            CDirUIHelper::ExitAllUI(m_nType);
        }
        break;
    case REMOTE_PHONEBOOK_ALL_DOWNLOADED_FINISHED:
        {
            DIRUI_INFO("Receive REMOTE_PHONEBOOK_ALL_DOWNLOADED_FINISHED message");

            OnDownLoadFinished(refMessage);
        }
        break;
    case LOCAL_SEARCH_RESULT:
        {
            DIRUI_INFO("Receive LOCAL_SEARCH_RESULT message");

            OnSearchResult(refMessage);
        }
        break;
    default:
        return FALSE;
        break;
    }

    return TRUE;
}

void CRemoteContactControler::OnDownLoadFinished(msgObject & refMessage)
{
    CRemoteUIHelper * pRemoteUIHelper = GetRemoteUIHelper();

    if (NULL == pRemoteUIHelper)
    {
        return;
    }

    int nGroupId = refMessage.wParam;

    DIRUI_INFO("CRemoteContactControler::OnDownLoadFinished(CurGroupId:%d GroupId:%d lp:%d)",
               m_nDownLoadGroupId, nGroupId, refMessage.lParam);

    // 当前组更新消息到时就要处理
    if (nGroupId == m_nDownLoadGroupId)
    {
        if (m_bUpdating || m_bDownloading)
        {
            CDirUIHelper::EndWaiting();
        }

        m_bUpdating = false;
        m_bDownloading = false;

        if (1 == refMessage.lParam)
        {
            CDirUIHelper::ShowMsgBox(TRID_FAIL_TO_DOWNLOAD_REMOTE_PHONEBOOK);
        }
        else if (401 == refMessage.lParam)
        {
            CDirUIHelper::ShowMsgBox(TRID_DIR_REMOTE_TIP_AUTH_FAILED);
        }
#ifdef FL_SUPPORT_TITLE
        else
        {
            pRemoteUIHelper = GetRemoteUIHelper();

            if (NULL != pRemoteUIHelper)
            {
                pRemoteUIHelper->RefreshTitle();
            }
        }
#endif

        CDirUIHelper::RefreshUI(true, false);
        return;
    }

#ifdef FL_SUPPORT_TITLE
    bool bAllLoad = Dir_IsAllLoadFinish(m_nDownLoadGroupId, m_eLogicType);
#endif

    if (kszRootId == m_nDownLoadGroupId)
    {
        if (m_bUpdating || m_bDownloading)
        {
            CDirUIHelper::EndWaiting();
        }

        m_bUpdating = false;
        m_bDownloading = false;

        CDirUIHelper::RefreshUI(true, false);
    }
#ifdef FL_SUPPORT_TITLE
    else if (bAllLoad)
    {
        pRemoteUIHelper = GetRemoteUIHelper();

        if (NULL != pRemoteUIHelper)
        {
            int nAllContactAmount = GetContactAmount(GetGroupId());

            pRemoteUIHelper->RefreshUpdateTitle(nAllContactAmount, true);
        }
    }
#endif
}
