#if IF_FEATURE_METASWITCH_DIRECTORY
#include "mtswcontactcontroler.h"

//#include "directorystring.h"

#include "metaswitch/mtswcontrol/include/modmtsw.h"
//#include "settingui/modsettingui.h"
#include "diruihelper.h"

#include "mtswcontactuihelper.h"
#include "settingui/include/modsettingui.h"

#include "threadtask/include/common.h"
#include "configparser/modconfigparser.h"
#include "uicommon/uikernel/languagehelper.h"
#include "uicommon/messagebox/modmessagebox.h"
#include "metaswitch/mtswcontrol/include/mtsw_messagedefine.h"

#ifdef DIR_SUPPORT_BATCH_OPERATE
#include "localcontactcontroler.h"
#endif

CMTSWContactControler::CMTSWContactControler()
    : CCommonDirCtrl(DIRECTORY_TYPE_METASWICTH, MTSW_DIR)
{
    m_bDownloading = false;
    m_bUpdating = false;
    m_nFailResult = MTSW_DOWNLOAD_RESULT_SUCCESS;
    m_nDownloadGroupId = kszAllContactId;
    m_listPerID.clear();
    m_bAddContact = false;
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

bool CMTSWContactControler::CheckContactListDataValid(ContactMetaSwitchContactList & itemData)
{
    if (itemData.m_strDisplayName.empty())
    {
        CDirUIHelper::ShowMsgBox(TRID_MTSW_NAME_CANNOT_BE_EMPTY);

        return false;
    }
    return true;
}

bool CMTSWContactControler::CheckNumberValid(const char * pszNumber, int nMaxLength/* = 32*/)
{
    if (NULL == pszNumber)
    {
        return false;
    }

    int nLength = strlen(pszNumber);

    if (nLength == 0)
    {
        return true;
    }

    if (nLength < 1 || nLength > nMaxLength)
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

    int nSize = vecId.size();

    if (nIndex < 0 || nIndex >= nSize)
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

//  if (!Dir_GetContactListByGroupId(&listData, kszCustomId, BSFT_DIR))
//  {
//      return;
//  }

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

    //若没有登录Commportal则跳转登录界面
    if (!MTSW_IsLogined())
    {
        MessageBox_ShowMTSWCommportalBox(NULL);
        return;
    }

    m_nDownloadGroupId = nGroupId;
    DIRUI_INFO("CMTSWContactControler::UpdateContacts(%d)", m_nDownloadGroupId);

    DIRECTORY_RET_CODE retCode = MtswDir_Update(nGroupId);

    if (DIR_RET_SUCC == retCode)
    {
        m_bUpdating = true;
        //ResetFailResult();

        CDirUIHelper::ShowWaiting(TRID_NOTE, TRID_UPDATING_PLEASE_WAIT, DIR_ACTION_MTSW_UPDATE);
    }
    else
    {
        CDirUIHelper::ShowErrorMsg(retCode);
    }
}


bool CMTSWContactControler::OnDelContactList(int nId)
{
    return CDirUIHelper::PopSureMsgBox(TRID_WARNING, TRID_IF_DELETE_SELECTED_CONTACT_ITEM,
                                       DIR_ACTION_MTSW_DEL_CONTACTLIST, nId);
}

bool CMTSWContactControler::OnClearContact(int nId)
{
    return CDirUIHelper::PopSureMsgBox(TRID_WARNING, TRID_MTSW_CLEAR_ALL,
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

    stAttr.m_strName = ATTR_NAME;
    stAttr.m_strValue = pRecord->GetDisplayName();
    if (stAttr.m_strValue.empty())
    {
        stAttr.m_strValue = pRecord->GetNumberByIndex(0);
    }

    lsAttr.push_back(stAttr);

    stAttr.m_strName = ATTR_NUMBER;
    stAttr.m_strValue = pRecord->GetNumberByIndex(0);
    lsAttr.push_back(stAttr);

    CDirUIHelper::SetAttributeList(m_nType, lsAttr);
    CDirUIHelper::EnterUI(DIRECTORY_TYPE_LOCAL, DIR_UI_STATE_ADDITION,
                          DIR_ACTION_LOCAL_ENTER_COPY_TO_CONTACTS, knInvalidRId);
    return true;
}

bool CMTSWContactControler::AddContactList(bool bFouceAdd /*= false*/)
{
    CMTSWContactUIHelper * pMTSWUIHelper = GetMTSWUIHelper();

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
        CDirUIHelper::RefreshUIAfterDeleted();
    }

    return bSuccess;
}
//clear contactlist
bool CMTSWContactControler::ClearContactList(int nGroupId)
{
    MtswDir_ClearContact(nGroupId);

    CDirUIHelper::RefreshUI(true, true);
    return true;
}

bool CMTSWContactControler::SaveContactList(bool bFouceEdit /*= false*/)
{
    CMTSWContactUIHelper * pMTSWUIHelper = GetMTSWUIHelper();

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

    if (nRetCode == DIR_RET_CONTACTID_UNEXISTED)
    {
        CDirUIHelper::ShowMsgBox(TRID_SAVE_FAILED);
        CDirUIHelper::ExitCurrentUI(DIR_UI_STATE_DETAIL);
    }

    if (nRetCode == DIR_RET_CONTACT_EXIST)
    {
        return CheckChangeContactList(nRetCode, DIR_ACTION_MTSW_EDIT_CONTACTLIST);
    }

    return CheckChangeContactList(nRetCode, DIR_ACTION_MTSW_EDIT_CONTACTLIST_FINISH);
}


bool CMTSWContactControler::SortContactList()
{
    //OnBack();
    MtswDir_SortContactList();
    CDirUIHelper::RefreshUI(true, true);
    return true;
}

bool CMTSWContactControler::IsContactChanged()
{
    SmartPtr pDetail = GetDetail(m_nContactId);

    if (pDetail.IsEmpty() || DIR_TYPE_METASWITCH_ITEM_CONTACTLIST != pDetail->GetType())
    {
        return false;
    }

    ContactMetaSwitchContactList * pContact = (ContactMetaSwitchContactList *)pDetail.Get();
    CMTSWContactUIHelper * pMTSWUIHelper = GetMTSWUIHelper();

    if (NULL == pMTSWUIHelper)
    {
        return false;
    }

    ContactMetaSwitchContactList itemData;

    if (!pMTSWUIHelper->GetMTSWContactListData(itemData))
    {
        return false;
    }


    if (itemData.m_strFirstName != pContact->m_strFirstName)
    {
        return true;
    }

    if (itemData.m_strLastName != pContact->m_strLastName)
    {
        return true;
    }

    if (itemData.m_strNickName != pContact->m_strNickName)
    {
        return true;
    }

    if (itemData.m_strJobTitle != pContact->m_strJobTitle)
    {
        return true;
    }

    if (itemData.m_strOrganisation != pContact->m_strOrganisation)
    {
        return true;
    }


    yl::string strInputNum;
    yl::string strNum;

    pContact->GetNumberByName(kszMtswHomeNumber, strInputNum);
    itemData.GetNumberByName(kszMtswHomeNumber, strNum);
    if (strNum != strInputNum)
    {
        return true;
    }

    strInputNum.clear();
    strNum.clear();
    pContact->GetNumberByName(kszMtswWorkNumber, strInputNum);
    itemData.GetNumberByName(kszMtswWorkNumber, strNum);
    if (strNum != strInputNum)
    {
        return true;
    }

    strInputNum.clear();
    strNum.clear();
    pContact->GetNumberByName(kszMtswMobile, strInputNum);
    itemData.GetNumberByName(kszMtswMobile, strNum);
    if (strNum != strInputNum)
    {
        return true;
    }

    strInputNum.clear();
    strNum.clear();
    pContact->GetNumberByName(kszMtswFax, strInputNum);
    itemData.GetNumberByName(kszMtswFax, strNum);
    if (strNum != strInputNum)
    {
        return true;
    }

    strInputNum.clear();
    strNum.clear();
    pContact->GetNumberByName(kszMtswOtherNumber, strInputNum);
    itemData.GetNumberByName(kszMtswOtherNumber, strNum);
    if (strNum != strInputNum)
    {
        return true;
    }

    return false;
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

void CMTSWContactControler::AddCurrentIdBeforeEnterPage()
{
    int nCurGroupId = GetGroupId();
    m_listPerID.push_back(nCurGroupId);
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
        DIR_INFO("-=-=-=-= MTSW_IsLogined fail =-=-=-=-");
        m_nGroupId = nGroupId;
        CDirUIHelper::EnterUI(DIRECTORY_TYPE_METASWICTH, DIR_UI_STATE_CHG_PWD, DIR_ACTION_MTSW_CHG_PWD,
                              nGroupId);
        return true;
    }

    int nStatus = FILE_DOWNLOADED_OK;

    if (nGroupId == kszAllContactId)
    {
        if (!Dir_IsAllLoadFinish(nGroupId, MTSW_DIR))
        {
            nStatus = FILE_DOWNLOADING;
        }
    }
    else
    {
        SmartPtr pDetail = Dir_GetDetailById(nGroupId, m_eLogicType);
        DirGroupRecord * pMenu = GetGroupRecord(pDetail);

        if (NULL == pMenu)
        {
            DIR_INFO("-=-=-=-= pMenu == NULL =-=-=-=-");
            return false;
        }

        int nStatus = pMenu->m_eStatus;
    }


    if (FILE_DOWNLOADED_OK == nStatus)
    {
        return true;
    }

    m_nDownloadGroupId = nGroupId;

    if (FILE_DOWNLOADING == nStatus)
    {
        m_bDownloading = true;
        CDirUIHelper::ShowWaiting(TRID_NOTE, TRID_UPDATING_PLEASE_WAIT, DIR_ACTION_MTSW_UPDATE);
    }
    return true;
}

bool CMTSWContactControler::PreSearchContact()
{
    return CCommonDirCtrl::PreSearchContact();
}

bool CMTSWContactControler::LoadMenuData(int nDataIndex, int nDataCount)
{
    return CCommonDirCtrl::LoadMenuData(nDataIndex, nDataCount);
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
            int iGid = GetGroupId();
            if (nId == kszContactList
                    || nId == kszMLHG
                    || nId == kszExtention)
            {
                UpdateContacts(0);
            }
            else
            {
                UpdateContacts(iGid);
            }
        }
        break;
    case DIR_ACTION_MTSW_ENTER_ADD_CONTACTLISTL:
        {
            CDirUIHelper::EnterUI(m_nType, DIR_UI_STATE_ADDITION, nAction, GetGroupId());
        }
        break;
    case DIR_ACTION_MTSW_ADD_CONTACTLIST:
        {
            return AddContactList();
        }
        break;
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
    case DIR_ACTION_MTSW_ADD_TO_LOCAL:
        {
            return OnEnterCopyToContactList(nAction, nId);
        }
        break;
    case DIR_ACTION_MTSW_SORT:
        {
            return SortContactList();
        }
        break;
    case DIR_ACTION_MTSW_BATCH_DEL_CONTACT:
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
    case DIR_ACTION_MTSW_BATCH_MOVE_TO_CONTACT:
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
    case DIR_ACTION_MTSW_CHG_PWD:
        {
            DIRUI_INFO("CMTSWContactControler::OnAction m_curid :%d", m_nGroupId);
            m_nDownloadGroupId = m_nGroupId;
            m_bDownloading = true;
            CDirUIHelper::ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT,
                                      DIR_ACTION_MTSW_DOWNLOAD);
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

            CDirUIHelper::ExitCurrentUI(nUIState);

            if (IsSearchState())
            {
                RefreshAfterContactListChange();
            }
        }
        break;
    case DIR_ACTION_MTSW_BATCH_DEL_CONTACT:
        {
            VEC_CONTACT_ID vecContactId;

            if (!CDirUIHelper::GetSelectContactId(vecContactId) || vecContactId.size() <= 0)
            {
                return false;
            }

            int nRetCode = MtswDir_DeleteContactList(vecContactId);

            bool bSuccess = CheckDeleteContactList(nRetCode);

            if (bSuccess)
            {
                CDirUIHelper::RefreshUIAfterDeleted();
            }

            return bSuccess;
        }
        break;
    case DIR_ACTION_MTSW_BATCH_MOVE_TO_CONTACT:
        {
            VEC_CONTACT_ID vecSelectId;

            if (!CDirUIHelper::GetSelectContactId(vecSelectId))
            {
                return false;
            }

            return CopyToLocalContact(nId, vecSelectId);
        }
        break;
    case DIR_ACTION_MTSW_ERROR_PWD:
        {
            CDirUIHelper::EnterUI(DIRECTORY_TYPE_METASWICTH, DIR_UI_STATE_CHG_PWD, DIR_ACTION_MTSW_CHG_PWD,
                                  m_nGroupId);
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

    switch (pDetail->GetType())
    {
    case DIR_TYPE_METASWITCH_ITEM_CONTACTLIST:
        {
            GetContactListNumber(lsNumber, (ContactMetaSwitchContactList *)pDetail.Get());
        }
        break;
    case DIR_TYPE_METASWITCH_ITEM_EXTENTION:
        {
            GetExtentionNumber(lsNumber, (ContactMetaSwitchExtention *)pDetail.Get());
        }
        break;
    case DIR_TYPE_METASWITCH_ITEM_MLHG:
        {
            GetMLHGNumber(lsNumber, (ContactMetaSwitchMLHG *)pDetail.Get());
        }
        break;
    default:
        break;
    }

    return CDirUIHelper::Dial(nAccountId, pDetail->GetDisplayName(), lsNumber, this, pDetail.Get());
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
        CDirUIHelper::RefreshUI(false, false);
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
        {

        }
        break;
    case METASWITCH_PHONEBOOK_RELOAD:
        {
            m_nDownloadGroupId = knInvalidRId;
            SetGroupId(knInvalidRId);

            if (m_bUpdating || m_bDownloading)
            {
                CDirUIHelper::EndWaiting();

                m_bUpdating = false;
                m_bDownloading = false;
            }

            CDirUIHelper::ExitAllUI(m_nType);
        }
        break;
    case METASWITCH_PHONEBOOK_POST_RESULT:
        {
            CMTSWContactUIHelper * pUIHelper = GetMTSWUIHelper();

            if (NULL != pUIHelper && 1 == refMessage.lParam)
            {
                RefreshAfterContactListChange();
            }
        }
        break;
    case MTSW_MESSAGE_AUTH_ERR:
        {
            MessageBox_RemoveAllMessageBox();

            m_bUpdating = false;
            m_bDownloading = false;
            NS_TA::TaskActionErrorType eErrCode = (NS_TA::TaskActionErrorType)refMessage.lParam;
            if ((eErrCode == NS_TA::TA_HTTP_DOWNLOAD_UNAUTHORIZED)
                    || eErrCode == NS_TA::TA_HTTP_ERR403)
            {
                CDirUIHelper::PopSureMsgBox(TRID_WARNING, TRID_COMMPORTAL_LOGIN_FAILED,
                                            DIR_ACTION_MTSW_ERROR_PWD, knInvalidRId);
            }
            else
            {
                CDirUIHelper::PopSureMsgBox(TRID_WARNING, TRID_LOGIN_FAILED,
                                            DIR_ACTION_MTSW_ERROR_PWD, knInvalidRId);
            }
        }
        break;
    default:
        break;
    }

    return retValue;
}

