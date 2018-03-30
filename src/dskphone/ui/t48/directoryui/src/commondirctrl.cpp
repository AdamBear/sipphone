#include "commondirctrl.h"
#include "translateiddefine.h"
#ifndef DIR_SUPPORT_SINGLE_UIHELPER
#include "commonuihelper.h"
#endif
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "directorymgr.h"
#include "uikernel/languagehelper.h"


CCommonDirCtrl::CCommonDirCtrl(int nDirType, DIR_TYPE eLogicType)
    : CDirectoryControler(nDirType)
    , m_eLogicType(eLogicType)
    , m_nContactId(knInvalidRId)
    , m_nSearchType(SEARCH_NONE)
#ifdef DIR_SUPPORT_SWITCH_DETAIL
    , m_nDetailStatus(DIR_UI_STATE_NONE)
#endif
{
}

CCommonDirCtrl::~CCommonDirCtrl()
{
}

CCommonUIHelper * CCommonDirCtrl::GetCommonUIHelper()
{
#ifdef DIR_SUPPORT_SINGLE_UIHELPER
    return m_pUIHelper;
#else
    return CCommonUIHelper::GetUIHelper(m_pUIHelper);
#endif
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
    case DIRECTORY_TYPE_BLUETOOTH:
        {
            return BLUETOOTH_DIR;
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
#if IF_BUG_PLCMDIR
    case DIRECTORY_TYPE_PLCM:
        {
            return PLCM_DIR;
        }
        break;
#endif
#if IF_FEATURE_GBBOOK
    case DIRECTORY_TYPE_GENBAND:
        {
            return GB_DIR;
        }
        break;
#endif
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
#if IF_FEATURE_METASWITCH_CALLLOG
    case CALL_LOG_TYPE_MTSW:
        {
            return MTSW_LOG;
        }
        break;
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    case DIRECTORY_TYPE_METASWICTH:
        {
            return MTSW_DIR;
        }
        break;
#endif
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

    if (DIRECTORY_TYPE_BROADSOFT == nDirType)
    {
        return BsftDir_IsEnable();
    }

#if IF_BUG_PLCMDIR
    if (DIRECTORY_TYPE_PLCM == nDirType)
    {
        return (0 != configParser_GetConfigInt(kszPlcmPhoneEnable));
    }
#endif

#if IF_FEATURE_GBBOOK
    if (DIRECTORY_TYPE_GENBAND == nDirType)
    {
        return (0 != configParser_GetConfigInt(kszGbSoupEnable));
    }
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
    if (CALL_LOG_TYPE_MTSW == nDirType)
    {
        return MTSWLog_IsEnable();
    }

    if (CALL_LOG_TYPE_LOCAL == nDirType)
    {
        return LocalLog_IsEnable();
    }
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
    if (DIRECTORY_TYPE_METASWICTH == nDirType)
    {
        return MtswDir_IsEnable();
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
    if (GetSearchKey().empty())
    {
        return true;
    }

    int nGroupId = GetSearchGroupId();

    if (kszRootId == nGroupId)
    {
        nGroupId = kszAllContactId;
    }


    DIRUI_INFO("CCommonDirCtrl::PreSearchContact() Key:%s GroupId:%d LogicType:%d",
               GetSearchKey().c_str(), nGroupId, (int)m_eLogicType);

    Dir_LocalSearch(GetSearchKey(), nGroupId, m_eLogicType);
    return false;
}

// nDataCount为0时，表示全加载
bool CCommonDirCtrl::LoadMenuData(int nDataIndex, int nDataCount)
{
    CCommonUIHelper * pCommonUIHelper = GetCommonUIHelper();

    if (NULL == pCommonUIHelper)
    {
        return false;
    }

    m_nSearchType = SEARCH_NONE;

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
        m_nSearchType = SEARCH_LOCAL;

        return LoadSearchListData(nDataIndex, nDataCount);
    }
    else
    {
        m_nSearchType = SEARCH_NONE;

        return LoadGroupListData(nDataIndex, nDataCount);
    }
}

bool CCommonDirCtrl::LoadSearchListData(int nDataIndex, int nDataCount)
{
    DIRUI_INFO("CCommonDirCtrl::LoadSearchListData(Index:%d Count:%d)", nDataIndex, nDataCount);

    CCommonUIHelper * pCommonUIHelper = GetCommonUIHelper();

    if (NULL == pCommonUIHelper)
    {
        DIRUI_INFO("CCommonDirCtrl::GetCommonUIHelper() fail in LoadSearchListData.");
        return false;
    }

    ContactListData lsData;

    lsData.m_nCountToGet = nDataCount;
    lsData.m_nIndexFrom = nDataIndex;
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    if (pControler.IsEmpty())
    {
        return false;
    }
    if ((pControler->GetType() != DIRECTORY_TYPE_BROADSOFT) || configParser_GetConfigInt(kszBsftDirUpdateEnable) == 1)
    {
        yl::string strSearchKey = GetSearchKey();


        DIRUI_INFO("Dir_GetSearchResultList(Type:%d SearchKey:%s Count:%d Index:%d)",
                   m_eLogicType, strSearchKey.c_str(), nDataCount, nDataIndex);

        if (!Dir_GetSearchResultList(strSearchKey, &lsData, m_eLogicType))
        {
            DIRUI_ERR("Dir_GetSearchResultList(Key:%s, Type:%d) fail.",
                      strSearchKey.c_str(), (int)m_eLogicType);
            return false;
        }

        if (pCommonUIHelper->AdjustListData(lsData.m_nTotalCount, nDataIndex, nDataCount))
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
    }
    else
    {
        m_nSearchType = SEARCH_SERVER;

        DIRUI_INFO("Dir_GetContactListByGroupId(Type:%d GroupId:%d Count:%d Index:%d)",
                   (int)m_eLogicType, kszServerSearchGroupId, nDataCount, nDataIndex);

        if (!Dir_GetContactListByGroupId(&lsData, kszServerSearchGroupId, m_eLogicType))
        {
            DIRUI_ERR("Dir_GetContactListByGroupId(GroupId:%d Type:%d) fail",
                      kszServerSearchGroupId, (int)m_eLogicType);
            return false;
        }

        if (pCommonUIHelper->AdjustListData(lsData.m_nTotalCount, nDataIndex, nDataCount))
        {
            DIRUI_INFO("AdjustListData when browser, DataIndex:%d DataCount:%d", nDataIndex, nDataCount);

            lsData.m_listId.clear();
            lsData.m_nCountToGet = nDataCount;
            lsData.m_nIndexFrom = nDataIndex;

            if (!Dir_GetContactListByGroupId(&lsData, kszServerSearchGroupId, m_eLogicType))
            {
                DIRUI_ERR("Dir_GetContactListByGroupId(GroupId:%d Type:%d) fail",
                          kszServerSearchGroupId, (int)m_eLogicType);
                return false;
            }
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

    CCommonUIHelper * pCommonUIHelper = GetCommonUIHelper();

    if (NULL == pCommonUIHelper)
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

    if (pCommonUIHelper->AdjustListData(lsData.m_nTotalCount, nDataIndex, nDataCount))
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

bool CCommonDirCtrl::LoadDetailData(int nDataIndex, int nDataCount)
{
    CCommonUIHelper * pCommonUIHelper = GetCommonUIHelper();

    if (NULL == pCommonUIHelper)
    {
        return false;
    }

    SmartPtr pDetail = Dir_GetDetailById(m_nContactId, m_eLogicType);

    return pCommonUIHelper->FillDetail(m_nContactId, pDetail, nDataIndex, nDataCount);
}

bool CCommonDirCtrl::OnAction(int nAction, int nDataIndex, int nId)
{
    DIRUI_INFO("CCommonDirCtrl::OnAction(Action:%d DataIndex:%d Id:%d)", nAction, nDataIndex, nId);

    switch (nAction)
    {
    case DIR_ACTION_COMMON_DETAIL:
        {
            m_nContactId = nId;

#ifdef DIR_SUPPORT_SWITCH_DETAIL
            SetCurIndex(nDataIndex);
            m_nDetailStatus = GetState();
#endif

            CDirUIHelper::EnterUI(m_nType, DIR_UI_STATE_DETAIL, nAction, nId);
            return true;
        }
        break;
    case DIR_ACTION_COMMON_SELECT:
        {
            if (nId == kszViewMoreId)
            {
                //加载更多
                CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
                if (!pControler.IsEmpty())
                {
                    pControler->LoadMore();
                }
                return true;
            }
            if (IsGroupId(nId))
            {
                CDirUIHelper::EnterUI(m_nType, DIR_UI_STATE_BROWSER, nAction, nId);
            }
            else
            {
                DialContact(nId);
            }
            return true;
        }
        break;
    default:
        break;
    }

    return false;
}

bool CCommonDirCtrl::GetContactName(yl::string & strName)
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
    int nGroupId = GetLogicListGroupId();

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

bool CCommonDirCtrl::GetMenuGroupId(VEC_CONTACT_ID & vecId)
{
    return Dir_GetConfigMenuIdList(vecId, m_eLogicType);
}

int CCommonDirCtrl::GetAdjoinGroupId(bool bPrevious, int & nIndex)
{
    if (DIRECTORY_TYPE_LOCAL == m_nType && kszBlackListGroupId == m_nGroupId)
    {
        nIndex = 0;
        return knInvalidRId;
    }

    VEC_CONTACT_ID vecId;

    if (!Dir_GetConfigMenuIdList(vecId, m_eLogicType))
    {
        nIndex = 0;
        return knInvalidRId;
    }

    int nSize = vecId.size();

    if (nSize <= 0)
    {
        return knInvalidRId;
    }

    int i = 0;

    for (; i < nSize; ++i)
    {
        if (vecId[i] == m_nGroupId)
        {
            break;
        }
    }

    if (i >= nSize)
    {
        return vecId[0];
    }

    if (bPrevious)
    {
        --i;
    }
    else
    {
        ++i;
    }

    if (i < 0)
    {
        i = nSize - 1;
    }
    else if (i >= nSize)
    {
        i = 0;
    }

    nIndex = i;

    return vecId[i];
}

SmartPtr CCommonDirCtrl::GetDetail(int nId)
{
    if (nId == kszViewMoreId)
    {
        SmartPtr pDetail = Dir_GetDetailById(1, m_eLogicType);
        pDetail->m_strDisplayName = LANG_TRANSLATE(TRID_DIRECTORY_MORE_ITEM).toUtf8().data();
        return pDetail;
    }
    return Dir_GetDetailById(nId, m_eLogicType);
}

SmartPtr CCommonDirCtrl::GetCurrentDetail() const
{
    return Dir_GetDetailById(m_nContactId, m_eLogicType);
}

bool CCommonDirCtrl::IsGroupId(int nId)
{
    return IsGroupId(m_eLogicType, nId);
}

bool CCommonDirCtrl::IsGroupId(DIR_TYPE eLogicType, int nId)
{
    SmartPtr pDetail = Dir_GetDetailById(nId, eLogicType);

    if (pDetail.IsEmpty())
    {
        return false;
    }

    int nType = pDetail->GetType();

    if (DIR_TYPE_GROUP == nType || DIR_TYPE_LOCAL_GROUP == nType || DIR_TYPE_BROADSOFT_MENU == nType
            || DIR_TYPE_REMOTE_MENU == nType || DIR_TYPE_REMOTE_HOTKEY == nType
            || DIR_TYPE_BSFT_LOG_GROUP == nType || DIR_TYPE_LOCALLOG_GROUP == nType
#if IF_FEATURE_METASWITCH_DIRECTORY
            || DIR_TYPE_METASWITCH_MENU == nType
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
            || DIR_TYPE_MTSW_LOG_GROUP == nType
#endif
       )
    {
        return true;
    }

    return false;
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

int CCommonDirCtrl::GetGroupIdByIndex(int nGroupIndex)
{
    return GetGroupIdByIndex(m_eLogicType, nGroupIndex);
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

    if (pDetail.IsEmpty())
    {
        return false;
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
#if IF_FEATURE_METASWITCH_CALLLOG
            && DIR_TYPE_MTSW_LOG_GROUP != nType
#endif
       )
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
#if IF_FEATURE_METASWITCH_DIRECTORY
            && DIR_TYPE_METASWITCH_MENU != nType
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
            && DIR_TYPE_MTSW_LOG_GROUP != nType
#endif
       )
    {
        return NULL;
    }

    return (DirGroupRecord *)pDetail.Get();
}

int CCommonDirCtrl::GetGroupIdByName(const yl::string & strGroupName)
{
    VEC_CONTACT_ID vecGroupId;

    if (!Dir_GetConfigMenuIdList(vecGroupId, m_eLogicType))
    {
        return knInvalidRId;
    }

    int nSize = vecGroupId.size();

    for (int i = 0; i < nSize; ++i)
    {
        int nGroupId = vecGroupId[i];
        yl::string strName;

        if (!GetGroupName(nGroupId, strName))
        {
            continue;
        }

        if (strGroupName == strName)
        {
            return nGroupId;
        }
    }

    return knInvalidRId;
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

void CCommonDirCtrl::OnSearchResult(msgObject & refMessage)
{
    CCommonUIHelper * pUIHelper = GetCommonUIHelper();

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

    CDirUIHelper::RefreshUI(true, true);
}

int CCommonDirCtrl::GetSearchGroupId() const
{
    if (knInvalidRId == m_nGroupId)
    {
        return kszAllContactId;
    }

    return m_nGroupId;
}

int CCommonDirCtrl::GetLogicListGroupId()
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

bool CCommonDirCtrl::IsSpecialSearchGroup(int nGroupId)
{
    int nUiStatus = GetState();
    if (DIR_UI_STATE_PRECISE_SEARCH == nUiStatus
            || (DIR_UI_STATE_SEARCH == nUiStatus && !configParser_GetConfigInt(kszBsftDirUpdateEnable) == 1))
    {
        return true;
    }

    return false;
}
