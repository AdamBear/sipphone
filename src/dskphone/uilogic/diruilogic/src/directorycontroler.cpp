#include "directorycontroler.h"
#include "feature/include/modfeature.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "diruiadapter.h"


#ifdef DIR_SUPPORT_SWITCH_DETAIL
int CDirectoryControler::s_nTotalCount = 0;
int CDirectoryControler::s_nCurIndex = 0;
#endif
LIST_ATTRIBUTE CDirectoryControler::s_listAtrr;
int CDirectoryControler::s_nAttrType = DIRECTORY_TYPE_NONE;


namespace
{
// 打开本地联系人页面的Click Action.
static const char kszMenuCALocal[] = "local_contact_list";
// 打开本地联系人页面的Click Action.
static const char kszMenuCABluetooth[] = "bluetooth_contact_list";
// 打开黑名单页面的Click Action.
static const char  kszMenuCABlacklist[] = "blacklist_list";
// 打开BSFT联系人页面的Click Action.
static const char kszMenuCANetwork[] = "broadsoft_contact_list";
// 打开远程联系人页面的Click Action.
static const char kszMenuCARemote[] = "remote_contact_list";
// 打开UC联系人页面的Click Action.
static const char kszMenuCAUC[] = "uc_contact_list";
// 打开整个联系人页面的Click Action
static const char kszMenuCADirectory[] = "directory";

#ifdef IF_FEATURE_FAVORITE_DIR
static const char  kszMenuCAFavorite[] = "favorite_list";
#endif

#if IF_BUG_PLCMDIR
static const char kszMenuCAPlcm[] = "plcm_contact_list";
#endif

#if IF_FEATURE_GBBOOK
static const char kszMenuCAGAB[] = "gab_contact_list";
static const char kszMenuCAPAB[] = "pab_contact_list";
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
static const char kszMenuCAMetaswith[] = "metaswitch_contact_list";
#endif

// 打开本地CallLog页面的Click Action.
static const char kszMenuCALocalCallLog[] = "local_call_log_list";
// 打开BSFT CallLog页面的Click Action.
static const char kszMenuCANetworkCallLog[] = "broadsoft_calllog_list";
// 打开CallLog总页面的Click Action.
static const char kszMenuCACallLog[] = "calllog_list";

#if IF_FEATURE_METASWITCH_CALLLOG
static const char kszMenuCAMetaswitchCallLog[] = "metaswitch_calllog_list";
#endif
}

yl::string GetClickAction(int nDirType, int nGroupId)
{
    switch (nDirType)
    {
    case DIRECTORY_TYPE_LOCAL:
        {
#ifdef IF_FEATURE_FAVORITE_DIR
            if (kszBlackListGroupId == nGroupId)
            {
                return kszMenuCABlacklist;
            }
            else if (kszFavoriteGroupId == nGroupId)
            {
                return kszMenuCAFavorite;
            }
            else
            {
                return kszMenuCALocal;
            }
#else
            return (kszBlackListGroupId == nGroupId)
                   ? kszMenuCABlacklist : kszMenuCALocal;
#endif
        }
        break;
    case DIRECTORY_TYPE_BLUETOOTH:
        {
            return kszMenuCABluetooth;
        }
        break;
    case DIRECTORY_TYPE_REMOTE:
        {
            return kszMenuCARemote;
        }
        break;
    case DIRECTORY_TYPE_BROADSOFT:
        {
            return kszMenuCANetwork;
        }
        break;
    case DIRECTORY_TYPE_BTUC:
        {
            return kszMenuCAUC;
        }
        break;
#if IF_BUG_PLCMDIR
    case DIRECTORY_TYPE_PLCM:
        {
            return kszMenuCAPlcm;
        }
        break;
#endif
#if IF_FEATURE_GBBOOK
    case DIRECTORY_TYPE_GENBAND:
        {
            return (kszGabBookGroupId == nGroupId) ? kszMenuCAGAB : kszMenuCAPAB;
        }
        break;
#endif
    case CALL_LOG_TYPE_LOCAL:
        {
            return kszMenuCALocalCallLog;
        }
        break;
    case CALL_LOG_TYPE_BROADSOFT:
        {
            return kszMenuCANetworkCallLog;
        }
        break;
#if IF_FEATURE_METASWITCH_DIRECTORY
    case DIRECTORY_TYPE_METASWICTH:
        {
            return kszMenuCAMetaswith;
        }
        break;
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
    case CALL_LOG_TYPE_MTSW:
        {
            return kszMenuCAMetaswitchCallLog;
        }
        break;
#endif
    default:
        break;
    }

    return "";
}

