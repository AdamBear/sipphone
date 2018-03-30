#include "commondirctrl.h"
#include "translateiddefine.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "diruiadapter.h"
#if DIR_UI_SUPPORT_LOAD_DATA_LIST
#include "icommonuihelper.h"
#endif


CCommonDirCtrl::CCommonDirCtrl(int nDirType, DIR_TYPE eLogicType)
    : CDirectoryControler(nDirType)
    , m_eLogicType(eLogicType)
    , m_nContactId(knInvalidRId)
{
}

CCommonDirCtrl::~CCommonDirCtrl()
{
}

CCommonDirCtrl * CCommonDirCtrl::GetCommonControler(CDirectoryControlerPtr & pControler)
{
    if (pControler.IsEmpty())
    {
        return NULL;
    }

    int nType = pControler->GetType();

    if (DIRECTORY_TYPE_LDAP == nType || DIRECTORY_TYPE_BTUC == nType)
    {
        return NULL;
    }

    return (CCommonDirCtrl *)pControler.Get();
}

DIR_TYPE CCommonDirCtrl::GetLogicType(int nDirType)
{
    switch (nDirType)
    {
    case DIRECTORY_TYPE_LOCAL:
        {
            return LOCAL_DIR;
        }
        break;
    case DIRECTORY_TYPE_REMOTE:
        {
            return REMOTE_DIR;
        }
        break;
    case DIRECTORY_TYPE_LDAP:
        {
            return BASE_DIR;
        }
        break;
    case DIRECTORY_TYPE_BROADSOFT:
        {
            return BSFT_DIR;
        }
        break;
    case DIRECTORY_TYPE_REMOTE_SEARCH:
        {
            return REMOTE_DIR;
        }
        break;
    case DIRECTORY_TYPE_BLUETOOTH:
        {
            return BLUETOOTH_DIR;
        }
        break;
    case DIRECTORY_TYPE_BTUC:
        {
            return UC_DIR;
        }
        break;
    case DIRECTORY_TYPE_METASWICTH:
        {
            return MTSW_DIR;
        }
        break;
    case CALL_LOG_TYPE_LOCAL:
        {
            return LOCAL_LOG;
        }
        break;
    case CALL_LOG_TYPE_BROADSOFT:
        {
            return BSFT_LOG;
        }
        break;
    case CALL_LOG_TYPE_MTSW:
        {
            return MTSW_LOG;
        }
        break;
    default:
        break;
    }

    return BASE_DIR;
}

bool CCommonDirCtrl::IsDirEnable(int nDirType)
{
    if (CALL_LOG_TYPE_BROADSOFT == nDirType)
    {
        return BsftLog_IsEnable();
    }
    else if (DIRECTORY_TYPE_BROADSOFT == nDirType)
    {
        return BsftDir_IsEnable();
    }
#if IF_FEATURE_METASWITCH_DIRECTORY
    else if (DIRECTORY_TYPE_METASWICTH == nDirType)
    {
        return MtswDir_IsEnable();
    }
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
    else if (CALL_LOG_TYPE_MTSW == nDirType)
    {
        return MTSWLog_IsEnable();
    }
#endif
#if IF_BUG_PLCMDIR
    else if (DIRECTORY_TYPE_PLCM == nDirType)
    {
        return (0 != configParser_GetConfigInt(kszPlcmPhoneEnable));
    }
#endif
#if IF_FEATURE_GBBOOK
    else if (DIRECTORY_TYPE_GENBAND == nDirType)
    {
        return true;
    }
#endif

    DIR_TYPE eLogicType = GetLogicType(nDirType);

    if (BASE_DIR == eLogicType)
    {
        return false;
    }

    return Dir_IsDirConfiged(eLogicType);
}

bool CCommonDirCtrl::GetDisplayName(DIR_TYPE eLogicType, int nId, yl::string & strName)
{
    SmartPtr pDetail = Dir_GetDetailById(nId, eLogicType);

    if (pDetail.IsEmpty())
    {
        return false;
    }

    strName = pDetail->GetDisplayName();

    return true;
}

void CCommonDirCtrl::ResetContactId()
{
    m_nContactId = knInvalidRId;
}

int CCommonDirCtrl::GetContactId() const
{
    return m_nContactId;
}

int CCommonDirCtrl::GetParGroupId(int nGroupId)
{
    int nParentId = Dir_GetParentIdById(nGroupId, m_eLogicType);

    return nParentId;
}

