#include "talkasync_inc.h"

IMPLEMENT_GETINSTANCE(CAsyncHelper)

CAsyncHelper::CAsyncHelper()
{
}

CAsyncHelper::~CAsyncHelper()
{
}

void CAsyncHelper::OnTaskActionCallback(CTaskAction* pTaskAction)
{
    if (pTaskAction == NULL)
    {
        return;
    }

    ASYNC_MAP_ACTION::iterator it = m_mapAction.find(pTaskAction);
    if (it != m_mapAction.end())
    {
        OnCallback(it->second, pTaskAction);
        m_mapAction.erase(it);
    }
}

void CAsyncHelper::SetCallID(CTaskAction* pTaskAction, int iCallID)
{
    if (pTaskAction != NULL && iCallID != -1)
    {
        m_mapAction[pTaskAction] = iCallID;
    }
}

bool CAsyncHelper::OnCallback(int iCallID, CTaskAction* pTaskAction)
{
    if (pTaskAction == NULL)
    {
        return false;
    }

    switch (pTaskAction->GetReqType())
    {
    case NS_TA::TA_REQUEST_SIMPLEACTION:
        return OnContractMatchCallback(iCallID, pTaskAction);
    case NS_TA::TA_REQUEST_RING_FILE:
        return OnRingDownloadCallback(iCallID, pTaskAction);
    case NS_TA::TA_REQUEST_SIP_PIC:
        return OnImageDownloadCallback(iCallID, pTaskAction);
    default:
        break;
    }

    return false;
}

bool CAsyncHelper::OnRingDownloadCallback(int iCallID, CTaskAction* pTaskAction)
{
    CRingFileAction* pRingFileAction = dynamic_cast<CRingFileAction*>(pTaskAction);
    if (pRingFileAction == NULL)
    {
        return false;
    }

    yl::string strPath = pRingFileAction->GetLocalSavedPath();
    int iSessionID = talklogic_GetSessionIdByCallID(iCallID, false);

    int iStatus = pTaskAction->IsOperationSucceed() ? TALK_RING_LS_LOADED : TALK_RING_LS_FAILED;
    return talklogic_UIEvent(iSessionID, TALK_ACTION_PLAY_TONE, iStatus, 0, (void*)strPath.c_str());
}

bool CAsyncHelper::OnImageDownloadCallback(int iCallID, CTaskAction* pTaskAction)
{
    CSipPicAction* pAction = dynamic_cast<CSipPicAction*>(pTaskAction);
    if (pAction == NULL)
    {
        return false;
    }

    const yl::string& strPath = pAction->GetFilePath();

    CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(iCallID);
    if (pCallInfo == NULL)
    {
        unlink(strPath.c_str());
        return false;
    }

    TALK_INFO("HelperImageFromSIP Set Photo [%s]", strPath.c_str());

    pCallInfo->SetCallerImageURL(strPath);
    return true;
}

bool CAsyncHelper::OnContractMatchCallback(int iCallID, CTaskAction* pTaskAction)
{
    //返回类型为ContactMatchAction
    CContactMatchAction* pAction = dynamic_cast<CContactMatchAction*>(pTaskAction);
    if (pAction == NULL)
    {
        return false;
    }

    CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(iCallID);
    if (pCallInfo == NULL)
    {
        return false;
    }

    const yl::string& strName = pAction->GetTargetName();
    if (!strName.empty())
    {
        pCallInfo->m_tRemoteInfo.sDisplayName = strName;
        TALK_INFO("CHelperContactMatch Set DisplayName [%s]", strName.c_str());
        // 刷新UI
        return talklogic_UpdateWnd();
    }

    return false;
}

