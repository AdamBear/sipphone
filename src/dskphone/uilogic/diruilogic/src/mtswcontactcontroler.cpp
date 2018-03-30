#if IF_FEATURE_METASWITCH_DIRECTORY
#include "mtswcontactcontroler.h"
#include "metaswitch/mtswcontrol/include/modmtsw.h"
#include "settingui/include/modsettingui.h"
#include "metaswitch/mtswcontrol/include/mtsw_messagedefine.h"
#include "diruiadapter.h"
#include "translateiddefine.h"
#include "uilogiccommon/include/uilogiclanghelper.h"
#include "taskaction/common.h"
#include "directorycommon.h"

CMTSWContactControler::CMTSWContactControler()
    : CCommonDirCtrl(DIRECTORY_TYPE_METASWICTH, MTSW_DIR)
{
    m_bDownloading = false;
    m_bUpdating = false;
    m_nFailResult = MTSW_DOWNLOAD_RESULT_SUCCESS;
    m_nDownloadGroupId = kszAllContactId;
}

CMTSWContactControler::~CMTSWContactControler()
{

}

CMTSWContactControler * CMTSWContactControler::GetControler(CDirectoryControlerPtr & pControler)
{
    if (pControler.IsEmpty() || DIRECTORY_TYPE_METASWICTH != pControler->GetType())
    {
        return NULL;
    }

    return (CMTSWContactControler *)pControler.Get();
}

ContactMetaSwitchContactList* CMTSWContactControler::GetMetaSwitchContact(SmartPtr & pDetail)
{
    if (pDetail.IsEmpty() || DIR_TYPE_METASWITCH_ITEM_CONTACTLIST != pDetail->GetType())
    {
        return NULL;
    }

    return (ContactMetaSwitchContactList *)pDetail.Get();
}

bool CMTSWContactControler::CheckNumberValid(const char * pszNumber, int nMaxLength/* = 32*/)
{
    if (NULL == pszNumber)
    {
        return false;
    }

    int nLength = strlen(pszNumber);

    if (nLength < 0 || nLength > nMaxLength)
    {
        return false;
    }

    //An E.164 number used for outgoing calls must contain only characters +, - and 0-9.
    //Only the leading character may be a +.
    for (int i = 0; i < nLength; ++i)
    {
        if ('+' == pszNumber[i])
        {
            if (i > 0)
            {
                return false;
            }
        }
        else if (pszNumber[i] != '-' && (pszNumber[i] < '0' || pszNumber[i] > '9') && pszNumber[i] != ' ')
        {
            return false;
        }
    }

    return true;
}

int CMTSWContactControler::GetGroupIdInAllGroup(int nIndex)
{
    VEC_CONTACT_ID vecId;

    GetAllGroup(vecId);

    if (nIndex < 0 || nIndex >= vecId.size())
    {
        return knInvalidRId;
    }

    return vecId[nIndex];
}

void CMTSWContactControler::GetAllGroup(VEC_CONTACT_ID & vecId)
{
    Dir_GetConfigMenuIdList(vecId, MTSW_DIR);

    ContactListData listData;

    listData.m_nIndexFrom = 0;
    listData.m_nCountToGet = 0;

    int nSize = listData.m_listId.size();
    for (int i = 0; i < nSize; ++i)
    {
        int nId = listData.m_listId[i];

        vecId.push_back(nId);
    }
}

void CMTSWContactControler::UpdateContacts(int nGroupId)
{
    if (knInvalidRId == nGroupId)
    {
        nGroupId = kszAllContactId;
    }

    if (!MTSW_IsLogined())
    {
        return ;
    }

    m_nDownloadGroupId = nGroupId;
    DIRUI_INFO("CMTSWContactControler::UpdateContacts(%d)", m_nDownloadGroupId);

    DIRECTORY_RET_CODE retCode = MtswDir_Update(nGroupId);
    if (DIR_RET_SUCC == retCode)
    {
        m_bUpdating = true;
        g_DirUIAdapter.ShowWaiting(TRID_NOTE, TRID_UPDATING_PLEASE_WAIT, DIR_ACTION_MTSW_UPDATE);
    }
    else
    {
        CCommonDirCtrl::ShowErrorMsg(retCode);
    }
}

bool CMTSWContactControler::OnDelContactList(int nId)
{
    return g_DirUIAdapter.PopSureMsgBox(TRID_WARNING, TRID_IF_DELETE_SELECTED_CONTACT_ITEM,
                                        DIR_ACTION_MTSW_DEL_CONTACTLIST, nId);
}

