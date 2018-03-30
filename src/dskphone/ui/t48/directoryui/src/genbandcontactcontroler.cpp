#include "genbandcontactcontroler.h"
//#include "directorystring.h"
#ifndef DIR_SUPPORT_SINGLE_UIHELPER
#include "genbandcontactuihelper.h"
#endif
#include "configparser/modconfigparser.h"
#include <configiddefine.h>

#include "directorymgr.h"

#if IF_FEATURE_GBBOOK

CGenbandContactControler::CGenbandContactControler()
    : CCommonDirCtrl(DIRECTORY_TYPE_GENBAND, GB_DIR)
    , m_bDownloading(false)
    , m_bUpdating(false)
    , m_nDownloadGroupId(knInvalidRId)
    , m_nPreciseSearchState(PRECISE_SEARCH_STATE_NONE)
{
}

CGenbandContactControler::~CGenbandContactControler()
{
}

CGenbandContactUIHelper * CGenbandContactControler::GetGenbandUIHelper()
{
#ifdef DIR_SUPPORT_SINGLE_UIHELPER
    return m_pUIHelper;
#else
    return CGenbandContactUIHelper::GetUIHelper(m_pUIHelper);
#endif
}

CGenbandContactControler * CGenbandContactControler::GetControler(CDirectoryControlerPtr &
        pControler)
{
    if (pControler.IsEmpty() || DIRECTORY_TYPE_GENBAND != pControler->GetType())
    {
        return NULL;
    }

    return (CGenbandContactControler *)pControler.Get();
}

ContactGBBookUnit * CGenbandContactControler::GetGenbandContact(SmartPtr & pDetail)
{
    if (pDetail.IsEmpty() || DIR_TYPE_GB_UNIT != pDetail->GetType())
    {
        return NULL;
    }

    return (ContactGBBookUnit *)pDetail.Get();
}

void CGenbandContactControler::UnInit()
{
    //退出模块时候清空GAB联系人
//  int nCurGroupId = GetGroupId();
//
//  if (nCurGroupId == kszGabBookGroupId && GetState() == DIR_UI_STATE_BROWSER)
//  {
//      Dir_ClearContactData(kszGabBookGroupId, GB_DIR);
//      if (configParser_GetConfigInt(kszGbRetainSearchFilter) != 1)
//      {
//          GAB_ClearSearchInfo();
//      }
//  }

    CDirectoryControler::UnInit();
}

bool CGenbandContactControler::IsExitPabGroup()
{
    return configParser_GetConfigInt(kszGbPabBookEnable) == 1;
}

void CGenbandContactControler::UpdateContacts(int nGroupId)
{
    if (m_bUpdating)
    {
        return;
    }

    if (knInvalidRId == nGroupId)
    {
        nGroupId = kszAllContactId;
    }

    m_nDownloadGroupId = nGroupId;
    DIRUI_INFO("CGenbandContactControler::UpdateContacts(%d)", m_nDownloadGroupId);

    bool retCode;

    if (nGroupId == kszGabBookGroupId)
    {
        GBBOOK_OPERATION_TYPE eType = GAB_GetCurrentGabOptType();
        if (eType < GAB_SEARCH_NAME || eType > GAB_SEARCH_NUMBER)
        {
            CDirUIHelper::ShowMsgBox(TRID_FAIL_TO_CONNECT_TO_SERVER);
            return;
        }
        DIRECTORY_RET_CODE eRetCode = GAB_ReqSearchGABByType(GAB_GetCurentGabKey(),
                                      GAB_GetCurrentGabOptType());
        if (eRetCode == DIR_RET_SUCC
                || eRetCode == GET_LIST_DOWNLOADING)
        {
            m_bUpdating = true;
            CDirUIHelper::ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT, DIR_ACTION_GENBAND_UPDATE);
        }
        else
        {
            CDirUIHelper::ShowErrorMsg(eRetCode);
        }
    }
    else
    {
        bool bRetCode = PAB_UpdatePabData();
        if (bRetCode)
        {
            m_bUpdating = true;
            CDirUIHelper::ShowWaiting(TRID_NOTE, TRID_UPDATING_PLEASE_WAIT, DIR_ACTION_GENBAND_UPDATE);
        }
        else
        {
            CDirUIHelper::ShowErrorMsg(DIR_RET_FAIL);
        }
    }
}

bool CGenbandContactControler::OnBack()
{
    if (PRECISE_SEARCH_STATE_NONE != m_nPreciseSearchState)
    {
        bool bBack = false;

        m_nPreciseSearchState = PRECISE_SEARCH_STATE_NONE;

        int nCurGroupId = GetGroupId();

        if (nCurGroupId == kszGabBookGroupId && GetState() == DIR_UI_STATE_BROWSER)
        {
            Dir_ClearContactData(kszGabBookGroupId, GB_DIR);
            if (configParser_GetConfigInt(kszGbRetainSearchFilter) != 1)
            {
                GAB_ClearSearchInfo();
            }
        }

        return bBack;
    }

    int nCurGroupId = GetGroupId();

    if (knInvalidRId == nCurGroupId)
    {
        return false;
    }

    int nParentGroupId = Dir_GetParentIdById(nCurGroupId, m_eLogicType);

    if (knInvalidRId == nParentGroupId || nParentGroupId < 0)
    {
        return false;
        //nParentGroupId = kszAllContactId;
    }

    SetGroupId(nParentGroupId);

    return true;
}

bool CGenbandContactControler::OnDelPabContact(int nId)
{
    yl::string strExtTitle = Dir_GetDisplayNameById(nId, GB_DIR);
    return CDirUIHelper::PopSureMsgBox(TRID_DEL_GENBAND_CONTACT_TITLE, TRID_DEL_GENBAND_CONTACT,
                                       DIR_ACTION_GENBAND_DEL_PAB, nId, strExtTitle.c_str());
}

