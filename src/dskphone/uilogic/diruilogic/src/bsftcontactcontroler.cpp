#include "bsftcontactcontroler.h"
#include "bsftpwdmgr.h"
#include "translateiddefine.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "diruiadapter.h"
#include "ibsftcontactuihelper.h"
#include "translateiddefine.h"
#include "uilogiccommon/include/uilogiclanghelper.h"


CBSFTContactControler::CBSFTContactControler()
    : CPreciseSearchControlBase(DIRECTORY_TYPE_BROADSOFT, BSFT_DIR)
    , m_bDownloading(false)
    , m_bUpdating(false)
    , m_bBackToSearchResult(false)
    , m_nFailResult(BSFT_DOWNLOAD_RESULT_SUCCESS)
    , m_nDownloadGroupId(knInvalidRId)
    , m_nSearchType(SEARCH_NONE)
    , m_nPreciseSearchGroupId(knInvalidRId)
{
}

CBSFTContactControler::~CBSFTContactControler()
{
}

CBSFTContactControler * CBSFTContactControler::GetControler(CDirectoryControlerPtr & pControler)
{
    if (pControler.IsEmpty() || DIRECTORY_TYPE_BROADSOFT != pControler->GetType())
    {
        return NULL;
    }

    return (CBSFTContactControler *)pControler.Get();
}

ContactBroadSoftPersonal * CBSFTContactControler::GetPersonalContact(SmartPtr & pDetail)
{
    if (pDetail.IsEmpty() || DIR_TYPE_BROADSOFT_ITEM_PERSONAL != pDetail->GetType())
    {
        return NULL;
    }

    return (ContactBroadSoftPersonal *)pDetail.Get();
}

bool CBSFTContactControler::IsExitPersonal()
{
    SmartPtr pDetail = Dir_GetDetailById(kszPersonalId, BSFT_DIR);

    return (!pDetail.IsEmpty());
}

void CBSFTContactControler::EnterState(int nState)
{
    CCommonDirCtrl::EnterState(nState);

    CheckSearchType();
}

void CBSFTContactControler::LeaveState(int nState)
{
    CCommonDirCtrl::LeaveState(nState);

    CheckSearchType();
}

void CBSFTContactControler::CheckSearchType()
{
    if (DIR_UI_STATE_PRECISE_SEARCH == m_nState || IsManualSearch())
    {
        m_nSearchType = SEARCH_SERVER;
    }
    else if (DIR_UI_STATE_SEARCH == m_nState)
    {
        m_nSearchType = SEARCH_LOCAL;
    }
    else if (DIR_UI_STATE_BROWSER == m_nState)
    {
        m_nSearchType = SEARCH_NONE;
    }
}

void CBSFTContactControler::OnEnterMenu()
{
    if (!BWCallControl_IsAccountXSIEnable(0))
    {
        g_DirUIAdapter.ShowMsgBox(TRID_XSI_INFORMATION_IS_INCOMPLETE);
    }
}

void CBSFTContactControler::OnEnterPreciseSearch()
{
    IBSFTContactUIHelper * pBSFTUIHelper = g_DirUIAdapter.GetBSFTContactUIHelper(m_pUIHelper);

    if (NULL != pBSFTUIHelper)
    {
        pBSFTUIHelper->OnEnterPreciseSearch();
    }
}

bool CBSFTContactControler::IsManualSearch() const
{
    if (DIR_UI_STATE_SEARCH == m_nState && !BsftDir_UpdateEnable())
    {
        return true;
    }

    return false;
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

    _BsftPwdMgr.SetDirType(m_nType);

    int nStatus = _BsftPwdMgr.CheckBSFTPassword(m_pUIHelper.Get(), m_nDownloadGroupId);

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

        g_DirUIAdapter.ShowWaiting(TRID_NOTE, TRID_UPDATING_PLEASE_WAIT, DIR_ACTION_BSFT_UPDATE);
    }
    else
    {
        CCommonDirCtrl::ShowErrorMsg(retCode);
        m_nDownloadGroupId = knInvalidRId;
    }
}

