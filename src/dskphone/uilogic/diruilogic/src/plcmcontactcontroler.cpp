#include "plcmcontactcontroler.h"
#ifdef DIR_SUPPORT_BATCH_OPERATE
#include "localcontactcontroler.h"
#endif
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "iplcmuihelper.h"
#include "diruiadapter.h"
#include "remotecontactcontroler.h"


#if IF_BUG_PLCMDIR
CPlcmContactControler::CPlcmContactControler()
    : CCommonDirCtrl(DIRECTORY_TYPE_PLCM, PLCM_DIR)
    , m_bDownloading(false)
    , m_bUpdating(false)
    , m_nDownLoadGroupId(knInvalidRId)
{
}

CPlcmContactControler::~CPlcmContactControler()
{
}

ContactRemoteUnit * CPlcmContactControler::GetPlcmContact(SmartPtr & pDetail)
{
    if (pDetail.IsEmpty() || DIR_TYPE_REMOTE_UNIT != pDetail->GetType())
    {
        return NULL;
    }

    return (ContactRemoteUnit *)pDetail.Get();
}

ContactRemoteMenu * CPlcmContactControler::GetPlcmGroup(SmartPtr & pDetail)
{
    if (pDetail.IsEmpty() || DIR_TYPE_REMOTE_MENU != pDetail->GetType())
    {
        return NULL;
    }

    return (ContactRemoteMenu *)pDetail.Get();
}

CPlcmContactControler * CPlcmContactControler::GetControler(CDirectoryControlerPtr & pControler)
{
    if (pControler.IsEmpty() || DIRECTORY_TYPE_PLCM != pControler->GetType())
    {
        return NULL;
    }

    return (CPlcmContactControler *)pControler.Get();
}

int CPlcmContactControler::GetDetailType(int nId)
{
    SmartPtr pDetail = Dir_GetDetailById(nId, m_eLogicType);

    if (pDetail.IsEmpty())
    {
        return DIR_TYPE_NONE;
    }

    return pDetail->GetType();
}

bool CPlcmContactControler::GetGroupTitle(int nGroupId, yl::string & strTitle)
{
    SmartPtr pDetail = Dir_GetDetailById(nGroupId, m_eLogicType);

    if (pDetail.IsEmpty())
    {
        return false;
    }

    strTitle.clear();

    int nParentId = Dir_GetParentIdById(nGroupId, m_eLogicType);
    int nType = pDetail->GetType();

    if ((nParentId != kszRootId && DIR_TYPE_REMOTE_MENU == nType))
    {
        ContactRemoteMenu * pPlcmMenu = (ContactRemoteMenu *)pDetail.Get();

        strTitle = pPlcmMenu->m_strTitle;
    }

    if (strTitle.empty())
    {
        strTitle = pDetail->GetDisplayName();
    }

    return true;
}

void CPlcmContactControler::UpdateContacts(int nGroupId)
{
    if (m_bUpdating)
    {
        return;
    }

    m_nDownLoadGroupId = kszPlcmDirGroupId;
    DIRUI_INFO("CPlcmContactControler::UpdateContacts(%d)", m_nDownLoadGroupId);

    int nRetCode = PclmDir_Update();

    if (DIR_RET_SUCC == nRetCode)
    {
        m_bUpdating = true;

        g_DirUIAdapter.ShowWaiting(TRID_NOTE, TRID_UPDATING_PLEASE_WAIT, DIR_ACTION_PLCM_UPDATE);
    }
    else
    {
        CCommonDirCtrl::ShowErrorMsg(nRetCode);
    }
}

bool CPlcmContactControler::OnBack()
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

int CPlcmContactControler::GetBrowseItemAmount()
{
    if (kszPlcmDirGroupId == m_nGroupId)
    {
        return GetGroupChildrenAmount(m_nGroupId, DIR_TYPE_REMOTE_MENU);
    }

    return CCommonDirCtrl::GetBrowseItemAmount();
}

