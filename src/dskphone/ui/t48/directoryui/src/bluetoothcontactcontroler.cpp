#include "bluetoothcontactcontroler.h"
#ifndef DIR_SUPPORT_SINGLE_UIHELPER
#include "bluetoothuihelper.h"
#endif
#ifdef DIR_SUPPORT_BATCH_OPERATE
#include "localcontactcontroler.h"
#endif


CBluetoothContactControler::CBluetoothContactControler()
    : CCommonDirCtrl(DIRECTORY_TYPE_BLUETOOTH, BLUETOOTH_DIR)
    , m_bDownloading(false)
    , m_bUpdating(false)
{

}

CBluetoothContactControler::~CBluetoothContactControler()
{

}

CBluetoothContactControler * CBluetoothContactControler::GetControler(
    CDirectoryControlerPtr & pControler)
{
    if (pControler.IsEmpty() || DIRECTORY_TYPE_BLUETOOTH != pControler->GetType())
    {
        return NULL;
    }

    return (CBluetoothContactControler *)pControler.Get();
}

LocalDirContactRecord * CBluetoothContactControler::GetLocalContact(SmartPtr & pDetail)
{
    if (pDetail.IsEmpty() || DIR_TYPE_BT_CONTACT != pDetail->GetType())
    {
        return NULL;
    }

    return (LocalDirContactRecord *)pDetail.Get();
}

CBluetoothUIHelper * CBluetoothContactControler::GetBluetoothUIHelper()
{
#ifdef DIR_SUPPORT_SINGLE_UIHELPER
    return m_pUIHelper;
#else
    return CBluetoothUIHelper::GetUIHelper(m_pUIHelper);
#endif
}

void CBluetoothContactControler::UpdateContacts()
{
    if (m_bUpdating)
    {
        return;
    }

    int nRetCode = BlueToothDir_Update();

    if (DIR_RET_SUCC == nRetCode)
    {
        m_bUpdating = true;

        CDirUIHelper::ShowWaiting(TRID_NOTE, TRID_UPDATING_PLEASE_WAIT, DIR_ACTION_BLUETOOTH_UPDATE);
    }
    else
    {
        CDirUIHelper::ShowErrorMsg(nRetCode);
    }
}

bool CBluetoothContactControler::PreLoadContact(int nGroupId)
{
    DIRUI_INFO("CBluetoothContactControler::PreLoadContact(GroupId:%d)", nGroupId);

    if (!CCommonDirCtrl::PreLoadContact(nGroupId))
    {
        return false;
    }

    FileDownloadStatus eStatus = BlueToothDir_GetDownloadStatus();

    if (FILE_DOWNLOADING != eStatus)
    {
        m_bDownloading = false;
        m_bUpdating = false;
        return true;
    }

    m_bDownloading = true;
    m_bUpdating = false;

    CDirUIHelper::ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT, DIR_ACTION_BLUETOOTH_DOWNLOAD);
    return true;
}

