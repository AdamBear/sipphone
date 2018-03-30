#ifndef __DIALPLAN_DIALNOW_PROCESS_H__
#define __DIALPLAN_DIALNOW_PROCESS_H__

#include "singletonclassdefine.h"
#include <ylstring.h>

#include "talk/talklogic/include/talklogic_common.h"
#include "talk/talklogic/include/uiandlogic_common.h"
#include "talk/talklogic/include/modtalklogic.h"

class CDialPlanManager
{
    DECLARE_SINGLETON_CLASS(CDialPlanManager)

public:

    /* 定时器响应 */
    bool OnTimer(UINT uTimerID);

    /* 输入改变 */
    bool OnTextChanged(int iSessionID, const yl::string& strText);

    /* 进入拨号 */
    bool OnEnterDial(int iSessionID, int iAccountID, const yl::string& strText);

    /* 退出拨号 */
    bool OnLeaveDial(int iSessionID);

    /* 账号改变 */
    bool OnAccountChanged(int iSessionID, int iAccountNew);

    /* 按键触发的重新计时 */
    bool OnRestart(int iSessionID);

    /* 焦点变更 */
    bool OnFocusChanged(int iSessionID, bool bFocus);

private:

    /* Hotline、Authcall 等比DialPlan优先级还高的流程处理 */
    bool HotLineProcess();

    /* DigitMap处理流程 */
    bool DigitMapProcess();

    /* 所有流程 */
    bool DialPlanProcess();

    /* DialNow处理流程 */
    bool DialNowProcess();

    /* 呼出接口 */
    bool CallOut(const yl::string& strText, int iMask = 0, int iTimer = 0);

    /* 是否启用DigitMap */
    bool IsEnableDigitMap();

    /* 设置AutoDial和AutoQuit、两者有依赖关系 */
    void SetAutoDialAndQuitTimer();

    /* 检测是否需要匹配 */
    void CheckDialNow();

private:

    /* 当前是否在预拨号 */
    inline bool IsPredialRoutine();

    /* 当前是否在普通的sip拨号 */
    inline bool IsDialRoutine();

    /* 当前是否在蓝牙拨号 */
    bool IsBTDialRoutine();

    /* 设置DialNow计时器 */
    inline void SetDialNow();

    /* 设置AutoDial计时器 */
    inline  void SetAutoDial();

    /* 设置AutoQuit计时器 */
    inline void SetAutoQuit();

    /* 取消AutoDial计时器 */
    inline void CancelAutoDial();

    /* 取消AutoQuit计时器 */
    inline void CancelAutoQuit();

    /* 取消DialNow计时器 */
    inline void CancelDialNow();

    /* 取消所有计时器 */
    inline void CancelAll();

    /* 获取AutoQuit时间 */
    inline int GetAutoDialTimer();

    /* 获取AutoQuit时间 */
    inline int GetAutoQuitTimer();

private:
    /* Dial 目前只有一个，所以只需要缓存一份 */
    bool            m_bFirstDial;       /* 是否第一次输入 */
    bool            m_bEnableDialNow;   /* 是否匹配dialnow（digitmap）规则 */
    int             m_iSessionID;       /* 通话模块的Session */
    int             m_iAccount;         /* 当前使用的账号 */
    int             m_iDialNowTimer;    /* DialNow定时器时间 */
    int             m_iAutoDialTimer;   /* AutoDial定时器时间 */
    int             m_iAutoQuitTimer;   /* AutoQuit定时器时间 */
    int             m_iCurrentMode;     /* DialPlan当前运行的模式 */
    SESSION_STATE   m_eDialState;       /* 通话模块的通话状态 */
    yl::string      m_strDialText;      /* 当前输入的号码 */
};

#define _DialPlanManager GET_SINGLETON_INSTANCE(CDialPlanManager)

/************************************************************************/
/*                                                                      */
/************************************************************************/

bool CDialPlanManager::IsPredialRoutine()
{
    return (m_eDialState == SESSION_PREDIAL);
}

bool CDialPlanManager::IsDialRoutine()
{
    return (m_eDialState >= SESSION_PREDIAL && m_eDialState <= SESSION_PRE_RETRIEVEPARK);
}

void CDialPlanManager::SetDialNow()
{
    if (m_iDialNowTimer > 0)
    {
        TALK_DBG("SetDialNow timer=[%d]", m_iDialNowTimer);
        timerSetThreadTimer(TIMER_ID(m_iDialNowTimer), m_iDialNowTimer);
    }

    SetAutoDialAndQuitTimer();
}

void CDialPlanManager::SetAutoDial()
{
    /* DialPlan 命中生效的情况下，AutoDial需要被动失效
    * BT Routine不处理AutoDial */

    if (m_iDialNowTimer <= 0 && m_iAutoDialTimer > 0 && !m_strDialText.empty()
            && IsDialRoutine())
    {
        TALK_DBG("SetAutoDial timer=[%d]", m_iAutoDialTimer);
        timerSetThreadTimer(TIMER_ID(m_iAutoDialTimer), m_iAutoDialTimer);
    }
}

void CDialPlanManager::SetAutoQuit()
{
    if (m_iAutoQuitTimer > 0)
    {
        TALK_DBG("SetAutoQuit timer=[%d]", m_iAutoQuitTimer);
        timerSetThreadTimer(TIMER_ID(m_iAutoQuitTimer), m_iAutoQuitTimer);
    }
}

void CDialPlanManager::CancelAll()
{
    CancelDialNow();
    CancelAutoDial();
    CancelAutoQuit();
}

void CDialPlanManager::CancelAutoDial()
{
    if (m_iAutoDialTimer > 0)
    {
        TALK_DBG("CancelAutoDial timer=[%d]", m_iAutoDialTimer);
        timerKillThreadTimer(TIMER_ID(m_iAutoDialTimer));
    }
}

void CDialPlanManager::CancelAutoQuit()
{
    if (m_iAutoQuitTimer > 0)
    {
        TALK_DBG("CancelAutoQuit timer=[%d]", m_iAutoQuitTimer);
        timerKillThreadTimer(TIMER_ID(m_iAutoQuitTimer));
    }
}

void CDialPlanManager::CancelDialNow()
{
    if (m_iDialNowTimer > 0)
    {
        TALK_DBG("CancelDialNow timer=[%d]", m_iDialNowTimer);
        timerKillThreadTimer(TIMER_ID(m_iDialNowTimer));
    }
}

int CDialPlanManager::GetAutoDialTimer()
{
    return configParser_GetConfigInt(kszInterDigitTime) * 1000;
}

int CDialPlanManager::GetAutoQuitTimer()
{
    int iTimer = configParser_GetConfigInt(kszDialToneTimeout);
    /* 为0时是禁用, BT不受此限制 */
    if (iTimer == 0 && !IsBTDialRoutine())
    {
        return 0;
    }
    else if (iTimer <= 0)
    {
        iTimer = 15;
    }

    return iTimer * 1000;
}

#endif // __DIALPLAN_DIALNOW_PROCESS_H__