bool CGenbandContactControler::OnEnterCopyToPabContact(int nAction, int nId)
{
    CGenbandContactUIHelper * pUIHelper = GetGenbandUIHelper();

    if (NULL == pUIHelper)
    {
        return false;
    }

    SmartPtr pDetail = Dir_GetDetailById(nId, m_eLogicType);
    ContactGBBookUnit * pRecord = GetGenbandContact(pDetail);

    if (pRecord == NULL)
    {
        return false;
    }

    LIST_ATTRIBUTE lsAttr;
    ST_CONTACT_ATTRIBUTE stAttr;

    stAttr.m_strName = ATTR_NAME;
    stAttr.m_strValue = pRecord->GetDisplayName();
    lsAttr.push_back(stAttr);

    stAttr.m_strName = ATTR_GENBAND_FIRSTNAME;
    stAttr.m_strValue = pRecord->m_strFirstName;
    lsAttr.push_back(stAttr);

    stAttr.m_strName = ATTR_GENBAND_LASTNAME;
    stAttr.m_strValue = pRecord->m_strLastName;
    lsAttr.push_back(stAttr);

    stAttr.m_strName = ATTR_GENBAND_EMAIL;
    stAttr.m_strValue = pRecord->m_strEmail;
    lsAttr.push_back(stAttr);

    switch (pRecord->GetType())
    {
    case DIR_TYPE_GB_UNIT:
        {
            GetGenbandNumber(lsAttr, (ContactGBBookUnit *)pDetail.Get());
        }
        break;
    default:
        {
            return false;
        }
        break;
    }

    CDirUIHelper::SetAttributeList(m_nType, lsAttr);
    CDirUIHelper::EnterUI(DIRECTORY_TYPE_GENBAND, DIR_UI_STATE_ADDITION, nAction, kszAllContactId);

    return true;
}



bool CGenbandContactControler::GetGenbandNumber(LIST_ATTRIBUTE & lsAttr,
        ContactGBBookUnit * pContact)
{
    if (NULL == pContact)
    {
        return false;
    }

    ST_CONTACT_ATTRIBUTE stAttr;


    if (pContact->GetNumberByName(kszGbBookbusinessNumber, stAttr.m_strValue)
            && !stAttr.m_strValue.empty())
    {
        stAttr.m_strName = TRID_BUSINESS_NUMBER;
        lsAttr.push_back(stAttr);
    }

    if (pContact->GetNumberByName(kszGbBookhomeNumber, stAttr.m_strValue) && !stAttr.m_strValue.empty())
    {
        stAttr.m_strName = TRID_HOME_NUMBER;
        lsAttr.push_back(stAttr);
    }

    if (pContact->GetNumberByName(kszGbBookmobile, stAttr.m_strValue) && !stAttr.m_strValue.empty())
    {
        stAttr.m_strName = TRID_MOBILE_NUMBER;
        lsAttr.push_back(stAttr);
    }

    if (pContact->GetNumberByName(kszGbBookpager, stAttr.m_strValue) && !stAttr.m_strValue.empty())
    {
        stAttr.m_strName = TRID_PAGER;
        lsAttr.push_back(stAttr);
    }

    if (pContact->GetNumberByName(kszGbBookFax, stAttr.m_strValue) && !stAttr.m_strValue.empty())
    {
        stAttr.m_strName = TRID_FAX;
        lsAttr.push_back(stAttr);
    }

    if (pContact->GetNumberByName(kszGbconferenceURL, stAttr.m_strValue) && !stAttr.m_strValue.empty())
    {
        stAttr.m_strName = TRID_CONFERENCE_URL;
        lsAttr.push_back(stAttr);
    }

    if (pContact->GetNumberByName(kszGbBookprimaryContact, stAttr.m_strValue)
            && !stAttr.m_strValue.empty())
    {
        stAttr.m_strName = TRID_PRIMARYCONTACT;
        lsAttr.push_back(stAttr);
    }


    return true;
}

bool CGenbandContactControler::OnCopyToPab(bool bOverwrite)
{
    CGenbandContactUIHelper * pGenbandUIHelper = GetGenbandUIHelper();

    if (NULL == pGenbandUIHelper)
    {
        return false;
    }

    ContactGBBookUnit itemData;
    if (!pGenbandUIHelper->GetPABContactData(itemData))
    {
        return false;
    }

    if (!CheckPersonalValid(itemData))
    {
        return false;
    }

    int nRetCode = DIR_RET_SUCC;
    int nContactId = Dir_IsContactNameExist(itemData.m_strDisplayName, m_eLogicType);

    if (-1 == nContactId)
    {
        DIRUI_INFO("AddPersonal(Name:%s Number:%s ParentId:%d)",
                   itemData.m_strDisplayName.c_str(),
                   itemData.GetNumberByIndex(0).c_str(),
                   itemData.m_nParentId);

        nRetCode = PAB_ReqAddContact(itemData);
    }
    else
    {
        if (!bOverwrite)
        {
            return CDirUIHelper::PopSureMsgBox(TRID_WARNING, TRID_IF_OVERWRITE_THE_ORIGINAL_CONTACT,
                                               DIR_ACTION_GENBAND_ADD_TO_PAB_OVERWRITE, nContactId);
        }

        DIRUI_INFO("PAB_ReqModifyContact(Id:%d Name:%s Number:%s)",
                   nContactId, itemData.m_strDisplayName.c_str(),
                   itemData.GetNumberByIndex(0).c_str());

        nRetCode = PAB_ReqModifyContact(nContactId, itemData);
    }

    return CheckChangePabContact(nRetCode, DIR_ACTION_GENBAND_COPY_PERSONAL_FINISH);
}

