#include "talklogic_inc.h"

#ifdef IF_SUPPORT_LINE_DEVICE

CLineDeviceTalkRoutine::CLineDeviceTalkRoutine()
{
}

CLineDeviceTalkRoutine::~CLineDeviceTalkRoutine()
{
}

// 创建Routine之后的处理
void CLineDeviceTalkRoutine::AfterRoutineCreate()
{
    // 设置通话状态
    m_pCallInfo->CheckHoldStatus();

    // 开启通话
    StartTalk();

    m_pCallInfo->m_tRemoteInfo.sDisplayName = _LANG(TRID_PC_MOBILE_MODE);
}

// 处理ui发送过来的Action事件
bool CLineDeviceTalkRoutine::OnActionEvent(DataUI2Logic* pActionData)
{
    bool bHandled = true;
    switch (pActionData->eAction)
    {
    case TALK_ACTION_TRAN:
    case TALK_ACTION_CANCEL:
    case TALK_ACTION_HOLD_PRIVATE:
    case TALK_ACTION_PARK:
    case TALK_ACTION_GROUP_PARK:
        break;
    case TALK_ACTION_CONF:
        {
            // 添加处理, 重新选取账号
            // http://10.3.5.199/Bug.php?BugID=74272
            ReSelectAccount();

            bHandled = CTalkRoutine::OnActionEvent(pActionData);
        }
        break;
    default:
        bHandled = CTalkRoutine::OnActionEvent(pActionData);
        break;
    }

    return bHandled;
}

// 开启通话
void CLineDeviceTalkRoutine::StartTalk(bool bNeedHold/* = true*/)
{
    if (bNeedHold)
    {
        // 开始通话前先将其他话路的通话stop
        _SessionManager.HoldAllSession(m_pCallSession);
    }

    // 设置标志
    m_pCallInfo->SetTalkStart(true);
    m_pCallInfo->SetFarMute(false);

    // 开启多模声道
    voice_SetLineDeviceChannel(IPVP_FL_DIR_ALL);

    // http://10.3.5.199/Bug.php?BugID=74153
    ResetVolume();
}

// 停止通话
void CLineDeviceTalkRoutine::StopTalk()
{
    if (m_pCallInfo->IsTalkStart())
    {
#if IF_FEATURE_PAGING
        _MulticastPage.PagingTalkSwitch(m_pCallInfo->GetCallID(), false);
#endif
    }
}

// hold/unhold当前话路
bool CLineDeviceTalkRoutine::SetCallHold(bool bHold)
{
    if (bHold)
    {
        voice_SetLineDeviceChannel(IPVP_FL_DIR_NONE);
    }
    else
    {
        voice_SetLineDeviceChannel(IPVP_FL_DIR_ALL);
        ResetVolume();
    }

    return true;
}

// 重新开启声音
void CLineDeviceTalkRoutine::ResetVolume()
{
    int iVolume = voice_GetVolume(VT_TALK_HANDFREE);
    voice_SetVolume(iVolume, VT_TALK_HANDFREE);
}

// 重新选取账号
void CLineDeviceTalkRoutine::ReSelectAccount()
{
    if (!acc_IsAccountIDValid(m_pCallInfo->GetAccountID())
            || acc_IsDirectIPAccount(m_pCallInfo->GetAccountID()))
    {
        m_pCallInfo->SetAccountID(acc_GetUsableAccount());
    }
}

#endif // IF_SUPPORT_LINE_DEVICE
