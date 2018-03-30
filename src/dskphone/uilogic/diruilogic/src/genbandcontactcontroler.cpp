#include "genbandcontactcontroler.h"

#if IF_FEATURE_GBBOOK

#include "igenbandcontactuihelper.h"
#include "diruiadapter.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "translateiddefine.h"
#include "uilogiccommon/include/uilogiclanghelper.h"

CGenbandContactControler::CGenbandContactControler()
    : CPreciseSearchControlBase(DIRECTORY_TYPE_GENBAND, GB_DIR)
    , m_bDownloading(false)
    , m_bUpdating(false)
    , m_nDownloadGroupId(knInvalidRId)
    , m_nSearchMode(PAB_GAB_NONE)
{
}

CGenbandContactControler::~CGenbandContactControler()
{
}

CGenbandContactControler* CGenbandContactControler::GetControler(CDirectoryControlerPtr& pControler)
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
    ClearSearchData();

    CPreciseSearchControlBase::UnInit();
}

void CGenbandContactControler::OnEnterPreciseSearch()
{
    IGenbandContactUIHelper* pUiHelper = g_DirUIAdapter.GetGenbandContactUIhelper(m_pUIHelper);
    if (NULL != pUiHelper)
    {
        pUiHelper->OnEnterPreciseSearch();
    }
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
    DIRUI_DEBUG("CGenbandContactControler::UpdateContacts(%d)", m_nDownloadGroupId);

    if (nGroupId == kszGabBookGroupId)
    {
        DIRECTORY_RET_CODE eRetCode = GAB_ReqSearchGABByType(GAB_GetCurentGabKey(),
                                      GAB_GetCurrentGabOptType());
        if (eRetCode != DIR_RET_SUCC && eRetCode != GET_LIST_DOWNLOADING)
        {
            CCommonDirCtrl::ShowErrorMsg(eRetCode);
            return;
        }
    }
    else if (!PAB_UpdatePabData())
    {
        CCommonDirCtrl::ShowErrorMsg(DIR_RET_FAIL);
        return;
    }

    m_bUpdating = true;
    g_DirUIAdapter.ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT, DIR_ACTION_GENBAND_UPDATE);
}

bool CGenbandContactControler::PreLoadContact(int nGroupId)
{
    DIRUI_DEBUG("CGenbandContactControler::PreLoadContact(%d)", nGroupId);

    if (!CCommonDirCtrl::PreLoadContact(nGroupId))
    {
        return false;
    }

    m_nPreciseSearchState = PRECISE_SEARCH_STATE_NONE;

    if (nGroupId == kszGabBookGroupId)
    {
        GBBOOK_OPERATION_TYPE eType = GAB_GetCurrentGabOptType();
        if (eType >= GAB_SEARCH_NAME && eType <= GAB_SEARCH_NUMBER)
        {
            m_nPreciseSearchState = PRECISE_SEARCH_STATE_RESULT;
            DIRECTORY_RET_CODE eRetCode = GAB_ReqSearchGABByType(GAB_GetCurentGabKey(), eType);
            if (eRetCode == DIR_RET_SUCC || eRetCode == GET_LIST_DOWNLOADING)
            {
                m_bUpdating = true;
                g_DirUIAdapter.ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT, DIR_ACTION_GENBAND_UPDATE);
            }
            else
            {
                CCommonDirCtrl::ShowErrorMsg(eRetCode);
            }
        }
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
                CCommonDirCtrl::ShowErrorMsg(nRetCode);
                return true;
            }
        }

        m_bDownloading = true;

        g_DirUIAdapter.ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT, DIR_ACTION_GENBAND_DOWNLOAD);
    }

    return true;
}

