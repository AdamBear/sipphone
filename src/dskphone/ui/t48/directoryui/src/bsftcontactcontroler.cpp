#include "bsftcontactcontroler.h"
#include "bsftpwdmgr.h"
#include "translateiddefine.h"
#ifndef DIR_SUPPORT_SINGLE_UIHELPER
#include "bsftcontactuihelper.h"
#endif
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "configiddefine.h"
#include "modconfigparser.h"
#include "moddirectoryui.h"


CBSFTContactControler::CBSFTContactControler()
    : CCommonDirCtrl(DIRECTORY_TYPE_BROADSOFT, BSFT_DIR)
    , m_bDownloading(false)
    , m_bUpdating(false)
    , m_nFailResult(BSFT_DOWNLOAD_RESULT_SUCCESS)
    , m_nDownloadGroupId(knInvalidRId)
    , m_nPreciseSearchGroupId(knInvalidRId)
    , m_nPreciseSearchState(PRECISE_SEARCH_STATE_NONE)
    , m_bIsReloadContact(false)
{
}

CBSFTContactControler::~CBSFTContactControler()
{
}

CBSFTContactUIHelper * CBSFTContactControler::GetBSFTUIHelper()
{
#ifdef DIR_SUPPORT_SINGLE_UIHELPER
    return m_pUIHelper;
#else
    return CBSFTContactUIHelper::GetUIHelper(m_pUIHelper);
#endif
}

CBSFTContactControler * CBSFTContactControler::GetControler(CDirectoryControlerPtr & pControler)
{
    if (pControler.IsEmpty() || DIRECTORY_TYPE_BROADSOFT != pControler->GetType())
    {
        return NULL;
    }

    return (CBSFTContactControler *)pControler.Get();
}

bool CBSFTContactControler::IsExitPersonal()
{
    SmartPtr pDetail = Dir_GetDetailById(kszPersonalId, BSFT_DIR);

    return (!pDetail.IsEmpty());
}

void CBSFTContactControler::UpdateContacts(int nGroupId)
{
    if (knInvalidRId != m_nDownloadGroupId || m_bUpdating)
    {
        DIRUI_WARN("UpdateContacts() when updating DownloadGroupId:%d Updating:%d",
                   m_nDownloadGroupId, (m_bUpdating ? 1 : 0));
        return;
    }

    if (knInvalidRId == nGroupId)
    {
        nGroupId = kszAllContactId;
    }
    else if (nGroupId > kszCustomId)
    {
        nGroupId = kszCustomId;
    }

    m_nDownloadGroupId = nGroupId;
    DIRUI_INFO("CBSFTContactControler::UpdateContacts(%d)", m_nDownloadGroupId);

    CBSFTContactUIHelper * pBSFTUIHelper = GetBSFTUIHelper();

    _BsftPwdMgr.SetDirType(m_nType);

    int nStatus = _BsftPwdMgr.CheckBSFTPassword(pBSFTUIHelper, m_nDownloadGroupId);

    if (XSI_SUCCESS != nStatus)
    {
        m_nDownloadGroupId = knInvalidRId;
        return;
    }

    DIRECTORY_RET_CODE retCode = BsftDir_Update(nGroupId);

    if (DIR_RET_SUCC == retCode)
    {
        m_bUpdating = true;
        ResetFailResult();

        CDirUIHelper::ShowWaiting(TRID_NOTE, TRID_UPDATING_PLEASE_WAIT, DIR_ACTION_BSFT_UPDATE);
    }
    else
    {
        CDirUIHelper::ShowErrorMsg(retCode);
        m_nDownloadGroupId = knInvalidRId;
    }
}

bool CBSFTContactControler::OnBack()
{
    if (PRECISE_SEARCH_STATE_NONE != m_nPreciseSearchState)
    {
        bool bBack = false;

        if (PRECISE_SEARCH_STATE_ENTER != m_nPreciseSearchState)
        {
            bBack = true;

            EnterPreciseSearch();
        }
        else
        {
            m_nPreciseSearchState = PRECISE_SEARCH_STATE_NONE;
        }

        return bBack;
    }

    int nCurGroupId = GetGroupId();

    if (knInvalidRId == nCurGroupId || nCurGroupId <= kszCustomId)
    {
        return false;
    }

    int nParentGroupId = Dir_GetParentIdById(nCurGroupId, m_eLogicType);

    if (knInvalidRId == nParentGroupId || nParentGroupId < 0)
    {
        nParentGroupId = kszAllContactId;
    }

    SetGroupId(nParentGroupId);

    return true;
}

bool CBSFTContactControler::OnDelPersonal(int nId)
{
    return CDirUIHelper::PopSureMsgBox(TRID_WARNING, TRID_IF_DELETE_SELECTED_CONTACT_ITEM,
                                       DIR_ACTION_BSFT_DEL_PERSONAL, nId);
}

bool CBSFTContactControler::OnEnterCopyToPeronal(int nAction, int nId)
{
    CBSFTContactUIHelper * pUIHelper = GetBSFTUIHelper();

    if (NULL == pUIHelper)
    {
        return false;
    }

    SmartPtr pDetail = Dir_GetDetailById(nId, m_eLogicType);

    if (pDetail.IsEmpty())
    {
        return false;
    }

    LIST_ATTRIBUTE lsAttr;
    ST_CONTACT_ATTRIBUTE stAttr;

    stAttr.m_strName = ATTR_NAME;
    stAttr.m_strValue = pDetail->GetDisplayName();
    lsAttr.push_back(stAttr);

    switch (pDetail->GetType())
    {
    case DIR_TYPE_BROADSOFT_ITEM_GROUP:
        {
            GetGroupNumber(lsAttr, (ContactBroadSoftGroup *)pDetail.Get(), true);
        }
        break;
    case DIR_TYPE_BROADSOFT_ITEM_COMMON:
        {
            GetCommonNumber(lsAttr, (ContactBroadSoftCommon *)pDetail.Get(), true);
        }
        break;
    case DIR_TYPE_BROADSOFT_ITEM_ENTERPRISE:
        {
            GetEnterpriseNumber(lsAttr, (ContactBroadSoftEnterprise *)pDetail.Get(), true);
        }
        break;
    case DIR_TYPE_BROADSOFT_ITEM_CUSTOM:
        {
            GetCustomNumber(lsAttr, (ContactBroadSoftCustom *)pDetail.Get(), true);
        }
        break;
    default:
        {
            return false;
        }
        break;
    }

    CDirUIHelper::SetAttributeList(m_nType, lsAttr);
    CDirUIHelper::EnterUI(DIRECTORY_TYPE_BROADSOFT, DIR_UI_STATE_ADDITION, nAction, kszPersonalId);

    return true;
}