bool CPlcmContactControler::PreLoadContact(int nGroupId)
{
    DIRUI_INFO("CPlcmContactControler::PreLoadContact(GroupId:%d)", nGroupId);

    if (1 != configParser_GetConfigInt(kszPlcmPhoneEnable))
    {
        return false;
    }

    if (!CCommonDirCtrl::PreLoadContact(nGroupId))
    {
        return false;
    }

    FileDownloadStatus eStatus = FILE_DOWNLOAD_STATUS_NONE;

    if (kszRootId == nGroupId)
    {
        eStatus = FILE_DOWNLOADED_OK;
        return true;
    }

    SmartPtr pDetail = Dir_GetDetailById(nGroupId, m_eLogicType);
    DirGroupRecord * pMenu = GetGroupRecord(pDetail);

    if (NULL == pMenu)
    {
        DIRUI_ERR("CPlcmContactControler::PreLoadContact(%d) not group", nGroupId);
        return false;
    }

    eStatus = pMenu->m_eStatus;

    if (FILE_DOWNLOADED_OK == eStatus)
    {
        return true;
    }

    m_nDownLoadGroupId = nGroupId;
    m_bDownloading = true;

    if (FILE_DOWNLOADING != eStatus)
    {
        int nRetCode = PclmDir_ReqLoadContact(nGroupId);

        if (DIR_RET_SUCC != nRetCode)
        {
            CCommonDirCtrl::ShowErrorMsg(nRetCode);
            return true;
        }
    }

    g_DirUIAdapter.ShowWaiting(TRID_NOTE, TRID_UPDATING_PLEASE_WAIT, DIR_ACTION_PLCM_DOWNLOAD);
    return true;
}