bool CMTSWContactControler::OnClearContact(int nId)
{
    return g_DirUIAdapter.PopSureMsgBox(TRID_WARNING, TRID_MTSW_CLEAR_ALL,
                                        DIR_ACTION_MTSW_DEL_ALL_CONTACTLIST, nId);
}

bool CMTSWContactControler::OnEnterCopyToContactList(int nAction, int nId)
{
    DIRUI_INFO("CMTSWContactControler::OnEnterCopyToContactList(Action:%d Id:%d)", nAction, nId);
    SmartPtr pDetail = Dir_GetDetailById(nId, m_eLogicType);
    if (pDetail.IsEmpty())
    {
        return false;
    }

    BaseContactRecord * pRecord = NULL;

    if (DIR_TYPE_METASWITCH_ITEM_EXTENTION != pDetail->GetType())
    {
        pRecord = (ContactMetaSwitchExtention *)pDetail.Get();
    }
    else if (DIR_TYPE_METASWITCH_ITEM_MLHG != pDetail->GetType())
    {
        pRecord = (ContactMetaSwitchMLHG *)pDetail.Get();
    }

    if (NULL == pRecord)
    {
        return false;
    }

    LIST_ATTRIBUTE lsAttr;
    ST_CONTACT_ATTRIBUTE stAttr;

    stAttr.m_strName = DIR_ATTR_NAME;
    stAttr.m_strValue = pRecord->GetDisplayName();
    if (stAttr.m_strValue.empty())
    {
        stAttr.m_strValue = pRecord->GetNumberByIndex(0);
    }

    lsAttr.push_back(stAttr);

    stAttr.m_strName = DIR_ATTR_NUMBER;
    stAttr.m_strValue = pRecord->GetNumberByIndex(0);
    lsAttr.push_back(stAttr);

    SetAttributeList(m_nType, lsAttr);
    g_DirUIAdapter.EnterUI(DIRECTORY_TYPE_LOCAL, DIR_UI_STATE_ADDITION,
                           DIR_ACTION_LOCAL_COPY_TO_CONTACTS, knInvalidRId);
    return true;
}

bool CMTSWContactControler::AddContactList(bool bFouceAdd /*= false*/)
{
    IMTSWUIHelper* pMTSWUIHelper = g_DirUIAdapter.GetMTSWUIHelper(m_pUIHelper);
    if (NULL == pMTSWUIHelper)
    {
        return false;
    }

    ContactMetaSwitchContactList itemData;

    if (!pMTSWUIHelper->GetMTSWContactListData(itemData))
    {
        return false;
    }

    if (!CheckContactListValid(itemData))
    {
        return false;
    }

    DIRUI_INFO("Addcontact(%s %s %d)", itemData.m_strDisplayName.c_str(),
               itemData.GetNumberByIndex(0).c_str(), bFouceAdd);

    if (Dir_IsContactNameExist(itemData.m_strDisplayName, MTSW_DIR) != -1 && bFouceAdd == false)
    {
        return CheckChangeContactList(DIR_RET_CONTACT_EXIST, DIR_ACTION_MTSW_ADD_CONTACTLIST);
    }

    int nRetCode = MtswDir_AddContact2ContactList(itemData, bFouceAdd);

    if (nRetCode == DIR_RET_GROUP_SIZE_UNEXPECTED)
    {
        return CheckChangeContactList(nRetCode, DIR_ACTION_MTSW_ADD_CONTACTLIST);
    }

    return CheckChangeContactList(nRetCode, DIR_ACTION_MTSW_ADD_CONTACTLIST_FINISH);
}

bool CMTSWContactControler::DelContactList(int nContactId)
{
    VEC_CONTACT_ID vecId;

    vecId.push_back(nContactId);

    int nRetCode = MtswDir_DeleteContactList(vecId);

    bool bSuccess = CheckDeleteContactList(nRetCode);

    if (bSuccess)
    {
        IDirUIHelper * pUIHelper = m_pUIHelper.Get();
        if (NULL != pUIHelper)
        {
            pUIHelper->RefreshUIAfterDeleted();
        }
    }

    return bSuccess;
}

bool CMTSWContactControler::ClearContactList(int nGroupId)
{
    MtswDir_ClearContact(nGroupId);

    RefreshAfterContactListChange();

    return true;
}

