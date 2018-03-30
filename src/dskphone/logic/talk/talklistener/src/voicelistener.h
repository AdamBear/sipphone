#ifndef __VOICE_LISTENER_H__
#define __VOICE_LISTENER_H__

#include "basetalklistener.h"
#include "include/service_ipvp_types.h"

class CVoiceListener : public CBaseTalkListener
{
public:
    CVoiceListener();
    virtual bool OnMessage(msgObject& msg);
    /*处理Routine 创建前部分处理*/
    DECL_SIGNAL_HANDLER(OnSessionPreRoutineChange);
    /*处理Routine 创建后处理*/
    DECL_SIGNAL_HANDLER(OnSessionRoutineChange);
    /*Routine FocusEvent*/
    DECL_SIGNAL_HANDLER(OnSessionFocusChange);
    DECL_SIGNAL_HANDLER(OnSessionPreStartTalk);
    DECL_SIGNAL_HANDLER(OnSessionAftStartTalk);
    /*对应的是RoutinebeforeDestroy*/
    DECL_SIGNAL_HANDLER(OnRoutineDelete);
    DECL_SIGNAL_HANDLER(OnActionEvent);
    DECL_SIGNAL_HANDLER(OnHeldChange);
    DECL_SIGNAL_HANDLER(OnHoldChange);
    /*部分数据改变情况处理，部分原ACTION_EVENT流程*/
    DECL_SIGNAL_HANDLER(OnDataChange);
    /*部分铃声、提示音播放处理*/
    DECL_SIGNAL_HANDLER(OnPlayRing);
    DECL_SIGNAL_HANDLER(OnPlayBusyTone);
    DECL_SIGNAL_HANDLER(OnPlayDialTone);
    DECL_SIGNAL_HANDLER(OnPlayConfTone);
    DECL_SIGNAL_HANDLER(OnSessionAttendedTransferFail);
    /*应答处理*/
    DECL_SIGNAL_HANDLER(OnAnswer);
    DECL_SIGNAL_HANDLER(OnResetTalk);
    /*处理idle 到 话路情况声道切换*/
    DECL_SIGNAL_HANDLER(OnSessionCreate);
    DECL_SIGNAL_HANDLER(OnTextChange);

protected:
    /*相关通话状态 beforeDestroy处理*/
    DECL_SIGNAL_HANDLER(OnDialRoutineDestroy);
    DECL_SIGNAL_HANDLER(OnFinishRoutineDestroy);
    DECL_SIGNAL_HANDLER(OnConnectRoutineDestroy);
    DECL_SIGNAL_HANDLER(OnTalkingRoutineDestroy);
    DECL_SIGNAL_HANDLER(OnRingingRoutineDestroy);
#ifdef IF_FEATURE_BROADSOFT_CALLPARK
    DECL_SIGNAL_HANDLER(OnParkNotifyRoutineDestroy);
#endif
    /*处理话路 到 IDLE 情况下声道切换*/
    DECL_SIGNAL_HANDLER(OnSessionDelete);
    DECL_SIGNAL_HANDLER(OnCallIncome);
    DECL_SIGNAL_HANDLER(OnRoutineChanged);

private:
    bool OnSessionFocusGet(const CSessionProxy& sp);
    bool OnSessionFocusLose(const CSessionProxy& sp);
    void StopCallWaitingTone(bool bIsBtTalk = false);
    void StopRing(CCallInfo& pCallInfo);
    void PlayRing(CCallInfo& pCallInfo);
    bool IsNeedToPlayRing(CCallInfo& callInfo);
#if IF_FEATURE_LOOPBACK
    void HoldLoopBackSession();
#endif
    void SetHoldAlertTimer(CCallInfo& callInfo);
    int StopHoldAlert(CScopedTimer& timer);
    void OnHoldAlertTimer(CCallInfo& callInfo);
    void CancelHoldAlertTimer(CCallInfo& callInfo);
    bool IsPlayHoldAlert(CSessionProxy& sp);
    // 播放回铃音
    void PlayRingbackTone(CCallInfo& callInfo);
    void ProcessRingBackMedia(CCallInfo& callInfo);
    void StopRingbackTone(CCallInfo& callInfo);
    //停止拨号音
    void StopDialTone();
    void PlayDialToneDelay(VoiceInfo& vInfo, int iSessionId);

    /*定时器超时触发*/
    int OnTimerOut(CScopedTimer& timer);
    void SetAutoRedialTimeOut(VoiceInfo& info, int iSessionID, int iTime);

    void GetCallWaitingToneType(CCallInfo& callInfo);
    bool IsNeedPlayCallWaitingTone();
    // 播放通话中新来电提示
    void PlayCallWaitingTone(CCallInfo& callInfo);
    void SetCallWaitingToneTimer(CSessionStateProxy& ssP);
#ifdef IF_BT_SUPPORT_PHONE
    void GetIncomingCallRing(CCallInfo& callInfo);
#endif
    void ProcessMediaSniffer(CCallInfo& callInfo);
    void ProcessRingbackSIPMsg(msgObject& msg);
    void StartTalkByIpvpParam(ipvp_speech_t* pSpeechInfo, CCallInfo* pCallInfo, bool bNeedHold = true);
    bool ProcessIPVPMsg(CCallInfo& callInfo, msgObject& msg);
    void ProcessRingbackRing(CCallInfo& callInfo, PlayRingType ePlayType);
    void SetToneType(CCallInfo& callInfo);
    void SetToneDelay(CCallInfo& callInfo);
    void SetMSGTone(CCallInfo& callInfo, TONE_TYPE eTone = TONE_NONE, int iTimerPlayMsgTone = DEFAULT_TONE_INTERVAL);
    void CancelMSGTone(VoiceInfo& vInfo);
    void PlayDialTone(CCallInfo& callInfo, TONE_TYPE eToneType = TONE_NONE);
    void CancelPlayDialTone(VoiceInfo& vInfo);
    // 判断是否需要播放拨号音
    bool IsNeedToPlayDialTone(CCallInfo& callInfo);
    // 是否响忙音
    bool IsNeedPlayBusyTone(CCallInfo& callInfo);
    void SetIntercomToneTimer(CCallInfo& callInfo);
    void CancelIntercomToneTimer(VoiceInfo& vInfo);
    bool IsForbinPlayRingByOtherSession();

    //sca失败忙音
    void SetBusyToneDelay(CCallInfo& callInfo);
    void CancelBusyTone(CSessionStateProxy& sp, VoiceInfo& vInfo);
    //转移失败提示音
    void AlertForTransFailed(CSessionStateProxy& ssP, bool bCallParkFailed = false);
    void ExitTransFailedAlert(CCallInfo& callInfo);
#ifdef IF_FEATURE_BROADSOFT_CALLPARK
    void SetNotifyAlertTimer(CCallInfo& callInfo);
#endif
};

#endif