CMTSWContactUIHelper * CMTSWContactControler::GetMTSWUIHelper()
{
    return CMTSWContactUIHelper::GetUIHelper(m_pUIHelper);
}


bool CMTSWContactControler::GetContactListNumber(LIST_ATTRIBUTE & lsAttr,
        ContactMetaSwitchContactList * pContactList)
{
    if (NULL == pContactList)
    {
        return false;
    }

    ST_CONTACT_ATTRIBUTE stAttr;

    if (pContactList->GetNumberByName(kszMtswHomeNumber, stAttr.m_strValue)
            && !stAttr.m_strValue.empty())
    {
        stAttr.m_strName = TRID_MTSW_HOME_NUMBER;

        lsAttr.push_back(stAttr);
    }
    if (pContactList->GetNumberByName(kszMtswWorkNumber, stAttr.m_strValue)
            && !stAttr.m_strValue.empty())
    {
        stAttr.m_strName = TRID_MTSW_WORK_NUMBER;
        lsAttr.push_back(stAttr);
    }

    if (pContactList->GetNumberByName(kszMtswMobile, stAttr.m_strValue) && !stAttr.m_strValue.empty())
    {
        stAttr.m_strName = TRID_MTSW_MOBILE_NUMBER;
        lsAttr.push_back(stAttr);
    }

    if (pContactList->GetNumberByName(kszMtswFax, stAttr.m_strValue) && !stAttr.m_strValue.empty())
    {
        stAttr.m_strName = TRID_MTSW_FAX_NUMBER;
        lsAttr.push_back(stAttr);
    }

    if (pContactList->GetNumberByName(kszMtswOtherNumber, stAttr.m_strValue)
            && !stAttr.m_strValue.empty())
    {
        stAttr.m_strName = TRID_MTSW_OTHER_NUMBER;
        lsAttr.push_back(stAttr);
    }

    return true;
}