bool CMTSWContactControler::SaveContactList(bool bFouceEdit /*= false*/)
{
    IMTSWUIHelper* pMTSWUIHelper = g_DirUIAdapter.GetMTSWUIHelper(m_pUIHelper);
    if (NULL == pMTSWUIHelper)
    {
        return false;
    }

    ContactMetaSwitchContactList itemData;

    if (!pMTSWUIHelper->GetMTSWContactListData(itemData))
    {
        return false;
    }

    if (!CheckContactListValid(itemData))
    {
        return false;
    }

    int nRetCode = MtswDir_EditContactList(m_nContactId, itemData, bFouceEdit);

    if (DIR_RET_CONTACTID_UNEXISTED == nRetCode)
    {
        g_DirUIAdapter.ShowMsgBox(TRID_SAVE_FAILED);
        g_DirUIAdapter.ExitCurrentUI(DIR_UI_STATE_DETAIL);
    }
    else if (DIR_RET_CONTACT_EXIST == nRetCode)
    {
        return CheckChangeContactList(nRetCode, DIR_ACTION_MTSW_EDIT_CONTACTLIST);
    }

    return CheckChangeContactList(nRetCode, DIR_ACTION_MTSW_EDIT_CONTACTLIST_FINISH);
}


bool CMTSWContactControler::SortContactList()
{
    MtswDir_SortContactList();
    RefreshAfterContactListChange();
    return true;
}

bool CMTSWContactControler::IsContactList(int nContactId)
{
    SmartPtr pDetail = Dir_GetDetailById(nContactId, MTSW_DIR);

    if (pDetail.IsEmpty() || DIR_TYPE_METASWITCH_ITEM_CONTACTLIST != pDetail->GetType())
    {
        return false;
    }

    return true;
}

bool CMTSWContactControler::IsCurrentContactList()
{
    return IsContactList(m_nGroupId);
}

int CMTSWContactControler::GetBrowseItemAmount()
{
    if (knInvalidRId == m_nGroupId)
    {
        VEC_CONTACT_ID vecID;

        Dir_GetConfigMenuIdList(vecID, m_eLogicType);

        return vecID.size();
    }

    return GetGroupChildrenAmount(m_nGroupId, DIR_TYPE_METASWITCH_MENU);
}

bool CMTSWContactControler::IsGroupData(SmartPtr & pDetail)
{
    if (pDetail.IsEmpty())
    {
        return false;
    }

    if (DIR_TYPE_METASWITCH_MENU == pDetail->GetType())
    {
        return true;
    }

    return CCommonDirCtrl::IsGroupData(pDetail);
}

bool CMTSWContactControler::PreLoadContact(int nGroupId)
{
    DIRUI_INFO("CMTSWContactControler::PreLoadContact(%d)", nGroupId);

    if (!CCommonDirCtrl::PreLoadContact(nGroupId))
    {
        return false;
    }

    //若没有登录Commportal则跳转登录界面
    if (!MTSW_IsLogined())
    {
        DIR_INFO("MTSW_IsLogined fail, enter login page");
        IMTSWUIHelper* pMTSWUIHelper = g_DirUIAdapter.GetMTSWUIHelper(m_pUIHelper);
        if (NULL != pMTSWUIHelper)
        {
            pMTSWUIHelper->EnterMTSWLoginPage();
        }
        return true;
    }

    SmartPtr pDetail = Dir_GetDetailById(nGroupId, m_eLogicType);
    DirGroupRecord * pMenu = GetGroupRecord(pDetail);
    if (NULL == pMenu)
    {
        return false;
    }

    int nStatus = pMenu->m_eStatus;

    if (FILE_DOWNLOADED_OK == nStatus)
    {
        return true;
    }

    m_nDownloadGroupId = nGroupId;

    if (FILE_DOWNLOADING == nStatus)
    {
        m_bDownloading = true;
        g_DirUIAdapter.ShowWaiting(TRID_NOTE, TRID_UPDATING_PLEASE_WAIT, DIR_ACTION_MTSW_UPDATE);
    }
    return true;
}

