#include "autorefuseasyncproxy.h"
#include "talklogic/src/talklogic_inc.h"
#include "talklogic/include/modtalklogic.h"

CAutoRefuseASyncProxy::CAutoRefuseASyncProxy(int iSessionID) : CBaseTalkASyncProxy(iSessionID)
{

}


void CAutoRefuseASyncProxy::SetSessionID(int iSessionID)
{
    if (GetSessionID() != iSessionID)
    {
        Kill();
        m_iSessionID = iSessionID;
    }
}

bool CAutoRefuseASyncProxy::OnCallBack()
{
    // 自动拒绝来电
    TALK_INFO("Refuse incoming call timeout");
    Kill();
    CCallInfo* pCallInfo = talklogic_GetCallInfoBySessionID(m_iSessionID);
    if (pCallInfo == NULL)
    {
        return false;
    }
    // http://10.2.1.199/Bug.php?BugID=55955
    pCallInfo->m_eCallType = CT_MISSCALL;
    talklogic_RefuseCall(pCallInfo->GetCallID(), RT_BUSY);
    return true;
}


void CAutoRefuseASyncProxy::SetAutoRefuseCallTimer(int iSessionID)
{
    SetSessionID(iSessionID);
    int iTimerAutoRefuse = configParser_GetConfigInt(kszAutoRefuseTime) * 1000 + 1;

    if (iTimerAutoRefuse <= 0)
    {
        return;
    }
    SetTimer(iTimerAutoRefuse);
}
