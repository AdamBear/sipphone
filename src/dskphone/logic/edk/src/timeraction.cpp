#include "timeraction.h"
#include "edkactionmanager.h"
#include <etlmsghandler/modetlmsghandler.h>

#if IF_FEATURE_EDK
CTimerAction::CTimerAction()
    : CEdkActionBase(EDK_EXCUTETIMER)
{
    //m_iTimerID = 0;
    m_mapTimer.clear();

    // 注册消息
    etl_RegisterMsgHandle(TM_TIMER, TM_TIMER, &CTimerAction::OnTimerActionMessage);
}

CTimerAction::~CTimerAction()
{
    etl_UnregisterMsgHandle(TM_TIMER, TM_TIMER, &CTimerAction::OnTimerActionMessage);
}


// 处理消息
bool CTimerAction::OnMessage(msgObject & msg)
{
    if (msg.message != TM_TIMER)
    {
        return false;
    }

    for (MAP_ACTIONID_TO_TIMER_ITER iter = m_mapTimer.begin();
            iter != m_mapTimer.end(); ++iter)
    {
        int iActionId = iter->first;

        if ((UINT) & (iter->second) == (msg.wParam))
        {
            EDK_INFO("CTimerAction::OnTimerOut [%d]", msg.wParam);
            // 处理完之后杀掉定时器
            timerKillThreadTimer((UINT)msg.wParam);

            // 删除map
            RemoveTimer(iActionId);

            // 定时器回来，继续执行命令流
            return _EdkActionManager.DoAction(iActionId);
        }
    }

    return false;
}

bool CTimerAction::DoAction(int iActionId, edk_action_data_t & tEdkAction, bool & bResult)
{
    int iTimer = atoi(tEdkAction.m_strAction.c_str());
    // 存入map
    m_mapTimer.insert(iActionId, iTimer);
    MAP_ACTIONID_TO_TIMER_ITER iter = m_mapTimer.find(iActionId);
    if (iter != m_mapTimer.end())
    {
        timerSetThreadTimer((UINT) & (iter->second), iTimer * 1000);
        EDK_INFO("CTimerAction::DoAction SetTime [%d]", iTimer);
    }
    return false;
}

/*
**定时器处理
*/
LRESULT CTimerAction::OnTimerActionMessage(msgObject & objMsg)
{
    return _EdkActionManager.OnMessage(EDK_EXCUTETIMER, objMsg) ? 1 : 0;
}

// 删除map
bool CTimerAction::RemoveTimer(int iActionId)
{
    MAP_ACTIONID_TO_TIMER_ITER iter = m_mapTimer.find(iActionId);
    if (iter != m_mapTimer.end())
    {
        m_mapTimer.erase(iter);
        return true;
    }
    return false;
}

#endif