bool CMTSWContactControler::OnAction(int nAction, int nDataIndex, int nId)
{
    DIRUI_INFO("CMTSWContactControler::OnAction(Action:%d Index:%d Id:%d)",
               nAction, nDataIndex, nId);

    if (CCommonDirCtrl::OnAction(nAction, nDataIndex, nId))
    {
        return true;
    }

    switch (nAction)
    {
    case DIR_ACTION_MTSW_UPDATE:
        {
            if (kszContactList == nId || kszMLHG == nId || kszExtention == nId)
            {
                UpdateContacts(0);
            }
            else
            {
                UpdateContacts(GetGroupId());
            }
        }
        break;
    case DIR_ACTION_MTSW_ENTER_ADD_CONTACTLISTL:
        {
            g_DirUIAdapter.EnterUI(m_nType, DIR_UI_STATE_ADDITION, DIR_ACTION_MTSW_ADD_CONTACTLIST, GetGroupId());
        }
        break;
    case DIR_ACTION_MTSW_ADD_CONTACTLIST:
    case DIR_ACTION_MTSW_ADD_TO_CONTACTLIST:
        {
            return AddContactList();
        }
        break;
    case DIR_ACTION_MTSW_ENTER_EDIT_CONTACTLISTL:
        {
            if (knInvalidRId != nId)
            {
                m_nContactId = nId;

                m_nDetailStatus = GetState();
#ifdef DIR_SUPPORT_SWITCH_DETAIL
                SetCurIndex(nDataIndex);
#endif
                g_DirUIAdapter.EnterUI(m_nType, DIR_UI_STATE_DETAIL, DIR_ACTION_MTSW_EDIT_CONTACTLIST, nId);
            }
        }
        break;;
    case DIR_ACTION_MTSW_EDIT_CONTACTLIST:
        {
            return SaveContactList();
        }
        break;
    case DIR_ACTION_MTSW_DEL_CONTACTLIST:
        {
            return OnDelContactList(nId);
        }
        break;
    case DIR_ACTION_MTSW_DEL_ALL_CONTACTLIST:
        {
            return OnClearContact(GetGroupId());
        }
        break;
    case DIR_ACTION_LOCAL_ENTER_COPY_TO_CONTACTS:
    case DIR_ACTION_MTSW_ENTER_ADD_TO_LOCAL:
        {
            return OnEnterCopyToContactList(nAction, nId);
        }
        break;
    case DIR_ACTION_MTSW_SORT:
        {
            return SortContactList();
        }
        break;
    default:
        {
            DIRUI_ERR("Action:%d is not be Processed in OnAction", nAction);
            return false;
        }
        break;
    }

    return true;
}
bool CMTSWContactControler::ExecAction(int nAction, int nDataIndex, int nId)
{
    DIRUI_INFO("CMTSWContactControler::ExecAction(Action:%d Index:%d Id:%d)",
               nAction, nDataIndex, nId);

    switch (nAction)
    {
    case DIR_ACTION_MTSW_DEL_CONTACTLIST:
        {
            DelContactList(nId);
        }
        break;
    case DIR_ACTION_MTSW_DEL_ALL_CONTACTLIST:
        {
            ClearContactList(GetGroupId());
        }
        break;
    case DIR_ACTION_MTSW_ADD_CONTACTLIST:
        {
            AddContactList(true);
        }
        break;
    case DIR_ACTION_MTSW_EDIT_CONTACTLIST:
        {
            SaveContactList(true);
        }
        break;
    case DIR_ACTION_MTSW_ADD_CONTACTLIST_FINISH:
    case DIR_ACTION_MTSW_EDIT_CONTACTLIST_FINISH:
        {
            int nCheckUIState = DIR_UI_STATE_ADDITION;
            int nUIState = GetState();

            if (DIR_ACTION_MTSW_EDIT_CONTACTLIST_FINISH == nAction)
            {
                nCheckUIState = DIR_UI_STATE_DETAIL;
            }

            if (nCheckUIState != nUIState)
            {
                break;
            }

            g_DirUIAdapter.ExitCurrentUI(nUIState);

            if (IsSearchState())
            {
                RefreshAfterContactListChange();
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

bool CMTSWContactControler::DialContact(int nContactId, int nAccountId /*= AUTO_ACCOUNT_IDENTIFY*/)
{
    SmartPtr pDetail = GetDetail(nContactId);

    if (pDetail.IsEmpty())
    {
        return false;
    }

    m_nContactId = nContactId;

    LIST_ATTRIBUTE lsNumber;
    BaseContactRecord* pRecord = (BaseContactRecord*)pDetail.Get();
    if (NULL == pRecord)
    {
        return false;
    }

    switch (pDetail->GetType())
    {
    case DIR_TYPE_METASWITCH_ITEM_CONTACTLIST:
        {
            GetNumberByName(pRecord, lsNumber, kszMtswHomeNumber, TRID_MTSW_HOME_NUMBER);
            GetNumberByName(pRecord, lsNumber, kszMtswWorkNumber, TRID_MTSW_WORK_NUMBER);
            GetNumberByName(pRecord, lsNumber, kszMtswMobile, TRID_MTSW_MOBILE_NUMBER);
            GetNumberByName(pRecord, lsNumber, kszMtswFax, TRID_MTSW_FAX_NUMBER);
            GetNumberByName(pRecord, lsNumber, kszMtswOtherNumber, TRID_MTSW_OTHER_NUMBER);
        }
        break;
    case DIR_TYPE_METASWITCH_ITEM_EXTENTION:
        {
            GetNumberByName(pRecord, lsNumber, kszMtswExtention, TRID_MTSW_EXTENTION_NUMBER);
            GetNumberByName(pRecord, lsNumber, kszMtswDirectoryNumber, TRID_NUMBER);
        }
        break;
    case DIR_TYPE_METASWITCH_ITEM_MLHG:
        {
            GetNumberByName(pRecord, lsNumber, kszMtswDirectoryNumber, TRID_NUMBER);
        }
        break;
    default:
        break;
    }

    return Dial(nAccountId, pDetail->GetDisplayName(), lsNumber, GetDialAction());
}

bool CMTSWContactControler::CancelAction(int nAction, int nDataIndex, int nId)
{
    if (DIR_ACTION_MTSW_UPDATE == nAction || DIR_ACTION_MTSW_DOWNLOAD == nAction)
    {
        if (knInvalidRId != m_nDownloadGroupId && (m_bDownloading || m_bUpdating))
        {
            MtswDir_CancelLoad();
        }

        m_bDownloading = false;
        m_bUpdating = false;
        m_nDownloadGroupId = knInvalidRId;

        RefreshUI(DIR_UI_REFRESH_TYPE_NONE, DIR_UI_REFRESH_PARAM_NONE);

        return true;
    }

    return false;
}

LRESULT CMTSWContactControler::OnMessage(msgObject & refMessage)
{
    DIRUI_INFO("CMTSWContactControler::OnMessage() msg:%d wp:%d lp:%d",
               refMessage.message, refMessage.wParam, refMessage.lParam);

    LRESULT retValue = FALSE;
    switch (refMessage.message)
    {
    case LOCAL_SEARCH_RESULT:
        {
            OnSearchResult(refMessage);
            retValue = TRUE;
        }
        break;
    case METASWITCH_PHONEBOOK_DOWNLOADED_RESULT:
        {
            OnDownLoadFinished(refMessage);
        }
        break;
    case MTSW_MESSAGE_LOGIN:
    case METASWITCH_PHONEBOOK_RELOAD:
        {
            m_nDownloadGroupId = knInvalidRId;
            SetGroupId(knInvalidRId);

            if (m_bUpdating || m_bDownloading)
            {
                g_DirUIAdapter.EndWaiting();

                m_bUpdating = false;
                m_bDownloading = false;
            }

            g_DirUIAdapter.ExitAllUI(m_nType);

            RefreshUI(DIR_UI_REFRESH_TYPE_NONE, DIR_UI_REFRESH_PARAM_NONE);
        }
        break;
    case METASWITCH_PHONEBOOK_POST_RESULT:
        {
            IMTSWUIHelper* pMTSWUIHelper = g_DirUIAdapter.GetMTSWUIHelper(m_pUIHelper);

            if (NULL != pMTSWUIHelper && 1 == refMessage.lParam)
            {
                RefreshAfterContactListChange();
            }
        }
        break;
    default:
        break;
    }

    return retValue;
}

void CMTSWContactControler::OnLoginSuccess()
{
    DIRUI_DEBUG("CMTSWContactControler::OnLoginSuccess");
    m_bUpdating = true;
    g_DirUIAdapter.ShowWaiting(TRID_NOTE, TRID_UPDATING_PLEASE_WAIT, DIR_ACTION_MTSW_UPDATE);
}

void CMTSWContactControler::PrepareDetailData()
{
    m_vecDetail.clear();

    SmartPtr pDetail = GetCurrentDetail();

    if (pDetail.IsEmpty())
    {
        return;
    }

    int nType = pDetail->GetType();
    if (DIR_TYPE_METASWITCH_ITEM_CONTACTLIST == nType)
    {
        PrepareContactlistDetail((ContactMetaSwitchContactList *)pDetail.Get());
    }
    if (DIR_TYPE_METASWITCH_ITEM_EXTENTION == nType)
    {
        PrepareExtentionDetail((ContactMetaSwitchExtention *)pDetail.Get());
    }
    if (DIR_TYPE_METASWITCH_ITEM_MLHG == nType)
    {
        PrepareMLHGDetail((ContactMetaSwitchMLHG *)pDetail.Get());
    }
}

void CMTSWContactControler::PrepareContactlistDetail(ContactMetaSwitchContactList * pContact)
{
    if (NULL == pContact)
    {
        return;
    }

    ST_DETAIL_DATA stDetail;

    stDetail.nType = DETAIL_DATA_TYPE_NONE;
    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_FIRST_NAME);
    stDetail.stAttr.m_strValue = pContact->m_strFirstName;
    m_vecDetail.push_back(stDetail);

    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_LAST_NAME);
    stDetail.stAttr.m_strValue = pContact->m_strLastName;
    m_vecDetail.push_back(stDetail);

    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_MTSW_NICKNAME);
    stDetail.stAttr.m_strValue = pContact->m_strNickName;
    m_vecDetail.push_back(stDetail);

    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_MTSW_JOBTITLE);
    stDetail.stAttr.m_strValue = pContact->m_strJobTitle;
    m_vecDetail.push_back(stDetail);

    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_MTSW_ORGANISATION);
    stDetail.stAttr.m_strValue = pContact->m_strOrganisation;
    m_vecDetail.push_back(stDetail);

    AddDetailData(pContact, TRID_MTSW_HOME_NUMBER, kszMtswHomeNumber);
    AddDetailData(pContact, TRID_MTSW_WORK_NUMBER, kszMtswWorkNumber);
    AddDetailData(pContact, TRID_MTSW_MOBILE_NUMBER, kszMtswMobile);
    AddDetailData(pContact, TRID_MTSW_FAX_NUMBER, kszMtswFax);
    AddDetailData(pContact, TRID_MTSW_OTHER_NUMBER, kszMtswOtherNumber);
}