bool CMTSWContactControler::GetExtentionNumber(LIST_ATTRIBUTE & lsAttr,
        ContactMetaSwitchExtention * pExtention)
{
    if (NULL == pExtention)
    {
        return false;
    }

    ST_CONTACT_ATTRIBUTE stAttr;

    if (pExtention->GetNumberByName(kszMtswExtention, stAttr.m_strValue) && !stAttr.m_strValue.empty())
    {
        stAttr.m_strName = TRID_MTSW_EXTENTION_NUMBER;
        lsAttr.push_back(stAttr);
        return true;
    }

    if (pExtention->GetNumberByName(kszMtswDirectoryNumber, stAttr.m_strValue)
            && !stAttr.m_strValue.empty())
    {
        stAttr.m_strName = TRID_NUMBER;
        lsAttr.push_back(stAttr);
    }

    return true;
}

bool CMTSWContactControler::GetMLHGNumber(LIST_ATTRIBUTE & lsAttr, ContactMetaSwitchMLHG * pMLHG)
{
    if (NULL == pMLHG)
    {
        return false;
    }

    ST_CONTACT_ATTRIBUTE stAttr;

    if (pMLHG->GetNumberByName(kszMtswDirectoryNumber, stAttr.m_strValue) && !stAttr.m_strValue.empty())
    {
        stAttr.m_strName = TRID_NUMBER;
        lsAttr.push_back(stAttr);
    }

    return true;
}