bool CPlcmContactControler::OnAction(int nAction, int nDataIndex, int nId)
{
    DIRUI_INFO("CPlcmContactControler::OnAction(Action:%d Index:%d Id:%d)",
               nAction, nDataIndex, nId);

    if (CCommonDirCtrl::OnAction(nAction, nDataIndex, nId))
    {
        return true;
    }

    switch (nAction)
    {
    case DIR_ACTION_PLCM_UPDATE:
        {
            UpdateContacts(nId);
        }
        break;
    case DIR_ACTION_LOCAL_ENTER_COPY_TO_CONTACTS:
        {
            OnCopyContact(nId, DIR_ACTION_LOCAL_COPY_TO_CONTACTS, kszAllContactId);
        }
        break;
    case DIR_ACTION_LOCAL_ENTER_COPY_TO_BLACKLIST:
        {
            OnCopyContact(nId, DIR_ACTION_LOCAL_COPY_TO_BLACKLIST, kszBlackListGroupId);
        }
        break;
#ifdef DIR_SUPPORT_BATCH_OPERATE
    case DIR_ACTION_LOCAL_BATCH_MOVE_CONTACT:
        {
            IDirUIHelper * pUIHelper = m_pUIHelper.Get();

            if (NULL == pUIHelper)
            {
                return false;
            }

            int nAllContactSize = Dir_GetContactCount(kszAllContactId, LOCAL_DIR);
            VEC_CONTACT_ID vecSelectId;

            if (!pUIHelper->GetSelectContactId(vecSelectId))
            {
                return false;
            }

            if ((nAllContactSize + vecSelectId.size()) > DIRECTORY_MAX_CONTACT_SIZE)
            {
                g_DirUIAdapter.ShowMsgBox(TRID_ONLY_1000_CONTACTS_ALLOWED);
                return false;
            }

            return pUIHelper->EnterSelectGroup(DIRECTORY_TYPE_LOCAL, kszBlackListGroupId, true, nAction);
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
        {
            DIRUI_ERR("Action:%d is not be Processed in OnAction", nAction);
            return false;
        }
        break;
    }

    return true;
}

bool CPlcmContactControler::ExecAction(int nAction, int nDataIndex, int nId)
{
    switch (nAction)
    {
#ifdef DIR_SUPPORT_BATCH_OPERATE
    case DIR_ACTION_LOCAL_BATCH_MOVE_TO_BLACKLIST:
        {
            IDirUIHelper * pUIHelper = m_pUIHelper.Get();

            if (NULL == pUIHelper)
            {
                return false;
            }

            int nBlackListSize = Dir_GetContactCount(kszBlackListGroupId, LOCAL_DIR);
            VEC_CONTACT_ID vecSelectId;

            if (!pUIHelper->GetSelectContactId(vecSelectId))
            {
                return false;
            }

            if ((nBlackListSize + vecSelectId.size()) > DIRECTORY_MAX_BLACKLIST_SIZE)
            {
                g_DirUIAdapter.ShowMsgBox(TRID_ONLY_30_CONTACTS_ALLOWED);
                return false;
            }

            return CopyToLocalContact(kszBlackListGroupId, vecSelectId);
        }
        break;
    case DIR_ACTION_LOCAL_BATCH_MOVE_CONTACT:
        {
            IDirUIHelper * pUIHelper = m_pUIHelper.Get();

            if (NULL == pUIHelper)
            {
                return false;
            }

            VEC_CONTACT_ID vecSelectId;

            if (!pUIHelper->GetSelectContactId(vecSelectId))
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

bool CPlcmContactControler::CancelAction(int nAction, int nDataIndex, int nId)
{
    IDirUIHelper * pUIHelper = m_pUIHelper.Get();

    if (NULL == pUIHelper)
    {
        return false;
    }

    if (DIR_ACTION_PLCM_UPDATE == nAction || DIR_ACTION_PLCM_DOWNLOAD == nAction)
    {
        if (m_bUpdating || m_bDownloading)
        {
            Dir_CancelLoad(GetGroupId(), m_eLogicType);
        }

        m_bUpdating = false;
        m_bDownloading = false;
        pUIHelper->RefreshUI(DIR_UI_REFRESH_TYPE_LIST_VIEW | DIR_UI_REFRESH_TYPE_TITLE,
                             DIR_UI_REFRESH_PARAM_NONE);
        return true;
    }

    return false;
}

#ifdef DIR_SUPPORT_BATCH_OPERATE
bool CPlcmContactControler::CopyToLocalContact(int nGroupId, VEC_CONTACT_ID & vecContactId)
{
    int nSize = vecContactId.size();
    YLList<ContactGeneralItemData> listData;

    for (int i = 0; i < nSize; ++i)
    {
        int nContactId = vecContactId[i];
        SmartPtr pDetail = Dir_GetDetailById(nContactId, m_eLogicType);
        ContactRemoteUnit * pPlcmUnit = GetPlcmContact(pDetail);

        if (NULL == pPlcmUnit)
        {
            continue;
        }

        ContactGeneralItemData itemData;

        CLocalContactControler::GetCopyDefaultContact(itemData);

        itemData.m_strDisplayName = pPlcmUnit->m_strDisplayName;
        itemData.m_strOfficePhoneNumber = pPlcmUnit->GetNumberByIndex(0);
        itemData.m_strMobilePhoneNumber = pPlcmUnit->GetNumberByIndex(1);
        itemData.m_strOtherPhoneNumber = pPlcmUnit->GetNumberByIndex(2);

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
            g_DirUIAdapter.ShowMsgBox(TRID_MOVE_BLACKLIST_SUCCESS);
        }
        else
        {
            g_DirUIAdapter.ShowMsgBox(TRID_MOVE_CONTACTS_SUCCESS);
        }
    }
    else if (DIR_RET_GROUP_SIZE_UNEXPECTED == nRetCode)
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
        CCommonDirCtrl::ShowErrorMsg(nRetCode);
    }

    return true;
}
#endif

void CPlcmContactControler::PrepareDetailData()
{
    m_vecDetail.clear();

    SmartPtr pDetail = GetCurrentDetail();
    ContactRemoteUnit * pRemoteUnit = GetPlcmContact(pDetail);

    if (NULL == pRemoteUnit)
    {
        return;
    }

    CRemoteContactControler::PrepareRemoteUnitDetail(pRemoteUnit, m_vecDetail);
}

void CPlcmContactControler::OnCopyContact(int nContactId, int nAction, int nTarGroupId)
{
    IDirUIHelper * pUIHelper = m_pUIHelper.Get();

    if (NULL == pUIHelper)
    {
        return;
    }

    SmartPtr pDetail = Dir_GetDetailById(nContactId, m_eLogicType);
    ContactRemoteUnit * pRecord = GetPlcmContact(pDetail);

    if (NULL == pRecord)
    {
        DIRUI_ERR("GetPlcmContact fail OnCopyContact(Action:%d Id:%d)", nAction, nContactId);
        return;
    }

    LIST_ATTRIBUTE lsAttr;
    ST_CONTACT_ATTRIBUTE stAttr;

    stAttr.m_strName = DIR_ATTR_NAME;
    stAttr.m_strValue = pRecord->GetDisplayName();
    lsAttr.push_back(stAttr);

    CRemoteContactControler::GetNumberAttr(pRecord, lsAttr, true);

    SetAttributeList(m_nType, lsAttr);

    g_DirUIAdapter.EnterUI(DIRECTORY_TYPE_LOCAL, DIR_UI_STATE_ADDITION, nAction, nTarGroupId);
}

bool CPlcmContactControler::DialContact(int nContactId, int nAccountId/*  = AUTO_ACCOUNT_IDENTIFY*/)
{
    SmartPtr pDetail = Dir_GetDetailById(nContactId, m_eLogicType);
    ContactRemoteUnit * pRecord = GetPlcmContact(pDetail);

    if (NULL == pRecord)
    {
        DIRUI_ERR("GetPlcmContact fail DialContact(ContactId:%d)", nContactId);
        return false;
    }

    m_nContactId = nContactId;

    ST_CONTACT_ATTRIBUTE stAttr;
    LIST_ATTRIBUTE lsNumber;

    CRemoteContactControler::GetNumberAttr(pRecord, lsNumber, false);

    g_DirUIAdapter.Dial(nAccountId, pRecord->GetDisplayName(), lsNumber, GetDialAction());

    return true;
}

LRESULT CPlcmContactControler::OnMessage(msgObject & refMessage)
{
    if (LOCAL_SEARCH_RESULT != refMessage.message
            && PLCM_PHONEBOOK_ALL_DOWNLOADED_FINISHED != refMessage.message)
    {
        return FALSE;
    }

    switch (refMessage.message)
    {
    case PLCM_PHONEBOOK_ALL_DOWNLOADED_FINISHED:
        {
            DIRUI_INFO("Receive PLCM_PHONEBOOK_ALL_DOWNLOADED_FINISHED message");

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
        break;
    }

    return FALSE;
}

void CPlcmContactControler::OnDownLoadFinished(msgObject & refMessage)
{
    IPLCMUIHelper * pPLCMUIHelper = g_DirUIAdapter.GetPLCMUIHelper(m_pUIHelper);

    if (NULL == pPLCMUIHelper)
    {
        return;
    }

    int nGroupId = refMessage.wParam;

    DIRUI_INFO("CPlcmContactControler::OnDownLoadFinished(CurGroupId:%d GroupId:%d lp:%d)",
               m_nDownLoadGroupId, nGroupId, refMessage.lParam);

    // 当前组更新消息到时就要处理
    if (nGroupId == m_nDownLoadGroupId)
    {
        if (m_bUpdating || m_bDownloading)
        {
            g_DirUIAdapter.EndWaiting();
        }

        m_bUpdating = false;
        m_bDownloading = false;

        pPLCMUIHelper->OnDownLoadGroup(m_nDownLoadGroupId, (0 == refMessage.lParam));

        if (!m_pUIHelper.IsEmpty() && GetGroupId() == m_nDownLoadGroupId)
        {
            m_pUIHelper->RefreshUI(DIR_UI_REFRESH_TYPE_LIST_VIEW | DIR_UI_REFRESH_TYPE_TITLE,
                                   DIR_UI_REFRESH_PARAM_ALL);
        }

        return;
    }

    if (kszRootId == m_nDownLoadGroupId)
    {
        if (m_bUpdating || m_bDownloading)
        {
            g_DirUIAdapter.EndWaiting();
        }

        m_bUpdating = false;
        m_bDownloading = false;

        if (!m_pUIHelper.IsEmpty())
        {
            m_pUIHelper->RefreshUI(DIR_UI_REFRESH_TYPE_LIST_VIEW | DIR_UI_REFRESH_TYPE_TITLE,
                                   DIR_UI_REFRESH_PARAM_DATA_CHANGE);
        }
    }
    else if (Dir_IsAllLoadFinish(m_nDownLoadGroupId, m_eLogicType))
    {
        pPLCMUIHelper->OnFinishAllDownLoad(GetGroupId());
    }
}
#endif