void CMTSWContactControler::AddDetailData(ContactMetaSwitchContactList* pContact
        , const yl::string& strDialName, const yl::string& strNumberName)
{
    if (NULL == pContact)
    {
        return;
    }

    ST_DETAIL_DATA stDetail;
    stDetail.nType = DETAIL_DATA_TYPE_NUMBER;
    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(strDialName.c_str());
    if (!pContact->GetNumberByName(strNumberName, stDetail.stAttr.m_strValue))
    {
        stDetail.stAttr.m_strValue = "";
    }
    m_vecDetail.push_back(stDetail);
}

void CMTSWContactControler::PrepareExtentionDetail(ContactMetaSwitchExtention * pContact)
{
    if (NULL == pContact)
    {
        return;
    }

    ST_DETAIL_DATA stDetail;
    stDetail.nType = DETAIL_DATA_TYPE_NONE;
    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_NAME);
    stDetail.stAttr.m_strValue = pContact->GetDisplayName();
    m_vecDetail.push_back(stDetail);

    stDetail.nType = DETAIL_DATA_TYPE_NUMBER;
    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_MTSW_NUMBER);
    pContact->GetNumberByName(kszMtswDirectoryNumber, stDetail.stAttr.m_strValue);
    m_vecDetail.push_back(stDetail);

    stDetail.nType = DETAIL_DATA_TYPE_NONE;
    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_DEPARTMENT);
    stDetail.stAttr.m_strValue = pContact->m_strDepartment;
    m_vecDetail.push_back(stDetail);

    stDetail.nType = DETAIL_DATA_TYPE_NUMBER;
    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_MTSW_EXTENTION_NUMBER);
    pContact->GetNumberByName(kszMtswExtention, stDetail.stAttr.m_strValue);
    m_vecDetail.push_back(stDetail);
}