bool CGenbandContactControler::OnAction(int nAction, int nDataIndex, int nId)
{
    DIRUI_DEBUG("CGenbandContactControler::OnAction(Action:%d Index:%d Id:%d)",
                nAction, nDataIndex, nId);

    if (DIR_ACTION_COMMON_SELECT == nAction && DIR_UI_STATE_CHOOSE_ACCOUNT == GetState())
    {
        IGenbandContactUIHelper* pUIHelper = g_DirUIAdapter.GetGenbandContactUIhelper(m_pUIHelper);
        if (NULL != pUIHelper)
        {
            pUIHelper->OnChooseAccountResult(nId);
        }
        return true;
    }

    if (CCommonDirCtrl::OnAction(nAction, nDataIndex, nId))
    {
        return true;
    }

    switch (nAction)
    {
    //高级搜索
    case DIR_ACTION_GBBOOK_SEARCH_MODE_CHANGE:
        {
            IGenbandContactUIHelper* pGenband = g_DirUIAdapter.GetGenbandContactUIhelper(m_pUIHelper);
            if (NULL != pGenband)
            {
                pGenband->ShowSearchModeOption(GetState());
            }
        }
        break;
    case DIR_ACTION_GENBAND_SEARCH_BY_NAME:
    case DIR_ACTION_GENBAND_SEARCH_BY_FIRSTNAME:
    case DIR_ACTION_GENBAND_SEARCH_BY_LASTNAME:
    case DIR_ACTION_GENBAND_SEARCH_BY_NUMBER:
        {
            IGenbandContactUIHelper* pGenband = g_DirUIAdapter.GetGenbandContactUIhelper(m_pUIHelper);
            if (NULL != pGenband)
            {
                UpdateSearchMode(nAction);
                pGenband->EnterPreciseSearch();
            }
        }
        break;
    case DIR_ACTION_GENBAND_PRECISE_SEARCH:
        {
            PreciseSearch();
        }
        break;
    //组管理
    case DIR_ACTION_GENBAND_ENTER_MANAGER_GROUP:
        {
            OnEnterManagerGroup(nAction);
            return true;
        }
        break;
    case DIR_ACTION_GENBAND_UPDATE:
        {
            int nGroupId = Dir_GetParentIdById(nId, GB_DIR);
            UpdateContacts(nGroupId);
        }
        break;
    //组
    case DIR_ACTION_GENBAND_ENTER_ADD_GROUP:
        {
            g_DirUIAdapter.EnterUI(m_nType, DIR_UI_STATE_ADDITION, DIR_ACTION_GENBAND_ADD_GROUP, kszPabBookGroupId);
            return true;
        }
        break;
    case DIR_ACTION_GENBAND_ADD_GROUP:
        {
            return OnAddGroup();
        }
        break;
    case DIR_ACTION_GENBAND_ENTER_EDIT_GROUP:
        {
            m_nContactId = nId;
            g_DirUIAdapter.EnterUI(m_nType, DIR_UI_STATE_DETAIL, DIR_ACTION_GENBAND_EDIT_GROUP, nId);
            return true;
        }
        break;
    case DIR_ACTION_GENBAND_EDIT_GROUP:
        {
            OnSaveGroup(m_nContactId);
        }
        break;
    case DIR_ACTION_GENBAND_DEL_GROUP:
        {
            return OnDelGroup(nId);
        }
        break;
    // PAB
    case DIR_ACTION_GENBAND_ENTER_ADD_PAB:
        {
            m_nContactId = knInvalidRId;
            g_DirUIAdapter.EnterUI(m_nType, DIR_UI_STATE_ADDITION, DIR_ACTION_GENBAND_ADD_PAB, GetGroupId());
        }
        break;
    case DIR_ACTION_GENBAND_ADD_PAB:
        {
            return AddPabContact();
        }
        break;
    case DIR_ACTION_GENBAND_ENTER_EDIT_PAB:
        {
            if (DIR_UI_STATE_DETAIL == GetState())
            {
                g_DirUIAdapter.ExitCurrentUI(DIR_UI_STATE_DETAIL);
            }
            else
            {
                m_nContactId = nId;
            }
            g_DirUIAdapter.EnterUI(m_nType, DIR_UI_STATE_DETAIL, DIR_ACTION_GENBAND_EDIT_PAB, nId);
            return true;
        }
        break;
    case DIR_ACTION_GENBAND_EDIT_PAB:
        {
            return SavePabContact();
        }
        break;
    case DIR_ACTION_GENBAND_DEL_PAB:
        {
            OnDelPabContact(nId);
            return true;
        }
        break;
    case DIR_ACTION_GENBAND_ENTER_ADD_TO_PAB:
        {
            m_nContactId = nId;

            g_DirUIAdapter.EnterUI(m_nType, DIR_UI_STATE_ADDITION, DIR_ACTION_GENBAND_ADD_TO_PAB, m_nContactId);
        }
        break;
    case DIR_ACTION_GENBAND_ADD_TO_PAB:
        {
            OnCopyToPab(false);
        }
        break;
    case  DIR_ACTION_GENBAND_SELECT_ACCOUNT:
        {
            g_DirUIAdapter.EnterUI(m_nType, DIR_UI_STATE_CHOOSE_ACCOUNT, DIR_ACTION_GENBAND_SELECT_ACCOUNT, nId);
        }
    default:
        {
            DIRUI_ERR("Action:%d is not be Processed in OnAction", nAction);
            return false;
        }
        break;
    }

    return true;
}

