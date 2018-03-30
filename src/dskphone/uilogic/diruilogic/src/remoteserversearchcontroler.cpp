#include "remoteserversearchcontroler.h"
#include "diruiadapter.h"


CRemoteServerSearchControler::CRemoteServerSearchControler()
    : m_nSearchGroupId(knInvalidRId)
{
    m_nType = DIRECTORY_TYPE_REMOTE_SEARCH;
}

CRemoteServerSearchControler::~CRemoteServerSearchControler()
{
}

CRemoteServerSearchControler * CRemoteServerSearchControler::GetServerSearchControler(
    CDirectoryControlerPtr & pControler)
{
    if (pControler.IsEmpty() || DIRECTORY_TYPE_REMOTE_SEARCH != pControler->GetType())
    {
        return NULL;
    }

    return (CRemoteServerSearchControler *)pControler.Get();
}

void CRemoteServerSearchControler::UnInit()
{
    CRemoteContactControler::UnInit();

    ClearSearchData();
}

void CRemoteServerSearchControler::EnterState(int nState)
{
    CRemoteContactControler::EnterState(nState);

    if (IsSearchState())
    {
        ClearSearchData();

        m_nSearchGroupId = GetGroupId();
    }
}

int CRemoteServerSearchControler::GetResultGroupId()
{
    return m_nGroupId;
}

int CRemoteServerSearchControler::GetSearchGroupId() const
{
    return m_nSearchGroupId;
}

int CRemoteServerSearchControler::GetPreGroupId()
{
    SmartPtr pDetail = GetDetail(m_nGroupId);
    ContactRemoteMenu * pRemoteMenu = GetRemoteGroup(pDetail);

    return (NULL == pRemoteMenu ? knInvalidRId : pRemoteMenu->m_nprevPageId);
}

int CRemoteServerSearchControler::GetNextGroupId()
{
    SmartPtr pDetail = GetDetail(m_nGroupId);
    ContactRemoteMenu * pRemoteMenu = GetRemoteGroup(pDetail);

    return (NULL == pRemoteMenu ? knInvalidRId : pRemoteMenu->m_nNextPageId);
}

void CRemoteServerSearchControler::ClearSearchData()
{
    if (!IsServerSearchGroup(m_nGroupId))
    {
        return;
    }

    int nRootId = RemoteDir_GetRootGroupId(m_nGroupId);
    Dir_ClearContactData(nRootId, m_eLogicType);

    m_bDownloading = false;

    if (knInvalidRId != m_nDownLoadGroupId)
    {
        Dir_CancelLoad(m_nDownLoadGroupId, m_eLogicType);
        m_nDownLoadGroupId = knInvalidRId;
    }
}

bool CRemoteServerSearchControler::PreSearchContact()
{
    if (!IsServerSearchGroup(m_nSearchGroupId))
    {
        return false;
    }

    const yl::string & strSearchKey = GetSearchKey();

    if (strSearchKey.empty())
    {
        return true;
    }

    DIRUI_INFO("Search Contact Key = [%s]", strSearchKey.c_str());

    if (m_nSearchGroupId != m_nGroupId)
    {
        SetGroupId(m_nSearchGroupId);
    }

    int nRetCode = RemoteDir_ReqLoadContact(m_nGroupId, strSearchKey);

    if (DIR_RET_SUCC == nRetCode)
    {
        m_bDownloading = true;
        m_nDownLoadGroupId = m_nGroupId;
        g_DirUIAdapter.ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT, DIR_ACTION_REMOTE_SERVER_SEARCH);
    }
    else
    {
        CCommonDirCtrl::ShowErrorMsg(nRetCode);
    }

    return true;
}

bool CRemoteServerSearchControler::SwitchToGroup(int nGroupId, bool bPreGroup)
{
    SmartPtr pDetail = Dir_GetDetailById(nGroupId, m_eLogicType);
    DirGroupRecord * pMenu = GetGroupRecord(pDetail);

    if (NULL == pMenu)
    {
        DIRUI_ERR("CRemoteServerSearchControler::SwitchToGroup(%d) not group", nGroupId);
        return false;
    }

    if (FILE_DOWNLOADED_OK == pMenu->m_eStatus)
    {
        SetGroupId(nGroupId);

        if (!m_pUIHelper.IsEmpty())
        {
            m_pUIHelper->RefreshUI(DIR_UI_REFRESH_TYPE_ALL, DIR_UI_REFRESH_PARAM_ALL);
        }

        return true;
    }

    m_nDownLoadGroupId = nGroupId;
    m_bDownloading = true;

    if (FILE_DOWNLOADING != pMenu->m_eStatus)
    {
        int nRetCode = RemoteDir_ReqLoadContact(nGroupId);

        if (DIR_RET_SUCC != nRetCode)
        {
            CCommonDirCtrl::ShowErrorMsg(nRetCode);
            return true;
        }
    }

    int nAction = DIR_ACTION_REMOTE_NEXTPAGE;

    if (bPreGroup)
    {
        nAction = DIR_ACTION_REMOTE_PREVPAGE;
    }

    g_DirUIAdapter.ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT, nAction);
    return true;
}