bool CBSFTContactControler::OnBack()
{
    if (CheckBackToMenuList() || CheckBackToSearchResult())
    {
        return true;
    }

    bool bBack = false;

    if (PRECISE_SEARCH_STATE_NONE != m_nPreciseSearchState)
    {

        if (PRECISE_SEARCH_STATE_ENTER != m_nPreciseSearchState)
        {
            bBack = true;

            IBSFTContactUIHelper * pBSFTUIHelper = g_DirUIAdapter.GetBSFTContactUIHelper(m_pUIHelper);

            if (NULL != pBSFTUIHelper)
            {
                pBSFTUIHelper->EnterPreciseSearch();
            }
        }
        else
        {
            AdjustPreciseState(PRECISE_SEARCH_STATE_NONE);
        }

        return bBack;
    }

    int nCurGroupId = GetGroupId();

    if (knInvalidRId == nCurGroupId || nCurGroupId <= kszCustomId)
    {
        CleanContactLoadInfo(nCurGroupId);

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

bool CBSFTContactControler::IsBackToMenuList()
{
    if (!BsftDir_UpdateEnable() && IsPreciseSearchResult())
    {
        return true;
    }

    return false;
}

bool CBSFTContactControler::CheckBackToMenuList()
{
    if (IsBackToMenuList())
    {
        AdjustPreciseState(PRECISE_SEARCH_STATE_NONE);
        g_DirUIAdapter.ExitCurrentUI(GetState());
        g_DirUIAdapter.ReturnRootGroup(DIRECTORY_TYPE_BROADSOFT, true);
        return true;
    }

    return false;
}

bool CBSFTContactControler::IsBackToSearchResult()
{
    if (DIR_UI_STATE_PRECISE_SEARCH == GetState()
            && IsPreciseSearchCondition()
            && m_bBackToSearchResult)
    {
        return true;
    }

    return false;
}

bool CBSFTContactControler::CheckBackToSearchResult()
{
    if (IsBackToSearchResult())
    {
        AdjustPreciseState(PRECISE_SEARCH_STATE_RESULT);
        RefreshUI(DIR_UI_REFRESH_TYPE_ALL, DIR_UI_REFRESH_PARAM_NONE);
        m_bBackToSearchResult = false;
        return true;
    }

    return false;
}

bool CBSFTContactControler::OnDelPersonal(int nId)
{
    return g_DirUIAdapter.PopSureMsgBox(TRID_WARNING, TRID_IF_DELETE_SELECTED_CONTACT_ITEM,
                                        DIR_ACTION_BSFT_DEL_PERSONAL, nId);
}

bool CBSFTContactControler::GetContactShowNumber(SmartPtr & pDetail, yl::string & strShowNumber)
{
    BaseContactRecord* pContact = (BaseContactRecord*)pDetail.Get();
    if (NULL == pContact)
    {
        return false;
    }

    int nType = pContact->GetType();
    if (DIR_TYPE_BROADSOFT_ITEM_PERSONAL == nType || DIR_TYPE_BROADSOFT_ITEM_COMMON == nType)
    {
        pContact->GetNumberByName(kszBsftNumber, strShowNumber);
        return true;
    }
    else if (DIR_TYPE_BROADSOFT_ITEM_CUSTOM == nType
             || DIR_TYPE_BROADSOFT_ITEM_ENTERPRISE == nType
             || DIR_TYPE_BROADSOFT_ITEM_GROUP == nType)
    {
        if (IsSearchState())
        {
            strShowNumber = pContact->GetSearchShowNumber(GetSearchKey());
        }
        else if (!pContact->GetNumberByName(kszBsftExtension, strShowNumber) || strShowNumber.empty())
        {
            if (!pContact->GetNumberByName(kszBsftNumber, strShowNumber) || strShowNumber.empty())
            {
                pContact->GetNumberByName(kszBsftMobile, strShowNumber);
            }
        }

        return true;
    }

    return false;
}

void CBSFTContactControler::PrepareDetailData()
{
    m_vecDetail.clear();

    SmartPtr pDetail = GetCurrentDetail();

    if (pDetail.IsEmpty())
    {
        return;
    }

    switch (pDetail->GetType())
    {
    case DIR_TYPE_BROADSOFT_ITEM_GROUP:
        {
            PrepareGroupDetail((ContactBroadSoftGroup *)pDetail.Get());
        }
        break;
    case DIR_TYPE_BROADSOFT_ITEM_COMMON:
        {
            PrepareCommonDetail((ContactBroadSoftCommon *)pDetail.Get());
        }
        break;
    case DIR_TYPE_BROADSOFT_ITEM_ENTERPRISE:
        {
            PrepareEnterpriseDetail((ContactBroadSoftEnterprise *)pDetail.Get());
        }
        break;
    case DIR_TYPE_BROADSOFT_ITEM_CUSTOM:
        {
            PrepareCustomDetail((ContactBroadSoftCustom *)pDetail.Get());
        }
        break;
    default:
        break;
    }
}

bool CBSFTContactControler::PrepareCommonDetail(ContactBroadSoftCommon * pContact)
{
    if (NULL == pContact)
    {
        return false;
    }

    AddDetailData(TRID_NAME, pContact->GetDisplayName());

    AddNumberDetailData(pContact, TRID_NUMBER, kszBsftNumber);

    return true;
}

bool CBSFTContactControler::PrepareEnterpriseDetail(ContactBroadSoftEnterprise * pContact)
{
    if (NULL == pContact)
    {
        return false;
    }

    AddDetailData(TRID_FIRST_NAME, pContact->m_strFirstName);

    AddDetailData(TRID_LAST_NAME, pContact->m_strLastName);

    AddNumberDetailData(pContact, TRID_NUMBER, kszBsftNumber);

    AddNumberDetailData(pContact, TRID_EXTENSION_NUMBER, kszBsftExtension);

    AddNumberDetailData(pContact, TRID_MOBILE_NUMBER, kszBsftMobile);

    AddDetailData(TRID_DEPARTMENT, pContact->m_strDepartment);

    AddDetailData(TRID_EMAIL, pContact->m_strEmail);

    return true;
}

bool CBSFTContactControler::PrepareCustomDetail(ContactBroadSoftCustom * pContact)
{
    if (NULL == pContact)
    {
        return false;
    }

    AddDetailData(TRID_FIRST_NAME, pContact->m_strFirstName);

    AddDetailData(TRID_LAST_NAME, pContact->m_strLastName);

    AddNumberDetailData(pContact, TRID_NUMBER, kszBsftNumber);

    AddNumberDetailData(pContact, TRID_EXTENSION_NUMBER, kszBsftExtension);

    AddNumberDetailData(pContact, TRID_MOBILE_NUMBER, kszBsftMobile);

    AddDetailData(TRID_DEPARTMENT, pContact->m_strDepartment);

    AddDetailData(TRID_EMAIL, pContact->m_strEmail);

    return true;
}

bool CBSFTContactControler::PrepareGroupDetail(ContactBroadSoftGroup * pContact)
{
    if (NULL == pContact)
    {
        return false;
    }

    AddDetailData(TRID_FIRST_NAME, pContact->m_strFirstName);

    AddDetailData(TRID_LAST_NAME, pContact->m_strLastName);

    AddNumberDetailData(pContact, TRID_NUMBER, kszBsftNumber);

    AddNumberDetailData(pContact, TRID_EXTENSION_NUMBER, kszBsftExtension);

    AddNumberDetailData(pContact, TRID_MOBILE_NUMBER, kszBsftMobile);

    AddDetailData(TRID_DEPARTMENT, pContact->m_strDepartment);

    AddDetailData(TRID_EMAIL, pContact->m_strEmail);

    return true;
}

void CBSFTContactControler::AddDetailData(const yl::string& strName, const yl::string& strValue)
{
    ST_DETAIL_DATA stDetail;

    stDetail.nType = DETAIL_DATA_TYPE_NONE;
    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(strName.c_str());
    stDetail.stAttr.m_strValue = strValue;

    m_vecDetail.push_back(stDetail);
}

void CBSFTContactControler::AddNumberDetailData(BaseContactRecord* pContact,
        const yl::string& strName, const yl::string& strNumberId)
{
    if (NULL == pContact)
    {
        return;
    }

    ST_DETAIL_DATA stDetail;

    stDetail.nType = DETAIL_DATA_TYPE_NUMBER;
    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(strName.c_str());

    if (!pContact->GetNumberByName(strNumberId, stDetail.stAttr.m_strValue))
    {
        stDetail.stAttr.m_strValue = "";
    }

    m_vecDetail.push_back(stDetail);
}

bool CBSFTContactControler::OnEnterCopyToPeronal(int nContactId)
{
    if (m_pUIHelper.IsEmpty())
    {
        return false;
    }

    SmartPtr pDetail = Dir_GetDetailById(nContactId, m_eLogicType);

    if (pDetail.IsEmpty())
    {
        return false;
    }

    LIST_ATTRIBUTE lsAttr;
    ST_CONTACT_ATTRIBUTE stAttr;

    stAttr.m_strName = DIR_ATTR_NAME;
    stAttr.m_strValue = pDetail->GetDisplayName();
    lsAttr.push_back(stAttr);

    if (!GetNumberByType(lsAttr, (BaseContactRecord*)pDetail.Get(), true))
    {
        return false;
    }

    SetAttributeList(m_nType, lsAttr);
    g_DirUIAdapter.EnterUI(DIRECTORY_TYPE_BROADSOFT, DIR_UI_STATE_ADDITION,
                           DIR_ACTION_BSFT_ADD_TO_PERSONAL, kszPersonalId);

    return true;
}

bool CBSFTContactControler::GetNumberByType(LIST_ATTRIBUTE & lsAttr,
        BaseContactRecord* pContact, bool bCopy /*= false*/)
{
    if (NULL == pContact)
    {
        return false;
    }

    switch (pContact->GetType())
    {
    case DIR_TYPE_BROADSOFT_ITEM_COMMON:
        {
            AddNumberByName(lsAttr, pContact, TRID_NUMBER, kszBsftNumber, bCopy);
        }
        break;
    case DIR_TYPE_BROADSOFT_ITEM_PERSONAL:
        {
            AddNumberByName(lsAttr, pContact, TRID_NUMBER, kszBsftNumber, false);
        }
        break;
    case DIR_TYPE_BROADSOFT_ITEM_CUSTOM:
    case DIR_TYPE_BROADSOFT_ITEM_ENTERPRISE:
    case DIR_TYPE_BROADSOFT_ITEM_GROUP:
        {
            AddNumberByName(lsAttr, pContact, TRID_NUMBER, kszBsftNumber, bCopy);
            AddNumberByName(lsAttr, pContact, TRID_EXTENSION_NUMBER, kszBsftExtension, bCopy);
            AddNumberByName(lsAttr, pContact, TRID_MOBILE_NUMBER, kszBsftMobile, bCopy);
        }
        break;
    default:
        {
            return false;
        }
        break;
    }

    return true;
}

void CBSFTContactControler::AddNumberByName(LIST_ATTRIBUTE & lsAttr, BaseContactRecord* pContact,
        const yl::string& strDispName, const yl::string& strNumberId, bool bCopy)
{
    if (NULL == pContact)
    {
        return;
    }

    ST_CONTACT_ATTRIBUTE stAttr;

    if (pContact->GetNumberByName(strNumberId, stAttr.m_strValue) && !stAttr.m_strValue.empty())
    {
        stAttr.m_strName = bCopy ? DIR_ATTR_NUMBER : strDispName;
        lsAttr.push_back(stAttr);
    }
}

bool CBSFTContactControler::OnCopyToPersonal(bool bOverwrite)
{
    IBSFTContactUIHelper * pBSFTUIHelper = g_DirUIAdapter.GetBSFTContactUIHelper(m_pUIHelper);

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
            return g_DirUIAdapter.PopSureMsgBox(TRID_WARNING, TRID_IF_OVERWRITE_THE_ORIGINAL_CONTACT,
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
        g_DirUIAdapter.ShowMsgBox(TRID_DISPLAY_NAME_CANNOT_BE_EMPTY, NULL, DIR_ACTION_NAME_INVALID_TIP);
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
        g_DirUIAdapter.ShowMsgBox(strMsg.c_str(), NULL, DIR_ACTION_NUMBER_INVALID_TIP);
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
        g_DirUIAdapter.ShowMsgBox(TRID_SAVING_FILES, NULL, nAction);
        return true;
    }
    else if (DIR_RET_CONTACT_EXIST == nRetCode)
    {
        g_DirUIAdapter.ShowMsgBox(TRID_CONTACT_EXIST);

        IBSFTContactUIHelper * pBSFTUIHelper = g_DirUIAdapter.GetBSFTContactUIHelper(m_pUIHelper);

        if (NULL != pBSFTUIHelper)
        {
            pBSFTUIHelper->SetInputFocus(DIR_ATTR_NAME);
        }
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

bool CBSFTContactControler::CheckDeletePersonal(int nRetCode)
{
    if (DIR_RET_SUCC == nRetCode)
    {
        g_DirUIAdapter.ShowMsgBox(TRID_DELETING);
        return true;
    }
    else if (DIR_RET_FAIL == nRetCode)
    {
        g_DirUIAdapter.ShowMsgBox(TRID_DELETE_FIALED);
    }
    else
    {
        CCommonDirCtrl::ShowErrorMsg(nRetCode);
    }

    return false;
}

bool CBSFTContactControler::AddPersonal()
{
    IBSFTContactUIHelper * pBSFTUIHelper = g_DirUIAdapter.GetBSFTContactUIHelper(m_pUIHelper);

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
        IDirUIHelperPtr pDirUIHelper = m_pUIHelper;
        IDirUIHelper * pUIHelper = pDirUIHelper.Get();

        if (NULL != pUIHelper)
        {
            pUIHelper->RefreshUIAfterDeleted();
        }
    }

    return bSuccess;
}

bool CBSFTContactControler::SavePersonal()
{
    IBSFTContactUIHelper * pBSFTUIHelper = g_DirUIAdapter.GetBSFTContactUIHelper(m_pUIHelper);

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

    IDirUIHelper * pUIHelper = m_pUIHelper.Get();

    if (NULL != pUIHelper)
    {
        pUIHelper->RefreshUI(DIR_UI_REFRESH_TYPE_LIST_VIEW | DIR_UI_REFRESH_TYPE_TITLE,
                             DIR_UI_REFRESH_PARAM_ALL);
    }
}

bool CBSFTContactControler::IsContactChanged()
{
    SmartPtr pDetail = GetDetail(m_nContactId);

    if (pDetail.IsEmpty() || DIR_TYPE_BROADSOFT_ITEM_PERSONAL != pDetail->GetType())
    {
        return false;
    }

    ContactBroadSoftPersonal * pPersonal = (ContactBroadSoftPersonal *)pDetail.Get();
    IBSFTContactUIHelper * pBSFTUIHelper = g_DirUIAdapter.GetBSFTContactUIHelper(m_pUIHelper);

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

bool CBSFTContactControler::PreciseSearch()
{
    CleanContactLoadInfo(kszServerSearchGroupId);

    IBSFTContactUIHelper * pBSFTUIHelper = g_DirUIAdapter.GetBSFTContactUIHelper(m_pUIHelper);

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
        g_DirUIAdapter.ShowMsgBox(TRID_PLEASE_ENTER_FILTER_CONDITIONS);
        return false;
    }

    return OnPreciseSearchByData(itemData, m_nPreciseSearchGroupId);
}

bool CBSFTContactControler::PreciseSearch(const yl::string& strSearchKey)
{
    CleanContactLoadInfo(kszServerSearchGroupId);

    if (strSearchKey.empty())
    {
        return;
    }

    ContactBroadSoftItemData itemData;
    itemData.m_strDisplayName = strSearchKey;
    itemData.m_strNumber = itemData.m_strDisplayName;
    itemData.m_bCriteriaMode = true;

    OnPreciseSearchByData(itemData, GetGroupId());

    return true;
}

bool CBSFTContactControler::OnPreciseSearchByData(ContactBroadSoftItemData& itemData, int nGroupId)
{
    if (m_pUIHelper.IsEmpty())
    {
        return false;
    }

    m_nPreciseSearchGroupId = nGroupId;

    _BsftPwdMgr.SetDirType(m_nType);

    int nStatus = _BsftPwdMgr.CheckBSFTPassword(m_pUIHelper.Get(), m_nPreciseSearchGroupId);

    if (XSI_SIPACCOUNT_NOT_AVAILABLE == nStatus)
    {
        AdjustPreciseState(PRECISE_SEARCH_STATE_RESULT);
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
        AdjustPreciseState(PRECISE_SEARCH_STATE_WAITING);
        ResetFailResult();

        g_DirUIAdapter.ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT, DIR_ACTION_BSFT_DOWNLOAD);
        return true;
    }

    if (DIR_RET_SUCC == retCode)
    {
        AdjustPreciseState(PRECISE_SEARCH_STATE_RESULT);
    }
    else if (DIR_RET_SEARCH_PARM_ERROR != retCode)
    {
        CCommonDirCtrl::ShowErrorMsg(retCode);
    }

    return true;
}

bool CBSFTContactControler::IsCanEnterGroupByKey(int nKey)
{
    if (kszCustomId == GetGroupId())
    {
        return true;
    }

    return false;
}

bool CBSFTContactControler::PreLoadContact(int nGroupId)
{
    DIRUI_INFO("CBSFTContactControler::PreLoadContact(%d)", nGroupId);

    int nOldGroupId = GetGroupId();
    if (knInvalidRId != nOldGroupId)
    {
        CleanContactLoadInfo(nOldGroupId);
    }

    if (nOldGroupId != nGroupId && knInvalidRId != nGroupId)
    {
        CleanContactLoadInfo(nGroupId);
    }

    AdjustPreciseState(PRECISE_SEARCH_STATE_NONE);

    if (!CCommonDirCtrl::PreLoadContact(nGroupId))
    {
        return false;
    }

    if (!BWCallControl_IsAccountXSIEnable(0))
    {
        g_DirUIAdapter.ShowMsgBox(TRID_XSI_INFORMATION_IS_INCOMPLETE);
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
        m_nDownloadGroupId = knInvalidRId;
        return true;
    }

    m_nDownloadGroupId = nGroupId;

    _BsftPwdMgr.SetDirType(m_nType);

    if (kszAllContactId != nGroupId)
    {
        int nXsiStatus = _BsftPwdMgr.CheckBSFTPassword(m_pUIHelper.Get(), nGroupId);

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
            CCommonDirCtrl::ShowErrorMsg(nRetCode);
            return true;
        }
    }

    m_bDownloading = true;
    ResetFailResult();

    g_DirUIAdapter.ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT, DIR_ACTION_BSFT_DOWNLOAD);
    return true;
}