bool CGenbandContactControler::CheckPabContactDataValid(ContactGBBookUnit & itemData)
{
    if (itemData.m_strDisplayName.empty())
    {
        return false;
    }

    yl::string strNumber;

    itemData.GetNumberByName(kszGbBookprimaryContact, strNumber);

    if (!CheckNumberValid(strNumber.c_str()) || strNumber.size() < 1)
    {
        return false;
    }

    return true;
}

bool CGenbandContactControler::CheckPersonalValid(ContactGBBookUnit & itemData,
        bool bIsAddContact /*= false*/)
{
    yl::string strMsg = "";
    if (itemData.m_strDisplayName.empty())
    {
        strMsg = TRID_GENBAND_NAME_CANNOT_BE_EMPTY;
    }
    int nContactID = Dir_IsContactNameExist(itemData.m_strDisplayName, GB_DIR);
    DIRUI_INFO("CGenbandContactControler::CheckPersonalValid [%d] [%d]", nContactID, m_nContactId);
    //添加联系人情况下只要逻辑层存在数据就不能添加，修改联系人需要判断id是否一样
    if (nContactID != -1 && (bIsAddContact || m_nContactId != nContactID))
    {
        strMsg = TRID_GENBAND_CONTACT_NAME_EXIST;
    }

    if (!strMsg.empty())
    {
        CDirUIHelper::PopSureMsgBox(TRID_NOTE, strMsg.c_str(), DIR_ACTION_NONE, -1);

        CGenbandContactUIHelper * pGenbandUIHelper = GetGenbandUIHelper();

        if (NULL != pGenbandUIHelper)
        {
            pGenbandUIHelper->SetInputFocus(ATTR_NAME);
        }
        return false;
    }

    yl::string strNumber;

    itemData.GetNumberByName(kszGbBookprimaryContact, strNumber);

    strMsg = "";

    if (strNumber.size() <= 0)
    {
        strMsg = TRID_GENBAND_NUMBER_CANNOT_BE_EMPTY;
    }
    else if (!CheckNumberValid(strNumber.c_str()) || strNumber.size() < 1)
    {
        strMsg = TRID_NUMBER_INVALID;
    }

    if (!strMsg.empty())
    {
        CDirUIHelper::PopSureMsgBox(TRID_NOTE, strMsg.c_str(), DIR_ACTION_NONE, -1);

        CGenbandContactUIHelper * pGenbandUIHelper = GetGenbandUIHelper();
        if (NULL != pGenbandUIHelper)
        {
            pGenbandUIHelper->SetInputFocus(kszGbBookprimaryContact);
        }
        return false;
    }

    return true;
}

bool CGenbandContactControler::CheckNumberValid(const char * pszNumber, int nMaxLength/* = 30*/)
{
#if 0
    if (NULL == pszNumber)
    {
        return false;
    }

    int nLength = strlen(pszNumber);

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
#endif

    return true;
}

bool CGenbandContactControler::CheckChangePabContact(int nRetCode, int nAction)
{
    if (DIR_RET_SUCC == nRetCode || nRetCode > 0)
    {
        CDirUIHelper::ShowWaiting(TRID_NOTE, TRID_SAVE_GENBAND_CONTACT, nAction);
        return true;
    }
    else if (DIR_RET_CONTACT_EXIST == nRetCode)
    {
        CDirUIHelper::ShowMsgBox(TRID_CONTACT_EXIST);
    }
    else if (DIR_RET_FAIL == nRetCode)
    {
        if (nAction == DIR_ACTION_GENBAND_ADD_PAB_FINISH
                || nAction == DIR_ACTION_GENBAND_EDIT_PAB_FINISH
                || nAction == DIR_ACTION_GENBAND_EDIT_GROUP_FINISH
                || nAction == DIR_ACTION_GENBAND_ADD_GROUP_FINISH)
        {
            CDirUIHelper::PopSureMsgBox(TRID_NOTE, TRID_SAVE_FAILED, DIR_ACTION_NONE, -1);
        }
        else
        {
            CDirUIHelper::PopSureMsgBox(TRID_NOTE, TRID_SAVE_FAILED, nAction, -1);
        }
        //CDirUIHelper::ShowMsgBox(TRID_SAVE_FAILED);
    }
    else
    {
        CDirUIHelper::ShowErrorMsg(nRetCode);
    }

    return false;
}

