#include "talklistener_inc.h"
#include "listencenter.h"

CBaseTalkListener::CBaseTalkListener(ListenerName nm) : name(nm)
{
}

void CBaseTalkListener::Connect(bool bsync, TALK_SIGNAL_TYPE sig, const CallBackFunctor& cb, int priority)
{
    ListenCenter.Register(bsync, sig, cb, this, priority);
}

bool CBaseTalkListener::OnETLMessage(msgObject& msg)
{
    TALK_DBG("OnETLMessage name[%s] msg[%d][%#x][%s] wl[%d][%d]", name, msg.message, msg.message, GetETLMSGName(msg.message), msg.wParam, msg.lParam);

    AUTO_WND_UPDATE;
    AUTO_SCOPED_TIMER;

    return OnMessage(msg);
}

void CBaseTalkListener::RegETLMsg(size_t from, size_t to /* = 0*/)
{
    do
    {
        SingleMsgReg(from, CMSGSubscriber(this, &CBaseTalkListener::OnETLMessage));
    }
    while (++from <= to);
}

CListenInfo* CBaseTalkListener::GetCallerData(int iCallID)
{
    /* 转换为SessionID处理 */
    return ListenCenter.GetData(talklogic_GetSessionIdByCallID(iCallID, false));
}

CListenInfo* CBaseTalkListener::GetCallerData(CCallInfo* p)
{
    if (p == NULL)
    {
        return NULL;
    }

    return GetCallerData(p->GetCallID());
}

CListenInfo* CBaseTalkListener::GetSessionData(int iSessionID)
{
    return ListenCenter.GetData(iSessionID);
}