bool CBSFTContactControler::PreSearchContact()
{
    if (IsManualSearch())
    {
        return PreciseSearch(GetSearchKey());
    }

    AdjustPreciseState(PRECISE_SEARCH_STATE_NONE);

    return CCommonDirCtrl::PreSearchContact();
}

#if DIR_UI_FEATURE_LOAD_DATA_LIST
bool CBSFTContactControler::LoadMenuData(int nDataIndex, int nDataCount)
{
    if (!BWCallControl_IsAccountXSIEnable(0))
    {
        g_DirUIAdapter.ShowMsgBox(TRID_XSI_INFORMATION_IS_INCOMPLETE);
        return true;
    }

    return CCommonDirCtrl::LoadMenuData(nDataIndex, nDataCount);
}
#endif

bool CBSFTContactControler::OnAction(int nAction, int nDataIndex, int nId)
{
    DIRUI_INFO("CBSFTContactControler::OnAction(Action:%d Index:%d Id:%d)",
               nAction, nDataIndex, nId);

    if (kszViewMoreId == nId && DIR_ACTION_COMMON_SELECT == nAction)
    {
        OnViewMoreContact();
        return true;
    }

    switch (nAction)
    {
    case DIR_ACTION_COMMON_DETAIL:
        {
            if (IsPersonal(nId))
            {
                EnterDetail(nId, nDataIndex, DIR_ACTION_BSFT_EDIT_PERSONAL);

                return true;
            }
        }
        break;
    case DIR_ACTION_BSFT_UPDATE:
        {
#ifdef IF_FEATURE_DIR_OPT
            if (IsBrowseState() && GetGroupId() != knInvalidRId)
            {
                nId = GetGroupId();
            }
#endif
            UpdateContacts(nId);

            return true;
        }
        break;
    case DIR_ACTION_BSFT_ENTER_ADD_PERSONAL:
        {
            LIST_ATTRIBUTE lsAttr;

            SetAttributeList(DIRECTORY_TYPE_NONE, lsAttr);

            g_DirUIAdapter.EnterUI(m_nType, DIR_UI_STATE_ADDITION, DIR_ACTION_BSFT_ADD_PERSONAL, GetGroupId());

            return true;
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
            OnEnterCopyToPeronal(nId);

            return true;
        }
        break;
    case DIR_ACTION_BSFT_ADD_TO_PERSONAL:
        {
            OnCopyToPersonal(false);

            return true;
        }
        break;
    case DIR_ACTION_BSFT_CHG_PWD:
        {
            int nGroupId = _BsftPwdMgr.GetGroupId();

            if (_BsftPwdMgr.RetryBSFTPwd(m_pUIHelper.Get()))
            {
                m_nDownloadGroupId = nGroupId;
                m_bDownloading = true;
                ResetFailResult();

                g_DirUIAdapter.ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT,
                                           DIR_ACTION_BSFT_DOWNLOAD);
            }

            return true;
        }
        break;
#ifdef DIR_SUPPORT_BATCH_OPERATE
    case DIR_ACTION_BSFT_BATCH_DEL_PERSONAL:
        {
            IDirUIHelper * pUIHelper = m_pUIHelper.Get();

            if (NULL != pUIHelper && !pUIHelper->IsSelectAllContact())
            {
                return g_DirUIAdapter.PopSureMsgBox(TRID_WARNING, TRID_IF_DELETE_ALL_CONTACT_ITEMS, nAction, nId);
            }
            else
            {
                return g_DirUIAdapter.PopSureMsgBox(TRID_WARNING, TRID_IF_DELETE_SELECTED_CONTACT_ITEM, nAction,
                                                    nId);
            }

            return true;
        }
        break;
    case DIR_ACTION_BSFT_BATCH_MOVE_TO_PERSONAL:
        {
            return g_DirUIAdapter.PopSureMsgBox(TRID_WARNING, TRID_ASK_ADD_SELECTED_TO_PERSONAL, nAction, nId);
        }
        break;
#endif
    case DIR_ACTION_BSFT_ENTER_PRECISE_SEARCH:
        {
            IBSFTContactUIHelper * pBSFTUIHelper = g_DirUIAdapter.GetBSFTContactUIHelper(m_pUIHelper);

            if (NULL == pBSFTUIHelper)
            {
                DIRUI_ERR("BSFTUIHelper is empty when enter precise search.");
                break;
            }

            if (IsPreciseSearchResult()
                    && !BsftDir_UpdateEnable()
                    && DIR_UI_STATE_PRECISE_SEARCH == GetState())
            {
                m_bBackToSearchResult = true;
            }
            else
            {
                m_bBackToSearchResult = false;
            }

            pBSFTUIHelper->EnterPreciseSearch();

            return true;
        }
        break;
    case DIR_ACTION_BSFT_PRECISE_SEARCH:
        {
            PreciseSearch();

            return true;
        }
        break;
    case DIR_ACTION_BSFT_BACK_TO_MENU_LIST:
        {
            CheckBackToMenuList();
            return true;
        }
    case DIR_ACTION_BSFT_BACK_TO_SEARCH_RESULT:
        {
            CheckBackToSearchResult();
            return true;
        }
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

            if (!g_DirUIAdapter.CheckState(nCheckUIState, nUIState))
            {
                break;
            }

            g_DirUIAdapter.ExitCurrentUI(nUIState);

            if (IsSearchState())
            {
                RefreshAfterPersonalChange();
            }
        }
        break;
    case DIR_ACTION_BSFT_COPY_PERSONAL_FINISH:
        {
            int nUIState = GetState();

            if (!g_DirUIAdapter.CheckState(DIR_UI_STATE_ADDITION, nUIState))
            {
                break;
            }

            g_DirUIAdapter.ExitCurrentUI(nUIState);
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
            IDirUIHelper * pUIHelper = m_pUIHelper.Get();

            if (NULL == pUIHelper || !pUIHelper->GetSelectContactId(vecContactId) || vecContactId.size() <= 0)
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
            IDirUIHelper * pUIHelper = m_pUIHelper.Get();

            if (NULL == pUIHelper || !pUIHelper->GetSelectContactId(vecContactId))
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

            g_DirUIAdapter.ShowMsgBox(TRID_ADD_CONTACTS_SUCCESS);

            IDirUIHelper * pUIHelper = m_pUIHelper.Get();

            if (NULL != pUIHelper)
            {
                pUIHelper->OnBatchCopyContactSuccess();
            }

            return true;
        }
        break;