bool CRemoteServerSearchControler::IsManualSearch() const
{
    return true;
}

#if DIR_UI_FEATURE_LOAD_DATA_LIST
bool CRemoteServerSearchControler::LoadListData(int nDataIndex, int nDataCount)
{
    DIRUI_INFO("CRemoteServerSearchControler::LoadListData(Index:%d Count:%d)", nDataIndex, nDataCount);

    return LoadGroupListData(nDataIndex, nDataCount);
}
#endif

bool CRemoteServerSearchControler::OnAction(int nAction, int nDataIndex, int nId)
{
    DIRUI_INFO("CRemoteContactControler::OnAction(Action:%d Index:%d Id:%d)",
               nAction, nDataIndex, nId);

    if (CRemoteContactControler::OnAction(nAction, nDataIndex, nId))
    {
        return true;
    }

    switch (nAction)
    {
    case DIR_ACTION_REMOTE_PREVPAGE:
        {
            int nGroupId = GetPreGroupId();

            if (knInvalidRId == nGroupId)
            {
                break;
            }

            SwitchToGroup(nGroupId, true);

            return true;
        }
        break;
    case DIR_ACTION_REMOTE_NEXTPAGE:
        {
            int nGroupId = GetNextGroupId();

            if (knInvalidRId == nGroupId)
            {
                break;
            }

            SwitchToGroup(nGroupId, false);

            return true;
        }
        break;
    default:
        break;
    }

    return false;
}

bool CRemoteServerSearchControler::CancelAction(int nAction, int nDataIndex, int nId)
{
    if (DIR_ACTION_REMOTE_SERVER_SEARCH == nAction)
    {
        ClearSearchData();
        return true;
    }

    if (DIR_ACTION_REMOTE_PREVPAGE == nAction || DIR_ACTION_REMOTE_NEXTPAGE == nAction)
    {
        if (knInvalidRId != m_nDownLoadGroupId)
        {
            Dir_CancelLoad(m_nDownLoadGroupId, m_eLogicType);
            m_nDownLoadGroupId = knInvalidRId;
        }

        m_bDownloading = false;

        return true;
    }

    return false;
}

LRESULT CRemoteServerSearchControler::OnMessage(msgObject & refMessage)
{
    if (REMOTE_PHONEBOOK_ALL_DOWNLOADED_FINISHED == refMessage.message)
    {
        OnDownLoadFinished(refMessage);

        return TRUE;
    }

    return CRemoteContactControler::OnMessage(refMessage);
}

void CRemoteServerSearchControler::OnDownLoadFinished(msgObject & refMessage)
{
    IDirUIHelper * pUIHelper = m_pUIHelper.Get();

    if (NULL == pUIHelper)
    {
        return;
    }

    int nGroupId = refMessage.wParam;

    DIRUI_INFO("CRemoteServerSearchControler::OnDownLoadFinished(CurGroupId:%d GroupId:%d lp:%d)",
               m_nDownLoadGroupId, nGroupId, refMessage.lParam);

    if (nGroupId != m_nDownLoadGroupId)
    {
        return;
    }

    if (m_bDownloading)
    {
        g_DirUIAdapter.EndWaiting();
    }

    if (m_nGroupId != m_nDownLoadGroupId)
    {
        SetGroupId(m_nDownLoadGroupId);
    }

    m_nDownLoadGroupId = knInvalidRId;
    m_bDownloading = false;

    if (0 != refMessage.lParam)
    {
        g_DirUIAdapter.ShowMsgBox(TRID_NO_RESULT);
    }

    pUIHelper->RefreshUI(DIR_UI_REFRESH_TYPE_LIST_VIEW | DIR_UI_REFRESH_TYPE_TITLE,
                         DIR_UI_REFRESH_PARAM_ALL);
}
