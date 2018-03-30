#include "noanswerfwdasyncproxy.h"
#include "talk/talklogic/src/talklogic_inc.h"
#include "talk/talkadapter/include/sessionstateproxy.h"
CNoAnswerFWDASyncProxy::CNoAnswerFWDASyncProxy(int iSessionID) : CBaseTalkASyncProxy(iSessionID)
{
    m_iTimerValue = STOP_TIMER_VALUE;
}

void CNoAnswerFWDASyncProxy::SetFWDTimer(CCallInfo* pCallInfo)
{
    // 无应答转移
    if (NULL == pCallInfo)
    {
        return;
    }

#ifdef IF_FEATURE_PSTN
    if (acc_IsPstnAccount(pCallInfo->GetAccountID()))
    {
        return;
    }
#endif // IF_FEATURE_PSTN

#if IF_BUG_23511_SYNC_IGNORE_LOCAL_DNDFWD
    if (feature_IsForwardSync() && !feature_IsForwardLocalProcess(pCallInfo->GetAccountID()))
    {
        return;
    }
#endif

    int iRingTime = 0; // 呼叫转移时间
    if (acc_IsAccountIDValid(pCallInfo->GetAccountID())
            && fwd_GetInfo(pCallInfo->GetAccountID(), FWD_TYPE_NO_ANSWER, NULL, &iRingTime))
    {
        // 不要设置时间为0 的定时器，同时不设置为默认值的定时器
        if (iRingTime >= 0)
        {
            int RingDuration = configParser_GetConfigInt(kszNoAnswerForwardRingDuration);
            if (RingDuration <= 0)
            {
                RingDuration = 6;
            }

            iRingTime *= RingDuration;
            int iTimerFWDCall = iRingTime * 1000;
            if (iTimerFWDCall >= TOTAL_RING_INTERVAL * RingDuration * 1000)
            {
                // 参考http://10.2.1.199/Bug.php?BugID=14911
                iTimerFWDCall = (TOTAL_RING_INTERVAL * RingDuration - 2 * RingDuration / 3) * 1000;
            }

            //暂存，提供给外部判读使用
            m_iTimerValue = iTimerFWDCall;
            SetTimerOnce(iTimerFWDCall);
        }
    }
}

bool CNoAnswerFWDASyncProxy::OnCallBack()
{
    CCallInfo* pCallInfo = CSessionStateProxy(m_iSessionID).GetCallInfo();
    if (NULL == pCallInfo)
    {
        return false;
    }

    // PCCW:只要本地功能同步开关开启就不处理fwd
    if (feature_IsForwardSync()
            && !feature_IsForwardLocalProcess(pCallInfo->GetAccountID()))
    {
        return true;
    }

    // 保存转移号码
    // m_strFWDNumber = g_pForward->GetForwardTargetByType(FORWARD_TYPE_NOANSWER);
    // 转移来电
    yl::string strFWDTarget;
    yl::string strRemoteNumber;
    if (configParser_GetConfigInt(kszUseFromNumberMatchEmergency) == 1)
    {
        strRemoteNumber = pCallInfo->m_tRemoteInfo.sFromSIPName;
    }
    else
    {
        strRemoteNumber = pCallInfo->m_tRemoteInfo.sSIPName;
    }

    //取消定时器
    Kill();

    if (Emergency_IsForwardEmergency(strRemoteNumber.c_str())
            || !fwd_GetInfo(pCallInfo->GetAccountID(), FWD_TYPE_NO_ANSWER, &strFWDTarget))
    {
        //http://10.2.1.199/Bug.php?BugID=92067 重新播放铃声
        SignalParams sigp(pCallInfo);
        modtklsn_emit_sync(TALK_SIG_SESSION_PLAY_RINGING, sigp);
        TALK_WARN("Not allow forward");
        return true;
    }

    ForwardIncomingCall(strFWDTarget.c_str(), FWD_TYPE_NO_ANSWER);
    return true;
}

// forward新来电
void CNoAnswerFWDASyncProxy::ForwardIncomingCall(const char* lpszNumber/* = NULL*/, FWD_TYPE eFwdType/* = FWD_TYPE_MANUAL*/)
{
    CSessionStateProxy ssP(m_iSessionID);
    CCallInfo* pCallInfo = ssP.GetCallInfo();
    if (NULL == pCallInfo)
    {
        return;
    }

    // 如果lpszNumber不为空则FWD到lpszNumber，否则FWD到m_strFWDNumber
    yl::string strTarget = pCallInfo->GetDialText();
    if (lpszNumber != NULL)
    {
        strTarget = lpszNumber;
    }

    if (strTarget.empty())
    {
        return;
    }

    // 不允许forward到lpszNumber则不做处理
    if (!fwd_IsAllowForwardTo(lpszNumber))
    {
        return;
    }

    // 根据替换规则替换转移号码
    dialplan_GetReplaceNumberEx(strTarget, pCallInfo->GetAccountID(), DPCM_FWD);

    modVoip_RequestForward(pCallInfo->GetAccountID(), pCallInfo->GetCallID(),
                           strTarget.c_str(),
                           eFwdType);


    // 通话类型设置为CT_FORWARDCALL
    pCallInfo->m_eCallType = CT_FORWARDCALL;

    pCallInfo->m_eCallRelation = CR_FWD_TO;
    pCallInfo->m_strRelationNum = strTarget;

    SignalParams sig(pCallInfo);
    sig.strData = strTarget;
    modtklsn_emit_sync(TALK_SIG_SESSION_SAVE_LOG, sig);
    pCallInfo->m_strForwardTarget = strTarget;
    ssP.ExitSession();
}