void CMTSWContactControler::PrepareMLHGDetail(ContactMetaSwitchMLHG * pContact)
{
    if (NULL == pContact)
    {
        return;
    }

    ST_DETAIL_DATA stDetail;
    stDetail.nType = DETAIL_DATA_TYPE_NONE;
    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_NAME);
    stDetail.stAttr.m_strValue = pContact->GetDisplayName();
    m_vecDetail.push_back(stDetail);

    stDetail.nType = DETAIL_DATA_TYPE_NUMBER;
    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_MTSW_NUMBER);
    pContact->GetNumberByName(kszMtswDirectoryNumber, stDetail.stAttr.m_strValue);
    m_vecDetail.push_back(stDetail);

    stDetail.nType = DETAIL_DATA_TYPE_NONE;
    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_DEPARTMENT);
    stDetail.stAttr.m_strValue = pContact->m_strDepartment;
    m_vecDetail.push_back(stDetail);
}

void CMTSWContactControler::GetNumberByName(BaseContactRecord* pContact, LIST_ATTRIBUTE & lsAttr
        , const yl::string& strName, const yl::string& strDispName)
{
    if (NULL == pContact)
    {
        return ;
    }

    ST_CONTACT_ATTRIBUTE stAttr;

    if (pContact->GetNumberByName(strName, stAttr.m_strValue) && !stAttr.m_strValue.empty())
    {
        stAttr.m_strName = strDispName;
        lsAttr.push_back(stAttr);
    }
}