bool CBluetoothContactControler::OnAction(int nAction, int nDataIndex, int nId)
{
    DIRUI_INFO("CBluetoothContactControler::OnAction(Action:%d Index:%d Id:%d)",
               nAction, nDataIndex, nId);

    if (CCommonDirCtrl::OnAction(nAction, nDataIndex, nId))
    {
        return true;
    }

    switch (nAction)
    {
    case DIR_ACTION_BLUETOOTH_UPDATE:
        {
            UpdateContacts();
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

bool CBluetoothContactControler::DialContact(int nContactId,
        int nAccountId/* = AUTO_ACCOUNT_IDENTIFY*/)
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

    LIST_ATTRIBUTE lsNumber;

    GetNumberAttr(pRecord, lsNumber, false);

    CDirUIHelper::Dial(nAccountId, pDetail->GetDisplayName(), lsNumber, this, pRecord);

    return true;
}

bool CBluetoothContactControler::ExecAction(int nAction, int nDataIndex, int nId)
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

bool CBluetoothContactControler::CancelAction(int nAction, int nDataIndex, int nId)
{
    if (DIR_ACTION_BLUETOOTH_UPDATE == nAction || DIR_ACTION_BLUETOOTH_DOWNLOAD == nAction)
    {
        if (m_bUpdating)
        {
            m_bUpdating = false;
            BlueToothDir_CancelUpdate();
        }

        m_bDownloading = false;

        CDirUIHelper::RefreshUI(false, false);
        return true;
    }

    return false;
}

void CBluetoothContactControler::GetNumberAttr(LocalDirContactRecord * pRecord,
        LIST_ATTRIBUTE & lsNumber, bool bCopy)
{
    if (NULL == pRecord)
    {
        return;
    }

    ST_CONTACT_ATTRIBUTE stAttr;

    if (pRecord->GetNumberByName(kszOfficeNumber, stAttr.m_strValue) && !stAttr.m_strValue.empty())
    {
        if (bCopy)
        {
            stAttr.m_strName = ATTR_OFFICENUMBER;
        }
        else
        {
            stAttr.m_strName = TRID_OFFICE_NUMBER;
        }

        lsNumber.push_back(stAttr);
    }

    if (pRecord->GetNumberByName(kszMobileNumber, stAttr.m_strValue) && !stAttr.m_strValue.empty())
    {
        if (bCopy)
        {
            stAttr.m_strName = ATTR_MOBILENUMBER;
        }
        else
        {
            stAttr.m_strName = TRID_MOBILE_NUMBER;
        }

        lsNumber.push_back(stAttr);
    }

    if (pRecord->GetNumberByName(kszOtherNumber, stAttr.m_strValue) && !stAttr.m_strValue.empty())
    {
        if (bCopy)
        {
            stAttr.m_strName = ATTR_OTHERNUMBER;
        }
        else
        {
            stAttr.m_strName = TRID_OTHER_NUMBER;
        }

        lsNumber.push_back(stAttr);
    }
}

void CBluetoothContactControler::OnCopyContact(int nAction, int nId)
{
    SmartPtr pDetail = Dir_GetDetailById(nId, m_eLogicType);
    LocalDirContactRecord * pRecord = GetLocalContact(pDetail);

    if (NULL == pRecord)
    {
        DIRUI_ERR("GetLocalContact fail OnCopyContact(Action:%d Id:%d)", nAction, nId);
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

#ifdef DIR_SUPPORT_BATCH_OPERATE
bool CBluetoothContactControler::CopyToLocalContact(int nGroupId, VEC_CONTACT_ID & vecContactId)
{
    int nSize = vecContactId.size();
    YLList<ContactGeneralItemData> listData;

    for (int i = 0; i < nSize; ++i)
    {
        int nContactId = vecContactId[i];
        SmartPtr pDetail = Dir_GetDetailById(nContactId, m_eLogicType);
        LocalDirContactRecord * pRecord = GetLocalContact(pDetail);

        if (NULL == pRecord)
        {
            continue;
        }

        ContactGeneralItemData itemData;

        CLocalContactControler::GetCopyDefaultContact(itemData);

        itemData.m_strDisplayName = pRecord->m_strDisplayName;
        pRecord->GetNumberByName(kszOfficeNumber, itemData.m_strOfficePhoneNumber);
        pRecord->GetNumberByName(kszMobileNumber, itemData.m_strMobilePhoneNumber);
        pRecord->GetNumberByName(kszOtherNumber, itemData.m_strOtherPhoneNumber);

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
            CDirUIHelper::ShowMsgBox(TRID_ADD_BLACKLIST_SUCCESS);
        }
        else
        {
            CDirUIHelper::ShowMsgBox(TRID_ADD_CONTACTS_SUCCESS);
        }

        CBluetoothUIHelper * pUIHelper = GetBluetoothUIHelper();

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

LRESULT CBluetoothContactControler::OnMessage(msgObject & refMessage)
{
    if (LOCAL_SEARCH_RESULT == refMessage.message)
    {
        OnSearchResult(refMessage);
        return TRUE;
    }
    else if (BLUETOOTH_CONTACT_DOWNLOAD_RESULT == refMessage.message)
    {
        OnDownLoadFinished(refMessage);
        return TRUE;
    }

    return FALSE;
}

void CBluetoothContactControler::OnDownLoadFinished(msgObject & refMessage)
{
    if (m_bDownloading || m_bUpdating)
    {
        m_bDownloading = false;
        m_bUpdating = false;
        CDirUIHelper::EndWaiting();
    }

    if (0 == refMessage.wParam)
    {
        CDirUIHelper::ShowMsgBox(TRID_FAIL_TO_DOWNLOAD_MOBILE_CONTACTS);
    }
    else if (2 == refMessage.wParam)
    {
        CDirUIHelper::ShowMsgBox(TRID_MOBILE_DISCONNECT_SYNC_FAILED);
    }

    CDirUIHelper::RefreshUI(true, true);
}