bool CCommonDirCtrl::PreSearchContact()
{
    const yl::string & strSearchKey = GetSearchKey();

    if (strSearchKey.empty())
    {
        return true;
    }

    int nGroupId = GetSearchGroupId();

    if (kszRootId == nGroupId)
    {
        nGroupId = kszAllContactId;
    }

    DIRUI_INFO("CCommonDirCtrl::PreSearchContact() Key:%s GroupId:%d LogicType:%d",
               strSearchKey.c_str(), nGroupId, (int)m_eLogicType);

    Dir_LocalSearch(strSearchKey, nGroupId, m_eLogicType);
    return false;
}

#if DIR_UI_FEATURE_LOAD_DATA_LIST
// nDataCount为0时，表示全加载
bool CCommonDirCtrl::LoadMenuData(int nDataIndex, int nDataCount)
{
    ICommonUIHelper * pCommonUIHelper = GetCommonUIHelper();

    if (NULL == pCommonUIHelper)
    {
        return false;
    }

    ContactListData lsData;

    if (!Dir_GetConfigMenuIdList(lsData.m_listId, m_eLogicType))
    {
        DIRUI_ERR("Dir_GetConfigMenuIdList fail LogicType=%d", (int)m_eLogicType);
        return false;
    }

    int nSize = lsData.m_listId.size();

    if (0 != nDataCount)
    {
        VEC_CONTACT_ID vecId;

        for (int i = 0; i < nSize; ++i)
        {
            if (i < nDataIndex)
            {
                continue;
            }

            vecId.push_back(lsData.m_listId[i]);
            ++lsData.m_nCountToGet;
        }

        lsData.m_nIndexFrom = nDataIndex;
        lsData.m_nTotalCount = nSize;
        lsData.m_listId = vecId;
    }
    else
    {
        lsData.m_nCountToGet = nSize;
        lsData.m_nTotalCount = nSize;
        lsData.m_nIndexFrom = 0;
    }

    pCommonUIHelper->FillData(lsData);
    return true;
}

bool CCommonDirCtrl::LoadListData(int nDataIndex, int nDataCount)
{
    if (IsSearchState())
    {
        return LoadSearchListData(nDataIndex, nDataCount);
    }
    else
    {
        return LoadGroupListData(nDataIndex, nDataCount);
    }
}

bool CCommonDirCtrl::LoadSearchListData(int nDataIndex, int nDataCount)
{
    DIRUI_INFO("CCommonDirCtrl::LoadSearchListData(Index:%d Count:%d)", nDataIndex, nDataCount);

    ICommonUIHelper * pCommonUIHelper = GetCommonUIHelper();
    IDirUIHelper * pUIHelper = m_pUIHelper.Get();

    if (NULL == pCommonUIHelper || NULL == pUIHelper)
    {
        DIRUI_INFO("CCommonDirCtrl::GetCommonUIHelper() fail in LoadSearchListData.");
        return false;
    }

    ContactListData lsData;

    lsData.m_nCountToGet = nDataCount;
    lsData.m_nIndexFrom = nDataIndex;

    const yl::string & strSearchKey = GetSearchKey();

    DIRUI_INFO("Dir_GetSearchResultList(Type:%d SearchKey:%s Count:%d Index:%d)",
               m_eLogicType, strSearchKey.c_str(), nDataCount, nDataIndex);

    if (!Dir_GetSearchResultList(strSearchKey, &lsData, m_eLogicType))
    {
        DIRUI_ERR("Dir_GetSearchResultList(Key:%s, Type:%d) fail.",
                  strSearchKey.c_str(), (int)m_eLogicType);
        return false;
    }

    if (pUIHelper->AdjustListData(lsData.m_nTotalCount, nDataIndex, nDataCount))
    {
        DIRUI_INFO("AdjustListData when search, DataIndex:%d DataCount:%d", nDataIndex, nDataCount);

        lsData.m_listId.clear();
        lsData.m_nCountToGet = nDataCount;
        lsData.m_nIndexFrom = nDataIndex;

        if (!Dir_GetSearchResultList(strSearchKey, &lsData, m_eLogicType))
        {
            DIRUI_ERR("Dir_GetSearchResultList(Key:%s, Type:%d) fail.",
                      strSearchKey.c_str(), (int)m_eLogicType);
            return false;
        }
    }

    lsData.m_nCountToGet = lsData.m_listId.size();

#ifdef DIR_SUPPORT_SWITCH_DETAIL
    SetTotalCount(lsData.m_nTotalCount);
#endif

    return pCommonUIHelper->FillData(lsData);
}