bool CGenbandContactControler::CheckDeletePersonal(int nRetCode)
{
    if (DIR_RET_SUCC == nRetCode)
    {
        CDirUIHelper::ShowWaiting(TRID_NOTE, TRID_DEL_GENBAND_CONTACT_WAITING,
                                  DIR_ACTION_GENBAND_DEL_FINISH);
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

bool CGenbandContactControler::AddPabContact()
{
    CGenbandContactUIHelper * pGenbandUIHelper = GetGenbandUIHelper();


    if (NULL == pGenbandUIHelper)
    {
        return false;
    }

    ContactGBBookUnit itemData;

    if (!pGenbandUIHelper->GetPABContactData(itemData))
    {
        return false;
    }

    if (!CheckPersonalValid(itemData, true))
    {
        return false;
    }

    DIRUI_INFO("CGenbandContactControler::AddPabContact(%s %s %d)",
               itemData.m_strDisplayName.c_str(), itemData.GetNumberByIndex(0).c_str(), itemData.m_nParentId);

    int nRetCode = PAB_ReqAddContact(itemData);

    return CheckChangePabContact(nRetCode, DIR_ACTION_GENBAND_ADD_PAB_FINISH);
}

bool CGenbandContactControler::DelPabContact(int nContactId)
{
    int nRetCode = PAB_ReqRemoveContact(nContactId);

    bool bSuccess = CheckDeletePersonal(nRetCode);

    return bSuccess;
}

bool CGenbandContactControler::SavePabContact()
{
    CGenbandContactUIHelper * pGenbandUIHelper = GetGenbandUIHelper();

    if (NULL == pGenbandUIHelper)
    {
        return false;
    }

    ContactGBBookUnit itemData;

    if (!pGenbandUIHelper->GetPABContactData(itemData))
    {
        return false;
    }
    if (!CheckPersonalValid(itemData))
    {
        return false;
    }

    DIRUI_INFO("CGenbandContactControler::SavePabContact(%s %s %d %d)",
               itemData.m_strDisplayName.c_str(), itemData.GetNumberByIndex(0).c_str(), itemData.m_nParentId,
               m_nContactId);

    int nRetCode = PAB_ReqModifyContact(m_nContactId, itemData);

    return CheckChangePabContact(nRetCode, DIR_ACTION_GENBAND_EDIT_PAB_FINISH);
}

void CGenbandContactControler::RefreshAfterPabContactChange()
{
    if (IsSearchState())
    {
        if (!PreSearchContact())
        {
            return;
        }
    }

    CDirUIHelper::RefreshUI(true, true);
}

void CGenbandContactControler::RefreshAfterPabContactChange(msgObject & refMessage)
{
    bool bSuc = 1 == refMessage.lParam;

    CDirUIHelper::EndWaiting();
    yl::string strMsg = "";
    switch (refMessage.wParam)
    {
    case PAB_OPERATION_ADD_GROUP:
    case PAB_OPERATION_ADD_CONTACT:
    case PAB_OPERATION_MODIFY_GROUP:
    case PAB_OPERATION_MODIFY_CONTACT:
        {
            if (bSuc)
            {
                int nUIState = GetState();
                if (DIR_UI_STATE_ADDITION == nUIState
                        || DIR_UI_STATE_DETAIL == nUIState)
                {
                    CDirUIHelper::ExitCurrentUI(nUIState);
                }
                strMsg = TRID_SAVE_GENBAND_CONTACT_COMPLETED;
            }
            else
            {
                strMsg = TRID_SAVE_FAILED;
            }
        }
        break;
    case PAB_OPERATION_REMOVE_GROUP:
    case PAB_OPERATION_REMOVE_CONTACT:
        {
            if (bSuc)
            {
                int nUIState = GetState();
                if (DIR_UI_STATE_DETAIL == nUIState)
                {
                    CDirUIHelper::ExitCurrentUI(nUIState);
                }
                CDirUIHelper::RefreshUIAfterDeleted();
                strMsg = TRID_DEL_GENBAND_CONTACT_COMPLETED;
            }
            else
            {
                strMsg = TRID_DELETE_FIALED;
            }
        }
        break;
    }

    DIRUI_INFO("CGenbandContactControler::RefreshAfterPabContactChange Suc [%d], Msg [%s]", bSuc,
               strMsg.c_str());

    if (bSuc)
    {
        CDirUIHelper::RefreshUI(true, true);
    }

    if (!strMsg.empty())
    {
        CDirUIHelper::ShowMsgBox(strMsg.c_str(), 2000);
    }
}

bool CGenbandContactControler::IsPabContact(int nContactId)
{
    if (Dir_GetParentIdById(nContactId, GB_DIR) != kszGabBookGroupId)
    {
        return true;
    }

    return false;
}

bool CGenbandContactControler::IsPreciseSearchResult() const
{
    return (PRECISE_SEARCH_STATE_RESULT == m_nPreciseSearchState);
}

void CGenbandContactControler::EnterPreciseSearch()
{
    m_nPreciseSearchState = PRECISE_SEARCH_STATE_ENTER;
}

int CGenbandContactControler::GetPreciseSearchState() const
{
    return m_nPreciseSearchState;
}

void CGenbandContactControler::AdjustPreciseState(int nState)
{
    m_nPreciseSearchState = nState;
}

bool CGenbandContactControler::PreciseSearch()
{
    CGenbandContactUIHelper * pGenbandUIHelper = GetGenbandUIHelper();
    if (pGenbandUIHelper != NULL)
    {
        GBBOOK_OPERATION_TYPE eType = pGenbandUIHelper->GetGABPreciseSearchType();
        return PreciseSearchByMode(eType);
    }
    return false;
}

bool CGenbandContactControler::PreciseSearchByMode(GBBOOK_OPERATION_TYPE eMode)
{
    DIRUI_INFO("CGenbandContactControler::PreciseSearchByMode eType [%d]\n", eMode);
    CGenbandContactUIHelper * pGenbandUIHelper = GetGenbandUIHelper();

    if (NULL == pGenbandUIHelper || PRECISE_SEARCH_STATE_ENTER != m_nPreciseSearchState)
    {
        DIRUI_ERR("CGenbandContactControler::PreciseSearchByMode fail State:%d UIHelp:%p",
                  m_nPreciseSearchState, pGenbandUIHelper);
        return false;
    }

    yl::string strSearchKey;
    if (!pGenbandUIHelper->GetGABPreciseSearchData(strSearchKey))
    {
        DIRUI_ERR("GetGABPreciseSearchData fail");
        return false;
    }
    if (eMode < GAB_SEARCH_NAME || eMode > GAB_SEARCH_NUMBER)
    {
        return false;
    }

    m_nSearchType = SEARCH_SERVER;

    DIRECTORY_RET_CODE retCode = GAB_ReqSearchGABByType(strSearchKey, eMode);

    if (GET_LIST_DOWNLOADING == retCode)
    {
        m_nPreciseSearchState = PRECISE_SEARCH_STATE_WAITING;
        CDirUIHelper::ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT, DIR_ACTION_GENBAND_DOWNLOAD);
    }
    else if (DIR_RET_SUCC == retCode)
    {
        m_nPreciseSearchState = PRECISE_SEARCH_STATE_RESULT;
        CDirUIHelper::ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT, DIR_ACTION_GENBAND_DOWNLOAD);
    }
    else
    {
        CDirUIHelper::ShowErrorMsg(retCode);
    }

    return true;
}

