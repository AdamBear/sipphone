#pragma once

#define INVALID_HANDLE  -1

class CBaseSession;
class CSingleSession;
class CCallInfo;

#include "talk/talkproxy/include/helpclass.h"
#include "uiandlogic_common.h"

/* Session代理，辅助查询通话状态，将通话Sesssion、Routine两级结构封装为一级结构 */
class CSessionProxy
    : protected app::StackObjectBase
{
public:
    // no explicit
    CSessionProxy(int iSessionID);

    CSessionProxy(CBaseSession* p = NULL);

    CSessionProxy(CCallInfo* p);

    ~CSessionProxy();

    LPCSTR Name() const;

    CBaseSession* Get() const
    {
        return m_pSession;
    }

public:
    // sesssion类型
    bool IsSingle() const;

    bool IsLocalConf() const;

    bool IsBeTransfer() const;

    bool IsInConf() const;

    // 具体业务
    bool IsHide() const;

    bool IsBla() const;

    bool IsDraftBla() const;

    bool IsSCA() const;

    bool IsBtSession() const;

    CCallInfo* GetCallInfo() const;

    int GetAccountID() const;

    bool IsBTTalk() const;

    int GetCallID() const;

public:
    // 状态
    bool IsHold() const;

    bool IsHeld() const;

    bool IsInHoldOrHeld() const;

    bool IsAllHeld() const;

    // 通话中（非Hold）
    bool IsInTalking() const;

    // 通话状态（hold等）
    bool IsTalking() const;

    // basetalkroutine
    bool IsTalkSession() const;

    bool IsRinging() const;

    bool IsDialing() const;

    bool IsConecting() const;

    bool IsFinished() const;

    bool IsInherits(ROUTINE_STATE eRoutine) const;

    bool IsInherits(SESSION_STATE eSession) const;

    bool BeMulticastPage() const;

    bool IsInPreTrans() const;

    bool IsInPreConf() const;

    bool IsAllOtherSessionHold() const;

    bool IsDestructing() const;

    SESSION_STATE GetSessionState() const;

    ROUTINE_STATE GetRoutineState() const;

    ROUTINE_STATE GetInheritsRoutine() const;

    TALK_STATE GetTalkState() const;

    TALK_CALLOUT_TYPE GetCallOutType() const;

    CALL_TYPE GetCallType() const;

public:
    void HoldSession(bool bHold, HOLD_TYPE eHoldType = HT_NORMAL) const;

public:
    // 获取
    CSessionProxy GetTopestHost() const;

    CSessionProxy GetHost() const;

    CSessionProxy GetThisOrTopestHost() const;

    CSessionProxy GetSponsor() const;

    CSessionProxy GetConfSponsor() const;

    CSessionProxy GetTranSponsor() const;

    CSessionProxy GetSubByIndex(int index) const;

    CSessionProxy GetAnOther() const;

    CSessionProxy GetAnOtherSubSession(const CSessionProxy& other) const;

    CSessionProxy GetIncomeSession() const;

    int GetSubSize() const;

    bool GetSubSession(YLVector<CSessionProxy>& vec) const;

    void SetConfSponsor(const CSessionProxy& proxy = CSessionProxy()) const;
    void SetTranSponsor(const CSessionProxy& proxy = CSessionProxy()) const;

    bool CallOut(const yl::string& strNumber, const CallOut_Param& callParam) const;

    bool CallOut() const;

    bool Forward(const yl::string& strNumber) const;

    bool BlindTransfer(const yl::string& strNumber) const;

    bool Transfer(const CSessionProxy& to) const;

    bool Answer() const;

    bool Refuse() const;

    bool StartTalk() const;

    bool StopTalk() const;

    bool SendDTMF(const yl::string& strDTMF) const;

    bool EstablishLocalConf() const;

public:

    bool ExitSession() const;

    bool TransferForCallpark(yl::string& strNumber) const;

    void SetHide(bool bHide) const;

    bool TranCurrentSession();

    bool IsConfManager() const;

    void SetConfManager(bool bConf);

public:
    // 转换函数
    operator int() const;

    operator bool() const
    {
        return m_pSession;
    }

    bool operator !() const
    {
        return !operator bool();
    }

    bool operator==(const CSessionProxy& other) const
    {
        return this == &other || m_pSession == other.m_pSession;
    }

    bool operator==(int id) const
    {
        return id == operator int();
    }

    bool operator!=(const CSessionProxy& other) const
    {
        return !(operator==(other));
    }

    friend bool operator==(int i, const CSessionProxy& other);

protected:
    CSingleSession* ToSingleSession() const;

    CBaseSession*  m_pSession;
};


