#include "directorycontroler.h"
#ifdef FL_SUPPORT
#include "directorylistdelegate.h"
#include "searchdelegate.h"
#else
#include "directoryframe.h"
#endif
#include "diruihelper.h"
#include "feature/include/modfeature.h"
#include "talk/talklogic/include/modtalklogic.h"


bool CDirectoryControler::s_bReadOnly = false;
#ifdef DIR_SUPPORT_SWITCH_DETAIL
int CDirectoryControler::s_nTotalCount = 0;
int CDirectoryControler::s_nCurIndex = 0;
#endif


namespace
{
// 打开本地联系人页面的Click Action.
static const char kszMenuCALocal[] = "local_contact_list";
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
#if IF_BUG_PLCMDIR
// 打开plcm联系人页面的Click Action.
static const char kszMenuCAPlcm[] = "plcm_contact_list";
#endif
#if IF_FEATURE_GBBOOK
// 打开genband联系人页面的Click Action.
static const char kszMenuCAGenbandGab[] = "gab_contact_list";
static const char kszMenuCAGenbandPab[] = "pab_contact_list";
#endif

// 打开本地CallLog页面的Click Action.
static const char kszMenuCALocalCallLog[] = "local_call_log_list";
// 打开BSFT CallLog页面的Click Action.
static const char kszMenuCANetworkCallLog[] = "broadsoft_calllog_list";
// 打开CallLog总页面的Click Action.
static const char kszMenuCACallLog[] = "calllog_list";

// 打开mtsw CallLog页面的Click Action.
static const char kszMenuCAMetaswitchCallLog[] = "metaswitch_calllog_list";
// 打开BSFT联系人页面的Click Action.
static const char kszMenuCAMetaswitchContact[] = "metaswitch_contact_list";
}

yl::string GetClickAction(int nDirType, int nGroupId)
{
    switch (nDirType)
    {
    case DIRECTORY_TYPE_LOCAL:
        {
            return (kszBlackListGroupId == nGroupId)
                   ? kszMenuCABlacklist : kszMenuCALocal;
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
            return (kszGabBookGroupId == nGroupId)
                   ? kszMenuCAGenbandGab : kszMenuCAGenbandPab;
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
#if IF_FEATURE_METASWITCH_CALLLOG
    case CALL_LOG_TYPE_MTSW:
        {
            return kszMenuCAMetaswitchCallLog;
        }
        break;
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    case DIRECTORY_TYPE_METASWICTH:
        {
            return kszMenuCAMetaswitchContact;
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
    , m_bBlindTransfer(false)
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

void CDirectoryControler::SetReadOnly(bool bReadOnly)
{
    s_bReadOnly = bReadOnly;
}

bool CDirectoryControler::IsReadOnly() const
{
    return s_bReadOnly;
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

#ifdef FL_SUPPORT
void CDirectoryControler::Init(CDirectoryListDelegatePtr & pDelegate, CDirUIHelperPtr & pUIHelper)
{
    m_pDelegate = pDelegate;
    m_pUIHelper = pUIHelper;
}
#else
void CDirectoryControler::Init(CDirUIHelperPtr & pUIHelper)
{
    m_pUIHelper = pUIHelper;
}
#endif

void CDirectoryControler::UnInit()
{
#ifdef FL_SUPPORT
    m_pDelegate.Reset();
#endif
#ifdef DIR_SUPPORT_SINGLE_UIHELPER
    m_pUIHelper = NULL;
#else
    m_pUIHelper.Reset();
#endif
}

CDirUIHelperPtr CDirectoryControler::GetUIHelper()
{
    return m_pUIHelper;
}

bool CDirectoryControler::IsInMenu() const
{
    return (knInvalidRId == m_nGroupId);
}

bool CDirectoryControler::IsBrowserState()
{
#ifdef FL_SUPPORT
    if (m_pDelegate.IsEmpty())
    {
        return false;
    }

    return (DIR_UI_STATE_BROWSER == m_pDelegate->GetState());
#else
    return (DIR_UI_STATE_BROWSER == _DirectoryFrame.GetPageType().nUIState);
#endif
}

bool CDirectoryControler::IsSearchState()
{
#ifdef FL_SUPPORT
    if (m_pDelegate.IsEmpty())
    {
        return false;
    }

    return (DIR_UI_STATE_SEARCH == m_pDelegate->GetState() || (DIR_UI_STATE_PRECISE_SEARCH == m_pDelegate->GetState() && configParser_GetConfigInt(kszBsftDirUpdateEnable) == 0));
#else
    return (DIR_UI_STATE_SEARCH == _DirectoryFrame.GetPageType().nUIState);
#endif
}

bool CDirectoryControler::IsDetailState()
{
#ifdef FL_SUPPORT
    if (m_pDelegate.IsEmpty())
    {
        return false;
    }

    return (DIR_UI_STATE_DETAIL == m_pDelegate->GetState());
#else
    return (DIR_UI_STATE_DETAIL == _DirectoryFrame.GetPageType().nUIState);
#endif
}

#ifdef DIR_SUPPORT_SETTING
bool CDirectoryControler::IsSettingState()
{
#ifdef FL_SUPPORT
    if (m_pDelegate.IsEmpty())
    {
        return false;
    }

    return (DIR_UI_STATE_SETTING == m_pDelegate->GetState());
#else
    return (DIR_UI_STATE_SETTING == _DirectoryFrame.GetPageType().nUIState);
#endif
}
#endif

int CDirectoryControler::GetState()
{
#ifdef FL_SUPPORT
    if (m_pDelegate.IsEmpty())
    {
        if (DIRECTORY_TYPE_LOCAL == m_nType || DIRECTORY_TYPE_BROADSOFT == m_nType
#if IF_FEATURE_GBBOOK
                || DIRECTORY_TYPE_GENBAND == m_nType
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
                || DIRECTORY_TYPE_METASWICTH == m_nType
#endif
           )
        {
            return DIR_UI_STATE_ADDITION;
        }

        return DIR_UI_STATE_NONE;
    }

    return m_pDelegate->GetState();
#else
    return _DirectoryFrame.GetPageType().nUIState;
#endif
}

const yl::string & CDirectoryControler::GetSearchKey()
{
#ifdef FL_SUPPORT
    static yl::string s_strEmpty = "";

    if (m_pDelegate.IsEmpty() || !IsSearchState())
    {
        return s_strEmpty;
    }

    CSearchDelegate * pDelegate = CSearchDelegate::GetDirDelegate(m_pDelegate);

    if (NULL == pDelegate)
    {
        return s_strEmpty;
    }

    return pDelegate->GetSearchKey();
#else
    static yl::string s_strKey;

    _DirectoryFrame.GetSearchKey(s_strKey);

    return s_strKey;
#endif
}

bool CDirectoryControler::IsCanOnlySearch() const
{
    return false;
}

void CDirectoryControler::OnEnterSearch()
{

}

int CDirectoryControler::GetParGroupId(int nGroupId)
{
    return knInvalidRId;
}

bool CDirectoryControler::PreLoadContact(int nGroupId)
{
    if (knInvalidRId != nGroupId)
    {
        SetGroupId(nGroupId);
    }
    return true;
}

int CDirectoryControler::GetAdjoinGroupId(bool bPrevious, int & nIndex)
{
    return knInvalidRId;
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

bool CDirectoryControler::IsBlindTransfer()
{
    return m_bBlindTransfer;
}

void CDirectoryControler::SetBlindTransfer(bool bBlindTransfer)
{
    m_bBlindTransfer = bBlindTransfer;
}