bool CBSFTContactControler::GetCommonNumber(LIST_ATTRIBUTE & lsAttr,
        ContactBroadSoftCommon * pCommon, bool bCopy/* = false*/)
{
    if (NULL == pCommon)
    {
        return false;
    }

    ST_CONTACT_ATTRIBUTE stAttr;

    if (pCommon->GetNumberByName(kszBsftNumber, stAttr.m_strValue) && !stAttr.m_strValue.empty())
    {
        if (bCopy)
        {
            stAttr.m_strName = ATTR_NUMBER;
        }
        else
        {
            stAttr.m_strName = TRID_NUMBER;
        }

        lsAttr.push_back(stAttr);
    }

    return true;
}

bool CBSFTContactControler::GetEnterpriseNumber(LIST_ATTRIBUTE & lsAttr,
        ContactBroadSoftEnterprise * pEnterprise, bool bCopy/* = false*/)
{
    if (NULL == pEnterprise)
    {
        return false;
    }

    ST_CONTACT_ATTRIBUTE stAttr;

    if (bCopy)
    {
        stAttr.m_strName = ATTR_NUMBER;

        if (pEnterprise->GetNumberByName(kszBsftExtension, stAttr.m_strValue) && !stAttr.m_strValue.empty())
        {
            lsAttr.push_back(stAttr);
        }

        if (pEnterprise->GetNumberByName(kszBsftNumber, stAttr.m_strValue) && !stAttr.m_strValue.empty())
        {
            lsAttr.push_back(stAttr);
        }

        if (pEnterprise->GetNumberByName(kszBsftMobile, stAttr.m_strValue) && !stAttr.m_strValue.empty())
        {
            lsAttr.push_back(stAttr);
        }
    }
    else
    {
        if (pEnterprise->GetNumberByName(kszBsftNumber, stAttr.m_strValue) && !stAttr.m_strValue.empty())
        {
            stAttr.m_strName = TRID_NUMBER;
            lsAttr.push_back(stAttr);
        }

        if (pEnterprise->GetNumberByName(kszBsftExtension, stAttr.m_strValue) && !stAttr.m_strValue.empty())
        {
            stAttr.m_strName = TRID_EXTENSION_NUMBER;
            lsAttr.push_back(stAttr);
        }

        if (pEnterprise->GetNumberByName(kszBsftMobile, stAttr.m_strValue) && !stAttr.m_strValue.empty())
        {
            stAttr.m_strName = TRID_MOBILE_NUMBER;
            lsAttr.push_back(stAttr);
        }
    }

    return true;
}

bool CBSFTContactControler::GetCustomNumber(LIST_ATTRIBUTE & lsAttr,
        ContactBroadSoftCustom * pCustom, bool bCopy/* = false*/)
{
    if (NULL == pCustom)
    {
        return false;
    }

    ST_CONTACT_ATTRIBUTE stAttr;

    if (bCopy)
    {
        stAttr.m_strName = ATTR_NUMBER;

        if (pCustom->GetNumberByName(kszBsftExtension, stAttr.m_strValue) && !stAttr.m_strValue.empty())
        {
            lsAttr.push_back(stAttr);
        }

        if (pCustom->GetNumberByName(kszBsftNumber, stAttr.m_strValue) && !stAttr.m_strValue.empty())
        {
            lsAttr.push_back(stAttr);
        }

        if (pCustom->GetNumberByName(kszBsftMobile, stAttr.m_strValue) && !stAttr.m_strValue.empty())
        {
            lsAttr.push_back(stAttr);
        }
    }
    else
    {
        if (pCustom->GetNumberByName(kszBsftNumber, stAttr.m_strValue) && !stAttr.m_strValue.empty())
        {
            stAttr.m_strName = TRID_NUMBER;
            lsAttr.push_back(stAttr);
        }

        if (pCustom->GetNumberByName(kszBsftExtension, stAttr.m_strValue) && !stAttr.m_strValue.empty())
        {
            stAttr.m_strName = TRID_EXTENSION_NUMBER;
            lsAttr.push_back(stAttr);
        }

        if (pCustom->GetNumberByName(kszBsftMobile, stAttr.m_strValue) && !stAttr.m_strValue.empty())
        {
            stAttr.m_strName = TRID_MOBILE_NUMBER;
            lsAttr.push_back(stAttr);
        }
    }

    return true;
}

bool CBSFTContactControler::GetGroupNumber(LIST_ATTRIBUTE & lsAttr, ContactBroadSoftGroup * pGroup,
        bool bCopy/* = false*/)
{
    if (NULL == pGroup)
    {
        return false;
    }

    ST_CONTACT_ATTRIBUTE stAttr;

    if (bCopy)
    {
        stAttr.m_strName = ATTR_NUMBER;

        if (pGroup->GetNumberByName(kszBsftExtension, stAttr.m_strValue) && !stAttr.m_strValue.empty())
        {
            lsAttr.push_back(stAttr);
        }

        if (pGroup->GetNumberByName(kszBsftNumber, stAttr.m_strValue) && !stAttr.m_strValue.empty())
        {
            lsAttr.push_back(stAttr);
        }

        if (pGroup->GetNumberByName(kszBsftMobile, stAttr.m_strValue) && !stAttr.m_strValue.empty())
        {
            lsAttr.push_back(stAttr);
        }
    }
    else
    {
        if (pGroup->GetNumberByName(kszBsftNumber, stAttr.m_strValue) && !stAttr.m_strValue.empty())
        {
            stAttr.m_strName = TRID_NUMBER;
            lsAttr.push_back(stAttr);
        }

        if (pGroup->GetNumberByName(kszBsftExtension, stAttr.m_strValue) && !stAttr.m_strValue.empty())
        {
            stAttr.m_strName = TRID_EXTENSION_NUMBER;
            lsAttr.push_back(stAttr);
        }

        if (pGroup->GetNumberByName(kszBsftMobile, stAttr.m_strValue) && !stAttr.m_strValue.empty())
        {
            stAttr.m_strName = TRID_MOBILE_NUMBER;
            lsAttr.push_back(stAttr);
        }
    }

    return true;
}

bool CBSFTContactControler::GetPersonalNumber(LIST_ATTRIBUTE & lsAttr,
        ContactBroadSoftPersonal * pPersonal)
{
    if (NULL == pPersonal)
    {
        return false;
    }

    ST_CONTACT_ATTRIBUTE stAttr;

    if (pPersonal->GetNumberByName(kszBsftNumber, stAttr.m_strValue) && !stAttr.m_strValue.empty())
    {
        stAttr.m_strName = TRID_NUMBER;

        lsAttr.push_back(stAttr);
    }

    return true;
}