bool CMTSWContactControler::CheckContactListValid(ContactMetaSwitchContactList & itemData)
{
    if (itemData.m_strDisplayName.empty())
    {
        CDirUIHelper::ShowMsgBox(TRID_MTSW_NAME_CANNOT_BE_EMPTY);

        return false;
    }

    yl::string strAttr;
    yl::string strNumber;
    strAttr.clear();

    itemData.GetNumberByName(kszMtswHomeNumber, strNumber);
    if (!CheckNumberValid(strNumber.c_str()))
    {
        strAttr = ATTR_MTSW_HOMENUMBER;
    }

    if (strAttr.empty())
    {
        strNumber.empty();
        itemData.GetNumberByName(kszMtswWorkNumber, strNumber);
        if (!CheckNumberValid(strNumber.c_str()))
        {
            strAttr = ATTR_MTSW_WORKNUMBER;
        }
    }

    if (strAttr.empty())
    {
        strNumber.empty();
        itemData.GetNumberByName(kszMtswMobile, strNumber);
        if (!CheckNumberValid(strNumber.c_str()))
        {
            strAttr = ATTR_MTSW_MOBILENUMBER;
        }
    }

    if (strAttr.empty())
    {
        strNumber.empty();
        itemData.GetNumberByName(kszMtswFax, strNumber);
        if (!CheckNumberValid(strNumber.c_str()))
        {
            strAttr = ATTR_MTSW_FAXNUMBER;
        }
    }

    if (strAttr.empty())
    {
        strNumber.empty();
        itemData.GetNumberByName(kszMtswOtherNumber, strNumber);
        if (!CheckNumberValid(strNumber.c_str()))
        {
            strAttr = ATTR_MTSW_OTHERNUMBER;
        }
    }

    //strAttr表示要跳转的Index
    if (!strAttr.empty())
    {
        CDirUIHelper::ShowMsgBox(TRID_MTSW_NUMBER_INVALID);
        CMTSWContactUIHelper * pMTSWUIHelper = GetMTSWUIHelper();
        if (NULL != pMTSWUIHelper)
        {
            pMTSWUIHelper->SetInputFocus(strAttr.c_str());
        }
        return false;
    }

    return true;
}

