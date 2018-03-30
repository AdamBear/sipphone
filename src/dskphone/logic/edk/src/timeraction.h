#ifndef __TIMER_ACTION_H__
#define __TIMER_ACTION_H__
#include "edkactionbase.h"
#include <ETLLib.hpp>

typedef YLHashMap<int, int> MAP_ACTIONID_TO_TIMER;
typedef MAP_ACTIONID_TO_TIMER::iterator MAP_ACTIONID_TO_TIMER_ITER;

class CTimerAction
    : public CEdkActionBase
{
public:
    CTimerAction(void);
    virtual ~CTimerAction(void);
    virtual bool DoAction(int iActionId, edk_action_data_t & tEdkAction, bool & bResult);

    // 处理消息
    virtual bool OnMessage(msgObject & msg);

private:
    // 处理定时器消息
    static LRESULT OnTimerActionMessage(msgObject & objMsg);

    // 删除map
    bool RemoveTimer(int iActionId);

private:
    //int m_iTimerID;   // 判断定时器是否开启
    MAP_ACTIONID_TO_TIMER m_mapTimer;
};

#endif // __TIMER_ACTION_H__