bool CMTSWContactControler::CheckContactListValid(ContactMetaSwitchContactList& itemData)
{
    if (itemData.m_strDisplayName.empty())
    {
        g_DirUIAdapter.ShowMsgBox(TRID_MTSW_NAME_CANNOT_BE_EMPTY);
        IMTSWUIHelper* pMTSWUIHelper = g_DirUIAdapter.GetMTSWUIHelper(m_pUIHelper);
        if (NULL != pMTSWUIHelper)
        {
            pMTSWUIHelper->SetInputFocus(DIR_ATTR_NAME);
        }

        return false;
    }

    yl::string strFocusItem = "";

    if (!CheckNumberValidByName(itemData, kszMtswHomeNumber, DIR_ATTR_MTSW_HOMENUMBER, strFocusItem)
            || !CheckNumberValidByName(itemData, kszMtswWorkNumber, DIR_ATTR_MTSW_WORKNUMBER, strFocusItem)
            || !CheckNumberValidByName(itemData, kszMtswMobile, DIR_ATTR_MTSW_MOBILENUMBER, strFocusItem)
            || !CheckNumberValidByName(itemData, kszMtswFax, DIR_ATTR_MTSW_FAXNUMBER, strFocusItem)
            || !CheckNumberValidByName(itemData, kszMtswOtherNumber, DIR_ATTR_MTSW_OTHERNUMBER, strFocusItem)
       )
    {
        g_DirUIAdapter.ShowMsgBox(TRID_MTSW_NUMBER_INVALID);
        IMTSWUIHelper* pMTSWUIHelper = g_DirUIAdapter.GetMTSWUIHelper(m_pUIHelper);
        if (NULL != pMTSWUIHelper)
        {
            pMTSWUIHelper->SetInputFocus(strFocusItem.c_str());
        }
        return false;
    }

    return true;
}

bool CMTSWContactControler::CheckNumberValidByName(ContactMetaSwitchContactList& itemData, const yl::string& strName,
        const yl::string& strDefaultError, yl::string& strError)
{
    yl::string strNumber;

    itemData.GetNumberByName(kszMtswHomeNumber, strNumber);
    if (!CheckNumberValid(strNumber.c_str()))
    {
        strError = strDefaultError;
        return false;
    }

    return true;
}

bool CMTSWContactControler::CheckChangeContactList(int nRetCode, int nAction)
{
    if (DIR_RET_SUCC == nRetCode || nRetCode > 0)
    {
        if (DIR_ACTION_MTSW_ADD_CONTACTLIST_FINISH == nAction)
        {
            g_DirUIAdapter.ShowWaiting(TRID_NOTE, TRID_SAVING_FILE, nAction);
        }
        else
        {
            g_DirUIAdapter.ShowMsgBox(TRID_SAVING_FILE, NULL, nAction);
        }
        return true;
    }
    else if (DIR_RET_CONTACT_EXIST == nRetCode)
    {
        g_DirUIAdapter.PopSureMsgBox(TRID_WARNING, TRID_MTSW_EXISTS_CONTACT,
                                     nAction, knInvalidRId);
    }
    else if (DIR_RET_FAIL == nRetCode)
    {
        g_DirUIAdapter.ShowMsgBox(TRID_SAVE_FAILED);
    }
    else
    {
        CCommonDirCtrl::ShowErrorMsg(nRetCode);
    }

    return false;
}

bool CMTSWContactControler::CheckDeleteContactList(int nRetCode)
{
    if (DIR_RET_SUCC == nRetCode)
    {
        g_DirUIAdapter.ShowMsgBox(_UILOGIC_LANG_TRAN(TRID_DELETING).c_str());
        return true;
    }
    else if (DIR_RET_FAIL == nRetCode)
    {
        g_DirUIAdapter.ShowMsgBox(_UILOGIC_LANG_TRAN(TRID_DELETE_FIALED).c_str());
    }
    else
    {
        CCommonDirCtrl::ShowErrorMsg(nRetCode);
    }

    return false;
}

