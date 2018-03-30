#pragma once

#include "yllist.h"
#include "talk/talkproxy/include/helpclass.h"

class CTalkAdaptManager
{
public:
    typedef YLList<int>       LIST_SESSION;
    typedef YLVector<int>     VEC_SESSION;

    CTalkAdaptManager();

    static CTalkAdaptManager& GetInstance() DECLARE_ATTR
    {
        static CTalkAdaptManager s_Instance;
        return s_Instance;
    }

    // 处理ui的Action事件, 返回值true表示不再继续往下传
    bool ProcessAction(int iSessionID, DataUI2Logic& ActionData);

    bool ProcessAction(CSessionStateProxy& proxy, DataUI2Logic& ActionData);

    bool OnMessage(msgObject& msg);

public:
    bool ProcessBeforeSwitchSession();

    int GetSessionListSize();

    // 获取焦点Session Index
    int GetFocusedSessionIndex() const
    {
        return m_iFocusedSessionIndex;
    }

    // 获取焦点Session
    int GetFocusedSessionID() const
    {
        return m_iFocusedSessionID;
    }

    // 设置焦点session，参数为Session指针
    bool SetFocusSession(int iSessionID, bool bManualTrigger = false);
    // 删除Session后重置Session焦点
    void ResetFocusedSession(const CSessionProxy& except = CSessionProxy());
    // 获取下一路即将设置为焦点的话路
    CSessionProxy GetNextSession(const CSessionProxy& except = CSessionProxy());
    // 获取第一路非隐藏话路
    CSessionProxy GetFirstUnhideSession();

    bool IsEmpty() const
    {
        return m_listSession.begin() == m_listSession.end();
    }

public:
    // 根据账号和BLAindex找到对应的Session
    CSessionProxy GetSessionByBLAIndex(int iAccountID, int iIndex);

    bool IsBlaIndexUsing(int iIndex);

public:
    // 新增Session
    void OnSessionCreate(int iSessionID);
    // 退出清理操作
    void OnSessionDestory(int iSessionID);
    // 恢复焦点
    void ResetFocusedByLastSession(int iSessionID);

private:
    static bool IsAllowToSetAsFocused(const CSessionStateProxy& proxy);

    static bool CanBeSetAsNoFocused(const CSessionStateProxy& proxy);

private:
    // Focus Stack interface
    /* 以栈的形式组织焦点 */
    void Push(int iSessionID);

    int Pop();

    void Remove(int iSessionID);

    // session数量改变需要重新计算index
    int OnSessionSizeChanged();

private:
    LIST_SESSION            m_listSession;          // Sesssion列表
    LIST_SESSION            m_listStackFocus;       // 焦点栈
    int                     m_iFocusedSessionID;    // 当前UI的焦点
    int                     m_iFocusedSessionIndex; // 焦点序号
};

#define _TalkAdapter    GET_SINGLETON_INSTANCE(CTalkAdaptManager)