yl::string GetClickAction(int nDirModType)
{
    switch (nDirModType)
    {
    case DIR_MOD_TYPE_CONTACT:
        {
            return kszMenuCADirectory;
        }
        break;
    case DIR_MOD_TYPE_CALLLOG:
        {
            return kszMenuCACallLog;
        }
        break;
    default:
        break;
    }

    return "";
}

CDirectoryControler::CDirectoryControler(int nType)
#ifdef DIR_SUPPORT_SINGLE_UIHELPER
    : m_pUIHelper(NULL)
    , m_nType(nType)
#else
    : m_nType(nType)
#endif
    , m_nGroupId(knInvalidRId)
    , m_nState(DIR_UI_STATE_NONE)
{
}

CDirectoryControler::~CDirectoryControler()
{
    DIRUI_INFO("~CDirectoryControler(%p)", this);
}

bool CDirectoryControler::IsUserLevelShow(int nDirType, int nGroupId/* = -1*/)
{
    yl::string strAction = GetClickAction(nDirType, nGroupId);

    return feature_UserModeConfigItemShow(strAction);
}

bool CDirectoryControler::IsDirCanEdit(int nDirType, int nGroupId)
{
    yl::string strAction = GetClickAction(nDirType, nGroupId);

    if (!strAction.empty() && feature_UserModeConfigItemReadOnly(strAction))
    {
        return false;
    }

    if (talklogic_SessionExist())
    {
        return false;
    }

    return true;
}

bool CDirectoryControler::IsDirCanEidt(int nDirModType)
{
    yl::string strAction = GetClickAction(nDirModType);

    if (!strAction.empty() && feature_UserModeConfigItemReadOnly(strAction))
    {
        return false;
    }

    if (talklogic_SessionExist())
    {
        return false;
    }

    return true;
}

void CDirectoryControler::SetAttributeList(int nDirType, LIST_ATTRIBUTE & lsAtrr)
{
    s_nAttrType = nDirType;
    s_listAtrr.clear();

    LIST_ATTRIBUTE::iterator iter = lsAtrr.begin();

    for (; iter != lsAtrr.end(); iter++)
    {
        ST_CONTACT_ATTRIBUTE & stAttr = *iter;

        s_listAtrr.push_back(stAttr);
    }
}

int CDirectoryControler::GetAttributeType()
{
    return s_nAttrType;
}

int CDirectoryControler::GetAttributeSize()
{
    return s_listAtrr.size();
}

int CDirectoryControler::GetAttributeCount(const char * pszName)
{
    int nCount = 0;

    if (NULL == pszName)
    {
        return nCount;
    }

    LIST_ATTRIBUTE::iterator iter = s_listAtrr.begin();

    for (int i = 0; iter != s_listAtrr.end(); ++iter, ++i)
    {
        const ST_CONTACT_ATTRIBUTE & stAttr = *iter;

        if (0 == strcmp(stAttr.m_strName.c_str(), pszName))
        {
            ++nCount;
        }
    }

    return nCount;
}

