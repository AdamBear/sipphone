#include "refuseasyncproxy.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talk/talklogic/src/talklogic_inc.h"

CRefuseASyncProxy::CRefuseASyncProxy(int iSessionID) : CBaseTalkASyncProxy(iSessionID)
{
}

bool CRefuseASyncProxy::OnCallBack()
{
    Kill();
    talklogic_RefuseCall(talklogic_GetCallIdBySessionID(GetSessionID()));
    return true;
}

void CRefuseASyncProxy::SetRefuseCallTimer()
{
    int iTimerRefuse = 100;
    SetTimer(iTimerRefuse);
}

bool CRefuseASyncProxy::RefuseBySIPInfo(CCallInfo* pCallInfo, CallInUserInfo* pUserInfo)
{
    if (pCallInfo == NULL)
    {
        return false;
    }

    int iSessionNum = _SessionManager.GetSessionListSize();
    TALK_INFO("RefuseBySIPInfo: Info[%d], Session Size[%d] AutoAnswerType[%d]",
              pUserInfo->iAutoAnswer, iSessionNum, pUserInfo->uAutoAnswerType);
    int iInterval = pUserInfo->iAutoAnswer > 0 ? pUserInfo->iAutoAnswer : 0;

    if (SIP_CALL_P_ANTO_ANSWER_URGENT == pUserInfo->uAutoAnswerType
            || SIP_CALL_P_ANTO_ANSWER_SILENT == pUserInfo->uAutoAnswerType
            || SIP_CALL_P_ANTO_ANSWER_NORMAL == pUserInfo->uAutoAnswerType)
    {
        if (iSessionNum != 1 && iSessionNum != 2)
        {
            SetRefuseCallTimer();
            return true;
        }
    }
    else if (SIP_CALL_AUTO_ANSWER_DEVICE_UNKNOWN == pUserInfo->uAutoAnswerDevice)
    {
        //http://10.2.1.199/Bug.php?BugID=15438
        //增加对force参数的支持
        if (pUserInfo->iAutoAnswer > 0)
        {
            if (iSessionNum != 1 && !pUserInfo->bForceAutoAnswer)
            {
                // 拒绝新来电
                SetRefuseCallTimer();
                return true;
            }
        }
    }
    return false;
}
