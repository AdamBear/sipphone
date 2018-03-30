#ifndef __TALK_ROUTINE_H__
#define __TALK_ROUTINE_H__

#include "lamp/ledlamp/include/modledlamp.h"
#include "media/commondefine.h"
#include "basetalkroutine.h"

class CVoiceInterface;

// 通话中过程
class CTalkRoutine : public CBaseTalkRoutine
{
public:
    CTalkRoutine();
    virtual ~CTalkRoutine();

    // 创建Routine之后的处理
    virtual void AfterRoutineCreate();
    // 销毁Routine之前的处理
    virtual void BeforeRoutineDestroy(bool bJumpToNextRoutine = true);
    // Routine的定时器响应
    virtual bool OnTimer(UINT uTimerID) DECLARE_ATTR;
    // Routine的SIP消息响应
    virtual bool OnSIPMsg(msgObject& msg);
    // 处理ui发送过来的Action事件
    virtual bool OnActionEvent(DataUI2Logic* pActionData);
    // 获取对应session的状态
    virtual SESSION_STATE GetHostSessionState();

    virtual bool HoldRoutine(bool bHold, HOLD_TYPE eHoldType = HT_NORMAL, bool bHoldByHost = false);

    // 盲转
    void BlindTransfer(const yl::string& strNumber);
    // 半咨询转，如果不允许半咨询转，则用盲转代替
    void SemiAttendedTransfer(CSingleSession* pSessionToTran);
    // 咨询转
    void AttendedTransfer(CSingleSession* pSessionToTran);
    // 转接实际操作
    virtual bool Transfer(CSingleSession* pSessionToTran);
    // Transfer to选择的session
    BOOL TransferToSession(CBaseSession* pSessionToTran);
    // 建立三方网络会议或者把一路通话加入到网络会议
    void AddNetworkConf(CSingleSession* pSessionAddToConf);
    // 当有两路通话时，建立会议，如果只有1路通话，则进入会议拨号界面
    void EstablishConf();

    // 建立网络会议
    void EstablishNetworkConf(CSingleSession* pSessionAddToConf);

    // 处理等待的操作
    bool ProcessDelayOperate();
protected:

    // hold/unhold当前话路
    virtual bool SetCallHold(bool bHold, bool bStatusOnly);

    virtual bool IsAllowAddToConf();
protected:
    // 当进入tran拨号界面，或者tran当前两路
    void EstablishTran();
    // 建立新的拨号话路
    void EstablishNewDial(bool bConfTo = false, LPCSTR lpszDialNumber = NULL);

    // 重新开启通话by sip msg
    bool ReStartTalk(ipvp_speech_t* pVoiceParam);

    // 直接TRANSFER
    bool TransferSessionDirectly();
#if IF_FEATURE_PAGING
    // Hold Paging
    bool HoldPagingTalk(bool bHold);
#endif
    // 盲转失败进入recall
    void EnterRecall();

    // 处理hold
    bool ProcessSIPHold(msgObject& msg);

    // 是否支持直接转接
    bool IsTransferDirectly();

    // 判断是否是talkroutine,包含外接设备话路
    bool IsTalkRoutine(ROUTINE_STATE eState);

    // 处理通话建立消息
    bool ProcessEstablish(msgObject& msg);
    // 合并所有允许建立网络会议话路
    bool MergeAllowJoinNetConfRoutine();

    virtual bool Redirectcall(LPCSTR lpTarget);

    virtual bool IsAllowSendDTMF()
    {
        return true;
    }

protected:
    // Tran之后隐藏Session的超时处理
    int         m_iWaitTranResultTimeout;
    // 重协商延时关闭会议
    int      m_iTimerRenego;
#if IF_BUG_30423
    yl::string m_strOldSIPName;  //当前通话对方号码
#endif

    bool         m_bReplacedInSub;

    int        m_iRespondTimer;
    int        m_iRequestFailedTimer;
    bool       m_bRequestVideo;

    yl::string  m_strTransferNum;
    TALK_DELAY_OPERATE  m_eDelayOperate;
    bool        m_bWaitForHold;
};

#endif //__TALK_ROUTINE_H__