bool CDirectoryControler::GetAttributeByIndex(int nIndex, yl::string & strName,
        yl::string & strValue)
{
    LIST_ATTRIBUTE::iterator iter = s_listAtrr.begin();

    for (int i = 0; iter != s_listAtrr.end(); ++iter, ++i)
    {
        if (i != nIndex)
        {
            continue;
        }

        ST_CONTACT_ATTRIBUTE & stAttr = *iter;

        strName = stAttr.m_strName;
        strValue = stAttr.m_strValue;
        return true;
    }

    return false;
}

bool CDirectoryControler::GetAttributeByName(int nIndex, const char * pszName,
        yl::string & strValue)
{
    if (NULL == pszName)
    {
        return false;
    }

    LIST_ATTRIBUTE::iterator iter = s_listAtrr.begin();

    for (int i = 0; iter != s_listAtrr.end(); ++iter)
    {
        ST_CONTACT_ATTRIBUTE & stAttr = *iter;

        if (0 != strcmp(pszName, stAttr.m_strName.c_str()))
        {
            continue;
        }

        if (i == nIndex)
        {
            strValue = stAttr.m_strValue;
            return true;
        }

        ++i;
    }

    return false;
}

bool CDirectoryControler::Dial(int nAccountId, const yl::string & strName, LIST_ATTRIBUTE & lsAttr,
                               int nDialAction)
{
    int nSize = lsAttr.size();

    if (0 == nSize)
    {
        g_DirUIAdapter.ShowMsgBox(TRID_PHONE_NUMBER_IS_EMPTY);
        return false;
    }
    else if (1 == nSize)
    {
        ST_CONTACT_ATTRIBUTE & stAttr = lsAttr.front();

        return g_DirUIAdapter.Dial(nAccountId, strName, stAttr.m_strValue, nDialAction);
    }

    SetAttributeList(m_nType, lsAttr);

    int nUiState = GetState();
    if (DIR_UI_STATE_DETAIL != nUiState)
    {
        m_nDetailStatus = nUiState;
    }

    return g_DirUIAdapter.Dial(nAccountId, strName, lsAttr, nDialAction);
}

void CDirectoryControler::RefreshUI(int nType, int nParam)
{
    IDirUIHelperPtr pDirUIHelper = m_pUIHelper;
    IDirUIHelper * pUIHelper = pDirUIHelper.Get();

    if (NULL != pUIHelper)
    {
        pUIHelper->RefreshUI(nType, nParam);
    }
}

void CDirectoryControler::Init(IDirUIHelperPtr pUIHelper)
{
    m_pUIHelper = pUIHelper;
}

void CDirectoryControler::UnInit()
{
    m_nState = DIR_UI_STATE_NONE;
    m_nDetailStatus = DIR_UI_STATE_NONE;
    m_listState.clear();
    m_pUIHelper.Reset();
}

IDirUIHelperPtr CDirectoryControler::GetUIHelper()
{
    return m_pUIHelper;
}

int CDirectoryControler::GetType() const
{
    return m_nType;
}

void CDirectoryControler::SetGroupId(int nGroupId)
{
    m_nGroupId = nGroupId;
}

int CDirectoryControler::GetGroupId() const
{
    return m_nGroupId;
}

bool CDirectoryControler::IsInMenu() const
{
    return (knInvalidRId == m_nGroupId);
}

