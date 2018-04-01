#include "talklistener_inc.h"
#include "application/src/doorlinecontroller.h"
#include "application/include/modapplication.h"
#include "talkadapter/src/talkadaptermanager.h"

#ifdef IF_FEATURE_ENTRANCE_GUARD

CSessionProxy GetCurrentDoorLineSession()
{
    CSessionProxy proxy = CDooorPhoneListener::GetRingSession();

    if (!proxy)
    {
        return _TalkAdapter.GetFocusedSessionID();
    }

    return CSessionProxy();
}

bool application_IsDoorLineCall(int iSessionId/* = INVALID_SESSION_ID*/)
{
    CSessionProxy proxy(iSessionId);
    if (iSessionId == INVALID_HANDLE)
    {
        proxy = CDooorPhoneListener::GetRingSession();
    }

    CDooorPhoneListener::GetTargetProxy(proxy);
    if (!proxy)
    {
        return false;
    }

    return CDooorPhoneListener::IsDoorLineCall(proxy);
}

bool CDooorPhoneListener::ReverseFindIncomeVistor::operator()(CSessionProxy& proxy)
{
    if (!proxy.IsInherits(ROUTINE_RINGING))
    {
        return false;
    }

    if (!first)
    {
        first = proxy;
    }

    if (IsDoorLineCall(proxy))
    {
        last = proxy;
    }

    return false;
}

CDooorPhoneListener::CDooorPhoneListener()
    : LSN_CLS_LOG_NAME
{
    REG_ASYNC_SIGNAL(TALK_SIG_SESSION_FOCUS_CHANGE, CDooorPhoneListener::OnFocusChange);
    REG_ASYNC_SIGNAL(TALK_SIG_SESSION_PRE_DELETE, CDooorPhoneListener::OnReleaseEvent);
    REG_ASYNC_SIGNAL(TALK_SIG_SESSION_DTMF_FINISH, CDooorPhoneListener::OnDtmfFinished);
    REG_ASYNC_SIGNAL(TALK_SIG_SESSION_HIDE_CHANGE, CDooorPhoneListener::OnTalkHide);
    REG_ASYNC_SIGNAL(TALK_SIG_SESSION_ROUTINE_CHANGED, CDooorPhoneListener::OnRoutineChanged);

    REG_SYNC_SIGNAL(TALK_SIG_ACTION_EVENT, CDooorPhoneListener::OnUIEvent);

}

CSessionProxy CDooorPhoneListener::GetRingSession()
{
    ReverseFindIncomeVistor vistor;
    talklogic_TraverseSession(vistor);
    return vistor.GetResult();
}

void CDooorPhoneListener::GetTargetProxy(CSessionProxy& proxy)
{
    /* 找到第一路门禁子话路 */
    if (!proxy.IsSingle())
    {
        YLVector<CSessionProxy> vec;
        if (!proxy.GetSubSession(vec))
        {
            return;
        }

        proxy = CSessionProxy(INVALID_SESSION_ID);
        YLVector<CSessionProxy>::iterator it = vec.begin();
        for (; it != vec.end(); ++it)
        {
            if (IsDoorLineCall(*it))
            {
                proxy = *it;
                break;
            }
        }
    }
}

bool CDooorPhoneListener::OpenDoor(const CSessionProxy& proxy)
{
    int iSessionID = proxy;
    if (!proxy.IsSingle()
            || !IsDoorLineCall(iSessionID))
    {
        TALK_WARN("ProcessAction OpenDoor can not find active task id");
        return false;
    }

    EDL_COMPATIBLE eType = application_GetEDLCType();//获取服务器对sip info的支持情况

    if (eType == EDLC_SERVER || proxy.IsTalkSession())//服务器支持 or 通话Session
    {
        yl::string strDtmf = application_GetDTMFString(iSessionID);
        TALK_INFO("OpenDoor session[%d] dtmf[%s]", iSessionID, strDtmf.c_str());

        proxy.SendDTMF(strDtmf);

        // 保证server模式下体验和phone一致，自动挂断
        if (eType == EDLC_SERVER && proxy.IsRinging())
        {
            application_SetAutoAnswer(iSessionID, true);
        }

        return true;
    }
    else if (eType == EDLC_PHONE || eType == EDLC_PHONE_SIP)//服务器不支持但是话机要支持
    {
        if (!proxy.IsRinging())
        {
            return false;
        }

        // 焦点不一样，处理需要有变化。最后结果是当前session失去焦点
        // 来电时，如果所有session都Hold则来电直接设置为焦点造成了这个差异，当前不打算去掉这个
        CSessionStateProxy focus;//构造函数返回焦点的Session ID
        if (focus == proxy)
        {
            CListenInfo* priv = GetSessionData(focus);
            if (priv != NULL)
            {
                CSessionStateProxy last = priv->iLastFocusSession;

                if (last && proxy != last)
                {
                    CListenInfo* priv = GetSessionData(last);
                    if (priv != NULL)
                    {
                        priv->bLastHold = last.IsHold();
                    }

                    // 当前为焦点，先切换焦点，否则下次刷新时，因找不到焦点，界面刷新出现空白
                    _TalkAdapter.SetFocusSession(last);
                }
            }
        }
        else if (focus)
        {
            CListenInfo* priv = GetSessionData(focus);
            if (priv != NULL)
            {
                priv->bLastHold = focus.IsHold();
            }
        }

        application_SetAutoAnswer(iSessionID, true);
        proxy.SetHide(true);
        proxy.Answer();

        TALK_INFO("OpenDoor ringing session=[%d]", iSessionID);
        return true;
    }

    return false;
}