bool CBSFTContactControler::OnCopyToPersonal(bool bOverwrite)
{
    CBSFTContactUIHelper * pBSFTUIHelper = GetBSFTUIHelper();

    if (NULL == pBSFTUIHelper)
    {
        return false;
    }

    ContactBroadSoftPersonal itemData;

    if (!pBSFTUIHelper->GetBSFTPersonalData(itemData))
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
        DIRUI_INFO("AddPersonal(Name:%s Number:%s)",
                   itemData.m_strDisplayName.c_str(),
                   itemData.GetNumberByIndex(0).c_str());

        nRetCode = BsftDir_AddContact2Personal(itemData);
    }
    else
    {
        if (!bOverwrite)
        {
            return CDirUIHelper::PopSureMsgBox(TRID_WARNING, TRID_IF_OVERWRITE_THE_ORIGINAL_CONTACT,
                                               DIR_ACTION_BSFT_ADD_TO_PERSONAL_OVERWRITE, nContactId);
        }

        DIRUI_INFO("BsftDir_EditPersonalContact(Id:%d Name:%s Number:%s)",
                   nContactId, itemData.m_strDisplayName.c_str(),
                   itemData.GetNumberByIndex(0).c_str());

        nRetCode = BsftDir_EditPersonalContact(nContactId, itemData, SEARCH_NONE);
    }

    return CheckChangePersonal(nRetCode, DIR_ACTION_BSFT_COPY_PERSONAL_FINISH);
}

bool CBSFTContactControler::CheckPersonalDataValid(ContactBroadSoftPersonal & itemData)
{
    if (itemData.m_strDisplayName.empty())
    {
        return false;
    }

    yl::string strNumber;

    itemData.GetNumberByIndex(0, strNumber);

    if (!CheckNumberValid(strNumber.c_str()) || strNumber.size() <= 1)
    {
        return false;
    }

    return true;
}

bool CBSFTContactControler::CheckPersonalValid(ContactBroadSoftPersonal & itemData)
{
    if (itemData.m_strDisplayName.empty())
    {
        CDirUIHelper::ShowMsgBox(TRID_DISPLAY_NAME_CANNOT_BE_EMPTY);

        CBSFTContactUIHelper * pBSFTUIHelper = GetBSFTUIHelper();

        if (NULL != pBSFTUIHelper)
        {
            pBSFTUIHelper->SetInputFocus(ATTR_NAME);
        }
        return false;
    }

    yl::string strNumber;

    itemData.GetNumberByIndex(0, strNumber);

    yl::string strMsg = "";

    if (strNumber.size() <= 0)
    {
        strMsg = TRID_NUMBER_CANNOT_BE_EMPTY;
    }
    else if (!CheckNumberValid(strNumber.c_str()) || strNumber.size() <= 1)
    {
        strMsg = TRID_NUMBER_INVALID;
    }

    if (!strMsg.empty())
    {
        CDirUIHelper::ShowMsgBox(strMsg.c_str());

        CBSFTContactUIHelper * pBSFTUIHelper = GetBSFTUIHelper();

        if (NULL != pBSFTUIHelper)
        {
            pBSFTUIHelper->SetInputFocus(ATTR_NUMBER);
        }

        return false;
    }

    return true;
}

bool CBSFTContactControler::CheckNumberValid(const char * pszNumber, int nMaxLength/* = 30*/)
{
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

    return true;
}