bool CCommonDirCtrl::LoadGroupListData(int nDataIndex, int nDataCount)
{
    DIRUI_INFO("CCommonDirCtrl::LoadGroupListData(Index:%d Count:%d)", nDataIndex, nDataCount);

    ICommonUIHelper * pCommonUIHelper = GetCommonUIHelper();
    IDirUIHelper * pUIHelper = m_pUIHelper.Get();

    if (NULL == pUIHelper || NULL == pCommonUIHelper)
    {
        DIRUI_INFO("CCommonDirCtrl::GetCommonUIHelper() fail in LoadGroupListData.");
        return false;
    }

    ContactListData lsData;

    lsData.m_nCountToGet = nDataCount;
    lsData.m_nIndexFrom = nDataIndex;

    DIRUI_INFO("Dir_GetContactListByGroupId(Type:%d GroupId:%d Count:%d Index:%d)",
               (int)m_eLogicType, GetGroupId(), nDataCount, nDataIndex);

    if (!Dir_GetContactListByGroupId(&lsData, GetGroupId(), m_eLogicType))
    {
        DIRUI_ERR("Dir_GetContactListByGroupId(GroupId:%d Type:%d) fail",
                  GetGroupId(), (int)m_eLogicType);
        return false;
    }

    if (pUIHelper->AdjustListData(lsData.m_nTotalCount, nDataIndex, nDataCount))
    {
        DIRUI_INFO("AdjustListData when browser, DataIndex:%d DataCount:%d", nDataIndex, nDataCount);

        lsData.m_listId.clear();
        lsData.m_nCountToGet = nDataCount;
        lsData.m_nIndexFrom = nDataIndex;

        if (!Dir_GetContactListByGroupId(&lsData, GetGroupId(), m_eLogicType))
        {
            DIRUI_ERR("Dir_GetContactListByGroupId(GroupId:%d Type:%d) fail",
                      GetGroupId(), (int)m_eLogicType);
            return false;
        }
    }

    lsData.m_nCountToGet = lsData.m_listId.size();

#ifdef DIR_SUPPORT_SWITCH_DETAIL
    SetTotalCount(lsData.m_nTotalCount);
#endif

    return pCommonUIHelper->FillData(lsData);
}
#endif

bool CCommonDirCtrl::OnAction(int nAction, int nDataIndex, int nId)
{
    DIRUI_INFO("CCommonDirCtrl::OnAction(Action:%d DataIndex:%d Id:%d)", nAction, nDataIndex, nId);

    switch (nAction)
    {
    case DIR_ACTION_COMMON_DETAIL:
        {
            EnterDetail(nId, nDataIndex, nAction);

            return true;
        }
        break;
    case DIR_ACTION_COMMON_SELECT:
        {
            g_DirUIAdapter.EnterUI(m_nType, DIR_UI_STATE_BROWSER, nAction, nId);

            return true;
        }
        break;
    case DIR_ACTION_COMMON_SEND:
        {
            DialContact(nId);

            return true;
        }
    default:
        break;
    }

    return false;
}

void CCommonDirCtrl::EnterDetail(int nContactId, int nDataIndex, int nAction)
{
    if (knInvalidRId == nContactId)
    {
        return;
    }

    m_nContactId = nContactId;

    m_nDetailStatus = GetState();
#ifdef DIR_SUPPORT_SWITCH_DETAIL
    SetCurIndex(nDataIndex);
#endif

    PrepareDetailData();

    g_DirUIAdapter.EnterUI(m_nType, DIR_UI_STATE_DETAIL, nAction, nContactId);
}

void CCommonDirCtrl::PrepareDetailData()
{

}

bool CCommonDirCtrl::GetContactShowName(yl::string & strName)
{
    SmartPtr pDetail = GetCurrentDetail();

    if (pDetail.IsEmpty())
    {
        return false;
    }

    strName = pDetail->GetDisplayName();
    return true;
}

#ifdef DIR_SUPPORT_SWITCH_DETAIL
void CCommonDirCtrl::GetDetailInfo(int & nId, int & nIndex, int & nCount) const
{
    nId = m_nContactId;
    nIndex = GetCurIndex();
    nCount = GetTotalCount();
}