//恢复hold状态
bool CDooorPhoneListener::RestoreHoldStatus(const CSessionProxy& proxy)
{
    CListenInfo* priv = GetSessionData(proxy);
    if (priv == NULL)
    {
        return false;
    }

    CSessionStateProxy last = priv->iLastFocusSession;
    if (!last || proxy == last)
    {
        return false;
    }

    if (last.IsHide()
            || !last.IsFocusSession()
            || !last.IsTalkSession())
    {
        return false;
    }

    priv = GetSessionData(last);
    if (priv == NULL)
    {
        return false;
    }

    last.HoldSession(priv->bLastHold);

    return true;
}

bool CDooorPhoneListener::IsDoorLineCall(const CSessionProxy& proxy)
{
    if (!proxy.IsTalkSession()
            && application_GetEDLCType() == EDLC_NONE)
    {
        return false;
    }

    return !g_refEntranceGuard.GetDTMFString(proxy).empty();
}

DECL_SIGNAL_HANDLER(CDooorPhoneListener::OnFocusChange)
{
    CSessionProxy proxy(sigp.iSessionID);
    if (proxy.IsHide())
    {
        return false;
    }

    GetTargetProxy(proxy);
    return g_refEntranceGuard.DealFocusChanged(proxy, sigp.lParam.bValue);
}

DECL_SIGNAL_HANDLER(CDooorPhoneListener::OnReleaseEvent)
{
    return g_refEntranceGuard.DealSessionDestory(sigp.iSessionID);
}

DECL_SIGNAL_HANDLER(CDooorPhoneListener::OnDtmfFinished)
{
    if (application_IsAutoAnswer(sigp.iSessionID))
    {
        CSessionProxy proxy(sigp.iSessionID);

        if (proxy.IsTalkSession())
        {
            RestoreHoldStatus(proxy);
        }
    }
    return g_refEntranceGuard.DealDtmfFinshed(sigp.iSessionID);
}

DECL_SIGNAL_HANDLER(CDooorPhoneListener::OnTalkHide)
{
    return g_refEntranceGuard.DealTalkHide(sigp.iSessionID, sigp.lParam.bValue);
}

DECL_SIGNAL_HANDLER(CDooorPhoneListener::OnRoutineChanged)
{
    CSessionStateProxy proxy(sigp.iSessionID);
    CCallInfo* pCallInfo = proxy.GetCallInfo();
    if (pCallInfo == NULL)
    {
        return false;
    }

    CallInUserInfo& userInfo = pCallInfo->GetRingUserInfo();
    int iState = sigp.lParam.iValue;

    if (iState == ROUTINE_RINGING)
    {
        /* 设置为焦点 */
        if (application_CreateDoorLineTask(sigp.iSessionID, userInfo.strDoorlineDTMF, userInfo.strDoorlineUrl))
        {
            if (talklogic_IsAllSessionRinging())
            {
                proxy.SetFocus();
            }
        }

        return g_refEntranceGuard.DealRingingEnter(sigp.iSessionID);
    }
    else if (iState == ROUTINE_TALKING)
    {
        return g_refEntranceGuard.DealTalkEnter(sigp.iSessionID);
    }

    return false;
}

DECL_SIGNAL_HANDLER(CDooorPhoneListener::OnUIEvent)
{
    DataUI2Logic* pActionData = (DataUI2Logic*)sigp.wParam.pValue;
    if (pActionData == NULL)
    {
        return false;
    }

    CSessionStateProxy proxy(sigp.iSessionID);
    CCallInfo* pCallInfo = proxy.GetCallInfo();
    if (pCallInfo == NULL)
    {
        return false;
    }

    if (pActionData->eAction != TALK_ACTION_OPENDOOR)
    {
        return false;
    }

    // 转交给 App Task焦点处理 -> 再交给Talk
    CSessionStateProxy actived = application_GetCurrentTaskId();
    if (!actived.IsSingle())
    {
        TALK_WARN("ProcessAction OpenDoor can not find app task id, try find in talk");
        actived = GetCurrentDoorLineSession();
    }

    TALK_INFO("OpenDoor target[%d] talk[%d]", (int)actived, sigp.iSessionID);
    OpenDoor(actived);

    SIG_REPLY(true);
}

#endif