bool CBSFTContactControler::CheckChangePersonal(int nRetCode, int nAction)
{
    if (DIR_RET_SUCC == nRetCode || nRetCode > 0)
    {
        CDirUIHelper::ShowMsgBox(TRID_SAVING_FILES, NULL, nAction);
        return true;
    }
    else if (DIR_RET_CONTACT_EXIST == nRetCode)
    {
        CDirUIHelper::ShowMsgBox(TRID_CONTACT_EXIST);
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

bool CBSFTContactControler::CheckDeletePersonal(int nRetCode)
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

bool CBSFTContactControler::AddPersonal()
{
    CBSFTContactUIHelper * pBSFTUIHelper = GetBSFTUIHelper();

    if (NULL == pBSFTUIHelper)
    {
        return false;
    }

    ContactBroadSoftPersonal itemData;

    if (!pBSFTUIHelper->GetBSFTPersonalData(itemData))
    {
        return false;
    }

    if (!CheckPersonalValid(itemData))
    {
        return false;
    }

    DIRUI_INFO("AddPersonal(%s %s)", itemData.m_strDisplayName.c_str(),
               itemData.GetNumberByIndex(0).c_str());

    int nRetCode = BsftDir_AddContact2Personal(itemData);

    return CheckChangePersonal(nRetCode, DIR_ACTION_BSFT_ADD_PERSONAL_FINISH);
}

bool CBSFTContactControler::DelPersonal(int nContactId)
{
    VEC_CONTACT_ID vecId;

    vecId.push_back(nContactId);

    int nRetCode = BsftDir_DeletePersonalContact(vecId, (BSFT_SEARCH_TYPE)m_nSearchType);

    bool bSuccess = CheckDeletePersonal(nRetCode);

    if (bSuccess)
    {
        CDirUIHelper::RefreshUIAfterDeleted();
    }

    return bSuccess;
}

bool CBSFTContactControler::SavePersonal()
{
    CBSFTContactUIHelper * pBSFTUIHelper = GetBSFTUIHelper();

    if (NULL == pBSFTUIHelper)
    {
        return false;
    }

    ContactBroadSoftPersonal itemData;

    if (!pBSFTUIHelper->GetBSFTPersonalData(itemData))
    {
        return false;
    }

    if (!CheckPersonalValid(itemData))
    {
        return false;
    }

    int nRetCode = BsftDir_EditPersonalContact(m_nContactId, itemData, (BSFT_SEARCH_TYPE)m_nSearchType);

    return CheckChangePersonal(nRetCode, DIR_ACTION_BSFT_EDIT_PERSONAL_FINISH);
}

void CBSFTContactControler::RefreshAfterPersonalChange()
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

bool CBSFTContactControler::IsContactChanged()
{
    SmartPtr pDetail = GetDetail(m_nContactId);

    if (pDetail.IsEmpty() || DIR_TYPE_BROADSOFT_ITEM_PERSONAL != pDetail->GetType())
    {
        return false;
    }

    ContactBroadSoftPersonal * pPersonal = (ContactBroadSoftPersonal *)pDetail.Get();
    CBSFTContactUIHelper * pBSFTUIHelper = GetBSFTUIHelper();

    if (NULL == pBSFTUIHelper)
    {
        return false;
    }

    ContactBroadSoftPersonal itemData;

    if (!pBSFTUIHelper->GetBSFTPersonalData(itemData))
    {
        return false;
    }

    if (itemData.m_strDisplayName != pPersonal->GetDisplayName())
    {
        return true;
    }

    yl::string strInputNum;
    yl::string strNum;

    itemData.GetNumberByName(kszBsftNumber, strInputNum);
    pPersonal->GetNumberByName(kszBsftNumber, strNum);

    if (strInputNum != strNum)
    {
        return true;
    }

    return false;
}

bool CBSFTContactControler::IsCurrentPersonal()
{
    return IsPersonal(m_nContactId);
}

bool CBSFTContactControler::IsPersonal(int nContactId)
{
    SmartPtr pDetail = Dir_GetDetailById(nContactId, BSFT_DIR);

    if (pDetail.IsEmpty() || DIR_TYPE_BROADSOFT_ITEM_PERSONAL != pDetail->GetType())
    {
        return false;
    }

    return true;
}

bool CBSFTContactControler::IsPreciseSearchResult() const
{
    return (PRECISE_SEARCH_STATE_RESULT == m_nPreciseSearchState);
}

void CBSFTContactControler::EnterPreciseSearch()
{
    m_nPreciseSearchState = PRECISE_SEARCH_STATE_ENTER;
}

int CBSFTContactControler::GetPreciseSearchState() const
{
    return m_nPreciseSearchState;
}

void CBSFTContactControler::AdjustPreciseState(int nState)
{
    m_nPreciseSearchState = nState;
}

bool CBSFTContactControler::PreciseSearch()
{
    CBSFTContactUIHelper * pBSFTUIHelper = GetBSFTUIHelper();

    if (NULL == pBSFTUIHelper || PRECISE_SEARCH_STATE_ENTER != m_nPreciseSearchState)
    {
        DIRUI_ERR("PreciseSearch fail State:%d UIHelp:%p", m_nPreciseSearchState, pBSFTUIHelper);
        return false;
    }

    ContactBroadSoftItemData itemData;

    m_nPreciseSearchGroupId = GetGroupId();

    if (!pBSFTUIHelper->GetBSFTPreciseSearchData(m_nPreciseSearchGroupId, itemData))
    {
        DIRUI_ERR("GetBSFTPreciseSearchData fail");
        return false;
    }

    if ("," == itemData.m_strDisplayName && itemData.m_strNumber.empty()
            && itemData.m_strExtension.empty()
            && itemData.m_strMobile.empty() && itemData.m_strDepartment.empty() && itemData.m_strEmail.empty())
    {
        CDirUIHelper::ShowMsgBox(TRID_PLEASE_ENTER_FILTER_CONDITIONS);
        return false;
    }

    _BsftPwdMgr.SetDirType(m_nType);

    m_nSearchType = SEARCH_SERVER;

    int nStatus = _BsftPwdMgr.CheckBSFTPassword(pBSFTUIHelper, m_nPreciseSearchGroupId);

    if (XSI_SIPACCOUNT_NOT_AVAILABLE == nStatus)
    {
        m_nPreciseSearchState = PRECISE_SEARCH_STATE_RESULT;
        return true;
    }
    else if (XSI_AUTH_ERROR == nStatus)
    {
        return false;
    }

    DIRUI_INFO("BsftDir_ReqSearchContact() GroupId:%d Name:%s Num:%s UserId:%s GroupId:%s Extension:%s Mobile:%s Email:%s Dep:%s",
               m_nPreciseSearchGroupId, itemData.m_strDisplayName.c_str(), itemData.m_strNumber.c_str(),
               itemData.m_strUserId.c_str(), itemData.m_strGroupId.c_str(),
               itemData.m_strExtension.c_str(), itemData.m_strMobile.c_str(),
               itemData.m_strEmail.c_str(), itemData.m_strDepartment.c_str());

    DIRECTORY_RET_CODE retCode = BsftDir_ReqSearchContact(m_nPreciseSearchGroupId, itemData);

    if (GET_LIST_DOWNLOADING == retCode)
    {
        m_nPreciseSearchState = PRECISE_SEARCH_STATE_WAITING;
        ResetFailResult();

        CDirUIHelper::ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT, DIR_ACTION_BSFT_DOWNLOAD);
        return true;
    }

    m_nPreciseSearchState = PRECISE_SEARCH_STATE_RESULT;

    if (DIR_RET_SUCC != retCode && DIR_RET_SEARCH_PARM_ERROR != retCode)
    {
        CDirUIHelper::ShowErrorMsg(retCode);
    }

    return true;
}

void CBSFTContactControler::PreLoadContactOnUIReturn(int nGroupId)
{
    if (configParser_GetConfigInt(kszBsftDirUpdateEnable) == 0)
    {
        if (m_bIsReloadContact)
        {
            PreLoadContact(nGroupId);
            m_bIsReloadContact = false;
        }
    }
}

bool CBSFTContactControler::PreLoadContact(int nGroupId)
{
    DIRUI_INFO("CBSFTContactControler::PreLoadContact(%d)", nGroupId);

    m_nPreciseSearchState = PRECISE_SEARCH_STATE_NONE;

    if (!CCommonDirCtrl::PreLoadContact(nGroupId))
    {
        return false;
    }

    //if (configParser_GetConfigInt(kszBsftDirUpdateEnable) == 0 && m_bIsReloadContact)
    //{
    //    BsftDir_ClearContactInfo(nGroupId);
    //}

    if (!BWCallControl_IsAccountXSIEnable(0))
    {
        CDirUIHelper::ShowMsgBox(TRID_XSI_INFORMATION_IS_INCOMPLETE);
        return true;
    }

    SmartPtr pDetail = Dir_GetDetailById(nGroupId, m_eLogicType);
    DirGroupRecord * pMenu = GetGroupRecord(pDetail);

    if (NULL == pMenu)
    {
        return false;
    }

    int nStatus = pMenu->m_eStatus;

    if (FILE_DOWNLOADED_OK == nStatus && configParser_GetConfigInt(kszBsftDirUpdateEnable) == 1)
    {
        m_nDownloadGroupId = knInvalidRId;
        return true;
    }

    m_nDownloadGroupId = nGroupId;

    CBSFTContactUIHelper * pBSFTUIHelper = GetBSFTUIHelper();

    _BsftPwdMgr.SetDirType(m_nType);

    if (kszAllContactId != nGroupId)
    {
        int nXsiStatus = _BsftPwdMgr.CheckBSFTPassword(pBSFTUIHelper, nGroupId);

        if (XSI_SIPACCOUNT_NOT_AVAILABLE == nXsiStatus)
        {
            m_nDownloadGroupId = knInvalidRId;
            return true;
        }
        else if (XSI_SUCCESS != nXsiStatus)
        {
            m_nDownloadGroupId = knInvalidRId;
            return false;
        }
    }

    if (FILE_DOWNLOADING != nStatus)
    {
        int nRetCode = BsftDir_ReqLoadContact(nGroupId);

        if (DIR_RET_SUCC != nRetCode)
        {
            m_nDownloadGroupId = knInvalidRId;
            CDirUIHelper::ShowErrorMsg(nRetCode);
            return true;
        }
    }

    m_bDownloading = true;
    ResetFailResult();

    CDirUIHelper::ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT, DIR_ACTION_BSFT_DOWNLOAD);
    return true;
}