bool CGenbandContactControler::PreLoadContact(int nGroupId)
{
    DIRUI_INFO("CGenbandContactControler::PreLoadContact(%d)", nGroupId);

    m_nPreciseSearchState = PRECISE_SEARCH_STATE_NONE;

    if (!CCommonDirCtrl::PreLoadContact(nGroupId))
    {
        return false;
    }

    if (nGroupId == kszGabBookGroupId)
    {
        GBBOOK_OPERATION_TYPE eType = GAB_GetCurrentGabOptType();
        if (eType >= GAB_SEARCH_NAME && eType <= GAB_SEARCH_NUMBER)
        {
            m_nPreciseSearchState = PRECISE_SEARCH_STATE_RESULT;
            DIRECTORY_RET_CODE eRetCode = GAB_ReqSearchGABByType(GAB_GetCurentGabKey(), eType);
            if (eRetCode == DIR_RET_SUCC
                    || eRetCode == GET_LIST_DOWNLOADING)
            {
                m_bUpdating = true;
                CDirUIHelper::ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT, DIR_ACTION_GENBAND_UPDATE);
            }
            else
            {
                CDirUIHelper::ShowErrorMsg(eRetCode);
            }
        }
        return true;
    }
    else if (nGroupId == kszAllContactId)
    {
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

        if (FILE_DOWNLOADING != nStatus)
        {
            int nRetCode = PAB_UpdatePabData();

            if (DIR_RET_SUCC != nRetCode)
            {
                m_nDownloadGroupId = knInvalidRId;
                CDirUIHelper::ShowErrorMsg(nRetCode);
                return true;
            }
        }

        m_bDownloading = true;

        CDirUIHelper::ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT, DIR_ACTION_GENBAND_DOWNLOAD);
        return true;
    }
    return true;
}

bool CGenbandContactControler::PreSearchContact()
{
    m_nPreciseSearchState = PRECISE_SEARCH_STATE_NONE;

    return CCommonDirCtrl::PreSearchContact();
}

