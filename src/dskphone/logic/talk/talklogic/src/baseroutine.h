#ifndef __BASE_ROUTINE_H__
#define __BASE_ROUTINE_H__

#include "service_ipvp_types.h"
#include "talk/talklogic/include/callinfo.h"
#include "talk/talkproxy/include/deletelater.h"

class CBaseRoutine;
class CSingleSession;
struct TalkErrorInfo;
struct BasicUserInfo;

// 根据Routine类型获取Routine指针
extern CBaseRoutine* GetRoutineByState(ROUTINE_STATE eState);

#include "voice/include/voicecommon.h"

// CBaseRoutine:Routine的基类，表示一个话路过程
class CBaseRoutine
    : public app::CBaseDeleteLater
{
public:
    CBaseRoutine();

    virtual ~CBaseRoutine();

public:
    // 状态
    virtual ROUTINE_STATE GetRoutineState() const = 0;

    // 名称
    virtual LPCSTR GetName() const = 0;

    // 继承关系
    bool IsInherits(ROUTINE_STATE eState) const;

    // 创建Routine之后的处理
    virtual void AfterRoutineCreate()
    {}
    // 销毁Routine之前的处理(bJumpToNextRoutine用来区别是跳转routine还是退出Session时调用)
    virtual void BeforeRoutineDestroy(bool bJumpToNextRoutine = true)
    {}

    // 发消息通知对方结束通话
    virtual void EndCall();
    // Routine的定时器响应
    virtual bool OnTimer(UINT uTimerID)
    {
        return false;
    }

    // Routine的SIP消息响应
    virtual bool OnSIPMsg(msgObject& msg)
    {
        return false;
    }

    // 处理ui发送过来的Action事件
    virtual bool OnActionEvent(DataUI2Logic* pActionData)
    {
        return false;
    }

    // 获取对应session的状态
    virtual SESSION_STATE GetHostSessionState() = 0;

    // 是否允许被加入会议
    virtual bool IsAllowAddToConf()
    {
        return false;
    }

    // 退出前停止通话相关
    void StopTalkBeforeDelete();

    // 获取该Routine对应的Session
    inline CSingleSession* GetSession() const;
    // 设置该Routine对应的Session
    void SetSession(CSingleSession* pSession, CCallInfo* pCallInfo);
    // 获取该Routine对应的通话信息
    inline CCallInfo* GetCallInfo() const;
    // 配置绑定的账号
    inline int GetBindAccount() const;

    // 开启通话
    virtual void StartTalk(bool bNeedHold = true);
    // 开启通话
    void StartTalk(int iCallId);
    // 停止通话
    virtual void StopTalk();
    // 停止通话
    void StopTalk(int iCallID);

    // 进入呼叫完成状态的处理
    void ProcessAfterCallFinished(const TalkErrorInfo& errorInfo, bool bCallFail = true,
                                  bool bPlayBusyTone = true, bool bEndCall = true);

    // 处理通话中来电
    virtual bool ProcessIncomingCall(bool bAnswer, int iCode)
    {
        return false;
    }

    virtual bool HoldRoutine(bool bHold, HOLD_TYPE eHoldType = HT_NORMAL, bool bHoldByHost = false)
    {
        return false;
    }

    virtual bool Redirectcall(LPCSTR lpTarget)
    {
        return false;
    }

    virtual bool IsAllowSendDTMF()
    {
        return false;
    }

    // 呼出当前已拨号码
    virtual bool CallOut(const yl::string& strNumber, yl::string strName = "",
                         int iCallOutTriggerMask = COT_DEFAULT,
                         bool bAudioOnly = true)
    {
        return false;
    }

    // 转接实际操作
    virtual bool Transfer(CSingleSession* pSession)
    {
        return false;
    }

    virtual bool AddToConf(CSingleSession* pSession)
    {
        return false;
    }

protected:
    // 根据SIP消息的参数开启通话
    virtual void StartTalkByIpvpParam(ipvp_speech_t* pSpeechInfo, CCallInfo* pCallInfo,
                                      bool bNeedHold = true);
    // 通话被转移
    void BeTransferred(msgObject& msg);

protected:

    // 该Routine所属的Session
    CSingleSession* m_pCallSession;
    // 该Routine对应的通话信息
    CCallInfo*      m_pCallInfo;
    int             m_iMadiaSniffer;
};

// 获取该Routine对应的Session
inline CSingleSession* CBaseRoutine::GetSession() const
{
    return m_pCallSession;
}

// 设置该Routine对应的Session

// 获取该Routine对应的通话信息
inline CCallInfo* CBaseRoutine::GetCallInfo() const
{
    return m_pCallInfo;
}

// 配置绑定的账号
inline int CBaseRoutine::GetBindAccount() const
{
    if (NULL != m_pCallInfo)
    {
        return m_pCallInfo->GetAccountID();
    }
    return 0;
}


#endif //__BASE_ROUTINE_H__
