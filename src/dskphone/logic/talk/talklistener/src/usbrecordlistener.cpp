#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
#include "talklistener_inc.h"

CUSBRecordListener::CUSBRecordListener()
    : LSN_CLS_LOG_NAME
{
    REG_ASYNC_SIGNAL(TALK_SIG_SESSION_ROUTINE_CHANGED, CUSBRecordListener::OnRoutineChanged);
    REG_ASYNC_SIGNAL(TALK_SIG_SESSION_HOLD_CHANGE, CUSBRecordListener::OnHoldChange);
    REG_ASYNC_SIGNAL(TALK_SIG_SESSION_PRE_DELETE, CUSBRecordListener::OnSessionExit);
    REG_SYNC_SIGNAL(TALK_SIG_TALK_TO_IDLE, CUSBRecordListener::OnTalkToIdle);
}

DECL_SIGNAL_HANDLER(CUSBRecordListener::OnRoutineChanged)
{
    CSessionProxy proxy(sigp.iSessionID);
    if (!proxy)
    {
        return false;
    }

    int iState = sigp.lParam.iValue;
    switch (iState)
    {
    case ROUTINE_DIALING:
        if (modRecord_IsRecording(RECORD_AUDIO)
                && !talklogic_IsTalkSessionExist())
        {
            modRecord_RecordPause(RECORD_AUDIO, true);
        }
        break;
    case ROUTINE_PREDIAL:
        if (modRecord_IsRecording(RECORD_AUDIO))
        {
            modRecord_RecordPause(RECORD_AUDIO, true);
        }
        break;
    case ROUTINE_TALKING:
    case ROUTINE_NETWORKCONF:
    case ROUTINE_BTTALKING:
        if (modRecord_IsRecordPause(RECORD_AUDIO))
        {
            modRecord_RecordResume(RECORD_AUDIO, true);
        }
        break;
    default:
        break;
    }
    return true;
}

DECL_SIGNAL_HANDLER(CUSBRecordListener::OnHoldChange)
{
    CSessionProxy proxy(sigp.iSessionID);
    if (!proxy || proxy.GetSubSize() > 0)
    {
        return false;
    }

    if (sigp.lParam.bValue)
    {
        if (!talklogic_IsTalkingSessionExist()
                && modRecord_IsRecording(RECORD_AUDIO))
        {
            modRecord_RecordPause(RECORD_AUDIO, true);
        }
    }
    else
    {
        if (modRecord_IsRecordPause(RECORD_AUDIO))
        {
            modRecord_RecordResume(RECORD_AUDIO, true);
        }
    }

    return true;
}

DECL_SIGNAL_HANDLER(CUSBRecordListener::OnSessionExit)
{
    if (talklogic_IsTalkingSessionExist())
    {
        return false;
    }

    // 还有Session存在,但是都不处理通话状态则自动暂停录音
    if (modRecord_IsRecording(RECORD_AUDIO))
    {
        modRecord_RecordPause(RECORD_AUDIO, true);
    }
    return true;
}

DECL_SIGNAL_HANDLER(CUSBRecordListener::OnTalkToIdle)
{
    // 如果是通话中录音,则停止
    if (modRecord_IsRecording(RECORD_AUDIO)
            || modRecord_IsRecordPause(RECORD_AUDIO))
    {
        if (modRecord_IsTalkRecording(RECORD_AUDIO))
        {
            modRecord_RecordStop(RECORD_AUDIO);
        }
        else
        {
            modRecord_RecordResume(RECORD_AUDIO, true);
        }
    }
    return true;
}

#endif // (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
