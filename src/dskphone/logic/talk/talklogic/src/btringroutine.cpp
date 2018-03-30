#include "talklogic_inc.h"

#ifdef IF_BT_SUPPORT_PHONE
CBTRingingRoutine::CBTRingingRoutine()
{
}

CBTRingingRoutine::~CBTRingingRoutine()
{

}

// Routine的Mobile消息响应
bool CBTRingingRoutine::OnSIPMsg(msgObject& msg)
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
            Established(callInfo.iHoldStatus);
        }
        break;
    case BT_MB_CALL_STATUS_FAIL:
        {
            _BTMobileManager.ProcessAfterCallFinished(m_pCallSession, (void*)"", true, false);
        }
        break;
    case BT_MB_CALL_STATUS_FINISHED:
        {
            m_pCallInfo->m_eCallType = CT_MISSCALL;
            // 来电响铃时挂断，则退出该Session
            m_pCallSession->ExitSession();
        }
        break;
    default:
        bHandle = false;
        break;
    }

    return bHandle;
}

// 处理ui发送过来的Action事件
bool CBTRingingRoutine::OnActionEvent(DataUI2Logic* pActionData)
{
    if (pActionData == NULL)
    {
        return false;
    }

    bool bHandled = true;
    switch (pActionData->eAction)
    {
    case TALK_ACTION_CANCEL:
    case TALK_ACTION_REJECT:
        {
            RefuseIncomingCall();
            bHandled = true;
        }
        break;
    case TALK_ACTION_ANSWER:
        {
            // 接听来电不刷新界面,Recall要刷新
            AnswerIncomingCall();
        }
        break;
    default:
        bHandled = CBaseRingRoutine::OnActionEvent(pActionData);
        break;
    }

    return bHandled;
}

void CBTRingingRoutine::EndCall()
{
    if (m_pCallInfo->m_bAnswered)
    {
        Bluetooth_HangupMobile(m_pCallInfo->GetCallID());
    }

    Bluetooth_RefuseMobile(m_pCallInfo->GetCallID());
}

// 拒绝新来电
void CBTRingingRoutine::RefuseIncomingCall(int iRefuseType/* = RT_NORMAL*/)
{
    if (m_pCallInfo->m_bAnswered)
    {
        m_pCallSession->ExitSession(true);
        return;
    }

    Bluetooth_RefuseMobile(m_pCallInfo->GetCallID());

    // 如果已经应答了再拒绝，则需要EndCall
    // 退出当前routine， 当拒接时，不用再发Hangup了
    m_pCallSession->ExitSession(false);
}

// 接起新来电，是否需要调整通道
bool CBTRingingRoutine::AnswerIncomingCall(bool bSwitchChannel /* = true */, bool bAudioOnly/* = true*/)
{
    // 如果已经应答,则不再处理
    if (m_pCallInfo->m_bAnswered)
    {
        return false;
    }
    if (Bluetooth_IsMobileConnected())
    {
        m_pCallInfo->m_bAnswered = true;
        // 发送接听通话的消息后等待通话建立的消息
        Bluetooth_AnswerMobile(m_pCallInfo->GetCallID());
    }

    return false;
}

//通话建立
bool CBTRingingRoutine::Established(int iHoldState)
{
    //TODO 判断是否为HOLD消息
    if (_BTMobileManager.IsHoldMsg(iHoldState))
    {
        return ProcessBTHold(iHoldState);
    }

    m_pCallInfo->SetHeld(false);

    //设置通话参数
    _BTMobileManager.SetVoiceParam(m_pCallInfo);

    StartTalk();

    //手机端接起时，将声音通道切换回手机
    if (!m_pCallInfo->m_bAnswered)
    {
        m_pCallInfo->m_bAnswered = true;
        if (_BTMobileManager.IsChannelLinked())
        {
            Bluetooth_SwitchChannel(false, m_pCallInfo->GetCallID());
        }
    }

    // 跳转到通话界面
    m_pCallSession->EnterRoutine(ROUTINE_BTTALKING);
    return true;
}

//处理HOLD消息
bool CBTRingingRoutine::ProcessBTHold(int iHoldState)
{
    m_pCallInfo->SetHeld(0 != (BT_MB_REMOTE_HOLD & iHoldState));
    return true;
}
#endif // IF_BT_SUPPORT_PHONE