void CDirectoryControler::EnterState(int nState)
{
    if (nState == m_nState)
    {
        return;
    }

    LIST_DIR_UI_STATE::iterator iter = m_listState.begin();

    while (iter != m_listState.end())
    {
        int nTmpState = *iter;

        if (nTmpState == nState)
        {
            iter = m_listState.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    m_listState.push_back(nState);
    m_nState = nState;
}

void CDirectoryControler::LeaveState(int nState)
{
    LIST_DIR_UI_STATE::iterator iter = m_listState.begin();

    while (iter != m_listState.end())
    {
        int nTmpState = *iter;

        if (nTmpState == nState)
        {
            iter = m_listState.erase(iter);
            break;
        }
        else
        {
            ++iter;
        }
    }

    if (0 == m_listState.size())
    {
        m_nState = DIR_UI_STATE_NONE;
        UnInit();
    }
    else
    {
        m_nState = m_listState.back();
    }
}

void CDirectoryControler::OnEnterMenu()
{

}

bool CDirectoryControler::IsBrowseState()
{
    return DIR_UI_STATE_BROWSER == m_nState;
}

bool CDirectoryControler::IsSearchState()
{
    return DIR_UI_STATE_SEARCH == m_nState;
}

bool CDirectoryControler::IsDetailState()
{
    return DIR_UI_STATE_DETAIL == m_nState;
}

#ifdef DIR_SUPPORT_SETTING
bool CDirectoryControler::IsSettingState()
{
    return DIR_UI_STATE_SETTING == m_nState;
}
#endif

int CDirectoryControler::GetState()
{
    return m_nState;
}

int CDirectoryControler::GetDetailState()
{
    return m_nDetailStatus;
}

int CDirectoryControler::GetDetailItemAmount()
{
    return m_vecDetail.size();
}

bool CDirectoryControler::GetDetailData(int nIndex, ST_DETAIL_DATA & stDetail)
{
    if (nIndex < 0 || nIndex >= m_vecDetail.size())
    {
        return false;
    }

    stDetail = m_vecDetail[nIndex];

    return true;
}

const yl::string & CDirectoryControler::GetSearchKey()
{
    if (m_pUIHelper.IsEmpty() || !IsSearchState())
    {
        static yl::string s_strEmpty = "";
    }

    return m_pUIHelper->GetSearchKey();
}

bool CDirectoryControler::IsCanOnlySearch() const
{
    return false;
}

bool CDirectoryControler::IsManualSearch() const
{
    return false;
}

bool CDirectoryControler::IsEmptyKeySearch() const
{
    return false;
}

bool CDirectoryControler::IsCanEnterSearchByKey(int nKey) const
{
    return true;
}

int CDirectoryControler::GetSearchDirType(int nGroupId)
{
    return m_nType;
}

int CDirectoryControler::GetParGroupId(int nGroupId)
{
    return knInvalidRId;
}

int CDirectoryControler::GetDialAction()
{
    int nDialAction = DIR_DIAL_ACTION_DIGIT_MAP_DIRECTORY;

#if IF_BUG_27045
    nDialAction |= DIR_DIAL_ACTION_ADD_AREA_CODE;
#endif

    return nDialAction;
}

bool CDirectoryControler::EnterGroupByHotKey(int nKey)
{
    return false;
}

bool CDirectoryControler::IsCanEnterGroupByKey(int nKey)
{
    return false;
}

bool CDirectoryControler::PreLoadContact(int nGroupId)
{
    if (knInvalidRId != nGroupId)
    {
        SetGroupId(nGroupId);
    }
    return true;
}

bool CDirectoryControler::ExecAction(int nAction, int nDataIndex, int nId)
{
    return false;
}

bool CDirectoryControler::CancelAction(int nAction, int nDataIndex, int nId)
{
    return false;
}

LRESULT CDirectoryControler::OnMessage(msgObject & refMessage)
{
    return FALSE;
}

bool CDirectoryControler::IsBlindTransfer()
{
    return m_bBlindTransfer;
}

void CDirectoryControler::SetBlindTransfer(bool bBlindTransfer)
{
    m_bBlindTransfer = bBlindTransfer;
}

#ifdef DIR_SUPPORT_SWITCH_DETAIL
void CDirectoryControler::SetTotalCount(int nTotalCount)
{
    s_nTotalCount = nTotalCount;
}

void CDirectoryControler::SetCurIndex(int nCurIndex)
{
    s_nCurIndex = nCurIndex;
}

int CDirectoryControler::GetTotalCount() const
{
    return s_nTotalCount;
}

int CDirectoryControler::GetCurIndex() const
{
    return s_nCurIndex;
}

void CDirectoryControler::RefreshTotalCount()
{

}
#endif
