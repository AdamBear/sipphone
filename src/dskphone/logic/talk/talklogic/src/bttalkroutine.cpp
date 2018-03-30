#include "talklogic_inc.h"

#ifdef IF_BT_SUPPORT_PHONE
CBTTalkingRoutine::CBTTalkingRoutine()
{
    m_iTimerChannelLinkDelay = STOP_TIMER_VALUE;
}

CBTTalkingRoutine::~CBTTalkingRoutine()
{
}

// 创建Routine之后的处理
void CBTTalkingRoutine::AfterRoutineCreate()
{
    // 设置通话状态
    m_pCallInfo->CheckHoldStatus();

    //记录原始通话信息
    m_pCallInfo->SetOriginalRemoteInfo();
}

// 销毁Routine之前的处理
void CBTTalkingRoutine::BeforeRoutineDestroy(bool bJumpToNextRoutine /* = true */)
{
    CBaseRoutine::BeforeRoutineDestroy(bJumpToNextRoutine);
    if (NULL != m_pCallInfo)
    {
        if (m_pCallInfo->m_eTalkState == TS_TALK
                || m_pCallInfo->m_eTalkState == TS_HELD)
        {
            // 停止通话
            StopTalk();
        }
    }
}

// Routine的Mobile消息响应
bool CBTTalkingRoutine::OnSIPMsg(msgObject& msg)
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
    case BT_MB_CALL_STATUS_ESTABLISHED:
        {
            bHandle = Establish(callInfo.iHoldStatus);
        }
        break;
    case BT_MB_CALL_STATUS_FAIL:
    case BT_MB_CALL_STATUS_FINISHED:
        {
            //先停止通话，在处理完成
            if (m_pCallInfo->IsTalkStart())
            {
                StopTalk();
            }
            _BTMobileManager.ProcessAfterCallFinished(m_pCallSession, (void*)"", false, false);
        }
        break;
    default:
        bHandle = false;
        break;
    }

    return bHandle;
}

// Routine的定时器响应
bool CBTTalkingRoutine::OnTimer(UINT uTimerID)
{
    bool bHandled = false;
    if (uTimerID == (UINT)&m_iTimerHoldDelay)
    {
        bHandled = true;

        CancelTimer(m_iTimerHoldDelay);
    }
    else if (uTimerID == (UINT)& m_iTimerChannelLinkDelay)
    {
        bHandled = true;
        CancelTimer(m_iTimerChannelLinkDelay);
    }

    return bHandled || CBaseRoutine::OnTimer(uTimerID);
}

// 处理ui发送过来的Action事件
bool CBTTalkingRoutine::OnActionEvent(DataUI2Logic* pActionData)
{
    if (pActionData == NULL)
    {
        return false;
    }

    bool bHandled = true;
    switch (pActionData->eAction)
    {
    case TALK_ACTION_TO_SIP:
        {
            OnChannelLinkChange(true);
        }
        break;
    case TALK_ACTION_TO_MOBILE:
        {
            OnChannelLinkChange(false);
        }
        break;
    default:
        bHandled = false;
        break;
    }

    return bHandled;
}

// hold/unhold当前话路
bool CBTTalkingRoutine::HoldRoutine(bool bHold, HOLD_TYPE eHoldType /* = HT_NORMAL */,
                                    bool bHoldByHost /* = false */)
{
    if ((m_pCallInfo->IsLocalHold() && bHold)
            || (!m_pCallInfo->IsLocalHold() && !bHold)
       )
    {
        TALK_WARN("[TALK] Current Mobile Routine is already hold or unhold!");
        return false;
    }

    // 只对unhold防抖，hold操作有可能是被动的（比如新起一路，hold其他路）
    // 会议中由confsession自己做防抖
    if (!m_pCallSession->InLocalConf()
            && m_pCallInfo->IsLocalHold())
    {
        // 如果有设置了Hold操作定时器,则下一次的hold不再处理
        if (m_iTimerHoldDelay != STOP_TIMER_VALUE)
        {
            return false;
        }
        else
        {
            m_iTimerHoldDelay = HOLD_DELAY_TIME;
            SetTimerEx(m_iTimerHoldDelay);
        }
    }

    //手机端主动发起的不再重复调用接口
    if (HT_BTPHONE != eHoldType)
    {
        Bluetooth_HoldMobile(m_pCallInfo->GetCallID(), bHold);
    }

    // 设置本地hold的状态
    m_pCallInfo->SetHold(bHold);

    if (m_pCallInfo->IsLocalHold())
    {
        // hold
        _VoiceInterface.HoldSession(m_pCallInfo->GetCallID());
        m_pCallInfo->SetTalkStart(false);
    }
    else
    {
        _VoiceInterface.ResumeSession(m_pCallInfo->GetCallID());
        if (!m_pCallInfo->IsRemoteHold())
        {
            m_pCallInfo->SetTalkStart(true);
        }
    }

    return true;
}

//切换声音通道
bool CBTTalkingRoutine::OnChannelLinkChange(bool bLinked)
{
    //防抖处理
    if (m_iTimerChannelLinkDelay != STOP_TIMER_VALUE)
    {
        return false;
    }

    //与HOLD防抖使用相同的时间
    m_iTimerChannelLinkDelay = HOLD_DELAY_TIME;
    SetTimerEx(m_iTimerChannelLinkDelay);

    //切换声音通道接口
    return Bluetooth_SwitchChannel(bLinked, m_pCallInfo->GetCallID());
}

//处理HOLD消息
void CBTTalkingRoutine::ProcessBTHold(int iHoldState)
{
    //不同步时，再去执行一次HOLD
    BOOL bLocalHold = (0 != (BT_MB_LOCAL_HOLD & iHoldState));

    if (!m_pCallInfo->IsLocalHold()
            && bLocalHold)
    {
        HoldRoutine(bLocalHold, HT_BTPHONE, true);
    }
}

//
bool CBTTalkingRoutine::Establish(int iHoldState)
{
    //处理HOLD状态
    if (_BTMobileManager.IsHoldMsg(iHoldState))
    {
        ProcessBTHold(iHoldState);
        return true;
    }

    //解HOLD
    bool bLocalResume = (0 == (BT_MB_LOCAL_HOLD & iHoldState));
    if (m_pCallInfo->IsRemoteHold()
            && bLocalResume)
    {
        m_pCallInfo->SetHold(false);
        m_pCallInfo->SetTalkStart(true);
        _VoiceInterface.ResumeSession(m_pCallInfo->GetCallID());
    }

    return true;
}

//结速通话
void CBTTalkingRoutine::EndCall()
{
    if (NULL != m_pCallInfo)
    {
        Bluetooth_HangupMobile(m_pCallInfo->GetCallID());
    }
}

#endif // IF_BT_SUPPORT_PHONE