bool CMTSWContactControler::CheckChangeContactList(int nRetCode, int nAction)
{
    if (DIR_RET_SUCC == nRetCode || nRetCode > 0)
    {
        m_bAddContact = true;
        if (nAction == DIR_ACTION_MTSW_ADD_CONTACTLIST_FINISH)
        {
            CDirUIHelper::ShowWaiting(TRID_NOTE, TRID_SAVING_FILE, nAction);
            return true;
        }
        CDirUIHelper::ShowMsgBox(TRID_SAVING_FILE, NULL, nAction);
        return true;
    }
    else if (DIR_RET_CONTACT_EXIST == nRetCode)
    {
        CDirUIHelper::PopSureMsgBox(TRID_WARNING, TRID_MTSW_CONTACT_EXIST,
                                    nAction, knInvalidRId);
    }
    else if (DIR_RET_FAIL == nRetCode)
    {
        CDirUIHelper::ShowMsgBox(TRID_SAVE_FAILED);
    }
    else
    {
        CDirUIHelper::ShowErrorMsg(nRetCode);
    }

    return false;
}

bool CMTSWContactControler::CheckDeleteContactList(int nRetCode)
{
    if (DIR_RET_SUCC == nRetCode)
    {
        CDirUIHelper::ShowMsgBox(TRID_DELETING);
        return true;
    }
    else if (DIR_RET_FAIL == nRetCode)
    {
        CDirUIHelper::ShowMsgBox(TRID_DELETE_FIALED);
    }
    else
    {
        CDirUIHelper::ShowErrorMsg(nRetCode);
    }

    return false;
}