bool CCommonDirCtrl::GetContactImage(int nId, yl::string & strImage) const
{
    strImage = "";
    return true;
}

bool CCommonDirCtrl::SwitchContact(int & nId, int & nIndex)
{
    int nGroupId = GetResultGroupId();

    nId = Dir_GetContactIdByIndex(nIndex, nGroupId, m_eLogicType);

    DIRUI_INFO("Dir_GetContactIdByIndex(Index:%d GroupId:%d Type:%d) return %d",
               nIndex, nGroupId, (int)m_eLogicType, nId);

    if (knInvalidRId == nId)
    {
        return false;
    }

    m_nContactId = nId;
    s_nCurIndex = nIndex;

    return true;
}
#endif

bool CCommonDirCtrl::IsGroupData(SmartPtr & pDetail)
{
    if (pDetail.IsEmpty() || DIR_TYPE_GROUP != pDetail->GetType())
    {
        return false;
    }

    return true;
}

bool CCommonDirCtrl::GetBrowseGroupList(VEC_CONTACT_ID & vecId)
{
    return Dir_GetConfigMenuIdList(vecId, m_eLogicType);
}

bool CCommonDirCtrl::GetMenuGroupId(VEC_CONTACT_ID & vecId)
{
    return Dir_GetConfigMenuIdList(vecId, m_eLogicType);
}

SmartPtr CCommonDirCtrl::GetDetail(int nId)
{
    if (knInvalidRId == nId)
    {
        return SmartPtr(NULL);
    }

    return Dir_GetDetailById(nId, m_eLogicType);
}

SmartPtr CCommonDirCtrl::GetDetailByIndex(int & nId, int nIndex)
{
    if (IsBrowseState() && (kszRootId == m_nGroupId || knInvalidRId == m_nGroupId))
    {
        nId = GetMenuIdByIndex(nIndex);
    }
    else
    {
        int nGroupId = GetResultGroupId();

        nId = Dir_GetContactIdByIndex(nIndex, nGroupId, m_eLogicType);
    }

    if (knInvalidRId == nId)
    {
        return SmartPtr(NULL);
    }

    return Dir_GetDetailById(nId, m_eLogicType);
}

int CCommonDirCtrl::GetMenuIdByIndex(int nIndex)
{
    int nId = knInvalidRId;
    VEC_CONTACT_ID vecMenuId;

    Dir_GetConfigMenuIdList(vecMenuId, m_eLogicType);

    if (nIndex >= 0 && nIndex < vecMenuId.size())
    {
        nId = vecMenuId[nIndex];
    }

    return nId;
}

SmartPtr CCommonDirCtrl::GetCurrentDetail() const
{
    if (knInvalidRId == m_nContactId)
    {
        return SmartPtr(NULL);
    }

    return Dir_GetDetailById(m_nContactId, m_eLogicType);
}

bool CCommonDirCtrl::IsGroupId(int nId)
{
    SmartPtr pDetail = Dir_GetDetailById(nId, m_eLogicType);

    if (pDetail.IsEmpty())
    {
        return false;
    }

    return IsGroupData(pDetail);
}

int CCommonDirCtrl::GetGroupIndexById(int nGroupId)
{
    VEC_CONTACT_ID vecGroupId;

    if (!Dir_GetConfigMenuIdList(vecGroupId, m_eLogicType))
    {
        return -1;
    }

    int nSize = vecGroupId.size();

    for (int i = 0; i < nSize; ++i)
    {
        if (vecGroupId[i] == nGroupId)
        {
            return i;
        }
    }

    return -1;
}

int CCommonDirCtrl::GetGroupIdByIndex(DIR_TYPE eLogicType, int nGroupIndex)
{
    if (nGroupIndex < 0)
    {
        return knInvalidRId;
    }

    VEC_CONTACT_ID vecGroupId;

    if (!Dir_GetConfigMenuIdList(vecGroupId, eLogicType))
    {
        return knInvalidRId;
    }

    if (nGroupIndex >= vecGroupId.size())
    {
        return knInvalidRId;
    }

    return vecGroupId[nGroupIndex];
}

bool CCommonDirCtrl::GetGroupName(int nGroupId, yl::string & strGroupName)
{
    SmartPtr pDetail = GetDetail(nGroupId);

    if (pDetail.IsEmpty() || !IsGroupData(pDetail))
    {
        return false;
    }

    strGroupName = pDetail->GetDisplayName();
    return true;
}