bool CBSFTContactControler::PreSearchContact()
{
    m_nPreciseSearchState = PRECISE_SEARCH_STATE_NONE;

    if (configParser_GetConfigInt(kszBsftDirUpdateEnable) == 0)
    {
        int nUiStatus = GetState();
        int nGroupId = GetSearchGroupId();
        //BsftDir_ClearContactInfo(nGroupId);
        ContactBroadSoftItemData itemData;
        itemData.m_strDisplayName = GetSearchKey();
        itemData.m_strNumber = GetSearchKey();
        if (nUiStatus == DIR_UI_STATE_SEARCH)
        {
            itemData.m_bCriteriaMode = true;
        }
        DIRECTORY_RET_CODE retCode = BsftDir_ReqSearchContact(nGroupId, itemData);
        if (GET_LIST_DOWNLOADING == retCode)
        {
            m_nPreciseSearchState = PRECISE_SEARCH_STATE_WAITING;
            ResetFailResult();

            CDirUIHelper::ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT, DIR_ACTION_BSFT_DOWNLOAD);
            return true;
        }

        m_nPreciseSearchState = PRECISE_SEARCH_STATE_RESULT;

        if (DIR_RET_SUCC != retCode && DIR_RET_SEARCH_PARM_ERROR != retCode)
        {
            CDirUIHelper::ShowErrorMsg(retCode);
        }
        return false;
    }

    return CCommonDirCtrl::PreSearchContact();
}

bool CBSFTContactControler::LoadMenuData(int nDataIndex, int nDataCount)
{
    if (!BWCallControl_IsAccountXSIEnable(0))
    {
        CDirUIHelper::ShowMsgBox(TRID_XSI_INFORMATION_IS_INCOMPLETE);
        return true;
    }

    return CCommonDirCtrl::LoadMenuData(nDataIndex, nDataCount);
}

