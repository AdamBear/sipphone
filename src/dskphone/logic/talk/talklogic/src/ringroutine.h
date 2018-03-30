#ifndef __RING_ROUTINE_H__
#define __RING_ROUTINE_H__

#include "talk/fwd_dnd/include/modfwddnd.h"

#define BUG_21442_9744 1

// 来电响铃过程
class CRingingRoutine : public CBaseRingRoutine
{
public:
    CRingingRoutine();
    virtual ~CRingingRoutine();

    // 创建Routine之后的处理
    virtual void AfterRoutineCreate();
    // Routine的定时器响应
    virtual bool OnTimer(UINT uTimerID);
    // Routine的SIP消息响应
    virtual bool OnSIPMsg(msgObject& msg);
    // 处理ui发送过来的Action事件
    virtual bool OnActionEvent(DataUI2Logic* pActionData);
    // 获取对应session的状态
    virtual SESSION_STATE GetHostSessionState()
    {
        return SESSION_RINGING;
    }
    // 发消息通知对方结束通话
    virtual void EndCall();
    // 拒绝新来电
    // | bSaveMisscall | [in] 是否保存呼叫记录。T2X中在自动拒绝时有此区分
    virtual void RefuseIncomingCall(int iRefuseType = RT_NORMAL);
    // 接起新来电，是否需要调整通道
    virtual bool AnswerIncomingCall(bool bSwitchChannel = true, bool bAudioOnly = true);
    // forward新来电
    bool ForwardIncomingCall(LPCSTR lpszNumber = NULL, FWD_TYPE eFwdType = FWD_TYPE_MANUAL);
protected:
    // 进入通话
    void Established(msgObject& msg);
#if IF_BUG_20643
    // 进入通话
    void EstablishTalk();
#endif  //IF_BUG_20643

    virtual bool Redirectcall(LPCSTR lpTarget);

    virtual bool IsAllowAddToConf();

    //初始化相关变量标记
    virtual void InitStateFlagData()
    {
        CBaseRingRoutine::InitStateFlagData();
        m_pCallInfo->SetFWD(false);
        m_pCallInfo->m_bAutoAnswerNow = true;
        m_pCallInfo->m_bPassiveRingMute = false;
    }
protected:
    // http://192.168.1.99/Bug.php?BugID=17900
    bool         m_bAutoANSEnable;  // 是否自动接，主要用于Intercom

#if BUG_21442_9744
    int          m_iDelayToEnterInTalk;
#endif

#if IF_BUG_20643
    bool            m_bFirstEstablishMsg;//是否已经收到一次Established消息
    int             m_iEstablished;  // Established 定时器
#endif  //IF_BUG_20643
};

#endif //__RING_ROUTINE_H__