void CMTSWContactControler::OnDownLoadFinished(msgObject & refMessage)
{
    DIRUI_INFO("CMTSWContactControler::OnDownLoadFinished[%d][%d]", refMessage.lParam,
               refMessage.wParam);
    CMTSWContactUIHelper * pUIHelper = GetMTSWUIHelper();

    int nUIState = GetState();
    if (nUIState == DIR_UI_STATE_ADDITION && m_bAddContact)
    {
        m_bAddContact = false;
        DIRUI_INFO("CMTSWContactControler::OnDownLoadFinished   addcontact");
        CDirUIHelper::EndWaiting();
        CDirUIHelper::ExitCurrentUI(DIR_UI_STATE_ADDITION);
    }

    if (NULL == pUIHelper)
    {
        return;
    }

    int nGroupId = refMessage.wParam;
    // AllGroup组不会收到消息，因此不需要判断
    if (nGroupId == m_nDownloadGroupId)
    {
        m_nDownloadGroupId = knInvalidRId;

        if (m_bUpdating || m_bDownloading)
        {
            CDirUIHelper::EndWaiting();
        }

        m_bUpdating = false;
        m_bDownloading = false;

        if (refMessage.lParam != 1)
        {
            if (refMessage.lParam == NS_TA::TA_FILE_EXCEED_MAX_SIZE)
            {
                CDirUIHelper::ShowMsgBox(TRID_DOWNLOADFILE_OUT_OF_MEMORY);
            }
            else
            {
                CDirUIHelper::ShowMsgBox(TRID_LOAD_FAILED);
            }
        }

        CDirUIHelper::RefreshUI(true, false);

        return;
    }

    // 定时更新或网页重新配置时，All组时自动刷新(兼容逻辑层All组状态)
    if (knInvalidRId == m_nDownloadGroupId && kszAllContactId == m_nGroupId)
    {
        m_nDownloadGroupId = kszAllContactId;
    }

    DIRUI_INFO("m_nDownloadGroupId[%d]", m_nDownloadGroupId);

    if (!Dir_IsAllLoadFinish(m_nDownloadGroupId, m_eLogicType))
    {
        return;
    }

    DIRUI_INFO("Dir_IsAllLoadFinish[%d][%d]", m_nDownloadGroupId, m_eLogicType);

    if (m_bUpdating || m_bDownloading)
    {
        CDirUIHelper::EndWaiting();
        m_bUpdating = false;
        m_bDownloading = false;
    }

    if (refMessage.lParam == 1)
    {
        CDirUIHelper::RefreshUI(true, false);
    }
    else
    {
        if (refMessage.lParam == NS_TA::TA_FILE_EXCEED_MAX_SIZE)
        {
            CDirUIHelper::ShowMsgBox(TRID_DOWNLOADFILE_OUT_OF_MEMORY);
        }
        else
        {
            CDirUIHelper::ShowMsgBox(TRID_LOAD_FAILED);
        }
    }
    return;
}