void CMTSWContactControler::OnDownLoadFinished(msgObject & refMessage)
{
    DIRUI_INFO("CMTSWContactControler::OnDownLoadFinished[%d][%d]", refMessage.lParam,
               refMessage.wParam);

    IMTSWUIHelper* pMTSWUIHelper = g_DirUIAdapter.GetMTSWUIHelper(m_pUIHelper);
    if (NULL == pMTSWUIHelper)
    {
        return;
    }

    if (pMTSWUIHelper->IsAddition())
    {
        g_DirUIAdapter.EndWaiting();
        g_DirUIAdapter.ExitCurrentUI(GetState());
        m_bUpdating = false;
    }

    int nGroupId = refMessage.wParam;
    // AllGroup组不会收到消息，因此不需要判断
    if (nGroupId == m_nDownloadGroupId)
    {
        m_nDownloadGroupId = knInvalidRId;

        if (m_bUpdating || m_bDownloading)
        {
            g_DirUIAdapter.EndWaiting();
        }

        m_bUpdating = false;
        m_bDownloading = false;

        if (1 != refMessage.lParam)
        {
            if (refMessage.lParam == NS_TA::TA_FILE_EXCEED_MAX_SIZE)
            {
                g_DirUIAdapter.ShowMsgBox(TRID_DOWNLOADFILE_OUT_OF_MEMORY);
            }
            else
            {
                g_DirUIAdapter.ShowMsgBox(TRID_LOAD_FAILED);
            }
        }

        RefreshUI(DIR_UI_REFRESH_TYPE_ALL, DIR_UI_REFRESH_PARAM_DATA_CHANGE);

        return;
    }

    // 定时更新或网页重新配置时，All组时自动刷新(兼容逻辑层All组状态)
    if (knInvalidRId == m_nDownloadGroupId && kszAllContactId == m_nGroupId)
    {
        m_nDownloadGroupId = kszAllContactId;
    }

    if (!Dir_IsAllLoadFinish(m_nDownloadGroupId, m_eLogicType))
    {
        return;
    }

    DIRUI_INFO("Dir_IsAllLoadFinish[%d][%d]", m_nDownloadGroupId, m_eLogicType);

    if (m_bUpdating || m_bDownloading)
    {
        g_DirUIAdapter.EndWaiting();
        m_bUpdating = false;
        m_bDownloading = false;
    }

    if (1 == refMessage.lParam)
    {
        RefreshUI(DIR_UI_REFRESH_TYPE_ALL, DIR_UI_REFRESH_PARAM_DATA_CHANGE);
    }
    else
    {
        if (refMessage.lParam == NS_TA::TA_FILE_EXCEED_MAX_SIZE)
        {
            g_DirUIAdapter.ShowMsgBox(TRID_DOWNLOADFILE_OUT_OF_MEMORY);
        }
        else
        {
            g_DirUIAdapter.ShowMsgBox(TRID_LOAD_FAILED);
        }
    }
    return;
}

void CMTSWContactControler::RefreshAfterContactListChange()
{
    RefreshUI(DIR_UI_REFRESH_TYPE_ALL, DIR_UI_REFRESH_PARAM_ALL);
}

void CMTSWContactControler::ShowErrorResult(int nGroupId, int nResult)
{
    // 防止多次弹窗
    if (!IsResultSuccess(m_nFailResult))
    {
        return;
    }

    m_nFailResult = nResult;

    yl::string strError = TRID_FAIL_TO_CONNECT_TO_SERVER;
    if (IsSvrURLInvalid(nResult))
    {
        strError = TRID_SERVER_URL_IS_INVALID;
    }

    g_DirUIAdapter.ShowMsgBox(_UILOGIC_LANG_TRAN(strError.c_str()).c_str());
}

bool CMTSWContactControler::IsContactCanEdit(int nContactID)
{
    SmartPtr pDetail = Dir_GetDetailById(nContactID, MTSW_DIR);

    if (!pDetail.IsEmpty())
    {
        int nType = pDetail->GetType();
        if (DIR_TYPE_METASWITCH_ITEM_EXTENTION == nType
                || DIR_TYPE_METASWITCH_ITEM_MLHG == nType)
        {
            return false;
        }
    }

    return true;
}

#endif
