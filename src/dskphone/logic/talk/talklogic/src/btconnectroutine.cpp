#include "talklogic_inc.h"

#ifdef IF_BT_SUPPORT_PHONE
CBTConnectingRoutine::CBTConnectingRoutine()
{
    m_iTimerCancelCall = STOP_TIMER_VALUE;
}

CBTConnectingRoutine::~CBTConnectingRoutine()
{
    CancelTimer(m_iTimerCancelCall);
}

// 创建Routine之后的处理
void CBTConnectingRoutine::AfterRoutineCreate()
{
    // 记录原始通话信息
    m_pCallInfo->SetOriginalRemoteInfo();

    //是否直接响彩铃
    if (m_pCallInfo->m_bIsColorRingback)
    {
        StartTalk();
    }

    //TODO 搜索LDAP联系人

    // 呼出无应答时，定时取消呼出
    SetAutoCancelCallTimer();
}

// 销毁Routine之前的处理
void CBTConnectingRoutine::BeforeRoutineDestroy(bool bJumpToNextRoutine /* = true */)
{
    CBaseRoutine::BeforeRoutineDestroy(bJumpToNextRoutine);
}

// Routine的定时器响应
bool CBTConnectingRoutine::OnTimer(UINT uTimerID)
{
    bool bHandled = false;
    if (uTimerID == (UINT)&m_iTimerCancelCall)
    {
        bHandled = true;
        CancelTimer(m_iTimerCancelCall);
    }
    // 不作处理
    return bHandled || CBaseRoutine::OnTimer(uTimerID);
}

// Routine的Mobile消息响应
bool CBTConnectingRoutine::OnSIPMsg(msgObject& msg)
{
    //蓝牙话路不处理SIP消息
    //在此处理蓝牙回调消息
    if (BLUETOOTH_MSG_MOBILE_STATECHANGE != msg.message
            || NULL == msg.GetExtraData())
    {
        return false;
    }

    BTTalkCallUserInfo callInfo;
    _BTMobileManager.MapBTCallUserInfo(callInfo, (const void*)msg.GetExtraData());

    bool bHandle = true;

    switch (callInfo.iCallStatus)
    {
    case BT_MB_CALL_STATUS_RINGBACK:
        {
            //TODO 进入ringback，可以播放彩铃了。
            CancelTimer(m_iTimerCancelCall);

            //进入回铃直接开启通话, 方式默认为彩铃
            m_pCallInfo->m_bIsColorRingback = true;
            StartTalk();
        }
        break;
    case BT_MB_CALL_STATUS_FINISHED:
        {
            _BTMobileManager.ProcessAfterCallFinished(m_pCallSession, (void*)"", false, false);
        }
        break;
    case BT_MB_CALL_STATUS_FAIL:
        {
            _BTMobileManager.ProcessAfterCallFinished(m_pCallSession, (void*)"Call Failed", true, true);
        }
        break;
    case BT_MB_CALL_STATUS_ESTABLISHED:
        {
            Establish(callInfo.iHoldStatus);
        }
        break;
    default:
        bHandle = false;
        break;
    }

    return bHandle;
}

void CBTConnectingRoutine::SetAutoCancelCallTimer()
{
    //手机改成1分钟
    m_iTimerCancelCall = 60 * 1000;
    SetTimerEx(m_iTimerCancelCall);
}

// 发消息通知对方结束通话
void CBTConnectingRoutine::EndCall()
{
    if (NULL != m_pCallInfo
            && m_pCallInfo->GetCallID() != -1)
    {
        Bluetooth_HangupMobile(m_pCallInfo->GetCallID());
    }
}

//处理HOLD消息
void CBTConnectingRoutine::ProcessBTHold(int iHoldState)
{
    //只设置状态
    m_pCallInfo->SetHold(0 != (BT_MB_LOCAL_HOLD & iHoldState));
}

//建立通话
bool CBTConnectingRoutine::Establish(int iHoldState)
{
    //判断是否为HOLD消息
    if (_BTMobileManager.IsHoldMsg(iHoldState))
    {
        //处理HOLD
        ProcessBTHold(iHoldState);
    }

    // 设置通话参数,开始通话
    _BTMobileManager.SetVoiceParam(m_pCallInfo);
    StartTalk();

    // 跳转到通话界面
    m_pCallSession->EnterRoutine(ROUTINE_BTTALKING);
    return true;
}
#endif // IF_BT_SUPPORT_PHONE
