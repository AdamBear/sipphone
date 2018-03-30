#include "talklistener_inc.h"
#include "talk/dialplan/src/dialplanmanager.h"

CDialPlanListener::CDialPlanListener()
    : LSN_CLS_LOG_NAME
{
    REG_ASYNC_SIGNAL(TALK_SIG_SESSION_ROUTINE_CHANGED, CDialPlanListener::OnCreateEvent);
    REG_ASYNC_SIGNAL(TALK_SIG_SESSION_PRE_DELETE, CDialPlanListener::OnSessionDestory);

    REG_SYNC_SIGNAL(TALK_SIG_ACTION_EVENT, CDialPlanListener::OnUIAction);
    REG_ASYNC_SIGNAL(TALK_SIG_SESSION_TEXT_CHANGE, CDialPlanListener::OnTextChange);
    REG_ASYNC_SIGNAL(TALK_SIG_SESSION_ACCOUNT_CHANGE, CDialPlanListener::OnAccountChange);

    REG_SYNC_SIGNAL(TALK_SIG_DIAL_CALLOUT, CDialPlanListener::OnDialCallout);
}

DECL_SIGNAL_HANDLER(CDialPlanListener::OnAccountChange)
{
    CSessionProxy session(sigp.iSessionID);
    TALK_DBG("CDialPlanListener::OnAccountChange session[%d] acc[%d]->[%d]", sigp.iSessionID,
             sigp.wParam.iValue, session.GetAccountID());
    return _DialPlanManager.OnAccountChanged(sigp.iSessionID, session.GetAccountID());
}

DECL_SIGNAL_HANDLER(CDialPlanListener::OnTextChange)
{
    TALK_DBG("CDialPlanListener::OnTextChange session[%d] text[%s]",
             sigp.iSessionID, sigp.strData.c_str());
    return _DialPlanManager.OnTextChanged(sigp.iSessionID, sigp.strData);
}

DECL_SIGNAL_HANDLER(CDialPlanListener::OnCreateEvent)
{
    if (Comm_IsRoutineInherits(ROUTINE_DIALING_CLS, (ROUTINE_STATE)sigp.wParam.iValue))
    {
        return CALL_HANDLER(OnSessionDestory);
    }
    else if (Comm_IsRoutineInherits(ROUTINE_DIALING_CLS, (ROUTINE_STATE)sigp.lParam.iValue))
    {
        CSessionProxy session(sigp.iSessionID);
        return _DialPlanManager.OnEnterDial(sigp.iSessionID, session.GetAccountID(), sigp.strData);
    }

    return false;
}

DECL_SIGNAL_HANDLER(CDialPlanListener::OnUIAction)
{
    DataUI2Logic* pDataUI2Logic = (DataUI2Logic*)sigp.wParam.pValue;
    if (pDataUI2Logic == NULL)
    {
        return false;
    }

    CSessionProxy proxy(sigp.iSessionID);
    if (!proxy.IsInherits(ROUTINE_DIALING_CLS))
    {
        return false;
    }

    TALK_ACTION_TYPE eAction = pDataUI2Logic->eAction;
    if (eAction == TALK_ACTION_KEYPRESS
            || eAction == TALK_ACTION_SCREEN_CLICK)
    {
        return _DialPlanManager.OnRestart(sigp.iSessionID);
    }
    else if (eAction == TALK_ACTION_KEY_PAD
             || eAction == TALK_ACTION_WND_FOCUSE_CHANGE)
    {
        bool bFocus = (eAction == TALK_ACTION_KEY_PAD
                       && pDataUI2Logic->wActionParam == 0)
                      || (eAction == TALK_ACTION_WND_FOCUSE_CHANGE
                          && pDataUI2Logic->wActionParam == 1);

        return _DialPlanManager.OnFocusChanged(sigp.iSessionID, bFocus);
    }

    return false;
}

DECL_SIGNAL_HANDLER(CDialPlanListener::OnDialCallout)
{
    CSessionProxy proxy(sigp.iSessionID);
    CCallInfo* pCallInfo = proxy.GetCallInfo();
    if (pCallInfo == NULL)
    {
        return false;
    }

    dialplan_GetCalloutParam(sigp.iSessionID, pCallInfo->GetAccountID(), sigp.wParam.iValue, pCallInfo->GetCallOutParam());
    return true;
}

DECL_SIGNAL_HANDLER(CDialPlanListener::OnSessionDestory)
{
    return _DialPlanManager.OnLeaveDial(sigp.iSessionID);
}
