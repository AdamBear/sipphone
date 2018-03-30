#include "scaasyncproxy.h"
#include "talk/talklogic/src/talklogic_inc.h"

bool CSCAASyncProxy::OnBLAError()
{
    return SetTimerOnce(50);
}

bool CSCAASyncProxy::OnCallBack()
{
    CSingleSession* pSession = _SessionManager.GetSingleSession(m_iSessionID);
    if (pSession == NULL)
    {
        return false;
    }

    return _TalklogicManager.TryEnterFinishedRoutine(pSession, TALK_ERROR_CALLOUT_SLU);
}