bool CGenbandContactControler::DialContact(int nContactId, int nAccountId /* = AUTO_ACCOUNT_IDENTIFY */)
{
    SmartPtr pDetail = GetDetail(nContactId);

    if (pDetail.IsEmpty())
    {
        return false;
    }

    m_nContactId = nContactId;

    LIST_ATTRIBUTE lsNumber;

    if (DIR_TYPE_GB_UNIT == pDetail->GetType())
    {
        IGenbandContactUIHelper* pGenbandUIHelper = g_DirUIAdapter.GetGenbandContactUIhelper(m_pUIHelper);
        if (NULL != pGenbandUIHelper)
        {
            pGenbandUIHelper->GetGenbandNumber(lsNumber, (ContactGBBookUnit *)pDetail.Get());
        }
    }

    return Dial(nAccountId, pDetail->GetDisplayName(), lsNumber, GetDialAction());
}

bool CGenbandContactControler::ExecAction(int nAction, int nDataIndex, int nId)
{
    DIRUI_DEBUG("CGenbandContactControler::ExecAction(Action:%d Index:%d Id:%d)",
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
    case DIR_ACTION_GENBAND_ADD_TO_PAB:
        {
            OnCopyToPab(true);
        }
        break;
    case DIR_ACTION_GENBAND_ADD_PAB:
    case DIR_ACTION_GENBAND_EDIT_PAB:
        {
            int nCheckUIState = (DIR_ACTION_GENBAND_EDIT_PAB == nAction) ? DIR_UI_STATE_DETAIL : DIR_UI_STATE_ADDITION;

            if (nCheckUIState == GetState())
            {
                g_DirUIAdapter.ExitCurrentUI(nCheckUIState);

                if (IsSearchState())
                {
                    RefreshAfterPabContactChange();
                }
            }
        }
        break;
    case DIR_ACTION_GENBAND_COPY_PERSONAL_FINISH:
        {
            if (DIR_UI_STATE_ADDITION == GetState())
            {
                g_DirUIAdapter.ExitCurrentUI(DIR_UI_STATE_ADDITION);
            }
        }
        break;
    case DIR_ACTION_GENBAND_ADD_GROUP:
    case DIR_ACTION_GENBAND_EDIT_GROUP:
        {
            if (DIR_UI_STATE_ADDITION == GetState())
            {
                g_DirUIAdapter.ExitCurrentUI(DIR_UI_STATE_ADDITION);
                RefreshAfterPabContactChange();
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

bool CGenbandContactControler::CancelAction(int nAction, int nDataIndex, int nId)
{
    if (DIR_ACTION_GENBAND_UPDATE == nAction || DIR_ACTION_GENBAND_DOWNLOAD == nAction)
    {
        if (knInvalidRId != m_nDownloadGroupId && (m_bDownloading || m_bUpdating))
        {
            Dir_CancelLoad(m_nDownloadGroupId, m_eLogicType);
        }

        if (kszGabBookGroupId == m_nDownloadGroupId)
        {
            g_DirUIAdapter.ExitCurrentUI(DIR_UI_STATE_PRECISE_SEARCH);
        }

        m_bDownloading = false;
        m_bUpdating = false;
        m_nDownloadGroupId = knInvalidRId;
        RefreshUI(DIR_UI_REFRESH_TYPE_NONE, DIR_UI_REFRESH_PARAM_NONE);
        return true;
    }

    return false;
}

LRESULT CGenbandContactControler::OnMessage(msgObject & refMessage)
{
    if (LOCAL_SEARCH_RESULT != refMessage.message
            && (refMessage.message < GENBAND_BOOK_REQ_RESULT
                || refMessage.message > GENBAND_BOOK_RELOAD))
    {
        return FALSE;
    }

    DIRUI_DEBUG("CGenbandContactControler::OnMessage MSG:%d WP:%d LP:%d", refMessage.message,
                refMessage.wParam, refMessage.lParam);

    switch (refMessage.message)
    {
    case GENBAND_BOOK_RELOAD:
        {
            m_nDownloadGroupId = knInvalidRId;
            SetGroupId(knInvalidRId);

            if (m_bUpdating || m_bDownloading)
            {
                g_DirUIAdapter.EndWaiting();

                m_bUpdating = false;
                m_bDownloading = false;
            }

            if (DIR_UI_STATE_PRECISE_SEARCH == GetState())
            {
                g_DirUIAdapter.ExitCurrentUI(DIR_UI_STATE_PRECISE_SEARCH);
            }

            g_DirUIAdapter.ExitAllUI(m_nType);
        }
        break;
    case GENBAND_BOOK_REQ_RESULT:
        {
            if (refMessage.wParam >= PAB_OPERATION_ADD_GROUP
                    && refMessage.wParam <= PAB_OPERATION_GET_GROUPLIST)
            {
                RefreshAfterPabContactChange(refMessage);
            }
            else
            {
                if (m_bUpdating || m_bDownloading)
                {
                    g_DirUIAdapter.EndWaiting();

                    m_bUpdating = false;
                    m_bDownloading = false;
                }

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

int CGenbandContactControler::GetBrowseItemAmount()
{
    int nItemAmount = GetGroupChildrenAmount(m_nGroupId, DIR_TYPE_GB_MENU);

    if (DIR_INVALID_AMOUNT != nItemAmount)
    {
        return nItemAmount;
    }

    return CCommonDirCtrl::GetBrowseItemAmount();
}

bool CGenbandContactControler::OnBack()
{
    if (PRECISE_SEARCH_STATE_NONE != m_nPreciseSearchState)
    {
        m_nPreciseSearchState = PRECISE_SEARCH_STATE_NONE;

        if (kszGabBookGroupId == GetGroupId() && DIR_UI_STATE_BROWSER == GetState())
        {
            Dir_ClearContactData(kszGabBookGroupId, GB_DIR);
            if (configParser_GetConfigInt(kszGbRetainSearchFilter) != 1)
            {
                GAB_ClearSearchInfo();
            }
        }

        return false;
    }

    int nCurGroupId = GetGroupId();

    if (knInvalidRId == nCurGroupId)
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

int CGenbandContactControler::GetSearchMode()
{
    return m_nSearchMode;
}

bool CGenbandContactControler::IsGroupData(SmartPtr & pDetail)
{
    if (pDetail.IsEmpty() || DIR_TYPE_GB_MENU != pDetail->GetType())
    {
        return false;
    }

    return true;
}

void CGenbandContactControler::UpdateSearchMode(int nAction)
{
    if (DIR_ACTION_GENBAND_SEARCH_BY_NAME == nAction)
    {
        m_nSearchMode = GAB_SEARCH_NAME;
    }
    else if (DIR_ACTION_GENBAND_SEARCH_BY_FIRSTNAME == nAction)
    {
        m_nSearchMode = GAB_SEARCH_FIRSTNAME;
    }
    else if (DIR_ACTION_GENBAND_SEARCH_BY_LASTNAME == nAction)
    {
        m_nSearchMode = GAB_SEARCH_LASTNAME;
    }
    else if (DIR_ACTION_GENBAND_SEARCH_BY_NUMBER == nAction)
    {
        m_nSearchMode = GAB_SEARCH_NUMBER;
    }
    else
    {
        m_nSearchMode = PAB_GAB_NONE;
    }
}

void CGenbandContactControler::PrepareDetailData()
{
    m_vecDetail.clear();

    SmartPtr pDetail = GetCurrentDetail();
    ContactGBBookUnit* pGenbandContact = CGenbandContactControler::GetGenbandContact(pDetail);
    if (NULL == pGenbandContact)
    {
        return;
    }

    ST_DETAIL_DATA stDetail;

    stDetail.nType = DETAIL_DATA_TYPE_NONE;
    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_NICK_NAME);
    stDetail.stAttr.m_strValue = pGenbandContact->GetDisplayName();
    m_vecDetail.push_back(stDetail);

    if (!IsPreciseSearchResult())
    {
        stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_GROUP);
        stDetail.stAttr.m_strValue = Dir_GetDisplayNameById(pGenbandContact->m_nParentId, GB_DIR);
        m_vecDetail.push_back(stDetail);
    }

    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_FIRST_NAME);
    stDetail.stAttr.m_strValue = pGenbandContact->m_strFirstName;
    m_vecDetail.push_back(stDetail);

    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_LAST_NAME);
    stDetail.stAttr.m_strValue = pGenbandContact->m_strLastName;
    m_vecDetail.push_back(stDetail);

    AddDetailData(pGenbandContact, TRID_BUSINESS_NUMBER, kszGbBookbusinessNumber);

    AddDetailData(pGenbandContact, TRID_HOME_NUMBER, kszGbBookhomeNumber);

    AddDetailData(pGenbandContact, TRID_MOBILE_NUMBER, kszGbBookmobile);

    AddDetailData(pGenbandContact, TRID_PAGER, kszGbBookpager);

    AddDetailData(pGenbandContact, TRID_FAX, kszGbBookFax);

    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_EMAIL);
    stDetail.stAttr.m_strValue = pGenbandContact->m_strEmail;
    m_vecDetail.push_back(stDetail);

    AddDetailData(pGenbandContact, TRID_CONFERENCE_URL, kszGbconferenceURL);

    AddDetailData(pGenbandContact, TRID_PRIMARYCONTACT, kszGbBookprimaryContact);
}

void CGenbandContactControler::AddDetailData(ContactGBBookUnit* pGenbandContact, const yl::string& strDialName, const yl::string& strNumberName)
{
    if (NULL == pGenbandContact)
    {
        return;
    }

    ST_DETAIL_DATA stDetail;
    stDetail.nType = DETAIL_DATA_TYPE_NUMBER;
    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(strDialName.c_str());
    if (!pGenbandContact->GetNumberByName(strNumberName, stDetail.stAttr.m_strValue))
    {
        stDetail.stAttr.m_strValue = "";
    }
    m_vecDetail.push_back(stDetail);
}

bool CGenbandContactControler::PreciseSearch()
{
    DIRUI_DEBUG("CGenbandContactControler::PreciseSearch, Status:%d, Mode:%d", m_nPreciseSearchState, m_nSearchMode);
    if (!IsPreciseSearchCondition())
    {
        return false;
    }

    if (m_nSearchMode < GAB_SEARCH_NAME || m_nSearchMode > GAB_SEARCH_NUMBER)
    {
        return false;
    }

    IGenbandContactUIHelper* pGenbandUIHelper = g_DirUIAdapter.GetGenbandContactUIhelper(m_pUIHelper);
    if (NULL == pGenbandUIHelper)
    {
        DIRUI_ERR("CGenbandContactControler::PreciseSearch UIHelp:%p", pGenbandUIHelper);
        return false;
    }

    yl::string strSearchKey;

    if (!pGenbandUIHelper->GetGABPreciseSearchData(strSearchKey))
    {
        DIRUI_ERR("GetGABPreciseSearchData fail");
        return false;
    }

    DIRECTORY_RET_CODE retCode = GAB_ReqSearchGABByType(strSearchKey, GBBOOK_OPERATION_TYPE(m_nSearchMode));

    if (GET_LIST_DOWNLOADING == retCode || DIR_RET_SUCC == retCode)
    {
        m_nPreciseSearchState = (GET_LIST_DOWNLOADING == retCode) ? PRECISE_SEARCH_STATE_WAITING : PRECISE_SEARCH_STATE_RESULT;
        g_DirUIAdapter.ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT, DIR_ACTION_GENBAND_DOWNLOAD);
    }
    else
    {
        CCommonDirCtrl::ShowErrorMsg(retCode);
    }

    return true;
}

void CGenbandContactControler::OnPreciseSearchResult(msgObject& refMessage)
{
    DIRUI_DEBUG("CGenbandContactControler::OnPreciseSearchResult(GroupId:%d wp:%d lp:%d)",
                kszGabBookGroupId, refMessage.wParam, refMessage.lParam);

    if (refMessage.wParam < GAB_SEARCH_NAME || refMessage.lParam > GAB_SEARCH_NUMBER)
    {
        return;
    }

    IDirUIHelperPtr pDirUIHelper = m_pUIHelper;
    IDirUIHelper * pUIHelper = pDirUIHelper.Get();

    if (NULL == pUIHelper)
    {
        return;
    }

    int nRefreshParam = DIR_UI_REFRESH_PARAM_DATA_CHANGE;
    if (PRECISE_SEARCH_STATE_WAITING == m_nPreciseSearchState)
    {
        m_nPreciseSearchState = PRECISE_SEARCH_STATE_RESULT;
        nRefreshParam = DIR_UI_REFRESH_PARAM_ALL;
    }
    else if (DIR_UI_STATE_PRECISE_SEARCH == GetState())
    {
        g_DirUIAdapter.ExitCurrentUI(DIR_UI_STATE_PRECISE_SEARCH);
    }

    g_DirUIAdapter.EndWaiting();

    if (GENBAND_DOWNLOAD_RESULT_SUCCESS != refMessage.lParam)
    {
        ShowErrorResult(kszGabBookGroupId, refMessage.lParam);
    }

    pUIHelper->RefreshUI(DIR_UI_REFRESH_TYPE_LIST_VIEW | DIR_UI_REFRESH_TYPE_TITLE, nRefreshParam);
}

void CGenbandContactControler::ShowErrorResult(int nGroupId, int nResult)
{
    if (GENBAND_DOWNLOAD_RESULT_SVR_URL_ERROR == nResult)
    {
        g_DirUIAdapter.ShowMsgBox(TRID_SERVER_URL_IS_INVALID);
    }
    else
    {
        g_DirUIAdapter.ShowMsgBox(TRID_FAIL_TO_CONNECT_TO_SERVER);
    }
}

void CGenbandContactControler::RefreshAfterPabContactChange(msgObject & refMessage)
{
    IDirUIHelperPtr pDirUIHelper = m_pUIHelper;
    IDirUIHelper * pUIHelper = pDirUIHelper.Get();

    if (NULL == pUIHelper)
    {
        return;
    }

    bool bSuc = (1 == refMessage.lParam);

    g_DirUIAdapter.EndWaiting();
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
                if (DIR_UI_STATE_ADDITION == nUIState || DIR_UI_STATE_DETAIL == nUIState)
                {
                    g_DirUIAdapter.ExitCurrentUI(nUIState);
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
                bool bBack = (DIR_UI_STATE_DETAIL == nUIState);
                if (bBack)
                {
                    g_DirUIAdapter.ExitCurrentUI(nUIState);
                }

                pUIHelper->RefreshUIAfterDeleted(bBack);

                strMsg = TRID_DEL_GENBAND_CONTACT_COMPLETED;
            }
            else
            {
                strMsg = TRID_DELETE_FIALED;
            }
        }
        break;
    default:
        break;
    }

    DIRUI_DEBUG("CGenbandContactControler::RefreshAfterPabContactChange Suc [%d], Msg [%s]", bSuc,
                strMsg.c_str());

    if (bSuc)
    {
        pUIHelper->RefreshUI(DIR_UI_REFRESH_TYPE_ALL, DIR_UI_REFRESH_PARAM_ALL);
    }

    if (!strMsg.empty())
    {
        g_DirUIAdapter.ShowMsgBox(strMsg.c_str());
    }
}

void CGenbandContactControler::RefreshAfterPabContactChange()
{
    if (IsSearchState())
    {
        m_nPreciseSearchState = PRECISE_SEARCH_STATE_NONE;
        if (!PreSearchContact())
        {
            return;
        }
    }

    IDirUIHelperPtr pDirUIHelper = m_pUIHelper;
    IDirUIHelper * pUIHelper = pDirUIHelper.Get();

    if (NULL != pUIHelper)
    {
        pUIHelper->RefreshUI(DIR_UI_REFRESH_TYPE_ALL, DIR_UI_REFRESH_PARAM_ALL);
    }
}

bool CGenbandContactControler::AddPabContact()
{
    IGenbandContactUIHelper * pGenbandUIHelper = g_DirUIAdapter.GetGenbandContactUIhelper(m_pUIHelper);

    if (NULL == pGenbandUIHelper)
    {
        return false;
    }

    ContactGBBookUnit itemData;

    if (!pGenbandUIHelper->GetGenbandContactData(itemData))
    {
        return false;
    }

    if (!CheckPersonalValid(itemData))
    {
        return false;
    }

    DIRUI_DEBUG("CGenbandContactControler::AddPabContact(%s %s)", itemData.m_strDisplayName.c_str(),
                itemData.GetNumberByIndex(0).c_str());

    int nRetCode = PAB_ReqAddContact(itemData);

    return CheckChangePabContact(nRetCode, DIR_ACTION_GENBAND_ADD_PAB);
}

bool CGenbandContactControler::DelPabContact(int nContactId)
{
    int nRetCode = PAB_ReqRemoveContact(nContactId);

    bool bSuccess = CheckDeletePersonal(nRetCode);

    return bSuccess;
}

void CGenbandContactControler::OnDelPabContact(int nContactId)
{
    yl::string strExtTitle = Dir_GetDisplayNameById(nContactId, GB_DIR);
    g_DirUIAdapter.PopSureMsgBox(TRID_DEL_GENBAND_CONTACT_TITLE, TRID_DEL_GENBAND_CONTACT,
                                 DIR_ACTION_GENBAND_DEL_PAB, nContactId);//, strExtTitle.c_str());
}

bool CGenbandContactControler::SavePabContact()
{
    IGenbandContactUIHelper * pGenbandUIHelper = g_DirUIAdapter.GetGenbandContactUIhelper(m_pUIHelper);
    if (NULL == pGenbandUIHelper)
    {
        return false;
    }

    ContactGBBookUnit itemData;

    if (!pGenbandUIHelper->GetGenbandContactData(itemData))
    {
        return false;
    }

    if (!CheckPersonalValid(itemData))
    {
        return false;
    }

    int nRetCode = PAB_ReqModifyContact(m_nContactId, itemData);

    return CheckChangePabContact(nRetCode, DIR_ACTION_GENBAND_EDIT_PAB);
}

bool CGenbandContactControler::CheckPersonalValid(ContactGBBookUnit & itemData)
{
    IGenbandContactUIHelper * pGenbandUIHelper = g_DirUIAdapter.GetGenbandContactUIhelper(m_pUIHelper);
    if (NULL == pGenbandUIHelper)
    {
        return false;
    }

    yl::string strMsg = "";
    if (itemData.m_strDisplayName.empty())
    {
        strMsg = TRID_GENBAND_NAME_CANNOT_BE_EMPTY;
    }
    else
    {
        int nContactID = Dir_IsContactNameExist(itemData.m_strDisplayName, GB_DIR);
        if (nContactID != -1 && m_nContactId != nContactID)
        {
            strMsg = TRID_GENBAND_CONTACT_NAME_EXIST;
        }
    }

    if (!strMsg.empty())
    {
        g_DirUIAdapter.PopSureMsgBox(TRID_NOTE, strMsg.c_str(), DIR_ACTION_NONE, -1);
        pGenbandUIHelper->SetInputFocus(DIR_ATTR_NAME);
        return false;
    }

    yl::string strNumber;

    itemData.GetNumberByName(kszGbBookprimaryContact, strNumber);

    if (strNumber.size() <= 0)
    {
        strMsg = TRID_GENBAND_NUMBER_CANNOT_BE_EMPTY;
    }

    if (!strMsg.empty())
    {
        g_DirUIAdapter.PopSureMsgBox(TRID_NOTE, strMsg.c_str(), DIR_ACTION_NONE, -1);
        pGenbandUIHelper->SetInputFocus(kszGbBookprimaryContact);
        return false;
    }

    return true;
}

bool CGenbandContactControler::CheckChangePabContact(int nRetCode, int nAction)
{
    if (DIR_RET_SUCC == nRetCode || nRetCode > 0)
    {
        g_DirUIAdapter.ShowWaiting(TRID_NOTE, TRID_SAVE_GENBAND_CONTACT, nAction);
        return true;
    }
    else if (DIR_RET_CONTACT_EXIST == nRetCode)
    {
        g_DirUIAdapter.ShowMsgBox(TRID_CONTACT_EXIST);
    }
    else if (DIR_RET_FAIL == nRetCode)
    {
        if (nAction == DIR_ACTION_GENBAND_ADD_PAB
                || nAction == DIR_ACTION_GENBAND_EDIT_PAB
                || nAction == DIR_ACTION_GENBAND_EDIT_GROUP
                || nAction == DIR_ACTION_GENBAND_ADD_GROUP)
        {
            g_DirUIAdapter.PopSureMsgBox(TRID_NOTE, TRID_SAVE_FAILED, DIR_ACTION_NONE, -1);
        }
        else
        {
            g_DirUIAdapter.PopSureMsgBox(TRID_NOTE, TRID_SAVE_FAILED, nAction, -1);
        }
    }
    else
    {
        CCommonDirCtrl::ShowErrorMsg(nRetCode);
    }

    return false;
}

bool CGenbandContactControler::CheckDeletePersonal(int nRetCode)
{
    if (DIR_RET_SUCC == nRetCode)
    {
        g_DirUIAdapter.ShowWaiting(TRID_NOTE, TRID_DEL_GENBAND_CONTACT_WAITING,
                                   DIR_ACTION_GENBAND_DEL_PAB);
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

bool CGenbandContactControler::OnCopyToPab(bool bOverwrite)
{
    IGenbandContactUIHelper* pGenbandUIHelper = g_DirUIAdapter.GetGenbandContactUIhelper(m_pUIHelper);
    if (NULL == pGenbandUIHelper)
    {
        return false;
    }

    ContactGBBookUnit itemData;

    if (!pGenbandUIHelper->GetGenbandContactData(itemData))
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
        DIRUI_DEBUG("AddPersonal(Name:%s Number:%s)",
                    itemData.m_strDisplayName.c_str(),
                    itemData.GetNumberByIndex(0).c_str());

        nRetCode = PAB_ReqAddContact(itemData);
    }
    else
    {
        if (!bOverwrite)
        {
            return g_DirUIAdapter.PopSureMsgBox(TRID_WARNING, TRID_IF_OVERWRITE_THE_ORIGINAL_CONTACT,
                                                DIR_ACTION_GENBAND_ADD_TO_PAB, nContactId);
        }

        DIRUI_DEBUG("PAB_ReqModifyContact(Id:%d Name:%s Number:%s)",
                    nContactId, itemData.m_strDisplayName.c_str(),
                    itemData.GetNumberByIndex(0).c_str());

        nRetCode = PAB_ReqModifyContact(nContactId, itemData);
    }

    return CheckChangePabContact(nRetCode, DIR_ACTION_GENBAND_COPY_PERSONAL_FINISH);
}

void CGenbandContactControler::OnEnterManagerGroup(int nAction)
{
    int nUIState = GetState();
    if (DIR_UI_STATE_SEARCH == nUIState)
    {
        g_DirUIAdapter.ExitCurrentUI(nUIState);
        nUIState = GetState();
    }

    if (DIR_UI_STATE_BROWSER == nUIState)
    {
        g_DirUIAdapter.EnterUI(m_nType, DIR_UI_STATE_BROWSER, nAction, kszPabBookGroupId);
    }
    else
    {
        IGenbandContactUIHelper* pGenbandUIHelper = g_DirUIAdapter.GetGenbandContactUIhelper(m_pUIHelper);
        if (NULL != pGenbandUIHelper)
        {
            pGenbandUIHelper->OnEnterManageGroup(m_nType, kszPabBookGroupId);
        }
    }
}

bool CGenbandContactControler::OnSaveGroup(int nGroupId)
{
    IGenbandContactUIHelper* pGenbandUIHelper = g_DirUIAdapter.GetGenbandContactUIhelper(m_pUIHelper);
    if (NULL == pGenbandUIHelper)
    {
        return false;
    }

    yl::string strGroupName;

    if (!pGenbandUIHelper->GetGenbandGroupData(strGroupName))
    {
        DIRUI_ERR("CDirUIHelper::GetGenbandGroupData() fail in OnSaveGroup!");
        return false;
    }

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
        g_DirUIAdapter.ShowMsgBox(strMsg.c_str());
        return false;
    }

    DIRUI_DEBUG("OnSaveGroup(%s)", strGroupName.c_str());

    int nRetCode = PAB_ReqModifyGroup(nGroupId, strGroupName);

    return CheckChangePabContact(nRetCode, DIR_ACTION_GENBAND_EDIT_GROUP);
}

bool CGenbandContactControler::OnDelGroup(int nGroupId)
{
    yl::string strExtTitle = Dir_GetDisplayNameById(nGroupId, GB_DIR);
    return g_DirUIAdapter.PopSureMsgBox(TRID_DEL_GENBAND_GROUP_TITLE, TRID_DEL_GENBAND_GROUP,
                                        DIR_ACTION_GENBAND_DEL_GROUP, nGroupId);
}

bool CGenbandContactControler::DeleteGroup(int nGroupId)
{
    int nRetCode = PAB_ReqRemoveGroup(nGroupId);

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

bool CGenbandContactControler::OnAddGroup()
{
    IGenbandContactUIHelper* pGenbandUIHelper = g_DirUIAdapter.GetGenbandContactUIhelper(m_pUIHelper);
    if (NULL == pGenbandUIHelper)
    {
        return false;
    }

    yl::string strGroupName;

    if (!pGenbandUIHelper->GetGenbandGroupData(strGroupName))
    {
        return false;
    }

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
        g_DirUIAdapter.ShowMsgBox(strMsg.c_str());
        return false;
    }

    int nRetCode = PAB_ReqAddGroup(strGroupName);

    return CheckChangePabContact(nRetCode, DIR_ACTION_GENBAND_ADD_GROUP);
}

void CGenbandContactControler::ClearSearchData()
{
    if (m_nSearchMode < GAB_SEARCH_NAME || m_nSearchMode > GAB_SEARCH_NUMBER)
    {
        return;
    }

    Dir_ClearContactData(kszGabBookGroupId, GB_DIR);
    if (1 != configParser_GetConfigInt(kszGbRetainSearchFilter))
    {
        GAB_ClearSearchInfo();
    }
}

#endif
