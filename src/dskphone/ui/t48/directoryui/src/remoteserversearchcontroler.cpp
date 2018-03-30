#include "remoteserversearchcontroler.h"
#ifndef DIR_SUPPORT_SINGLE_UIHELPER
#include "remoteserversearchuihelper.h"
#endif


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

CRemoteServerSearchUIHelper * CRemoteServerSearchControler::GetServerSearchUIHelper()
{
#ifdef DIR_SUPPORT_SINGLE_UIHELPER
    return m_pUIHelper;
#else
    return CRemoteServerSearchUIHelper::GetServerSearchUIHelper(m_pUIHelper);
#endif
}

int CRemoteServerSearchControler::GetLogicListGroupId()
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

bool CRemoteServerSearchControler::OnBack()
{
    int nCurGroupId = GetGroupId();

    int nParentGroupId = Dir_GetParentIdById(nCurGroupId, m_eLogicType);

    if (kszRootId != nParentGroupId)
    {
        PreSearchContact();

        return true;
    }

    return false;
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
        CDirUIHelper::ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT, DIR_ACTION_REMOTE_SERVER_SEARCH);
    }
    else
    {
        CDirUIHelper::ShowErrorMsg(nRetCode);
    }

    return true;
}

bool CRemoteServerSearchControler::SwitchToGroup(int nGroupId, int nAction)
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
        CDirUIHelper::RefreshUI(true, true);
        return true;
    }

    m_nDownLoadGroupId = nGroupId;
    m_bDownloading = true;

    if (FILE_DOWNLOADING != pMenu->m_eStatus)
    {
        int nRetCode = RemoteDir_ReqLoadContact(nGroupId);

        if (DIR_RET_SUCC != nRetCode)
        {
            CDirUIHelper::ShowErrorMsg(nRetCode);
            return true;
        }
    }

    CDirUIHelper::ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT, nAction);
    return true;
}

void CRemoteServerSearchControler::OnEnterSearch()
{
    ClearSearchData();

    m_nSearchType = SEARCH_NONE;
    m_nSearchGroupId = GetGroupId();
}

bool CRemoteServerSearchControler::LoadListData(int nDataIndex, int nDataCount)
{
    DIRUI_INFO("CRemoteServerSearchControler::LoadListData(Index:%d Count:%d)", nDataIndex, nDataCount);

    m_nSearchType = SEARCH_SERVER;

    return LoadGroupListData(nDataIndex, nDataCount);
}

bool CRemoteServerSearchControler::OnAction(int nAction, int nDataIndex, int nId)
{
    DIRUI_INFO("CRemoteServerSearchControler::OnAction(Action:%d Index:%d Id:%d)",
               nAction, nDataIndex, nId);

    switch (nAction)
    {
    case DIR_ACTION_REMOTE_PREVPAGE:
        {
            int nGroupId = GetPreGroupId();

            if (knInvalidRId == nGroupId)
            {
                break;
            }

            SwitchToGroup(nGroupId, DIR_ACTION_REMOTE_PREVPAGE);

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

            SwitchToGroup(nGroupId, DIR_ACTION_REMOTE_NEXTPAGE);

            return true;
        }
        break;
    case DIR_ACTION_COMMON_SELECT:
        {
            // 如果搜索的是远程组，则进入该群组，否则执行CRemoteContactControler中流程
            if (IsServerSearchGroup(nId))
            {
                SwitchToGroup(nId, DIR_ACTION_REMOTE_OPEN_APPOINT_GROUP);

                return true;
            }
        }
        break;
    default:
        break;
    }

    if (CRemoteContactControler::OnAction(nAction, nDataIndex, nId))
    {
        return true;
    }

    return false;
}

bool CRemoteServerSearchControler::CancelAction(int nAction, int nDataIndex, int nId)
{
    CRemoteContactControler::CancelAction(nAction, nDataIndex, nId);
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
    CRemoteServerSearchUIHelper * pUIHelper = GetServerSearchUIHelper();

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
        CDirUIHelper::EndWaiting();
    }

    if (m_nGroupId != m_nDownLoadGroupId && knInvalidRId != m_nDownLoadGroupId)
    {
        SetGroupId(m_nDownLoadGroupId);
    }

    m_nDownLoadGroupId = knInvalidRId;
    m_bDownloading = false;

    if (0 != refMessage.lParam)
    {
        CDirUIHelper::ShowMsgBox(TRID_NO_RESULT);
    }

    CDirUIHelper::RefreshUI(true, true);
}
