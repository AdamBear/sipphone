#ifndef __BASE_SESSION_H__
#define __BASE_SESSION_H__

#include "ylstring.h"
#include "talk/talkproxy/include/deletelater.h"
#include "talk/talklogic/include/uiandlogic_common.h"

class CCallInfo;
class CSingleSession;
class CBaseRoutine;
class CLocalConfSession;
class CBeTransferredSession;

// Session的基类
class CBaseSession
    : public app::CBaseDeleteLater
{
public:
    explicit CBaseSession(LPCSTR name = NULL);

    virtual ~CBaseSession();

public:

    // 只有两路Session时获取另外一路
    CBaseSession* GetAnotherSession() const;

    // 多路通话中，找到另外一路
    CBaseSession* GetAnotherSubSession(CBaseSession* pSubSession) const;

    // 子话路个数
    int GetSubSessionSize() const;

    // 子话路个数
    bool GetSubSessionList(YLVector<CBaseSession*>& vec) const;

    // 通过序号获取子话路
    CBaseSession* GetSubSessionByIndex(int index) const;

    // 找到会议第一路
    CBaseSession* GetFirstSub() const
    {
        return GetSubSessionByIndex(0);
    }

    // 取消Host
    void FwdHostSession(CBaseSession* p = NULL);

    // 作为SubSession时传给Host的CallInfo(如会议中的BeTran)
    CCallInfo* GetCallInfoAsSub();

    /* 是否是本地会议 */
    bool IsLocalConfSession() const
    {
        return GetSessionState() == SESSION_LOCALCONF;
    }

    /* 是否被转接 */
    bool IsBeTransferSession() const
    {
        return GetSessionState() == SESSION_BETRANSFERRED;
    }

    /* 单路通话 */
    bool IsSingleSession() const
    {
        SESSION_STATE e = GetSessionState();
        return e != SESSION_LOCALCONF && e != SESSION_BETRANSFERRED;
    }

    CSingleSession* ToSingleSession() const;

    CBeTransferredSession* ToBeTransferredSession() const;

    CLocalConfSession* ToLocalConfSession() const;

    /* 本地会议的sub */
    bool IsInLocalConf() const
    {
        return m_pHostSession != NULL && m_pHostSession->IsLocalConfSession();
    }

    /* 本地会议的子孙sub */
    bool IsTopestInLocalConf() const
    {
        CBaseSession* pTopest = GetTopestHostSession();
        return pTopest != NULL && pTopest->IsLocalConfSession();
    }

    /* 被转的sub */
    bool IsInBeTrans() const
    {
        return m_pHostSession != NULL && m_pHostSession->IsBeTransferSession();
    }

    bool IsInherits(ROUTINE_STATE eState);

    ROUTINE_STATE GetRoutineState();

public:
    virtual bool SendDTMF(const yl::string& strDTMF, int interval = 0);

    // 单路通话
    virtual CSingleSession* GetSession() = 0;

    // 根据Callid获取CallSession
    virtual CBaseSession* GetSessionByCallID(int iCallID);

    // 设置子Sesssion发起关系
    virtual void ResetSubSessonSponsor(CBaseSession* pSponsor)
    {}

    // 获取对应的Routine
    virtual CBaseRoutine* GetRoutine() const
    {
        return NULL;
    }

    virtual LPCSTR GetName()
    {
        return m_szName;
    }

    // Session创建之后的处理
    virtual void AfterSessionCreate();

public:

    // 获取SessionID
    inline int GetSessionID() const;
    // 获取隐藏状态
    inline bool IsHide();
    // 设置隐藏状态
    void SetHide(bool bHide = true);
    // 获取该session的最顶层父session
    CBaseSession* GetTopestHostSession() const;
    // 获取该session的最顶层父session
    CBaseSession* GetThisOrTopestHostSession() const;

    // 处理通话中来电
    bool ProcessIncomingCall(bool bAnswer, int iCode = 0);

    // 获取HostSession
    inline CBaseSession* GetHostSession() const;
    // 设置HostSession
    void SetHostSession(CBaseSession* pSession);

    // Hold、Unhold Session
    void HoldSession(bool bHold = true, HOLD_TYPE eHoldType = HT_NORMAL, bool bHoldByHost = false);

    // 基于当前创建新拨号
    CSingleSession* CreateNewDial(ROUTINE_STATE eRoutine);

public:
    // 处理ui发送过来的Action事件
    virtual bool OnActionEvent(DataUI2Logic* pActionData);
    // 退出当前Session
    virtual void ExitSession(bool bEndCall = false);
    // 定时器处理
    virtual bool OnTimer(UINT uTimerID);
    // SIP消息响应
    virtual bool OnSIPMsg(msgObject& msg) = 0;
    // SubSession退出时的处理
    virtual void OnSubSessionExit(CBaseSession* pSubSession) = 0;
    // SubSession状态改变的处理（如：会议中的某一路收到CallReplace消息）
    virtual void OnSubSessionChanged(CBaseSession* pSubSession);

    // session del之前的操作(析构函数之前)
    virtual void BeforeDelete();
    // Hold、Unhold Session
    virtual void SetHold(bool bHold = true, HOLD_TYPE eHoldType = HT_NORMAL,
                         bool bHoldByHost = false) = 0;

    // 判断是否通话session(conf/tran/talk等)
    virtual bool IsTalkSession()
    {
        return true;
    }

    // 判断session是否正在通话中
    virtual bool IsInTalking() = 0;
    // hold状态
    virtual bool IsInHold();
    // Held状态
    virtual bool IsInHeld();

    // 判断session是否全部都处于Held状态
    virtual bool IsAllInHeld()
    {
        return false;
    }

    // 获取Session状态
    virtual SESSION_STATE GetSessionState() const = 0;

    // 是否允许被加入会议
    virtual bool IsAllowAddToConf() = 0;

    // 某个话路远端静音
    virtual bool FarMute(int iCallID, bool bFarMute) = 0;

    // 退出Session，需要删除所有SubSession
    virtual void ExitSessionAndSub(bool bEndCall = true);
    void ExitSubSession(bool bEndCall = true);

    // 返回当前session是否处于Hold/held状态
    virtual bool IsInHoldOrHeld();

    virtual bool SetFarHold(int iCallID, bool bFarHold) = 0;

    virtual void SetConfManager(bool bConf);

    virtual bool IsConfManager();

    bool IsBtSession();

    void SetDestory(bool bDestory = true);

public:
    // 外部应用次数，用于日志记录
    bool     m_bDestory;

protected:

    // SessionID，Session的唯一标识
    int      m_iSessionID;
    // 该Session是否隐藏，如果隐藏则不计入话路总数(应用于网络会议呼出)
    bool     m_bHide;
    // 该Session是m_pHostSession的SubSession
    CBaseSession* m_pHostSession;
    // session显示名，用于日志记录
    LPCSTR   m_szName;
};

// 获取SessionID
inline int CBaseSession::GetSessionID() const
{
    return m_iSessionID;
}

// 获取隐藏状态
inline bool CBaseSession::IsHide()
{
    return m_bHide;
}

// 获取HostSession
inline CBaseSession* CBaseSession::GetHostSession() const
{
    return m_pHostSession;
}

#endif //__BASE_SESSION_H__