bool CBSFTContactControler::OnAction(int nAction, int nDataIndex, int nId)
{
    DIRUI_INFO("CBSFTContactControler::OnAction(Action:%d Index:%d Id:%d)",
               nAction, nDataIndex, nId);

    if (CCommonDirCtrl::OnAction(nAction, nDataIndex, nId))
    {
        return true;
    }

    switch (nAction)
    {
    case DIR_ACTION_BSFT_UPDATE:
        {
            UpdateContacts(nId);
        }
        break;
    case DIR_ACTION_BSFT_ENTER_ADD_PERSONAL:
        {
            CDirUIHelper::EnterUI(m_nType, DIR_UI_STATE_ADDITION, nAction, GetGroupId());
        }
        break;
    case DIR_ACTION_BSFT_ADD_PERSONAL:
        {
            return AddPersonal();
        }
        break;
    case DIR_ACTION_BSFT_EDIT_PERSONAL:
        {
            return SavePersonal();
        }
        break;
    case DIR_ACTION_BSFT_DEL_PERSONAL:
        {
            return OnDelPersonal(nId);
        }
        break;
    case DIR_ACTION_BSFT_ENTER_ADD_TO_PERSONAL:
        {
            OnEnterCopyToPeronal(nAction, nId);
        }
        break;
    case DIR_ACTION_BSFT_ADD_TO_PERSONAL:
        {
            OnCopyToPersonal(false);
        }
        break;
    case DIR_ACTION_BSFT_CHG_PWD:
        {
            CBSFTContactUIHelper * pBSFTUIHelper = GetBSFTUIHelper();
            int nGroupId = _BsftPwdMgr.GetGroupId();

            if (_BsftPwdMgr.RetryBSFTPwd(pBSFTUIHelper))
            {
                m_nDownloadGroupId = nGroupId;
                m_bDownloading = true;
                ResetFailResult();

                if (NULL != pBSFTUIHelper)
                {
                    pBSFTUIHelper->ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT,
                                               DIR_ACTION_BSFT_DOWNLOAD);
                }
            }
        }
        break;
#ifdef DIR_SUPPORT_BATCH_OPERATE
    case DIR_ACTION_BSFT_BATCH_DEL_PERSONAL:
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
    case DIR_ACTION_BSFT_BATCH_MOVE_TO_PERSONAL:
        {
            return CDirUIHelper::PopSureMsgBox(TRID_WARNING, TRID_ASK_ADD_SELECTED_TO_PERSONAL, nAction, nId);
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

bool CBSFTContactControler::ExecAction(int nAction, int nDataIndex, int nId)
{
    DIRUI_INFO("CBSFTContactControler::ExecAction(Action:%d Index:%d Id:%d)",
               nAction, nDataIndex, nId);

    switch (nAction)
    {
    case DIR_ACTION_BSFT_DEL_PERSONAL:
        {
            DelPersonal(nId);
        }
        break;
    case DIR_ACTION_BSFT_ADD_TO_PERSONAL_OVERWRITE:
        {
            OnCopyToPersonal(true);
        }
        break;
    case DIR_ACTION_BSFT_ADD_PERSONAL_FINISH:
    case DIR_ACTION_BSFT_EDIT_PERSONAL_FINISH:
        {
            int nCheckUIState = DIR_UI_STATE_ADDITION;
            int nUIState = GetState();

            if (DIR_ACTION_BSFT_EDIT_PERSONAL_FINISH == nAction)
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
                RefreshAfterPersonalChange();
            }
        }
        break;
    case DIR_ACTION_BSFT_COPY_PERSONAL_FINISH:
        {
            int nUIState = GetState();

            if (DIR_UI_STATE_ADDITION != nUIState)
            {
                break;
            }

            CDirUIHelper::ExitCurrentUI(nUIState);
        }
        break;
    case DIR_ACTION_BSFT_ENTER_CHG_PWD:
        {
            _BsftPwdMgr.EnterBSFTPwdUI();
        }
        break;
#ifdef DIR_SUPPORT_BATCH_OPERATE
    case DIR_ACTION_BSFT_BATCH_DEL_PERSONAL:
        {
            VEC_CONTACT_ID vecContactId;

            if (!CDirUIHelper::GetSelectContactId(vecContactId) || vecContactId.size() <= 0)
            {
                return false;
            }

            int nRetCode = BsftDir_DeletePersonalContact(vecContactId, SEARCH_NONE);
            bool bSuccess = true;

            if (DIR_RET_SUCC != nRetCode)
            {
                bSuccess = CheckDeletePersonal(nRetCode);
            }

            RefreshAfterPersonalChange();

            return bSuccess;
        }
        break;
    case DIR_ACTION_BSFT_BATCH_MOVE_TO_PERSONAL:
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

                ContactBroadSoftPersonal itemData;
                yl::string strNumber;

                if (!GetContactCopyInfo(pDetail, itemData.m_strDisplayName, strNumber))
                {
                    continue;
                }

                // 去掉左右空格
                itemData.m_strDisplayName = commonAPI_TrimString(itemData.m_strDisplayName, " ");

                itemData.SetNumber(strNumber);

                if (!CheckPersonalDataValid(itemData))
                {
                    continue;
                }

                int nOriContactId = Dir_IsContactNameExist(itemData.m_strDisplayName, m_eLogicType);

                if (nOriContactId == nContactId)
                {
                    DIRUI_INFO("Dir_IsContactNameExist(%d, %s, %s)", nContactId,
                               itemData.m_strDisplayName.c_str(), strNumber.c_str());

                    BsftDir_EditPersonalContact(nContactId, itemData, SEARCH_NONE);
                }
                else
                {
                    DIRUI_INFO("BSFTDir_AddContact2Personal(%s %s)",
                               itemData.m_strDisplayName.c_str(), strNumber.c_str());

                    BsftDir_AddContact2Personal(itemData);
                }
            }

            CDirUIHelper::ShowMsgBox(TRID_MOVE_CONTACTS_SUCCESS);

            CBSFTContactUIHelper * pUIHelper = GetBSFTUIHelper();

            if (NULL != pUIHelper)
            {
                pUIHelper->OnBatchCopyContactSuccess();
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

#ifdef DIR_SUPPORT_BATCH_OPERATE
bool CBSFTContactControler::GetContactCopyInfo(SmartPtr & pDetail, yl::string & strName,
        yl::string & strNumber)
{
    if (pDetail.IsEmpty())
    {
        return false;
    }

    strName = pDetail->GetDisplayName();

    switch (pDetail->GetType())
    {
    case DIR_TYPE_BROADSOFT_ITEM_GROUP:
        {
            ContactBroadSoftGroup * pGroup = (ContactBroadSoftGroup *)pDetail.Get();

            if (!pGroup->GetNumberByName(kszBsftNumber, strNumber) || strNumber.empty())
            {
                if (!pGroup->GetNumberByName(kszBsftExtension, strNumber) || strNumber.empty())
                {
                    pGroup->GetNumberByName(kszBsftMobile, strNumber);
                }
            }

            return true;
        }
        break;
    case DIR_TYPE_BROADSOFT_ITEM_COMMON:
        {
            ContactBroadSoftCommon * pCommon = (ContactBroadSoftCommon *)pDetail.Get();

            pCommon->GetNumberByName(kszBsftNumber, strNumber);
            return true;
        }
        break;
    case DIR_TYPE_BROADSOFT_ITEM_ENTERPRISE:
        {
            ContactBroadSoftEnterprise * pEnterprise = (ContactBroadSoftEnterprise *)pDetail.Get();

            if (!pEnterprise->GetNumberByName(kszBsftNumber, strNumber) || strNumber.empty())
            {
                if (!pEnterprise->GetNumberByName(kszBsftExtension, strNumber) || strNumber.empty())
                {
                    pEnterprise->GetNumberByName(kszBsftMobile, strNumber);
                }
            }

            return true;
        }
        break;
    case DIR_TYPE_BROADSOFT_ITEM_CUSTOM:
        {
            ContactBroadSoftCustom * pCustom = (ContactBroadSoftCustom *)pDetail.Get();

            if (!pCustom->GetNumberByName(kszBsftNumber, strNumber) || strNumber.empty())
            {
                if (!pCustom->GetNumberByName(kszBsftExtension, strNumber) || strNumber.empty())
                {
                    pCustom->GetNumberByName(kszBsftMobile, strNumber);
                }
            }

            return true;
        }
        break;
    }
    return false;
}
#endif

bool CBSFTContactControler::CancelAction(int nAction, int nDataIndex, int nId)
{
    if (DIR_ACTION_BSFT_UPDATE == nAction || DIR_ACTION_BSFT_DOWNLOAD == nAction)
    {
        if (knInvalidRId != m_nDownloadGroupId && (m_bDownloading || m_bUpdating))
        {
            if (IsSearchState())
            {
                Dir_CancelLoad(GetGroupId(), m_eLogicType);
            }
            else
            {
                Dir_CancelLoad(m_nDownloadGroupId, m_eLogicType);
            }
        }

        m_bDownloading = false;
        m_bUpdating = false;
        CDirUIHelper::RefreshUI(false, false);
        return true;
    }

    return false;
}

bool CBSFTContactControler::DialContact(int nContactId, int nAccountId/* = AUTO_ACCOUNT_IDENTIFY*/)
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
    case DIR_TYPE_BROADSOFT_ITEM_GROUP:
        {
            GetGroupNumber(lsNumber, (ContactBroadSoftGroup *)pDetail.Get());
        }
        break;
    case DIR_TYPE_BROADSOFT_ITEM_PERSONAL:
        {
            GetPersonalNumber(lsNumber, (ContactBroadSoftPersonal *)pDetail.Get());
        }
        break;
    case DIR_TYPE_BROADSOFT_ITEM_COMMON:
        {
            GetCommonNumber(lsNumber, (ContactBroadSoftCommon *)pDetail.Get());
        }
        break;
    case DIR_TYPE_BROADSOFT_ITEM_ENTERPRISE:
        {
            GetEnterpriseNumber(lsNumber, (ContactBroadSoftEnterprise *)pDetail.Get());
        }
        break;
    case DIR_TYPE_BROADSOFT_ITEM_CUSTOM:
        {
            GetCustomNumber(lsNumber, (ContactBroadSoftCustom *)pDetail.Get());
        }
        break;
    default:
        break;
    }

    return CDirUIHelper::Dial(nAccountId, pDetail->GetDisplayName(), lsNumber, this, pDetail.Get());
}

LRESULT CBSFTContactControler::OnMessage(msgObject & refMessage)
{
    if (LOCAL_SEARCH_RESULT != refMessage.message
            && (refMessage.message < BROADSOFT_DIRECTORY_MESSAGE_BEGIN
                || refMessage.message > BROADSOFT_PHONEBOOK_MESSAGE_END))
    {
        return FALSE;
    }

    switch (refMessage.message)
    {
    case BROADSOFT_PHONEBOOK_DOWNLOADED_RESULT:
        {
            OnDownLoadFinished(refMessage);
        }
        break;
    case BROADSOFT_PHONEBOOK_RELOAD:
        {
            m_nDownloadGroupId = knInvalidRId;
            SetGroupId(knInvalidRId);

            if (m_bUpdating || m_bDownloading)
            {
                CDirUIHelper::EndWaiting();

                m_bUpdating = false;
                m_bDownloading = false;
            }

            CBSFTContactUIHelper * pUIHelper = GetBSFTUIHelper();

            if (NULL != pUIHelper)
            {
                pUIHelper->ExitBSFTPwdUI();
            }

            CDirUIHelper::ExitAllUI(m_nType);
        }
        break;
    case BROADSOFT_PHONEBOOK_POST_RESULT:
        {
            CBSFTContactUIHelper * pUIHelper = GetBSFTUIHelper();

            if (NULL != pUIHelper && 1 == refMessage.lParam)
            {
                RefreshAfterPersonalChange();
            }
        }
        break;
    case BROADSOFT_PHONEBOOK_SERVERSEARCH_RESULT:
        {
            OnPreciseSearchResult(refMessage);
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

void CBSFTContactControler::OnDownLoadFinished(msgObject & refMessage)
{
    CBSFTContactUIHelper * pUIHelper = GetBSFTUIHelper();

    if (NULL == pUIHelper)
    {
        return;
    }

    int nGroupId = refMessage.wParam;
    bool bMsgEnd = false;

    if (IsResultSuccess(refMessage.lParam))
    {
        bool * pRequestNext = (bool *)refMessage.GetExtraData();

        if (NULL != pRequestNext)
        {
            bMsgEnd = !(*pRequestNext);
        }
    }
    else
    {
        bMsgEnd = true;
    }

    DIRUI_INFO("CBSFTContactControler::OnDownLoadFinished(CurGroupId:%d GroupId:%d lp:%d)",
               m_nDownloadGroupId, nGroupId, refMessage.lParam);

    // AllGroup组不会收到消息，因此不需要判断
    if (nGroupId == m_nDownloadGroupId && kszCustomId != m_nDownloadGroupId)
    {
        if (m_bUpdating || m_bDownloading)
        {
            CDirUIHelper::EndWaiting();
        }

        m_bUpdating = false;
        m_bDownloading = false;

        if (bMsgEnd)
        {
            m_nDownloadGroupId = knInvalidRId;

            if (IsResultSuccess(refMessage.lParam))
            {
                pUIHelper->ExitBSFTPwdUI();
            }
            else
            {
                ShowErrorResult(nGroupId, refMessage.lParam);
            }
        }
    }

    if (kszAllContactId == m_nDownloadGroupId || kszCustomId == m_nDownloadGroupId)
    {
        // Custom组的话，忽略非Custom组及非Custom子组消息
        if (kszCustomId == m_nDownloadGroupId && kszCustomId > nGroupId)
        {
            return;
        }

        if (m_bUpdating || m_bDownloading)
        {
            CDirUIHelper::EndWaiting();
        }

        m_bUpdating = false;
        m_bDownloading = false;

        // All组和Custom组要等到所有更新完才能再次处理
        if (!bMsgEnd || !Dir_IsAllLoadFinish(m_nDownloadGroupId, m_eLogicType))
        {
            if (!IsResultSuccess(refMessage.lParam))
            {
                m_nFailResult = refMessage.lParam;
            }

            RefreshAfterDownload(m_nDownloadGroupId);
            return;
        }

        int nDownloadGroupId = m_nDownloadGroupId;
        int nFailResult = refMessage.lParam;

        m_nDownloadGroupId = knInvalidRId;

        if (IsResultSuccess(nFailResult) && !IsResultSuccess(m_nFailResult))
        {
            nFailResult = m_nFailResult;
            ResetFailResult();
        }

        if (IsResultSuccess(nFailResult))
        {
            pUIHelper->ExitBSFTPwdUI();
        }
        else
        {
            ShowErrorResult(nDownloadGroupId, nFailResult);
        }

        if (kszCustomId == nDownloadGroupId)
        {
            CDirUIHelper::ReturnRootGroup(m_nType);
            return;
        }
    }

    RefreshAfterDownload(nGroupId);
}


#ifdef DIR_SUPPORT_SWITCH_DETAIL
void CBSFTContactControler::RefreshTotalCount()
{
    if (kszAllContactId != m_nGroupId)
    {
        return;
    }
    SetTotalCount(Dir_GetContactCount(GetLogicListGroupId(), m_eLogicType) - (BsftDir_HasNextPage(GetLogicListGroupId()) ? 1 : 0));
}
#endif

void CBSFTContactControler::RefreshAfterDownload(int nGroupId)
{
    if ((nGroupId == m_nGroupId || kszAllContactId == m_nGroupId)
#ifdef DIR_SUPPORT_SETTING
            && (IsBrowserState() || IsSettingState()))
#else
            && IsBrowserState())
#endif
    {
        if (configParser_GetConfigInt(kszBsftDirUpdateEnable) == 1)
        {
            CDirUIHelper::RefreshUI(true, false);
        }
        else
        {
            CDirUIHelper::RefreshUI(false, false);
        }
    }
}

void CBSFTContactControler::OnPreciseSearchResult(msgObject & refMessage)
{
    DIRUI_INFO("CBSFTContactControler::OnPreciseSearchResult(GroupId:%d wp:%d lp:%d)",
               m_nPreciseSearchGroupId, refMessage.wParam, refMessage.lParam);

    /*if (DIR_UI_STATE_PRECISE_SEARCH != GetState()
            && (PRECISE_SEARCH_STATE_WAITING != m_nPreciseSearchState
                || PRECISE_SEARCH_STATE_RESULT != m_nPreciseSearchState))*/
    int nUiStatus = GetState();

    if ((DIR_UI_STATE_PRECISE_SEARCH != nUiStatus && DIR_UI_STATE_SEARCH != nUiStatus)

            || (PRECISE_SEARCH_STATE_WAITING != m_nPreciseSearchState

                && PRECISE_SEARCH_STATE_RESULT != m_nPreciseSearchState))
    {
        return;
    }

    if (PRECISE_SEARCH_STATE_WAITING == m_nPreciseSearchState)
    {
        m_nPreciseSearchState = PRECISE_SEARCH_STATE_RESULT;
        CDirUIHelper::EndWaiting();

        if (!IsResultSuccess(refMessage.lParam))
        {
            ShowErrorResult(m_nPreciseSearchGroupId, refMessage.lParam);
        }

        if (DIR_UI_STATE_PRECISE_SEARCH == nUiStatus)

        {
            CBSFTContactUIHelper::OnPreciseSearchResult();

        }

        CDirUIHelper::RefreshUI(configParser_GetConfigInt(kszBsftDirUpdateEnable) == 1, true);
    }
    else
    {
        if (!IsResultSuccess(refMessage.lParam))
        {
            ShowErrorResult(m_nPreciseSearchGroupId, refMessage.lParam);
        }

        CDirUIHelper::RefreshUI(true, false);
    }
}

void CBSFTContactControler::ShowErrorResult(int nGroupId, int nResult)
{
    // 防止多次弹窗
    if (!IsResultSuccess(m_nFailResult))
    {
        return;
    }

    m_nFailResult = nResult;

    if (IsXsiPwdError(nResult))
    {
        DIRUI_INFO("RetryBSFTPwdEnter while BROADSOFT_CALLLOG_DOWNLOADED_RESULT");

        CBSFTContactUIHelper * pBSFTUIHelper = GetBSFTUIHelper();

        _BsftPwdMgr.SetDirType(m_nType);
        _BsftPwdMgr.CheckBSFTPassword(pBSFTUIHelper, nGroupId);
    }
    else if (IsSvrURLInvalid(nResult))
    {
        CDirUIHelper::ShowMsgBox(TRID_SERVER_URL_IS_INVALID);
    }
    else
    {
        CDirUIHelper::ShowMsgBox(TRID_FAIL_TO_CONNECT_TO_SERVER);
    }
}

int CBSFTContactControler::GetGroupIdInAllGroup(int nIndex)
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

void CBSFTContactControler::GetAllGroup(VEC_CONTACT_ID & vecId)
{
    Dir_GetConfigMenuIdList(vecId, BSFT_DIR);

    ContactListData listData;

    listData.m_nIndexFrom = 0;
    listData.m_nCountToGet = 0;

    if (!Dir_GetContactListByGroupId(&listData, kszCustomId, BSFT_DIR))
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

bool CBSFTContactControler::LoadPreciseData(int nDataIndex, int nDataCount)
{
    CBSFTContactUIHelper * pBSFTUIHelper = GetBSFTUIHelper();

    if (NULL == pBSFTUIHelper)
    {
        return false;
    }

    DIRUI_INFO("LoadPreciseData(State:%d DataIndex:%d DataCount:%d)",
               m_nPreciseSearchState, nDataIndex, nDataCount);

    if (PRECISE_SEARCH_STATE_ENTER == m_nPreciseSearchState)
    {
        pBSFTUIHelper->LoadBSFTPreciseSearchData(nDataIndex, nDataCount);
        return true;
    }
    else if (PRECISE_SEARCH_STATE_RESULT == m_nPreciseSearchState)
    {
        ContactListData lsData;

        lsData.m_nCountToGet = nDataCount;
        lsData.m_nIndexFrom = nDataIndex;

        if (!Dir_GetContactListByGroupId(&lsData, kszServerSearchGroupId, m_eLogicType))
        {
            DIRUI_ERR("Dir_GetContactListByGroupId(GroupId:kszServerSearchGroupId Type:%d) fail",
                      (int)m_eLogicType);
            return false;
        }

#ifdef DIR_SUPPORT_SWITCH_DETAIL
        SetTotalCount(lsData.m_nTotalCount);
#endif
        lsData.m_nCountToGet = lsData.m_listId.size();
        pBSFTUIHelper->FillData(lsData);
        return true;
    }

    return false;
}


void CBSFTContactControler::SetNeedReLoad(bool bIsReload)
{
    m_bIsReloadContact = bIsReload;
}

int CBSFTContactControler::GetLogicListGroupId()
{
    if (PRECISE_SEARCH_STATE_RESULT == m_nPreciseSearchState)
    {
        return kszServerSearchGroupId;
    }
    if (configParser_GetConfigInt(kszBsftDirUpdateEnable) == 0 && IsSearchState())
    {
        return kszServerSearchGroupId;
    }

    return CCommonDirCtrl::GetLogicListGroupId();
}

bool CBSFTContactControler::LoadMore()
{
    int nGroupId = GetGroupId();
    if (DIR_UI_STATE_PRECISE_SEARCH == GetState())
    {
        nGroupId = m_nPreciseSearchGroupId;
    }

    DIRUI_INFO("CBSFTContactControler::PreLoadContact(%d)", nGroupId);

    m_nPreciseSearchState = PRECISE_SEARCH_STATE_NONE;

    if (!BWCallControl_IsAccountXSIEnable(0))
    {
        CDirUIHelper::ShowMsgBox(TRID_XSI_INFORMATION_IS_INCOMPLETE);
        return true;
    }

    SmartPtr pDetail = Dir_GetDetailById(nGroupId, m_eLogicType);
    DirGroupRecord * pMenu = GetGroupRecord(pDetail);

    if (NULL == pMenu)
    {
        return false;
    }

    int nStatus = pMenu->m_eStatus;

    m_nDownloadGroupId = nGroupId;

    CBSFTContactUIHelper * pBSFTUIHelper = GetBSFTUIHelper();

    _BsftPwdMgr.SetDirType(m_nType);

    if (kszAllContactId != nGroupId)
    {
        int nXsiStatus = _BsftPwdMgr.CheckBSFTPassword(pBSFTUIHelper, nGroupId);

        if (XSI_SIPACCOUNT_NOT_AVAILABLE == nXsiStatus)
        {
            m_nDownloadGroupId = knInvalidRId;
            return true;
        }
        else if (XSI_SUCCESS != nXsiStatus)
        {
            m_nDownloadGroupId = knInvalidRId;
            return false;
        }
    }

    int nRetCode = BsftDir_ReqNextPageContact(nGroupId, IsSearchState());

    if (DIR_RET_SUCC != nRetCode)
    {
        m_nDownloadGroupId = knInvalidRId;

        CDirUIHelper::ShowErrorMsg(nRetCode);
        return true;
    }

    m_bDownloading = true;
    ResetFailResult();
    CDirUIHelper::ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT, DIR_ACTION_BSFT_DOWNLOAD);
    if (IsSearchState())
    {
        AdjustPreciseState(PRECISE_SEARCH_STATE_WAITING);
    }


    return true;
}


void CBSFTContactControler::GetDetailInfo(int & nId, int & nIndex, int & nCount) const
{
    CCommonDirCtrl::GetDetailInfo(nId, nIndex, nCount);
    if (DM_IsSearchDirectoryExist())
    {

        nCount -= (BsftDir_HasNextPage(kszServerSearchGroupId) ? 1 : 0);
    }
    else
    {
        nCount -= (BsftDir_HasNextPage(GetGroupId()) ? 1 : 0);
    }

}