bool CGenbandContactControler::OnAction(int nAction, int nDataIndex, int nId)
{
    DIRUI_INFO("CGenbandContactControler::OnAction(Action:%d Index:%d Id:%d)",
               nAction, nDataIndex, nId);

    if (CCommonDirCtrl::OnAction(nAction, nDataIndex, nId))
    {
        return true;
    }

    switch (nAction)
    {
    case DIR_ACTION_GENBAND_UPDATE:
        {

            int nGroupId = nId;
            if (nId != kszAllContactId
                    && nId != kszGabBookGroupId)
            {
                nGroupId = Dir_GetParentIdById(nId, GB_DIR);
            }
            UpdateContacts(nGroupId);
        }
        break;
    case DIR_ACTION_GENBAND_ENTER_ADD_PAB:
        {
            //进入联系人添加界面初始化contact id
            m_nContactId = knInvalidRId;
            CDirUIHelper::EnterUI(m_nType, DIR_UI_STATE_ADDITION, nAction, GetGroupId());
        }
        break;
    case DIR_ACTION_GENBAND_ENTER_ADD_GROUP:
        {
            CDirUIHelper::EnterUI(m_nType, DIR_UI_STATE_ADDITION, nAction, kszPabBookGroupId);
            return true;
        }
        break;
    case DIR_ACTION_GENBAND_ADD_PAB:
        {
            return AddPabContact();
        }
        break;
    case DIR_ACTION_GENBAND_EDIT_PAB:
        {
            return SavePabContact();
        }
        break;
    case DIR_ACTION_GENBAND_DEL_PAB:
        {
            return OnDelPabContact(nId);
        }
        break;
    case DIR_ACTION_GENBAND_ENTER_ADD_TO_PAB:
        {
            OnEnterCopyToPabContact(nAction, nId);
        }
        break;
    case DIR_ACTION_GENBAND_ADD_TO_PAB:
        {
            OnCopyToPab(false);
        }
        break;
    case DIR_ACTION_GENBAND_ADD_GROUP:
        {
            return OnAddGroup();
        }
        break;
    case DIR_ACTION_GENBAND_DEL_GROUP:
        {
            return OnDelGroup(nId);
        }
        break;
#ifdef DIR_SUPPORT_BATCH_OPERATE
    case DIR_ACTION_GENBAND_BATCH_DEL_PERSONAL:
        {
            VEC_CONTACT_ID vecContactId;

            if (!CDirUIHelper::GetSelectContactId(vecContactId) || vecContactId.size() <= 0)
            {
                return false;
            }

            return OnDelPabContact(vecContactId[0]);
        }
        break;
    case DIR_ACTION_GENBAND_BATCH_MOVE_TO_PERSONAL:
        {
            return CDirUIHelper::PopSureMsgBox(TRID_WARNING, TRID_ADD_TO_PERSONAL, nAction, nId);
        }
        break;
#endif
    case DIR_ACTION_GENBAND_ENTER_MANAGER_GROUP:
        {
            CDirUIHelper::EnterUI(m_nType, DIR_UI_STATE_BROWSER, nAction, kszPabBookGroupId);
            return true;
        }
        break;
    case DIR_ACTION_GENBAND_EDIT_GROUP:
        {
            return OnSaveGroup(m_nContactId);
        }
        break;
    case DIR_ACTION_GENBAND_EDIT_CONTACT:
        {
            int nUIState = GetState();
            if (DIR_UI_STATE_DETAIL == nUIState)
            {
                CDirUIHelper::ExitCurrentUI(nUIState);
            }
            m_nContactId = nId;
            CDirUIHelper::EnterUI(m_nType, DIR_UI_STATE_DETAIL, nAction, nId);
            return true;
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

bool CGenbandContactControler::ExecAction(int nAction, int nDataIndex, int nId)
{
    DIRUI_INFO("CGenbandContactControler::ExecAction(Action:%d Index:%d Id:%d)",
               nAction, nDataIndex, nId);

    switch (nAction)
    {
    case DIR_ACTION_GENBAND_DEL_PAB:
        {
            DelPabContact(nId);
        }
        break;
    case DIR_ACTION_GENBAND_DEL_GROUP:
        {
            DeleteGroup(nId);
        }
        break;
    case DIR_ACTION_GENBAND_ADD_TO_PAB_OVERWRITE:
        {
            OnCopyToPab(true);
        }
        break;
    case DIR_ACTION_GENBAND_ADD_PAB_FINISH:
    case DIR_ACTION_GENBAND_EDIT_PAB_FINISH:
        {
            int nCheckUIState = DIR_UI_STATE_ADDITION;
            int nUIState = GetState();

            if (DIR_ACTION_GENBAND_EDIT_PAB_FINISH == nAction)
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
                RefreshAfterPabContactChange();
            }
        }
        break;
    case DIR_ACTION_GENBAND_COPY_PERSONAL_FINISH:
        {
            int nUIState = GetState();

            if (DIR_UI_STATE_ADDITION != nUIState)
            {
                break;
            }

            CDirUIHelper::ExitCurrentUI(nUIState);
        }
        break;
    case DIR_ACTION_GENBAND_ADD_GROUP_FINISH:
    case DIR_ACTION_GENBAND_EDIT_GROUP_FINISH:
        {
            int nUIState = GetState();

            if (DIR_UI_STATE_ADDITION != nUIState)
            {
                break;
            }

            CDirUIHelper::ExitCurrentUI(nUIState);
            RefreshAfterPabContactChange();
        }
        break;
#ifdef DIR_SUPPORT_BATCH_OPERATE
    case DIR_ACTION_GENBAND_BATCH_DEL_PERSONAL:
        {
            VEC_CONTACT_ID vecContactId;

            if (!CDirUIHelper::GetSelectContactId(vecContactId) || vecContactId.size() <= 0)
            {
                return false;
            }

            DelPabContact(vecContactId[0]);
        }
        break;
    case DIR_ACTION_GENBAND_BATCH_MOVE_TO_PERSONAL:
        {
            VEC_CONTACT_ID vecContactId;

            if (!CDirUIHelper::GetSelectContactId(vecContactId))
            {
                return false;
            }

            int nSize = vecContactId.size();

            if (nSize <= 0)
            {
                return false;
            }

            for (int i = 0; i < nSize ; ++i)
            {
                int nContactId = vecContactId[i];
                SmartPtr pDetail = Dir_GetDetailById(nContactId, m_eLogicType);

                ContactGBBookUnit * pRecord;
                pRecord = GetGenbandContact(pDetail);
                if (pRecord == NULL)
                {
                    continue;
                }

                // 去掉左右空格
                pRecord->m_strDisplayName = commonAPI_TrimString(pRecord->m_strDisplayName, " ");

                if (!CheckPabContactDataValid(*pRecord))
                {
                    continue;
                }

                int nOriContactId = Dir_IsContactNameExist(pRecord->m_strDisplayName, m_eLogicType);

                if (nOriContactId == nContactId)
                {

                    PAB_ReqModifyContact(nContactId, *pRecord);
                }
                else
                {
                    DIRUI_INFO("PAB_ReqAddContact(%s)",
                               pRecord->m_strDisplayName.c_str());

                    PAB_ReqAddContact(*pRecord);
                }
            }

            CDirUIHelper::ShowMsgBox(TRID_MOVE_CONTACTS_SUCCESS);
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

bool CGenbandContactControler::CancelAction(int nAction, int nDataIndex, int nId)
{
    if (DIR_ACTION_GENBAND_UPDATE == nAction || DIR_ACTION_GENBAND_DOWNLOAD == nAction)
    {
        if (knInvalidRId != m_nDownloadGroupId && (m_bDownloading || m_bUpdating))
        {
            Dir_CancelLoad(m_nDownloadGroupId, m_eLogicType);
        }

        m_bDownloading = false;
        m_bUpdating = false;
        m_nDownloadGroupId = knInvalidRId;
        CDirUIHelper::RefreshUI(false, false);
        return true;
    }

    return false;
}

bool CGenbandContactControler::DialContact(int nContactId,
        int nAccountId/* = AUTO_ACCOUNT_IDENTIFY*/)
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
    case DIR_TYPE_GB_UNIT:
        {
            GetGenbandNumber(lsNumber, (ContactGBBookUnit *)pDetail.Get());
        }
        break;
    default:
        break;
    }

    return CDirUIHelper::Dial(nAccountId, pDetail->GetDisplayName(), lsNumber, this, pDetail.Get());
}

LRESULT CGenbandContactControler::OnMessage(msgObject & refMessage)
{
    if (LOCAL_SEARCH_RESULT != refMessage.message && (refMessage.message < GENBAND_BOOK_REQ_RESULT
            || refMessage.message > GENBAND_BOOK_RELOAD))
    {
        return FALSE;
    }

    DIRUI_INFO("CGenbandContactControler::OnMessage MSG:%d WP:%d LP:%d", refMessage.message,
               refMessage.wParam, refMessage.lParam);

    switch (refMessage.message)
    {
    case GENBAND_BOOK_RELOAD:
        {
            //if (GetGroupId() != kszGabBookGroupId)
            {
                m_nDownloadGroupId = knInvalidRId;
                SetGroupId(knInvalidRId);

                if (m_bUpdating || m_bDownloading)
                {
                    CDirUIHelper::EndWaiting();

                    m_bUpdating = false;
                    m_bDownloading = false;
                }

                if (GetState() == DIR_UI_STATE_PRECISE_SEARCH)
                {
                    CDirUIHelper::ExitCurrentUI(DIR_UI_STATE_PRECISE_SEARCH);
                }

                CDirUIHelper::ExitAllUI(m_nType);
            }
        }
        break;
    case GENBAND_BOOK_REQ_RESULT:
        {
            CGenbandContactUIHelper * pUIHelper = GetGenbandUIHelper();

            if (m_bUpdating || m_bDownloading)
            {
                CDirUIHelper::EndWaiting();

                m_bUpdating = false;
                m_bDownloading = false;
            }

            if (refMessage.wParam >= PAB_OPERATION_ADD_GROUP
                    && refMessage.wParam <= PAB_OPERATION_GET_GROUPLIST)
            {
                RefreshAfterPabContactChange(refMessage);
            }
            else
            {
                OnPreciseSearchResult(refMessage);
            }
        }
        break;
    case LOCAL_SEARCH_RESULT:
        {
            OnSearchResult(refMessage);
        }
        break;
    default:
        break;
    }

    return FALSE;
}

void CGenbandContactControler::OnPreciseSearchResult(msgObject & refMessage)
{
    DIRUI_INFO("CGenbandContactControler::OnPreciseSearchResult(GroupId:%d wp:%d lp:%d)",
               kszGabBookGroupId, refMessage.wParam, refMessage.lParam);

    if (refMessage.wParam < GAB_SEARCH_NAME || refMessage.lParam > GAB_SEARCH_NUMBER)
    {
        return;
    }

    if (PRECISE_SEARCH_STATE_WAITING == m_nPreciseSearchState)
    {
        m_nPreciseSearchState = PRECISE_SEARCH_STATE_RESULT;
        CDirUIHelper::EndWaiting();

        if (!IsResultSuccess(refMessage.lParam))
        {
            ShowErrorResult(kszGabBookGroupId, refMessage.lParam);
        }

        CDirUIHelper::RefreshUI(true, true);
    }
    else
    {
        int nCheckUIState = DIR_UI_STATE_PRECISE_SEARCH;
        int nUIState = GetState();

        if (nCheckUIState == nUIState)
        {
            CDirUIHelper::ExitCurrentUI(nUIState);
        }

        if (!IsResultSuccess(refMessage.lParam))
        {
            ShowErrorResult(kszGabBookGroupId, refMessage.lParam);
        }

        //g_DirectoryMgr.SetGABGroupIsSelect();

        CDirUIHelper::RefreshUI(true, false);
    }
}

void CGenbandContactControler::ShowErrorResult(int nGroupId, int nResult)
{
    if (IsSvrURLInvalid(nResult))
    {
        CDirUIHelper::ShowMsgBox(TRID_SERVER_URL_IS_INVALID);
    }
    else
    {
        CDirUIHelper::ShowMsgBox(TRID_FAIL_TO_CONNECT_TO_SERVER);
    }
}

void CGenbandContactControler::GetAllGroup(VEC_CONTACT_ID & vecId)
{
    Dir_GetConfigMenuIdList(vecId, GB_DIR);

    ContactListData listData;

    listData.m_nIndexFrom = 0;
    listData.m_nCountToGet = 0;

    if (!Dir_GetContactListByGroupId(&listData, kszCustomId, GB_DIR))
    {
        return;
    }

    int nSize = listData.m_listId.size();

    for (int i = 0; i < nSize; ++i)
    {
        int nId = listData.m_listId[i];

        vecId.push_back(nId);
    }
}

bool CGenbandContactControler::LoadPreciseData(int nDataIndex, int nDataCount)
{
    CGenbandContactUIHelper * pGenbandUIHelper = GetGenbandUIHelper();

    if (NULL == pGenbandUIHelper)
    {
        return false;
    }

    DIRUI_INFO("LoadPreciseData(State:%d DataIndex:%d DataCount:%d)",
               m_nPreciseSearchState, nDataIndex, nDataCount);

    if (PRECISE_SEARCH_STATE_ENTER == m_nPreciseSearchState)
    {
        SetGroupId(kszGabBookGroupId);
        pGenbandUIHelper->SetGABGroupIsSelect();
        pGenbandUIHelper->LoadGBPreciseSearchData(nDataIndex, nDataCount);
        return true;
    }
    else if (PRECISE_SEARCH_STATE_RESULT == m_nPreciseSearchState)
    {

        return true;
    }

    return false;
}

bool CGenbandContactControler::OnSaveGroup(int nGroupId)
{
    CGenbandContactUIHelper * pGBUIHelper = GetGenbandUIHelper();

    if (NULL == pGBUIHelper)
    {
        return false;
    }

    yl::string strGroupName;

    if (!pGBUIHelper->GetGenbandGroupData(strGroupName))
    {
        DIRUI_ERR("CDirUIHelper::GetGenbandGroupData() fail in OnSaveGroup!");
        return false;
    }

    return SaveGroup(nGroupId, strGroupName);
}

bool CGenbandContactControler::SaveGroup(int nGroupId, yl::string strGroupName)
{
    yl::string strMsg = "";
    if (strGroupName.empty())
    {
        strMsg = TRID_GENBAND_NAME_GROUP_BE_EMPTY;
    }
    else if (PAB_IsContactNameExist(strGroupName) != -1
             && PAB_IsContactNameExist(strGroupName) != nGroupId)
    {
        strMsg = TRID_GENBAND_GROUP_NAME_EXIST;
    }

    if (!strMsg.empty())
    {
        CDirUIHelper::ShowMsgBox(strMsg.c_str());
        return false;
    }

    int nRetCode = PAB_ReqModifyGroup(nGroupId, strGroupName);

    return CheckChangePabContact(nRetCode, DIR_ACTION_GENBAND_EDIT_GROUP_FINISH);
}

bool CGenbandContactControler::OnDelGroup(int nGroupId)
{
    yl::string strExtTitle = Dir_GetDisplayNameById(nGroupId, GB_DIR);
    return CDirUIHelper::PopSureMsgBox(TRID_DEL_GENBAND_GROUP_TITLE, TRID_DEL_GENBAND_GROUP,
                                       DIR_ACTION_GENBAND_DEL_GROUP, nGroupId, strExtTitle.c_str());
}

bool CGenbandContactControler::DeleteGroup(int nGroupId)
{
    int nRetCode = PAB_ReqRemoveGroup(nGroupId);

    bool bSuccess = CheckDeletePersonal(nRetCode);

    if (bSuccess)
    {
        CDirUIHelper::RefreshUIAfterDeleted();
    }

    return bSuccess;
}

bool CGenbandContactControler::OnAddGroup()
{
    CGenbandContactUIHelper * pGenbandUIHelper = GetGenbandUIHelper();

    if (NULL == pGenbandUIHelper)
    {
        return false;
    }

    yl::string strGroupName;

    if (!pGenbandUIHelper->GetGenbandGroupData(strGroupName))
    {
        return false;
    }

    return AddGroup(strGroupName);
}

bool CGenbandContactControler::AddGroup(yl::string strGroupName)
{
    yl::string strMsg = "";
    if (strGroupName.empty())
    {
        strMsg = TRID_GENBAND_NAME_GROUP_BE_EMPTY;
    }
    else if (PAB_IsContactNameExist(strGroupName) != -1)
    {
        strMsg = TRID_GENBAND_GROUP_NAME_EXIST;
    }

    if (!strMsg.empty())
    {
        CDirUIHelper::ShowMsgBox(strMsg.c_str());
        return false;
    }

    int nRetCode = PAB_ReqAddGroup(strGroupName);

    return CheckChangePabContact(nRetCode, DIR_ACTION_GENBAND_ADD_GROUP_FINISH);
}

bool CGenbandContactControler::IsContactChanged()
{
    CGenbandContactUIHelper * pGBUIHelper = GetGenbandUIHelper();

    if (NULL == pGBUIHelper)
    {
        return false;
    }

    SmartPtr pDetail = GetDetail(m_nContactId);
    ContactGBBookUnit * pRecord = GetGenbandContact(pDetail);

    if (NULL == pRecord)
    {
        DIRUI_ERR("Can't find Contact when IsContactChanged() ContactId:%d", m_nContactId);
        return false;
    }

    int nGroupId = pRecord->m_nParentId;
    ContactGBBookUnit itemData;

    if (!pGBUIHelper->GetPABContactData(itemData))
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

    if (itemData.m_strFirstName != pRecord->m_strFirstName)
    {
        return true;
    }

    if (itemData.m_strLastName != pRecord->m_strLastName)
    {
        return true;
    }

    if (itemData.m_strEmail != pRecord->m_strEmail)
    {
        return true;
    }

    yl::string strCurNum;
    yl::string strLastNum;

    pRecord->GetNumberByName(kszGbBookbusinessNumber, strLastNum);
    itemData.GetNumberByName(kszGbBookbusinessNumber, strCurNum);

    if (strCurNum != strLastNum)
    {
        DIRUI_INFO("Contact changed, Input Cur:%s Last:%s",
                   strCurNum.c_str(), strLastNum.c_str());
        return true;
    }

    strCurNum = "";
    strLastNum = "";
    pRecord->GetNumberByName(kszGbBookhomeNumber, strLastNum);
    itemData.GetNumberByName(kszGbBookhomeNumber, strCurNum);
    if (strCurNum != strLastNum)
    {
        return true;
    }

    strCurNum = "";
    strLastNum = "";
    pRecord->GetNumberByName(kszGbBookmobile, strLastNum);
    itemData.GetNumberByName(kszGbBookmobile, strCurNum);
    if (strCurNum != strLastNum)
    {
        DIRUI_INFO("Contact changed, Input Cur:%s Last:%s",
                   strCurNum.c_str(), strLastNum.c_str());
        return true;
    }

    strCurNum = "";
    strLastNum = "";
    pRecord->GetNumberByName(kszGbBookpager, strLastNum);
    itemData.GetNumberByName(kszGbBookpager, strCurNum);
    if (strCurNum != strLastNum)
    {
        DIRUI_INFO("Contact changed, Input Cur:%s Last:%s",
                   strCurNum.c_str(), strLastNum.c_str());
        return true;
    }

    strCurNum = "";
    strLastNum = "";
    pRecord->GetNumberByName(kszGbBookFax, strLastNum);
    itemData.GetNumberByName(kszGbBookFax, strCurNum);
    if (strCurNum != strLastNum)
    {
        DIRUI_INFO("Contact changed, Input Cur:%s Last:%s",
                   strCurNum.c_str(), strLastNum.c_str());
        return true;
    }

    strCurNum = "";
    strLastNum = "";
    pRecord->GetNumberByName(kszGbBookprimaryContact, strLastNum);
    itemData.GetNumberByName(kszGbBookprimaryContact, strCurNum);
    if (strCurNum != strLastNum)
    {
        DIRUI_INFO("Contact changed, Input Cur:%s Last:%s",
                   strCurNum.c_str(), strLastNum.c_str());
        return true;
    }

    strCurNum = "";
    strLastNum = "";
    pRecord->GetNumberByName(kszGbconferenceURL, strLastNum);
    itemData.GetNumberByName(kszGbconferenceURL, strCurNum);
    if (strCurNum != strLastNum)
    {
        DIRUI_INFO("Contact changed, Input Cur:%s Last:%s",
                   strCurNum.c_str(), strLastNum.c_str());
        return true;
    }


    return false;
}
#endif
