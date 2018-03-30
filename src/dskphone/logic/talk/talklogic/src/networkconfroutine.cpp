#include "talklogic_inc.h"

// 网络会议通话的过程
CNetworkConfRoutine::CNetworkConfRoutine()
{
}

CNetworkConfRoutine::~CNetworkConfRoutine()
{

}

// 创建Routine之后的处理
void CNetworkConfRoutine::AfterRoutineCreate()
{
    m_pCallInfo->CheckHoldStatus();
    etl_NotifyApp(FALSE, TALK_MSG_TALK_ENTER, m_pCallSession->GetSessionID(),
                  m_pCallInfo->GetAccountID());
}

// 销毁Routine之前的处理
void CNetworkConfRoutine::BeforeRoutineDestroy(bool bJumpToNextRoutine/* = true*/)
{
    _SessionManager.ShowSessionAddToConf(true);
    CBaseRoutine::BeforeRoutineDestroy(bJumpToNextRoutine);
}

// 将另外一路通话加入会议中
bool CNetworkConfRoutine::AddToConf(CSingleSession* pSession)
{
    if (pSession == NULL)
    {
        return false;
    }

    // 被加入会议的另外一路必须是通话中的话路
    if (pSession->GetRoutineState() != ROUTINE_TALKING)
    {
        return false;
    }

    CCallInfo* pAddToInfo = pSession->GetCallInfo();
    if (pAddToInfo != NULL)
    {
        pAddToInfo->m_bAddToNetworkConf = true;
    }

    return modVoip_AddToNetworkConf(pAddToInfo->GetAccountID(), m_pCallInfo->GetCallID(),
                             pAddToInfo->GetCallID());
}

// Routine的SIP消息响应
bool CNetworkConfRoutine::OnSIPMsg(msgObject& msg)
{
    bool bHandle = false;

    switch (msg.message)
    {
    case SIP_CALL_RELEASED:
        {
            // 呼叫失败则一定会播放忙音,不管BusyToneDelay是否为0
            _TalklogicManager.ProcessCallFailed(this, msg);
        }
        break;
    default:
        break;
    }

    if (!bHandle)
    {
        bHandle = CTalkRoutine::OnSIPMsg(msg);
    }

    return bHandle;
}

bool CNetworkConfRoutine::OnActionEvent(DataUI2Logic* pActionData)
{
    bool bHandle = false;

    switch (pActionData->eAction)
    {
    case TALK_ACTION_REMOVE_NET_CONF_PARTY:
        {
            yl::string strUserId = (char*)pActionData->pData;
            bHandle = RemoveConfParty(strUserId);
            APP_INFO("REMOVE_NET_CONF_PARTY strUserId[%s] bHandle[%d]\n", strUserId.c_str(), bHandle);
        }
        break;
    default:
        break;
    }

    if (!bHandle)
    {
        bHandle = CTalkRoutine::OnActionEvent(pActionData);
    }

    return bHandle;
}

//根据userid删除与会者
bool CNetworkConfRoutine::RemoveConfParty(yl::string strUserId)
{
    return modVoip_DeleteNetConfParty(m_pCallInfo->GetAccountID(), m_pCallInfo->GetCallID(),
                                      strUserId.c_str());
}