DirGroupRecord * CCommonDirCtrl::GetGroupRecord(SmartPtr & pDetail)
{
    if (pDetail.IsEmpty())
    {
        return NULL;
    }

    int nType = pDetail->GetType();

    if (DIR_TYPE_GROUP != nType && DIR_TYPE_LOCAL_GROUP != nType
            && DIR_TYPE_REMOTE_MENU != nType && DIR_TYPE_BROADSOFT_MENU != nType
            && DIR_TYPE_BSFT_LOG_GROUP != nType && DIR_TYPE_LOCALLOG_GROUP != nType
#if IF_FEATURE_GBBOOK
            && DIR_TYPE_GB_MENU != nType
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
            && DIR_TYPE_METASWITCH_MENU != nType
#endif
       )
    {
        return NULL;
    }

    return (DirGroupRecord *)pDetail.Get();
}

bool CCommonDirCtrl::GetErrMsg(int nError, yl::string & strErrMsg)
{
    switch (nError)
    {
    case DIR_RET_SUCC:
        {
            strErrMsg = TRID_SUCCESSED;
        }
        break;
    case DIR_RET_FAIL:
        {
            strErrMsg = TRID_FAIL;
        }
        break;
    case DIR_RET_GROUP_DUPLICATE:
        {
            strErrMsg = TRID_GROUP_ALREAD_EXIST;
        }
        break;
    case DIR_RET_CONTACT_NOT_BLACK:
        {
            strErrMsg = TRID_CONTACT_IS_NOT_BLACK;
        }
        break;
    case DIR_RET_INVALID:
        {
            strErrMsg = TRID_DATA_INVALID;
        }
        break;
    case DIR_RET_EMPTY:
        {
            strErrMsg = TRID_NUMBER_CANNOT_BE_EMPTY;
        }
        break;
    case DIR_RET_GROUP_UNEXISTED:
        {
            strErrMsg = TRID_GROUP_IS_NOT_EXIST;
        }
        break;
    case DIR_RET_GROUP_SIZE_UNEXPECTED:
        {
            strErrMsg = TRID_CONTACT_FULL;
        }
        break;
    case DIR_RET_GROUP_NAME_EMPTY:
        {
            strErrMsg = TRID_GROUP_NAME_CANNOT_BE_EMPTY;
        }
        break;
    case DIR_RET_CONTACT_EXIST:
        {
            strErrMsg = TRID_CONTACT_EXIST;
        }
        break;
    case DIR_RET_RERROR_NETWORK_ERROR:
        {
            strErrMsg = TRID_NETWORK_UNAVAILABLE;
        }
        break;
    case GET_LIST_DOWNLOADING:
        {
            strErrMsg = TRID_FAIL_TO_CONNECT_TO_SERVER;
        }
        break;
    case GET_LIST_DOWNLOADED_FAIL:
        {
            strErrMsg = TRID_LOAD_FAILED;
        }
        break;
    case GET_LIST_NONE_DOWNLOAD:
        {
            strErrMsg = TRID_URL_IS_EMPTY;
        }
        break;
    case GET_LIST_READ_CONFIG_FAIL:
        {
            strErrMsg = TRID_URL_IS_EMPTY;
        }
        break;
    case GET_LIST_CONFIG_FILE_IS_EMPTY:
        {
            strErrMsg = TRID_URL_IS_EMPTY;
        }
        break;
    case GET_LIST_CONFIG_FILE_INVALID:
        {
            strErrMsg = TRID_LOAD_FAILED;
        }
        break;
    case GET_LIST_NETWORK_NOT_CONNECTED:
        {
            strErrMsg = TRID_LOAD_FAILED;
        }
        break;
    case GET_LIST_USER_CANCELED:
        {
        }
        break;
    case GET_LIST_UPPER_LIMIT_REACHED:
        {
            strErrMsg = TRID_ONLY_4_LEVEL_MENU_SUPPORTED;
        }
        break;
    case GET_LIST_DOWNLOAD_OTHER_ERROR:
        {
            strErrMsg = TRID_LOAD_FAILED;
        }
        break;
    default:
        {
            DIRUI_ERR("GetErrMsg(%d) none match tips", nError);
            return false;
        }
        break;
    }

    return true;
}

void CCommonDirCtrl::ShowErrorMsg(int nError)
{
    yl::string strErrMsg;

    GetErrMsg(nError, strErrMsg);

    if (!strErrMsg.empty())
    {
        g_DirUIAdapter.ShowMsgBox(strErrMsg.c_str());
    }
}