void CMTSWContactControler::RefreshAfterContactListChange()
{
    CDirUIHelper::RefreshUI(true, true);
}

void CMTSWContactControler::ShowErrorResult(int nGroupId, int nResult)
{
    // 防止多次弹窗
    if (!IsResultSuccess(m_nFailResult))
    {
        return;
    }

    m_nFailResult = nResult;

    if (IsSvrURLInvalid(nResult))
    {
        CDirUIHelper::ShowMsgBox(TRID_SERVER_URL_IS_INVALID);
    }
    else
    {
        CDirUIHelper::ShowMsgBox(TRID_FAIL_TO_CONNECT_TO_SERVER);
    }
}

bool CMTSWContactControler::OnBack()
{
    int nCurGroupId = GetGroupId();

    if (knInvalidRId == nCurGroupId)
    {
        return false;
    }

    int nParentGroupId = Dir_GetParentIdById(nCurGroupId, m_eLogicType);

    if (knInvalidRId == nParentGroupId || nParentGroupId < 0)
    {
        return false;
    }

    DIRUI_INFO("CMTSWContactControler::OnBack ParentID [%d]", nParentGroupId);

    SetGroupId(nParentGroupId);
    return true;
}

#ifdef DIR_SUPPORT_BATCH_OPERATE
bool CMTSWContactControler::CopyToLocalContact(int nGroupId, VEC_CONTACT_ID & vecContactId)
{
    int nSize = vecContactId.size();
    if (nSize < 0)
    {
        return false;
    }


    YLList<ContactGeneralItemData> listData;

    for (int i = 0; i < nSize; ++i)
    {
        int nContactId = vecContactId[i];

        SmartPtr pDetail = Dir_GetDetailById(nContactId, m_eLogicType);
        if (pDetail.IsEmpty())
        {
            continue;;
        }

        ContactGeneralItemData itemData;
        CLocalContactControler::GetCopyDefaultContact(itemData);

        BaseContactRecord * pContact = (BaseContactRecord *)pDetail.Get();
        if (pContact == NULL)
        {
            continue;
        }

        itemData.m_strDisplayName = pContact->m_strDisplayName;

        if (DIR_TYPE_METASWITCH_ITEM_EXTENTION == pDetail->GetType())
        {
            itemData.m_strOfficePhoneNumber = pContact->GetNumberByIndex(0);
            itemData.m_strMobilePhoneNumber = pContact->GetNumberByIndex(1);
        }
        else if (DIR_TYPE_METASWITCH_ITEM_MLHG == pDetail->GetType())
        {
            itemData.m_strOfficePhoneNumber = pContact->GetNumberByIndex(0);
        }

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

        CMTSWContactUIHelper * pUIHelper = GetMTSWUIHelper();

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

#endif