#endif
    case DIR_ACTION_NAME_INVALID_TIP:
        {
            IBSFTContactUIHelper * pBSFTUIHelper = g_DirUIAdapter.GetBSFTContactUIHelper(m_pUIHelper);

            if (NULL != pBSFTUIHelper)
            {
                pBSFTUIHelper->SetInputFocus(DIR_ATTR_NAME);
            }
        }
        break;
    case DIR_ACTION_NUMBER_INVALID_TIP:
        {
            IBSFTContactUIHelper * pBSFTUIHelper = g_DirUIAdapter.GetBSFTContactUIHelper(m_pUIHelper);

            if (NULL != pBSFTUIHelper)
            {
                pBSFTUIHelper->SetInputFocus(DIR_ATTR_NUMBER);
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

bool CBSFTContactControler::IsGroupData(SmartPtr & pDetail)
{
    if (pDetail.IsEmpty())
    {
        return false;
    }

    if (DIR_TYPE_BROADSOFT_MENU == pDetail->GetType())
    {
        return true;
    }

    return CCommonDirCtrl::IsGroupData(pDetail);
}

int CBSFTContactControler::GetBrowseItemAmount()
{
    if (!BWCallControl_IsAccountXSIEnable(0))
    {
        return 0;
    }

    int nItemAmount = GetGroupChildrenAmount(m_nGroupId, DIR_TYPE_BROADSOFT_MENU);

    if (DIR_INVALID_AMOUNT != nItemAmount)
    {
        return nItemAmount;
    }

    return CCommonDirCtrl::GetBrowseItemAmount();
}

bool CBSFTContactControler::CancelAction(int nAction, int nDataIndex, int nId)
{
    if (DIR_ACTION_BSFT_UPDATE == nAction || DIR_ACTION_BSFT_DOWNLOAD == nAction)
    {
        if (knInvalidRId != m_nDownloadGroupId && (m_bDownloading || m_bUpdating))
        {
            Dir_CancelLoad(m_nDownloadGroupId, m_eLogicType);
        }

        m_bDownloading = false;
        m_bUpdating = false;
        m_nDownloadGroupId = knInvalidRId;

        RefreshUI(DIR_UI_REFRESH_TYPE_LIST_VIEW | DIR_UI_REFRESH_TYPE_TITLE,
                  DIR_UI_REFRESH_PARAM_NONE);
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

    if (!GetNumberByType(lsNumber, (BaseContactRecord*)pDetail.Get(), false))
    {
        return false;
    }

    return Dial(nAccountId, pDetail->GetDisplayName(), lsNumber, GetDialAction());
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
                g_DirUIAdapter.EndWaiting();

                m_bUpdating = false;
                m_bDownloading = false;
            }

            _BsftPwdMgr.ExitBSFTPwdUI();
            g_DirUIAdapter.ExitAllUI(m_nType);
        }
        break;
    case BROADSOFT_PHONEBOOK_POST_RESULT:
        {
            if (!m_pUIHelper.IsEmpty() && 1 == refMessage.lParam)
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

bool CBSFTContactControler::IsHasNextPageData()
{
    if (BsftDir_UpdateEnable())
    {
        return false;
    }

    int nUiStatus = GetState();
    int nGroupID = GetGroupId();
    if (DIR_UI_STATE_BROWSER == nUiStatus && kszCustomId == GetParGroupId(nGroupID))
    {
        return BsftDir_HasNextPage(nGroupID);
    }

    if (DIR_UI_STATE_PRECISE_SEARCH != nUiStatus && !IsManualSearch())
    {
        return false;
    }

    return BsftDir_HasNextPage(kszServerSearchGroupId);
}

void CBSFTContactControler::CleanContactLoadInfo(int nGroupId)
{
    if (!BsftDir_UpdateEnable())
    {
        BsftDir_ClearContactInfo(nGroupId);
    }
}

bool CBSFTContactControler::IsViewMoreId(int nIndex)
{
    if (BsftDir_UpdateEnable())
    {
        return false;
    }

    int nID = GetGroupId();
    int nState = GetState();
    if (DIR_UI_STATE_PRECISE_SEARCH == nState || DIR_UI_STATE_SEARCH == nState)
    {
        nID = kszServerSearchGroupId;
    }

    SmartPtr pDetail = Dir_GetDetailById(nID, m_eLogicType);

    if (pDetail.IsEmpty() || DIR_TYPE_BROADSOFT_MENU != pDetail->GetType())
    {
        return false;
    }

    DirGroupRecord * pGroupRecord = (DirGroupRecord *)pDetail.Get();
    if (nIndex < 0 || nIndex >= pGroupRecord->m_vChildrenId.size()
            || kszViewMoreId != pGroupRecord->m_vChildrenId[nIndex])
    {
        return false;
    }

    return true;
}

void CBSFTContactControler::OnDownLoadFinished(msgObject & refMessage)
{
    if (m_pUIHelper.IsEmpty())
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
            g_DirUIAdapter.EndWaiting();
        }

        m_bUpdating = false;
        m_bDownloading = false;

        if (bMsgEnd)
        {
            m_nDownloadGroupId = knInvalidRId;

            if (IsResultSuccess(refMessage.lParam))
            {
                _BsftPwdMgr.ExitBSFTPwdUI();
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
            g_DirUIAdapter.EndWaiting();
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
            _BsftPwdMgr.ExitBSFTPwdUI();
        }
        else
        {
            ShowErrorResult(nDownloadGroupId, nFailResult);
        }

        if (kszCustomId == nDownloadGroupId && BsftDir_UpdateEnable())
        {
            g_DirUIAdapter.ReturnRootGroup(m_nType);
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

    SetTotalCount(Dir_GetContactCount(GetResultGroupId(), m_eLogicType));
}
#endif

void CBSFTContactControler::RefreshAfterDownload(int nGroupId)
{
    if ((nGroupId == m_nGroupId || kszAllContactId == m_nGroupId)
#ifdef DIR_SUPPORT_SETTING
            && (IsBrowseState() || IsSettingState()))
#else
            && IsBrowseState())
#endif
    {
        RefreshUI(DIR_UI_REFRESH_TYPE_LIST_VIEW | DIR_UI_REFRESH_TYPE_TITLE,
                  DIR_UI_REFRESH_PARAM_DATA_CHANGE);
    }
}

void CBSFTContactControler::OnPreciseSearchResult(msgObject & refMessage)
{
    DIRUI_INFO("CBSFTContactControler::OnPreciseSearchResult(GroupId:%d wp:%d lp:%d)",
               m_nPreciseSearchGroupId, refMessage.wParam, refMessage.lParam);

    int nUiStatus = GetState();
    if ((DIR_UI_STATE_PRECISE_SEARCH != nUiStatus && DIR_UI_STATE_SEARCH != nUiStatus)
            || (PRECISE_SEARCH_STATE_WAITING != m_nPreciseSearchState
                && PRECISE_SEARCH_STATE_RESULT != m_nPreciseSearchState
                && PRECISE_SEARCH_STATE_WAITING_VIEW_MORE != m_nPreciseSearchState))
    {
        return;
    }

    int nParam = DIR_UI_REFRESH_PARAM_DATA_CHANGE;
    if (PRECISE_SEARCH_STATE_WAITING == m_nPreciseSearchState
            || PRECISE_SEARCH_STATE_WAITING_VIEW_MORE == m_nPreciseSearchState)
    {
        AdjustPreciseState(PRECISE_SEARCH_STATE_RESULT);
        g_DirUIAdapter.EndWaiting();

        if (BsftDir_UpdateEnable() && PRECISE_SEARCH_STATE_WAITING == m_nPreciseSearchState)
        {
            nParam = DIR_UI_REFRESH_PARAM_ALL;
        }
    }

    if (!IsResultSuccess(refMessage.lParam))
    {
        ShowErrorResult(m_nPreciseSearchGroupId, refMessage.lParam);
    }

    RefreshUI(DIR_UI_REFRESH_TYPE_ALL, nParam);
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

        _BsftPwdMgr.SetDirType(m_nType);
        _BsftPwdMgr.CheckBSFTPassword(m_pUIHelper.Get(), nGroupId);
    }
    else if (IsSvrURLInvalid(nResult))
    {
        g_DirUIAdapter.ShowMsgBox(TRID_SERVER_URL_IS_INVALID);
    }
    else
    {
        g_DirUIAdapter.ShowMsgBox(TRID_FAIL_TO_CONNECT_TO_SERVER);
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

#if DIR_UI_FEATURE_LOAD_DATA_LIST
bool CBSFTContactControler::LoadPreciseData(int nDataIndex, int nDataCount)
{
    DIRUI_INFO("LoadPreciseData(State:%d DataIndex:%d DataCount:%d)",
               m_nPreciseSearchState, nDataIndex, nDataCount);

    if (PRECISE_SEARCH_STATE_ENTER == m_nPreciseSearchState)
    {
        IBSFTContactUIHelper * pBSFTUIHelper = g_DirUIAdapter.GetBSFTContactUIHelper(m_pUIHelper);

        if (NULL == pBSFTUIHelper)
        {
            return false;
        }

        pBSFTUIHelper->LoadBSFTPreciseSearchData(nDataIndex, nDataCount);
        return true;
    }
    else if (PRECISE_SEARCH_STATE_RESULT == m_nPreciseSearchState)
    {
        ICommonUIHelper * pCommonUIHelper = g_DirUIAdapter.GetCommonUIHelper(m_pUIHelper);

        if (NULL == pCommonUIHelper)
        {
            return false;
        }

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
        pCommonUIHelper->FillData(lsData);
        return true;
    }

    return false;
}
#endif

int CBSFTContactControler::GetResultGroupId()
{
    if (PRECISE_SEARCH_STATE_RESULT == m_nPreciseSearchState)
    {
        return kszServerSearchGroupId;
    }

    if (IsSearchState() && !BsftDir_UpdateEnable())
    {
        return kszServerSearchGroupId;
    }

    return CCommonDirCtrl::GetResultGroupId();
}

void CBSFTContactControler::OnViewMoreContact()
{
    DIRUI_INFO("CBSFTContactControler::OnViewMoreContact(%d)", GetGroupId());

    if (!BWCallControl_IsAccountXSIEnable(0))
    {
        g_DirUIAdapter.ShowMsgBox(TRID_XSI_INFORMATION_IS_INCOMPLETE);
        return;
    }

    //高级搜索点击view more加载ID应该是搜索的ID，不是自身ID
    int nGroupId = GetGroupId();
    if (DIR_UI_STATE_PRECISE_SEARCH == GetState())
    {
        nGroupId = m_nPreciseSearchGroupId;
    }

    m_nDownloadGroupId = nGroupId;

    SmartPtr pDetail = Dir_GetDetailById(nGroupId, m_eLogicType);
    DirGroupRecord * pMenu = GetGroupRecord(pDetail);

    if (NULL == pMenu || FILE_DOWNLOADING == pMenu->m_eStatus)
    {
        return;
    }

    _BsftPwdMgr.SetDirType(m_nType);

    int nXsiStatus = _BsftPwdMgr.CheckBSFTPassword(m_pUIHelper.Get(), nGroupId);
    if (XSI_SUCCESS != nXsiStatus)
    {
        return;
    }

    bool bSearch = DIR_UI_STATE_BROWSER != GetState() ? true : false;
    int nRetCode = BsftDir_ReqNextPageContact(nGroupId, bSearch);

    if (DIR_RET_SUCC != nRetCode)
    {
        m_nDownloadGroupId = knInvalidRId;
        CCommonDirCtrl::ShowErrorMsg(nRetCode);
        return;
    }

    m_bDownloading = true;
    ResetFailResult();

    if (bSearch)
    {
        m_nPreciseSearchState = PRECISE_SEARCH_STATE_WAITING_VIEW_MORE;
    }

    g_DirUIAdapter.ShowWaiting(TRID_NOTE, TRID_UPDATING_PLEASE_WAIT, DIR_ACTION_BSFT_DOWNLOAD);
}