int CCommonDirCtrl::GetContactAmount(int nGroupId)
{
    if (kszRootId == nGroupId)
    {
        nGroupId = kszAllContactId;
    }
    else if (kszSearchGroupId == nGroupId && GetSearchKey().empty())
    {
        return 0;
    }

    return Dir_GetContactCount(nGroupId, m_eLogicType);
}

int CCommonDirCtrl::GetBrowseItemAmount()
{
    if (knInvalidRId == m_nGroupId)
    {
        VEC_CONTACT_ID vecID;

        Dir_GetConfigMenuIdList(vecID, m_eLogicType);

        return vecID.size();
    }

    return GetGroupChildrenAmount(m_nGroupId, DIR_TYPE_GROUP);
}

int CCommonDirCtrl::GetSearchItemAmount()
{
    return GetContactAmount(GetResultGroupId());
}

int CCommonDirCtrl::GetGroupChildrenAmount(int nGroupId, int nGroupType)
{
    SmartPtr pDetail = Dir_GetDetailById(nGroupId, m_eLogicType);

    if (pDetail.IsEmpty())
    {
        return DIR_INVALID_AMOUNT;
    }

    int nDetailType = pDetail->GetType();

    if (nGroupType != nDetailType)
    {
        return DIR_INVALID_AMOUNT;
    }

    DirGroupRecord * pGroupRecord = (DirGroupRecord *)pDetail.Get();

    return pGroupRecord->m_vChildrenId.size();
}

#ifdef DIR_SUPPORT_SETTING
bool CCommonDirCtrl::GetGroupContactId(VEC_CONTACT_ID & vecContactId)
{
    ContactListData listData;

    listData.m_nIndexFrom = 0;
    listData.m_nCountToGet = 0;

    if (!Dir_GetContactListByGroupId(&listData, GetGroupId(), m_eLogicType))
    {
        return false;
    }

    vecContactId = listData.m_listId;

    return true;
}
#endif

#if DIR_SUPPORT_INDICATOR
bool CCommonDirCtrl::IsHadContactList()
{
    if (CALL_LOG_TYPE_LOCAL == m_nType || CALL_LOG_TYPE_BROADSOFT == m_nType)
    {
        return false;
    }

    if (REMOTE_DIR == m_eLogicType && kszRootId == m_nGroupId)
    {
        return false;
    }

    DIR_INHERIT_TYPE eType = Dir_GetFirstChildTypeByGroupId(m_nGroupId, m_eLogicType);

    return (DIR_INHERIT_GROUP != eType);
}

bool CCommonDirCtrl::GetIndicatorList(FastIndexList & listIndicator)
{
    return Dir_GetFastIndexListByGroupId(m_nGroupId, listIndicator, m_eLogicType);
}
#endif

void CCommonDirCtrl::OnSearchResult(msgObject & refMessage)
{
    IDirUIHelperPtr pDirUIHelper = m_pUIHelper;
    IDirUIHelper * pUIHelper = pDirUIHelper.Get();

    if (NULL == pUIHelper || refMessage.wParam != m_eLogicType || 0 == refMessage.lParam)
    {
        return;
    }

    char * pszKey = (char *)refMessage.GetExtraData();

    if (NULL == pszKey)
    {
        DIRUI_ERR("Empty SearchKey in OnSearchResult(Type:%d)", (int)m_eLogicType);
        return;
    }

    if (!IsSearchState() || 0 != strcmp(pszKey, GetSearchKey().c_str()))
    {
        return;
    }

    pUIHelper->RefreshUI(DIR_UI_REFRESH_TYPE_LIST_VIEW | DIR_UI_REFRESH_TYPE_TITLE,
                         DIR_UI_REFRESH_PARAM_ALL);
}

int CCommonDirCtrl::GetSearchGroupId() const
{
    if (knInvalidRId == m_nGroupId)
    {
        return kszAllContactId;
    }

    return m_nGroupId;
}

int CCommonDirCtrl::GetResultGroupId()
{
    if (IsSearchState())
    {
        return kszSearchGroupId;
    }
#ifdef DIR_SUPPORT_SWITCH_DETAIL
    else if (IsDetailState() && DIR_UI_STATE_SEARCH == m_nDetailStatus)
    {
        return kszSearchGroupId;
    }
#endif

    return m_nGroupId;
}
