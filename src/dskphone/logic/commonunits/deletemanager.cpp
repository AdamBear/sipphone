#include <ETLLib.hpp>
#include "deletemanager.h"
#include "basedeletelater.h"
#include <yllist.h>
#include "dsklog/log.h"

#if 0

// 待删除的目标链表
typedef YLList<CBaseDeleteLater *> LIST_DELETE_TARGET;

#define _DeleteList GetGlobalList()
static LIST_DELETE_TARGET & GetGlobalList()
{
    // 为了控制全局sList的构造顺序
    // 调用deleteLater时必须构造出来
    static LIST_DELETE_TARGET sList;
    return sList;
}

static int s_iDelayTimer = 10;

// 异步删除
void CDeleteManager::deleteLater(CBaseDeleteLater * pTarget)
{
    if (pTarget == NULL)
    {
        return;
    }

    LIST_DELETE_TARGET::iterator it = _DeleteList.begin();
    for (; it != _DeleteList.end(); ++it)
    {
        if (pTarget == (*it))
        {
            UIMANAGER_WARN("target already exist!");
            return;
        }
    }

    _DeleteList.push_back(pTarget);
    timerSetThreadTimer((UINT)&s_iDelayTimer, s_iDelayTimer);
}

// 删除
void CDeleteManager::DeleteTraget()
{
    LIST_DELETE_TARGET::iterator it = _DeleteList.begin();
    for (; it != _DeleteList.end(); ++it)
    {
        delete (*it);
    }

    _DeleteList.clear();
}

// 定时器处理
BOOL CDeleteManager::OnTimer(UINT uTimerID)
{
    if (uTimerID == (UINT)&s_iDelayTimer)
    {
        timerKillThreadTimer((UINT)&s_iDelayTimer);
        DeleteTraget();
        return TRUE;
    }
    return FALSE;
}
#endif
